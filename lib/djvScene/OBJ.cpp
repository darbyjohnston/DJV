//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvScene/OBJ.h>

#include <djvScene/Material.h>
#include <djvScene/Primitive.h>
#include <djvScene/Scene.h>

#include <djvAV/TriangleMesh.h>

#include <djvCore/FileIO.h>
#include <djvCore/LogSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene
    {
        namespace IO
        {
            namespace OBJ
            {
                namespace
                {
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
                        for (; i < size; ++i)
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

                    void read(const std::string& fileName, AV::Geom::TriangleMesh& mesh, size_t threads)
                    {
                        // Open the file.
                        FileSystem::FileIO f;
                        f.open(fileName, FileSystem::FileIO::Mode::Read);
                        const size_t fileSize = f.getSize();
                        std::vector<char> data(fileSize);
                        char* fileStart = data.data();
                        const char* fileEnd = fileStart + fileSize;
                        f.read(fileStart, fileSize);

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
                        std::vector<AV::Geom::TriangleMesh*> meshPieces;
                        meshPieces.push_back(&mesh);
                        for (size_t i = 1; i < filePieces.size(); ++i)
                        {
                            meshPieces.push_back(new AV::Geom::TriangleMesh);
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
                                    AV::Geom::TriangleMesh::Face face;
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
                                            const char* lineBegin = line;
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
                                            AV::Geom::TriangleMesh::faceToTriangles(face, mesh->triangles);
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
                            for (const auto& v : meshPiece->v)
                                mesh.v.push_back(v);
                            for (const auto& c : meshPiece->c)
                                mesh.c.push_back(c);
                            for (const auto& t : meshPiece->t)
                                mesh.t.push_back(t);
                            for (const auto& n : meshPiece->n)
                                mesh.n.push_back(n);
                            for (const auto& t : meshPiece->triangles)
                                mesh.triangles.push_back(t);
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
                    const Core::FileSystem::FileInfo& fileInfo,
                    const std::shared_ptr<Core::ResourceSystem>& resourceSystem,
                    const std::shared_ptr<Core::LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileInfo, resourceSystem, logSystem);
                    return out;
                }

                std::future<Info> Read::getInfo()
                {
                    return std::async(
                        std::launch::async,
                        [this]
                        {
                            return Info();
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
                                auto mesh = std::shared_ptr<AV::Geom::TriangleMesh>(new AV::Geom::TriangleMesh);
                                read(_fileInfo.getFileName(), *mesh, 1);
                                primitive->addMesh(mesh);
                                auto material = DefaultMaterial::create();
                                primitive->setMaterial(material);
                                out->addPrimitive(primitive);
                            }
                            catch (const std::exception& e)
                            {
                                std::stringstream ss;
                                ss << DJV_TEXT("The file") << " '" << _fileInfo << "' " << DJV_TEXT("cannot be read") << ". " << e.what();
                                _logSystem->log("djv::Scene::OBJ", ss.str(), LogLevel::Error);
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

                std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<Context>& context)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(
                        pluginName,
                        DJV_TEXT("This plugin provides OBJ file I/O."),
                        fileExtensions,
                        context);
                    return out;
                }

                picojson::value Plugin::getOptions() const
                {
                    return toJSON(_p->options);
                }

                void Plugin::setOptions(const picojson::value& value)
                {
                    fromJSON(value, _p->options);
                }

                std::shared_ptr<IRead> Plugin::read(const FileSystem::FileInfo& fileInfo) const
                {
                    return Read::create(fileInfo, _resourceSystem, _logSystem);
                }

            } // namespace OBJ
        } // namespace IO
    } // namespace Scene
    
    picojson::value toJSON(const Scene::IO::OBJ::Options& value)
    {
        picojson::value out(picojson::object_type, true);
        {
        }
        return out;
    }

    void fromJSON(const picojson::value& value, Scene::IO::OBJ::Options& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

} // namespace djv

