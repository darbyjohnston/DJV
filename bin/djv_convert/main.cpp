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

#include <djvCmdLineApp/Application.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IO.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
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
            void _init(int & argc, char ** argv)
            {
                std::vector<std::string> args;
                for (int i = 0; i < argc; ++i)
                {
                    args.push_back(argv[i]);
                }
                CmdLine::Application::_init(args);

                if (!_parseArgs())
                {
                    exit(1);
                    return;
                }

                auto io = getSystemT<AV::IO::System>();
                Core::FileSystem::FileInfo readFileInfo(argv[1]);
                if (_readSeq)
                {
                    readFileInfo.evalSequence();
                }
                AV::IO::ReadOptions readOptions;
                readOptions.videoQueueSize = _readQueueSize;
                _read = io->read(readFileInfo, readOptions);
                _read->setThreadCount(_readThreadCount);
                auto info = _read->getInfo().get();
                auto & video = info.video;
                if (!video.size())
                {
                    throw std::invalid_argument(DJV_TEXT("Nothing to convert"));
                }
                auto & videoInfo = video[0];
                if (_resize)
                {
                    video[0].info.size = *_resize;
                }
                const size_t size = videoInfo.sequence.getSize();
                Core::FileSystem::FileInfo writeFileInfo(argv[2]);
                if (_writeSeq)
                {
                    writeFileInfo.evalSequence();
                }
                AV::IO::WriteOptions writeOptions;
                writeOptions.videoQueueSize = _writeQueueSize;
                _write = io->write(writeFileInfo, info, writeOptions);
                _write->setThreadCount(_writeThreadCount);
                
                _statsTimer = Core::Time::Timer::create(shared_from_this());
                _statsTimer->setRepeating(true);
                _statsTimer->start(
                    Core::Time::getTime(Core::Time::TimerValue::Slow),
                    [this, size](const std::chrono::steady_clock::time_point&, const Core::Time::Unit&)
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
            }

            Application()
            {}

        public:
            static std::shared_ptr<Application> create(int & argc, char ** argv)
            {
                auto out = std::shared_ptr<Application>(new Application);
                out->_init(argc, argv);
                return out;
            }

            void tick(const std::chrono::steady_clock::time_point& t, const Core::Time::Unit& dt) override
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

        private:
            bool _parseArgs()
            {
                bool out = true;
                auto args = getArgs();
                auto i = args.begin();
                while (i != args.end())
                {
                    if ("-h" == *i || "-help" == *i)
                    {
                        out = false;
                        _printUsage();
                        break;
                    }
                    else if ("-resize" == *i)
                    {
                        i = args.erase(i);
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
                        int value = 0;
                        std::stringstream ss(*i);
                        ss >> value;
                        i = args.erase(i);
                        _readQueueSize = std::max(value, 1);
                    }
                    else if ("-writeQueue" == *i)
                    {
                        i = args.erase(i);
                        int value = 0;
                        std::stringstream ss(*i);
                        ss >> value;
                        i = args.erase(i);
                        _writeQueueSize = std::max(value, 1);
                    }
                    else if ("-readThreads" == *i)
                    {
                        i = args.erase(i);
                        int value = 0;
                        std::stringstream ss(*i);
                        ss >> value;
                        i = args.erase(i);
                        _readThreadCount = std::max(value, 1);
                    }
                    else if ("-writeThreads" == *i)
                    {
                        i = args.erase(i);
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
                if (3 == args.size())
                {
                    _input = args[1];
                    _output = args[2];
                }
                else
                {
                    out = false;
                    _printUsage();
                }
                return out;
            }
            
            void _printUsage()
            {
                std::cout << std::endl;
                std::cout << DJV_TEXT(" Usage:") << std::endl;
                std::cout << std::endl;
                std::cout << DJV_TEXT("   djv_convert (input) (output) [option, ...]") << std::endl;
                std::cout << std::endl;
                std::cout << DJV_TEXT(" Options:") << std::endl;
                std::cout << std::endl;
                std::cout << DJV_TEXT("   -resize \"(width) (height)\"") << std::endl;
                std::cout << DJV_TEXT("   Resize the image.") << std::endl;
                std::cout << std::endl;
                std::cout << DJV_TEXT("   -readSeq") << std::endl;
                std::cout << DJV_TEXT("   Interpret the input file name as a sequence.") << std::endl;
                std::cout << std::endl;
                std::cout << DJV_TEXT("   -writeSeq") << std::endl;
                std::cout << DJV_TEXT("   Interpret the output file name as a sequence.") << std::endl;
                std::cout << std::endl;
                std::cout << DJV_TEXT("   -readQueue (value)") << std::endl;
                std::cout << DJV_TEXT("   Set the size of the read queue.") << std::endl;
                std::cout << std::endl;
                std::cout << DJV_TEXT("   -writeQueue (value)") << std::endl;
                std::cout << DJV_TEXT("   Set the size of the write queue.") << std::endl;
                std::cout << std::endl;
                std::cout << DJV_TEXT("   -readThreads (value)") << std::endl;
                std::cout << DJV_TEXT("   Set the number of threads for reading.") << std::endl;
                std::cout << std::endl;
                std::cout << DJV_TEXT("   -writeThreads (value)") << std::endl;
                std::cout << DJV_TEXT("   Set the number of threads for writing.") << std::endl;
                std::cout << std::endl;
            }

            std::string _input;
            std::string _output;
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
    int r = 0;
    try
    {
        return convert::Application::create(argc, argv)->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
