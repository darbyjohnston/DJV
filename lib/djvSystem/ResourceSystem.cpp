// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/ResourceSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfo.h>
#include <djvSystem/PathFunc.h>

#include <djvCore/OSFunc.h>

#include <iostream>
#include <map>

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        struct ResourceSystem::Private
        {
            File::Path applicationPath;
            std::map<File::ResourcePath, File::Path> paths;
        };

        namespace
        {
            File::Path getApplicationRoot(const File::Path& value)
            {
                File::Path out;
                auto pieces = File::splitDir(std::string(value));
                const size_t max = 3;
                for (size_t i = 0; i < max && pieces.size() && out.isEmpty(); ++i)
                {
                    const std::string tmp = File::joinDirs(pieces);
                    const std::vector<std::string> tests =
                    {
                        "djvSystem.en.text",
                        "etc/Text/djvSystem.en.text",
                        "bin"
                    };
                    for (const auto& test : tests)
                    {
                        if (File::Info(File::Path(tmp, test)).doesExist())
                        {
                            out = File::Path(tmp);
                            break;
                        }
                    }
                    pieces.pop_back();
                }
                return out;
            }

            bool doesExecutableExist(const std::string& path)
            {
                bool out = false;
                if (File::Info(File::Path(path)).doesExist())
                {
                    out = true;
                }
                else if (File::Info(File::Path(path + ".com")).doesExist())
                {
                    out = true;
                }
                else if (File::Info(File::Path(path + ".exe")).doesExist())
                {
                    out = true;
                }
                return out;
            }
            
        } // namespace
        
        void ResourceSystem::_init(const std::string& argv0, const std::shared_ptr<Context>& context)
        {
            ISystemBase::_init("djv::System::ResourceSystem", context);

            DJV_PRIVATE_PTR();
            try
            {
                // Try and get the application path from the command-line arguments,
                // if that doesn't work check the search path.
                if (doesExecutableExist(argv0))
                {
                    p.applicationPath = File::getAbsolute(File::Path(File::Path(argv0).getDirectoryName()));
                }
                else
                {
                    std::vector<std::string> searchPaths;
                    if (OS::getStringListEnv("PATH", searchPaths))
                    {
                    for (const auto& i : searchPaths)
                    {
                        if (doesExecutableExist(File::Path(i, argv0).get()))
                        {
                            p.applicationPath = File::Path(i);
                            break;
                        }
                    }
                    }
                }
            }
            catch (const std::exception& e)
            {
                //! \bug How should we handle this error?
                std::cerr << "[ERROR] Cannot find the application path: " << e.what() << std::endl;
            }
            
            std::string env;
            if (OS::getEnv("DJV_RESOURCE_PATH", env) && !env.empty())
            {
                p.paths[File::ResourcePath::Application] = File::Path(env);
            }
            else
            {
                p.paths[File::ResourcePath::Application] = getApplicationRoot(p.applicationPath);
            }

            File::Path documents;
            if (OS::getEnv("DJV_DOCUMENTS_PATH", env) && !env.empty())
            {
                documents = File::Path(env);
            }
            else
            {
                documents = File::getPath(File::DirectoryShortcut::Documents);
                try
                {
                    if (!File::Info(documents).doesExist())
                    {
                        mkdir(documents);
                    }
                    documents.append("DJV");
                    if (!File::Info(documents).doesExist())
                    {
                        mkdir(documents);
                    }
                }
                catch (const std::exception& e)
                {
                    //! \bug How should we handle this error?
                    std::cerr << "[ERROR] Cannot create the documents path: " << e.what() << std::endl;
                }
            }
            p.paths[File::ResourcePath::Documents] = documents;
            
            const std::string applicationName = File::Path(argv0).getBaseName();
            File::Path logFile = File::Path(documents, applicationName + ".log");
            p.paths[File::ResourcePath::LogFile] = logFile;

            File::Path settingsFile(documents, applicationName + ".json");
            p.paths[File::ResourcePath::SettingsFile] = settingsFile;

            File::Path testPath = p.paths[File::ResourcePath::Application];
            testPath.append("djvSystem.en.text");
            if (File::Info(testPath).doesExist())
            {
                // The resources are at the root (for example iOS).
                p.paths[File::ResourcePath::Audio]         = p.paths[File::ResourcePath::Application];
                p.paths[File::ResourcePath::Fonts]         = p.paths[File::ResourcePath::Application];
                p.paths[File::ResourcePath::Icons]         = p.paths[File::ResourcePath::Application];
                p.paths[File::ResourcePath::Images]        = p.paths[File::ResourcePath::Application];
                p.paths[File::ResourcePath::Models]        = p.paths[File::ResourcePath::Application];
                p.paths[File::ResourcePath::Shaders]       = p.paths[File::ResourcePath::Application];
                p.paths[File::ResourcePath::Text]          = p.paths[File::ResourcePath::Application];
                p.paths[File::ResourcePath::Color]         = p.paths[File::ResourcePath::Application];
                p.paths[File::ResourcePath::Documentation] = p.paths[File::ResourcePath::Application];
            }
            else
            {
                File::Path etc = File::Path(p.paths[File::ResourcePath::Application], "etc");
                p.paths[File::ResourcePath::Audio]         = File::Path(etc, "Audio");
                p.paths[File::ResourcePath::Fonts]         = File::Path(etc, "Fonts");
                p.paths[File::ResourcePath::Icons]         = File::Path(etc, "Icons");
                p.paths[File::ResourcePath::Images]        = File::Path(etc, "Images");
                p.paths[File::ResourcePath::Models]        = File::Path(etc, "Models");
                p.paths[File::ResourcePath::Shaders]       = File::Path(etc, "Shaders");
                p.paths[File::ResourcePath::Text]          = File::Path(etc, "Text");
                p.paths[File::ResourcePath::Color]         = File::Path(etc, "Color");
                File::Path docs = File::Path(p.paths[File::ResourcePath::Application], "docs");
                p.paths[File::ResourcePath::Documentation] = File::Path(docs, "documentation.html");
            }
        }

        ResourceSystem::ResourceSystem() :
            _p(new Private)
        {}

        ResourceSystem::~ResourceSystem()
        {}

        std::shared_ptr<ResourceSystem> ResourceSystem::create(const std::string& argv0, const std::shared_ptr<Context>& context)
        {
            auto out = context->getSystemT<ResourceSystem>();
            if (!out)
            {
                out = std::shared_ptr<ResourceSystem>(new ResourceSystem);
                out->_init(argv0, context);
            }
            return out;
        }

        File::Path ResourceSystem::getPath(File::ResourcePath value) const
        {
            DJV_PRIVATE_PTR();
            const auto i = p.paths.find(value);
            return i != p.paths.end() ? i->second : File::Path();
        }

    } // namespace System
} // namespace djv

