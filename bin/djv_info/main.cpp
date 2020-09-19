// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IOSystem.h>
#include <djvAV/TimeFunc.h>

#include <djvImage/ImageDataFunc.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfoFunc.h>
#include <djvSystem/PathFunc.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/ErrorFunc.h>
#include <djvCore/StringFormat.h>

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

                _textSystem = getSystemT<System::TextSystem>();

                _parseCmdLine(args);

                bool hasInputs = args.size();
                while (args.size())
                {
                    System::File::Info fileInfo(args.front());
                    if (fileInfo.doesExist())
                    {
                        _inputs.push_back(fileInfo);
                    }
                    else
                    {
                        auto textSystem = getSystemT<System::TextSystem>();
                        std::cout << Core::Error::format(Core::String::Format("{0}: {1}").
                            arg(fileInfo.getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_file_open")))) << std::endl;
                    }
                    args.pop_front();
                }
                if (!_inputs.size() && !hasInputs)
                {
                    _inputs.push_back(System::File::Info("."));
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
                auto io = getSystemT<AV::IO::IOSystem>();
                auto avSystem = getSystemT<AV::AVSystem>();
                for (const auto& i : _inputs)
                {
                    switch (i.getType())
                    {
                    case System::File::Type::File:
                    case System::File::Type::Sequence:
                        _print(i, io, avSystem);
                        break;
                    case System::File::Type::Directory:
                    {
                        std::cout << i.getPath() << ":" << std::endl;
                        System::File::DirectoryListOptions options;
                        options.sequences = true;
                        options.sequenceExtensions = io->getSequenceExtensions();
                        for (const auto& j : System::File::directoryList(i.getPath(), options))
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
                auto textSystem = getSystemT<System::TextSystem>();
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
            void _print(const System::File::Info& fileInfo, std::shared_ptr<AV::IO::IOSystem>& io, std::shared_ptr<AV::AVSystem>& avSystem)
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
                            const AV::Time::Units timeUnits = avSystem->observeTimeUnits()->get();
                            std::cout << "    Duration: " << AV::Time::toString(info.videoSequence.getFrameCount(), info.videoSpeed, timeUnits);
                            if (AV::Time::Units::Frames == timeUnits)
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

            std::shared_ptr<System::TextSystem> _textSystem;
            std::vector<System::File::Info> _inputs;
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
