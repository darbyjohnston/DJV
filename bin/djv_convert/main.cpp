// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IO.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
#include <djvCore/StringFormat.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Timer.h>
#include <djvCore/Vector.h>

using namespace djv;

namespace djv
{
    //! This namespace provides functionality for djv_convert.
    namespace convert
    {
        class Application : public CmdLine::Application
        {
            DJV_NON_COPYABLE(Application);

        protected:
            void _init(std::list<std::string>& args)
            {
                CmdLine::Application::_init(args);

                _parseCmdLine(args);
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
                if (_readSeq)
                {
                    _input.evalSequence();
                }
                AV::IO::ReadOptions readOptions;
                readOptions.videoQueueSize = _readQueueSize;
                _read = io->read(_input, readOptions);
                _read->setThreadCount(_readThreadCount);
                auto info = _read->getInfo().get();
                auto& video = info.video;
                auto textSystem = getSystemT<Core::TextSystem>();
                if (!video.size())
                {
                    throw std::invalid_argument(textSystem->getText(DJV_TEXT("djv_convert_nothing_convert")));
                }
                auto& videoInfo = video[0];
                if (_resize)
                {
                    video[0].info.size = *_resize;
                }
                const size_t size = videoInfo.sequence.getSize();
                if (_writeSeq)
                {
                    _output.evalSequence();
                }
                AV::IO::WriteOptions writeOptions;
                writeOptions.videoQueueSize = _writeQueueSize;
                _write = io->write(_output, info, writeOptions);
                _write->setThreadCount(_writeThreadCount);

                _statsTimer = Core::Time::Timer::create(shared_from_this());
                _statsTimer->setRepeating(true);
                _statsTimer->start(
                    Core::Time::getTime(Core::Time::TimerValue::Slow),
                    [this, size](const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)
                    {
                        Core::Frame::Number frame = 0;
                        {
                            std::lock_guard<std::mutex> lock(_read->getMutex());
                            auto& queue = _read->getVideoQueue();
                            if (!queue.isEmpty())
                            {
                                frame = queue.getFrame().frame;
                            }
                        }
                        if (frame && size)
                        {
                            std::cout << static_cast<size_t>(frame / static_cast<float>(size - 1) * 100.F) << "%" << std::endl;
                        }
                    });

                CmdLine::Application::run();
            }

            void tick(const std::chrono::steady_clock::time_point& t, const Core::Time::Duration& dt) override
            {
                CmdLine::Application::tick(t, dt);
                if (_read && _write)
                {
                    std::unique_lock<std::mutex> readLock(_read->getMutex(), std::try_to_lock);
                    if (readLock.owns_lock())
                    {
                        std::lock_guard<std::mutex> writeLock(_write->getMutex());
                        auto& readQueue = _read->getVideoQueue();
                        auto& writeQueue = _write->getVideoQueue();
                        if (!readQueue.isEmpty() && writeQueue.getCount() < writeQueue.getMax())
                        {
                            auto frame = readQueue.popFrame();
                            writeQueue.addFrame(frame);
                        } 
                        else if (readQueue.isFinished())
                        {
                            writeQueue.setFinished(true);
                        }
                    }
                }
                if (_write && !_write->isRunning())
                {
                    exit(0);
                }
            }

        protected:
            void _parseCmdLine(std::list<std::string>& args) override
            {
                CmdLine::Application::_parseCmdLine(args);
                if (0 == getExitCode())
                {
                    auto textSystem = getSystemT<Core::TextSystem>();
                    auto i = args.begin();
                    while (i != args.end())
                    {
                        if ("-resize" == *i)
                        {
                            i = args.erase(i);
                            if (args.end() == i)
                            {
                                throw std::runtime_error(Core::String::Format("{0}: {1}").
                                    arg("-resize").
                                    arg(textSystem->getText(DJV_TEXT("error_cannot_parse_argument"))));
                            }
                            AV::Image::Size resize;
                            std::stringstream ss(*i);
                            ss >> resize;
                            i = args.erase(i);
                            _resize.reset(new AV::Image::Size(resize));
                        }
                        else if ("-readSeq" == *i)
                        {
                            i = args.erase(i);
                            _readSeq = true;
                        }
                        else if ("-writeSeq" == *i)
                        {
                            i = args.erase(i);
                            _writeSeq = true;
                        }
                        else if ("-readQueue" == *i)
                        {
                            i = args.erase(i);
                            if (args.end() == i)
                            {
                                throw std::runtime_error(Core::String::Format("{0}: {1}").
                                    arg("-readQueue").
                                    arg(textSystem->getText(DJV_TEXT("error_cannot_parse_argument"))));
                            }
                            int value = 0;
                            std::stringstream ss(*i);
                            ss >> value;
                            i = args.erase(i);
                            _readQueueSize = std::max(value, 1);
                        }
                        else if ("-writeQueue" == *i)
                        {
                            i = args.erase(i);
                            if (args.end() == i)
                            {
                                throw std::runtime_error(Core::String::Format("{0}: {1}").
                                    arg("-writeQueue").
                                    arg(textSystem->getText(DJV_TEXT("error_cannot_parse_argument"))));
                            }
                            int value = 0;
                            std::stringstream ss(*i);
                            ss >> value;
                            i = args.erase(i);
                            _writeQueueSize = std::max(value, 1);
                        }
                        else if ("-readThreads" == *i)
                        {
                            i = args.erase(i);
                            if (args.end() == i)
                            {
                                throw std::runtime_error(Core::String::Format("{0}: {1}").
                                    arg("-readThreads").
                                    arg(textSystem->getText(DJV_TEXT("error_cannot_parse_argument"))));
                            }
                            int value = 0;
                            std::stringstream ss(*i);
                            ss >> value;
                            i = args.erase(i);
                            _readThreadCount = std::max(value, 1);
                        }
                        else if ("-writeThreads" == *i)
                        {
                            i = args.erase(i);
                            if (args.end() == i)
                            {
                                throw std::runtime_error(Core::String::Format("{0}: {1}").
                                    arg("-writeThreads").
                                    arg(textSystem->getText(DJV_TEXT("error_cannot_parse_argument"))));
                            }
                            int value = 0;
                            std::stringstream ss(*i);
                            ss >> value;
                            i = args.erase(i);
                            _writeThreadCount = std::max(value, 1);
                        }
                        else
                        {
                            ++i;
                        }
                    }

                    if (!args.size())
                    {
                        _printUsage();
                        exit(1);
                    }
                    else if (2 == args.size())
                    {
                        _input = args.front();
                        args.pop_front();
                        _output = args.front();
                        args.pop_front();
                    }
                    else
                    {
                        throw std::runtime_error(textSystem->getText(DJV_TEXT("djv_convert_output_error")));
                    }
                }
            }

            void _printUsage() override
            {
                auto textSystem = getSystemT<Core::TextSystem>();
                std::cout << std::endl;
                std::cout << " " << textSystem->getText(DJV_TEXT("djv_convert_description")) << std::endl;
                std::cout << std::endl;
                std::cout << " " << textSystem->getText(DJV_TEXT("djv_convert_usage")) << std::endl;
                std::cout << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_usage_format")) << std::endl;
                std::cout << std::endl;
                std::cout << " " << textSystem->getText(DJV_TEXT("djv_convert_options")) << std::endl;
                std::cout << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_resize")) << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_resize_description")) << std::endl;
                std::cout << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_readseq")) << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_readseq_description")) << std::endl;
                std::cout << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_writeseq")) << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_writeseq_description")) << std::endl;
                std::cout << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_readqueue")) << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_readqueue_description")) << std::endl;
                std::cout << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_writequeue")) << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_writequeue_description")) << std::endl;
                std::cout << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_readthreads")) << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_readthreads_description")) << std::endl;
                std::cout << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_writethreads")) << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_writethreads_description")) << std::endl;
                std::cout << std::endl;

                CmdLine::Application::_printUsage();
            }

        private:
            Core::FileSystem::FileInfo _input;
            Core::FileSystem::FileInfo _output;
            std::unique_ptr<AV::Image::Size> _resize;
            bool _readSeq = false;
            bool _writeSeq = false;
            //! \todo What's a good default for this?
            size_t _readQueueSize = 10;
            size_t _writeQueueSize = 10;
            size_t _readThreadCount = 4;
            size_t _writeThreadCount = 4;
            std::shared_ptr<AV::IO::IRead> _read;
            std::shared_ptr<Core::Time::Timer> _statsTimer;
            std::shared_ptr<AV::IO::IWrite> _write;
        };

    } // namespace convert
} // namespace djv

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        auto args = convert::Application::args(argc, argv);
        auto app = convert::Application::create(args);
        if (0 == app->getExitCode())
        {
            app->run();
        }
        r = app->getExitCode();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
