//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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
            Application()
            {}

        public:
            static std::shared_ptr<Application> create(const std::string& name)
            {
                auto out = std::shared_ptr<Application>(new Application);
                out->_init(name);
                return out;
            }

            void printUsage() override
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
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_help")) << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_convert_option_help_description")) << std::endl;
                std::cout << std::endl;

                CmdLine::Application::printUsage();
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

                CmdLine::Application::run();
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

          protected:
              void _parseArgs(std::list<std::string>& args) override
              {
                  CmdLine::Application::_parseArgs(args);
                  if (0 == getExitCode())
                  {
                      auto i = args.begin();
                      while (i != args.end())
                      {
                          if ("-resize" == *i)
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

                      if (!args.size())
                      {
                          std::stringstream ss;
                          auto textSystem = getSystemT<Core::TextSystem>();
                          ss << textSystem->getText(DJV_TEXT("djv_convert_input_error"));
                          throw std::runtime_error(ss.str());
                      }
                      _input = args.front();
                      args.pop_front();
                      
                      if (!args.size())
                      {
                          std::stringstream ss;
                          auto textSystem = getSystemT<Core::TextSystem>();
                          ss << textSystem->getText(DJV_TEXT("djv_convert_output_error"));
                          throw std::runtime_error(ss.str());
                      }
                      _output = args.front();
                      args.pop_front();
                  }
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
        auto app = convert::Application::create(argv[0]);
        app->parseArgs(argc, argv);
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
