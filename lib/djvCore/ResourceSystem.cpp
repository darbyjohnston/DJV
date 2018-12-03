//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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
            Path getApplicationRoot(const Path& value)
            {
                Path out;
                auto pieces = Path::splitDir(value);
                while (pieces.size())
                {
                    const std::string tmp = Path::joinDirs(pieces);
                    if (FileInfo(Path(tmp, "djvCore.en.text")).doesExist() ||
                        FileInfo(Path(tmp, "etc/Text/djvCore.en.text")).doesExist())
                    {
                        out = tmp;
                        break;
                    }
                    pieces.pop_back();
                }
                return out;
            }
            
        } // namespace
        
        void ResourceSystem::_init(const std::string& argv0, const std::shared_ptr<Context>& context)
        {
            ISystem::_init("djv::Core::ResourceSystem", context);

            try
            {
                _p->applicationPath = Path::getAbsolute(Path(argv0).getDirectoryName());
            }
            catch (const std::exception& e)
            {
                //! \bug [1.0 S] How should we really handle this error?
                std::cerr << "Cannot find the application path: " << e.what() << std::endl;
            }
            
            std::string env = OS::getEnv("DJV_RESOURCE_PATH");
            if (!env.empty())
            {
                _p->paths[ResourcePath::Application] = env;
            }
            else
            {
                _p->paths[ResourcePath::Application] = getApplicationRoot(_p->applicationPath);
            }

            Path documents;
            env = OS::getEnv("DJV_DOCUMENTS_PATH");
            if (!env.empty())
            {
                documents = env;
            }
            else
            {
                documents = _getDocumentsPath();
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
                catch (const std::exception& e)
                {
                    //! \bug [1.0 S] How should we really handle this error?
                    std::cerr << "Cannot create the documents path: " << e.what() << std::endl;
                }
            }
            
            const std::string applicationName = Path(argv0).getBaseName();
            Path logFile = Path(documents, applicationName + ".log");
            _p->paths[ResourcePath::LogFile] = logFile;

            Path settingsFile(documents, applicationName + ".json");
            _p->paths[ResourcePath::SettingsFile] = settingsFile;

            Path testPath = _p->paths[ResourcePath::Application];
            testPath.append("djvCore.en.text");
            if (FileInfo(testPath).doesExist())
            {
                // The resources are at the root (for example iOS).
                _p->paths[ResourcePath::AudioDirectory]   = _p->paths[ResourcePath::Application];
                _p->paths[ResourcePath::FontsDirectory]   = _p->paths[ResourcePath::Application];
                _p->paths[ResourcePath::IconsDirectory]   = _p->paths[ResourcePath::Application];
                _p->paths[ResourcePath::ImagesDirectory]  = _p->paths[ResourcePath::Application];
                _p->paths[ResourcePath::ModelsDirectory]  = _p->paths[ResourcePath::Application];
                _p->paths[ResourcePath::ShadersDirectory] = _p->paths[ResourcePath::Application];
                _p->paths[ResourcePath::TextDirectory]    = _p->paths[ResourcePath::Application];
            }
            else
            {
                Path etc = Path(_p->paths[ResourcePath::Application], "etc");
                _p->paths[ResourcePath::AudioDirectory]   = Path(etc, "Audio");
                _p->paths[ResourcePath::FontsDirectory]   = Path(etc, "Fonts");
                _p->paths[ResourcePath::IconsDirectory]   = Path(etc, "Icons");
                _p->paths[ResourcePath::ImagesDirectory]  = Path(etc, "Images");
                _p->paths[ResourcePath::ModelsDirectory]  = Path(etc, "Models");
                _p->paths[ResourcePath::ShadersDirectory] = Path(etc, "Shaders");
                _p->paths[ResourcePath::TextDirectory]    = Path(etc, "Text");
            }
        }

        ResourceSystem::ResourceSystem() :
            _p(new Private)
        {}

        ResourceSystem::~ResourceSystem()
        {}

        std::shared_ptr<ResourceSystem> ResourceSystem::create(const std::string& argv0, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ResourceSystem>(new ResourceSystem);
            out->_init(argv0, context);
            return out;
        }

        Path ResourceSystem::getPath(ResourcePath value) const
        {
            const auto i = _p->paths.find(value);
            return i != _p->paths.end() ? i->second : Path();
        }

    } // namespace Core
} // namespace djv

