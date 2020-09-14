// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/ErrorFunc.h>
#include <djvCore/FileInfoFunc.h>
#include <djvCore/PathFunc.h>
#include <djvCore/StringFormat.h>
#include <djvCore/TextSystem.h>
#include <djvCore/TimeFunc.h>

using namespace djv;

namespace djv
{
    //! This namespace provides functionality for djv_info.
    namespace info
    {
        class Application : public CmdLine::Application
        {
            DJV_NON_COPYABLE(Application);

        protected:
            void _init(std::list<std::string>& args)
            {
                CmdLine::Application::_init(args);

                _textSystem = getSystemT<Core::TextSystem>();

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
                        std::cout << Core::Error::format(Core::String::Format("{0}: {1}").
                            arg(fileInfo.getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_file_open")))) << std::endl;
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
            ~Application() override
            {}

            static std::shared_ptr<Application> create(std::list<std::string>& args)
            {
                auto out = std::shared_ptr<Application>(new Application);
                out->_init(args);
                return out;
            }

            void run() override
            {
                auto io = getSystemT<AV::IO::System>();
                auto avSystem = getSystemT<AV::AVSystem>();
                for (const auto& i : _inputs)
                {
                    switch (i.getType())
                    {
                    case Core::FileSystem::FileType::File:
                    case Core::FileSystem::FileType::Sequence:
                        _print(i, io, avSystem);
                        break;
                    case Core::FileSystem::FileType::Directory:
                    {
                        std::cout << i.getPath() << ":" << std::endl;
                        Core::FileSystem::DirectoryListOptions options;
                        options.sequences = true;
                        options.sequenceExtensions = io->getSequenceExtensions();
                        for (const auto& j : Core::FileSystem::directoryList(i.getPath(), options))
                        {
                            _print(j, io, avSystem);
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
                std::cout << " " << textSystem->getText(DJV_TEXT("djv_info_description")) << std::endl;
                std::cout << std::endl;
                std::cout << " " << textSystem->getText(DJV_TEXT("djv_info_usage")) << std::endl;
                std::cout << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_info_usage_format")) << std::endl;
                std::cout << std::endl;

                CmdLine::Application::_printUsage();
            }

        private:
            void _print(const Core::FileSystem::FileInfo& fileInfo, std::shared_ptr<AV::IO::System>& io, std::shared_ptr<AV::AVSystem>& avSystem)
            {
                if (io->canRead(fileInfo))
                {
                    try
                    {
                        const auto read = io->read(fileInfo);
                        const auto info = read->getInfo().get();
                        std::cout << fileInfo << std::endl;
                        std::cout.precision(2);
                        if (info.videoSequence.getFrameCount() > 1)
                        {
                            std::cout << "    Speed: " << info.videoSpeed.toFloat() << std::endl;
                            const Core::Time::Units timeUnits = avSystem->observeTimeUnits()->get();
                            std::cout << "    Duration: " << Core::Time::toString(info.videoSequence.getFrameCount(), info.videoSpeed, timeUnits);
                            if (Core::Time::Units::Frames == timeUnits)
                            {
                                std::cout << " " << "frames";
                            }
                            std::cout << std::endl;
                        }
                        for (const auto & video : info.video)
                        {
                            std::cout << "    " << video.name << std::endl;
                            std::cout << "        Size: " << video.size << " " << std::fixed << video.getAspectRatio() << std::endl;
                            std::stringstream ss;
                            ss << video.type;
                            std::cout << "        Type: " << _textSystem->getText(ss.str()) << std::endl;
                        }
                        if (info.audio.isValid())
                        {
                            std::cout << "    " << info.audio.name << std::endl;
                            std::cout << "        Channels: " << static_cast<int>(info.audio.channelCount) << std::endl;
                            std::stringstream ss;
                            ss << info.audio.type;
                            std::cout << "        Type: " << _textSystem->getText(ss.str()) << std::endl;
                            std::cout << "        Sample rate: " << info.audio.sampleRate << std::endl;
                            std::cout << "        Duration: " << (info.audio.sampleRate > 0 ? (info.audio.sampleCount / static_cast<float>(info.audio.sampleRate)) : 0.F) << " seconds" << std::endl;
                        }
                    }
                    catch (const std::exception & e)
                    {
                        std::cout << Core::Error::format(e) << std::endl;
                    }
                }
            }

            std::shared_ptr<Core::TextSystem> _textSystem;
            std::vector<Core::FileSystem::FileInfo> _inputs;
        };

    } // namespace info
} // namespace djv

DJV_MAIN()
{
    int r = 1;
    try
    {
        auto args = info::Application::args(argc, argv);
        auto app = info::Application::create(args);
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
