// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvAV/IOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/ErrorFunc.h>
#include <djvCore/FileInfoFunc.h>
#include <djvCore/PathFunc.h>
#include <djvCore/StringFormat.h>
#include <djvCore/TextSystem.h>

using namespace djv;

namespace djv
{
    //! This namespace provides functionality for djv_ls.
    namespace ls
    {
        class Application : public CmdLine::Application
        {
            DJV_NON_COPYABLE(Application);

        protected:
            void _init(std::list<std::string>& args)
            {
                CmdLine::Application::_init(args);

                _parseCmdLine(args);

                bool hasInputs = args.size();
                while (args.size())
                {
                    Core::FileSystem::FileInfo fileInfo(args.front());
                    if (fileInfo.doesExist())
                    {
                        _inputs.push_back(fileInfo);
                    }
                    else
                    {
                        auto textSystem = getSystemT<Core::TextSystem>();
                        const std::string s = Core::String::Format("{0}: {1}").
                            arg(fileInfo.getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_file_open")));
                        std::cout << Core::Error::format(s) << std::endl;
                    }
                    args.pop_front();
                }
                if (!_inputs.size() && !hasInputs)
                {
                    _inputs.push_back(Core::FileSystem::FileInfo("."));
                }
            }

            Application()
            {}

        public:
            static std::shared_ptr<Application> create(std::list<std::string>& args)
            {
                auto out = std::shared_ptr<Application>(new Application);
                out->_init(args);
                return out;
            }

            void run() override
            {
                auto io = getSystemT<AV::IO::System>();
                for (const auto& i : _inputs)
                {
                    switch (i.getType())
                    {
                    case Core::FileSystem::FileType::File:
                        _print(std::string(i));
                        break;
                    case Core::FileSystem::FileType::Directory:
                    {
                        std::cout << i.getPath() << ":" << std::endl;
                        Core::FileSystem::DirectoryListOptions options;
                        options.sequences = true;
                        options.sequenceExtensions = io->getSequenceExtensions();
                        for (const auto& j : Core::FileSystem::directoryList(i.getPath(), options))
                        {
                            _print(j.getFileName(Core::Frame::invalid, false));
                        }
                        break;
                    }
                    default: break;
                    }
                }
            }

        protected:
            void _printUsage() override
            {
                auto textSystem = getSystemT<Core::TextSystem>();
                std::cout << std::endl;
                std::cout << " " << textSystem->getText(DJV_TEXT("djv_ls_description")) << std::endl;
                std::cout << std::endl;
                std::cout << " " << textSystem->getText(DJV_TEXT("djv_ls_usage")) << std::endl;
                std::cout << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_ls_usage_format")) << std::endl;
                std::cout << std::endl;

                CmdLine::Application::_printUsage();
            }

            std::vector<Core::FileSystem::FileInfo> _inputs;

        private:
            void _print(const std::string & fileName)
            {
                std::cout << fileName << std::endl;
            }
        };

    } // namespace ls
} // namespace djv

DJV_MAIN()
{
    int r = 1;
    try
    {
        auto args = ls::Application::args(argc, argv);
        auto app = ls::Application::create(args);
        if (0 == app->getExitCode())
        {
            app->run();
        }
        r = app->getExitCode();
    }
    catch (const std::exception & error)
    {
        std::cout << Core::Error::format(error) << std::endl;
    }
    return r;
}
