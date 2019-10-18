//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvCore/ResourceSystem.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/OS.h>

#include <iostream>

using namespace djv::Core::FileSystem;

namespace djv
{
    namespace Core
    {
        struct ResourceSystem::Private
        {
            Path applicationPath;
            std::map<ResourcePath, Path> paths;
        };

        namespace
        {
            Path getApplicationRoot(const Path & value)
            {
                Path out;
                auto pieces = Path::splitDir(std::string(value));
                const size_t max = 3;
                for (size_t i = 0; i < max && pieces.size() && out.isEmpty(); ++i)
                {
                    const std::string tmp = Path::joinDirs(pieces);
                    const std::vector<std::string> tests =
                    {
                        "djvCore.en.text",
                        "etc/Text/djvCore.en.text",
                        "bin"
                    };
                    for (const auto& test : tests)
                    {
                        if (FileInfo(Path(tmp, test)).doesExist())
                        {
                            out = Path(tmp);
                            break;
                        }
                    }
                    pieces.pop_back();
                }
                return out;
            }
            
        } // namespace
        
        void ResourceSystem::_init(const std::string & argv0, const std::shared_ptr<Context>& context)
        {
            ISystemBase::_init("djv::Core::ResourceSystem", context);

            DJV_PRIVATE_PTR();
            try
            {
                p.applicationPath = Path::getAbsolute(Path(Path(argv0).getDirectoryName()));
            }
            catch (const std::exception & e)
            {
                //! \bug How should we really handle this error?
                std::cerr << "[ERROR] Cannot find the application path: " << e.what() << std::endl;
            }
            
            std::string env = OS::getEnv("DJV_RESOURCE_PATH");
            if (!env.empty())
            {
                p.paths[ResourcePath::Application] = Path(env);
            }
            else
            {
                p.paths[ResourcePath::Application] = getApplicationRoot(p.applicationPath);
            }

            Path documents;
            env = OS::getEnv("DJV_DOCUMENTS_PATH");
            if (!env.empty())
            {
                documents = Path(env);
            }
            else
            {
                documents = OS::getPath(OS::DirectoryShortcut::Documents);
                try
                {
                    if (!FileInfo(documents).doesExist())
                    {
                        Path::mkdir(documents);
                    }
                    documents.append("DJV");
                    if (!FileInfo(documents).doesExist())
                    {
                        Path::mkdir(documents);
                    }
                }
                catch (const std::exception & e)
                {
                    //! \bug How should we really handle this error?
                    std::cerr << "[ERROR] Cannot create the documents path: " << e.what() << std::endl;
                }
            }
            p.paths[ResourcePath::Documents] = documents;
            
            const std::string applicationName = Path(argv0).getBaseName();
            Path logFile = Path(documents, applicationName + ".log");
            p.paths[ResourcePath::LogFile] = logFile;

            Path settingsFile(documents, applicationName + ".json");
            p.paths[ResourcePath::SettingsFile] = settingsFile;

            Path testPath = p.paths[ResourcePath::Application];
            testPath.append("djvCore.en.text");
            if (FileInfo(testPath).doesExist())
            {
                // The resources are at the root (for example iOS).
                p.paths[ResourcePath::Audio]            = p.paths[ResourcePath::Application];
                p.paths[ResourcePath::Fonts]            = p.paths[ResourcePath::Application];
                p.paths[ResourcePath::Icons]            = p.paths[ResourcePath::Application];
                p.paths[ResourcePath::Images]           = p.paths[ResourcePath::Application];
                p.paths[ResourcePath::Models]           = p.paths[ResourcePath::Application];
                p.paths[ResourcePath::Shaders]          = p.paths[ResourcePath::Application];
                p.paths[ResourcePath::Text]             = p.paths[ResourcePath::Application];
                p.paths[ResourcePath::Color]            = p.paths[ResourcePath::Application];
                p.paths[ResourcePath::Documentation]    = p.paths[ResourcePath::Application];
            }
            else
            {
                Path etc = Path(p.paths[ResourcePath::Application], "etc");
                p.paths[ResourcePath::Audio]            = Path(etc, "Audio");
                p.paths[ResourcePath::Fonts]            = Path(etc, "Fonts");
                p.paths[ResourcePath::Icons]            = Path(etc, "Icons");
                p.paths[ResourcePath::Images]           = Path(etc, "Images");
                p.paths[ResourcePath::Models]           = Path(etc, "Models");
                p.paths[ResourcePath::Shaders]          = Path(etc, "Shaders");
                p.paths[ResourcePath::Text]             = Path(etc, "Text");
                p.paths[ResourcePath::Color]            = Path(etc, "Color");
                Path docs = Path(p.paths[ResourcePath::Application], "docs");
                docs = Path(docs, "_site");
                docs = Path(docs, "documentation.html");
                p.paths[ResourcePath::Documentation]    = docs;
            }
        }

        ResourceSystem::ResourceSystem() :
            _p(new Private)
        {}

        ResourceSystem::~ResourceSystem()
        {}

        std::shared_ptr<ResourceSystem> ResourceSystem::create(const std::string & argv0, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ResourceSystem>(new ResourceSystem);
            out->_init(argv0, context);
            return out;
        }

        Path ResourceSystem::getPath(ResourcePath value) const
        {
            DJV_PRIVATE_PTR();
            const auto i = p.paths.find(value);
            return i != p.paths.end() ? i->second : Path();
        }

    } // namespace Core
} // namespace djv

