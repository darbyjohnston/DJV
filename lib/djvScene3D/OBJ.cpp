// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene3D/OBJ.h>

#include <djvScene3D/Material.h>
#include <djvScene3D/MeshPrimitive.h>
#include <djvScene3D/Scene.h>

#include <djvSystem/FileIO.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/TextSystem.h>

#include <djvGeom/TriangleMesh.h>

#include <djvCore/StringFormat.h>
#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene3D
    {
        namespace OBJ
        {
            namespace
            {
                //! Should this be configurable?
                const size_t threadCount = 4;

                inline const char* findLineEnd(const char* start, const char* end)
                {
                    const char* out = start;
                    for (; out < end; ++out)
                    {
                        if (out < end - 2 && '\\' == out[0] && '\r' == out[1] && '\n' == out[2])
                        {
                            out += 2;
                            continue;
                        }
                        if (out < end - 1 && '\\' == out[0] && '\n' == out[1])
                        {
                            out += 1;
                            continue;
                        }
                        if (out < end - 1 && '\\' == out[0] && '\r' == out[1])
                        {
                            out += 1;
                            continue;
                        }
                        if ('\n' == out[0])
                            break;
                        if ('\r' == out[0])
                            break;
                    }
                    return out;
                }

                inline const char* findWhitespaceEnd(const char* start, const char* end)
                {
                    const char* out = start;
                    for (; out < end && (*out == ' ' || *out == '\\' || *out == '\n' || *out == '\r'); ++out)
                        ;
                    return out;
                }

                inline const char* findWordEnd(const char* start, const char* end)
                {
                    const char* out = start;
                    for (; out < end && *out != ' ' && *out != '\\' && *out != '\n' && *out != '\r'; ++out)
                        ;
                    return out;
                }

                void parseFaceIndex(const char* s, size_t size, size_t& v, size_t& t, size_t& n)
                {
                    // Find the slashes.
                    int slashPos[2] = { 0, 0 };
                    int slashIndex = 0;
                    size_t i = 0;
                    for (; i < size && slashIndex < 2; ++i)
                    {
                        if ('/' == s[i])
                        {
                            slashPos[slashIndex++] = static_cast<int>(i);
                        }
                    }

                    // Convert the strings to ints.
                    if (!slashPos[0])
                    {
                        String::fromString(s, i, v);
                    }
                    else if (!slashPos[1])
                    {
                        String::fromString(s, slashPos[0], v);
                        String::fromString(s + slashPos[0] + 1, i - slashPos[0] - 1, t);
                    }
                    else
                    {
                        String::fromString(s, slashPos[0], v);
                        String::fromString(s + slashPos[0] + 1, slashPos[1] - slashPos[0] - 1, t);
                        String::fromString(s + slashPos[1] + 1, i - slashPos[1] - 1, n);
                    }
                }

                void read(const std::string& fileName, Geom::TriangleMesh& mesh, size_t threads)
                {
                    // Open the file.
                    auto io = System::File::IO::create();
                    io->open(fileName, System::File::Mode::Read);
                    const size_t fileSize = io->getSize();
                    std::vector<char> data(fileSize);
                    char* fileStart = data.data();
                    const char* fileEnd = fileStart + fileSize;
                    io->read(fileStart, fileSize);

                    // Divide up the file for each thread.
                    threads = std::max(threads, size_t(1));
                    const size_t filePieceSize = fileSize / threads;
                    typedef std::pair<const char*, const char*> FilePiece;
                    std::vector<FilePiece> filePieces;
                    const char* line = fileStart;
                    const char* lineEnd = nullptr;
                    for (size_t i = 0; i < threads; ++i, line = lineEnd)
                    {
                        // Find the end of the line for this piece of the file.
                        lineEnd = findLineEnd(line + filePieceSize, fileEnd);
                        if (lineEnd < fileEnd)
                            ++lineEnd;

                        // Add this piece to the list.
                        filePieces.push_back(FilePiece(line, lineEnd));
                    }

                    // Read the file pieces.
                    std::vector<Geom::TriangleMesh*> meshPieces;
                    meshPieces.push_back(&mesh);
                    for (size_t i = 1; i < filePieces.size(); ++i)
                    {
                        meshPieces.push_back(new Geom::TriangleMesh);
                    }
                    std::vector<std::future<void> > futures;
                    for (size_t i = 0; i < filePieces.size(); ++i)
                    {
                        auto filePiece = filePieces[i];
                        auto mesh = meshPieces[i];
                        futures.push_back(std::async(
                            std::launch::async,
                            [filePiece, mesh]
                            {
                                const char* line = filePiece.first;
                                const char* lineEnd = filePiece.first;
                                const char* word = nullptr;
                                Geom::TriangleMesh::Face face;
                                glm::vec3 v;
                                glm::vec3 c;
                                for (; line < filePiece.second; ++lineEnd, line = lineEnd)
                                {
                                    // Find the end of the line.
                                    lineEnd = findLineEnd(lineEnd, filePiece.second);

                                    // Skip white space.
                                    line = findWhitespaceEnd(line, lineEnd);

                                    const size_t lineSize = lineEnd - line;
                                    if (lineSize >= 1 && '#' == line[0])
                                    {
                                        // Skip comments.
                                    }
                                    else if (lineSize >= 2 && 'v' == line[0] && 'n' == line[1])
                                    {
                                        // Read a normal.
                                        ++line;
                                        ++line;
                                        for (int component = 0; component < 3 && line < lineEnd; ++component)
                                        {
                                            line = findWhitespaceEnd(line, lineEnd);
                                            word = line;
                                            line = findWordEnd(line, lineEnd);
                                            String::fromString(word, line - word, v[component]);
                                        }
                                        mesh->n.push_back(v);
                                    }
                                    else if (lineSize >= 2 && 'v' == line[0] && 't' == line[1])
                                    {
                                        // Read a texture coordinate.
                                        ++line;
                                        ++line;
                                        for (int component = 0; component < 3 && line < lineEnd; ++component)
                                        {
                                            line = findWhitespaceEnd(line, lineEnd);
                                            word = line;
                                            line = findWordEnd(line, lineEnd);
                                            String::fromString(word, line - word, v[component]);
                                        }
                                        mesh->t.push_back(v);
                                    }
                                    else if (lineSize >= 1 && 'v' == line[0])
                                    {
                                        // Read a vertex.
                                        ++line;
                                        int component = 0;
                                        for (; component < 3 && line < lineEnd; ++component)
                                        {
                                            line = findWhitespaceEnd(line, lineEnd);
                                            word = line;
                                            line = findWordEnd(line, lineEnd);
                                            String::fromString(word, line - word, v[component]);
                                        }
                                        mesh->v.push_back(v);
                                        component = 0;
                                        for (; component < 3 && line < lineEnd; ++component)
                                        {
                                            line = findWhitespaceEnd(line, lineEnd);
                                            word = line;
                                            line = findWordEnd(line, lineEnd);
                                            String::fromString(word, line - word, c[component]);
                                        }
                                        if (3 == component)
                                        {
                                            mesh->c.push_back(c);
                                        }
                                    }
                                    else if (lineSize >= 1 && 'f' == line[0])
                                    {
                                        // Read a face.
                                        ++line;
                                        face.v.resize(3);
                                        int index = 0;
                                        while (line < lineEnd)
                                        {
                                            line = findWhitespaceEnd(line, lineEnd);
                                            word = line;
                                            line = findWordEnd(line, lineEnd);
                                            if (line - word)
                                            {
                                                size_t v = 0;
                                                size_t t = 0;
                                                size_t n = 0;
                                                parseFaceIndex(word, line - word, v, t, n);
                                                if (index < 3)
                                                {
                                                    face.v[index].v = v;
                                                    face.v[index].t = t;
                                                    face.v[index++].n = n;
                                                }
                                                else
                                                {
                                                    face.v.emplace_back(v, t, n);
                                                }
                                            }
                                        }
                                        Geom::TriangleMesh::faceToTriangles(face, mesh->triangles);
                                    }
                                }
                            }));
                    }
                    for (auto& future : futures)
                    {
                        future.get();
                    }

                    // Combine the mesh pieces.
                    for (size_t i = 1; i < meshPieces.size(); ++i)
                    {
                        auto meshPiece = meshPieces[i];
                        mesh.v.insert(mesh.v.end(), meshPiece->v.begin(), meshPiece->v.end());
                        mesh.c.insert(mesh.c.end(), meshPiece->c.begin(), meshPiece->c.end());
                        mesh.t.insert(mesh.t.end(), meshPiece->t.begin(), meshPiece->t.end());
                        mesh.n.insert(mesh.n.end(), meshPiece->n.begin(), meshPiece->n.end());
                        mesh.triangles.insert(mesh.triangles.end(), meshPiece->triangles.begin(), meshPiece->triangles.end());
                        delete meshPieces[i];
                    }

                    // Implied texture/normal indices.
                    const bool impliedT = mesh.v.size() == mesh.t.size();
                    const bool impliedN = mesh.v.size() == mesh.n.size();
                    if (impliedT || impliedN)
                    {
                        for (auto& t : mesh.triangles)
                        {
                            if (impliedT && !t.v0.t)
                            {
                                t.v0.t = t.v0.v;
                            }
                            if (impliedN && !t.v0.n)
                            {
                                t.v0.n = t.v0.v;
                            }
                            if (impliedT && !t.v1.t)
                            {
                                t.v1.t = t.v1.v;
                            }
                            if (impliedN && !t.v1.n)
                            {
                                t.v1.n = t.v1.v;
                            }
                            if (impliedT && !t.v2.t)
                            {
                                t.v2.t = t.v2.v;
                            }
                            if (impliedN && !t.v2.n)
                            {
                                t.v2.n = t.v2.v;
                            }
                        }
                    }

                    mesh.bboxUpdate();
                }

            } // namespace

            struct Read::Private
            {

            };

            Read::Read() :
                _p(new Private)
            {}

            Read::~Read()
            {}

            std::shared_ptr<Read> Read::create(
                const System::File::Info& fileInfo,
                const std::shared_ptr<System::TextSystem>& textSystem,
                const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                const std::shared_ptr<System::LogSystem>& logSystem)
            {
                auto out = std::shared_ptr<Read>(new Read);
                out->_init(fileInfo, textSystem, resourceSystem, logSystem);
                return out;
            }

            std::future<IO::Info> Read::getInfo()
            {
                return std::async(
                    std::launch::async,
                    [this]
                    {
                        return IO::Info();
                    });
            }

            std::future<std::shared_ptr<Scene> > Read::getScene()
            {
                return std::async(
                    std::launch::async,
                    [this]
                    {
                        std::shared_ptr<Scene> out;
                        try
                        {
                            out = Scene::create();
                            auto primitive = MeshPrimitive::create();
                            auto mesh = std::shared_ptr<Geom::TriangleMesh>(new Geom::TriangleMesh);
                            read(_fileInfo.getFileName(), *mesh, threadCount);
                            primitive->addMesh(mesh);
                            auto material = DefaultMaterial::create();
                            primitive->setMaterial(material);
                            out->addPrimitive(primitive);
                        }
                        catch (const std::exception& e)
                        {
                            _logSystem->log(
                                "djv::Scene::OBJ",
                                String::Format("{0}: {1}").
                                    arg(_fileInfo.getFileName()).
                                    arg(_textSystem->getText(DJV_TEXT("error_file_write"))),
                                System::LogLevel::Error);
                        }
                        return out;
                    });
            }

            struct Plugin::Private
            {
                Options options;
            };

            Plugin::Plugin() :
                _p(new Private)
            {}

            Plugin::~Plugin()
            {}

            std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Plugin>(new Plugin);
                out->_init(
                    pluginName,
                    DJV_TEXT("obj_plugin_description"),
                    fileExtensions,
                    context);
                return out;
            }

            rapidjson::Value Plugin::getOptions(rapidjson::Document::AllocatorType& allocator) const
            {
                return toJSON(_p->options, allocator);
            }

            void Plugin::setOptions(const rapidjson::Value& value)
            {
                fromJSON(value, _p->options);
            }

            std::shared_ptr<IO::IRead> Plugin::read(const System::File::Info& fileInfo) const
            {
                return Read::create(fileInfo, _textSystem, _resourceSystem, _logSystem);
            }

        } // namespace OBJ
    } // namespace Scene3D
    
    rapidjson::Value toJSON(const Scene3D::OBJ::Options& value, rapidjson::Document::AllocatorType&)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, Scene3D::OBJ::Options& out)
    {
        if (value.IsObject())
        {
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv

