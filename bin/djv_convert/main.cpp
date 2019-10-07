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
            void _init(int & argc, char ** argv)
            {
                std::vector<std::string> args;
                for (int i = 0; i < argc; ++i)
                {
                    args.push_back(argv[i]);
                }
                CmdLine::Application::_init(args);

                auto testSystem = getSystemT<Core::TextSystem>();
                const auto locale = testSystem->getCurrentLocale();

                _parseArgs();

                auto io = getSystemT<AV::IO::System>();
                AV::IO::ReadOptions readOptions;
                //! \todo What's a good default for this?
                readOptions.videoQueueSize = 10;
                _read = io->read(std::string(argv[1]), readOptions);
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
                _write = io->write(std::string(argv[2]), info);

                _statsTimer = Core::Time::Timer::create(shared_from_this());
                _statsTimer->setRepeating(true);
                _statsTimer->start(
                    Core::Time::getMilliseconds(Core::Time::TimerValue::Slow),
                    [this, size](float)
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
                        std::cout << (frame / static_cast<float>(size - 1) * 100.F) << "%" << std::endl;
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

            void tick(float dt) override
            {
                CmdLine::Application::tick(dt);
                while (_write->isRunning())
                {
                    bool sleep = false;
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
                            else
                            {
                                sleep = true;
                            }
                        }
                        else
                        {
                            sleep = true;
                        }
                    }
                    if (sleep)
                    {
                        std::this_thread::sleep_for(Core::Time::getMilliseconds(Core::Time::TimerValue::Fast));
                    }
                }
                exit();
            }

        private:
            void _parseArgs()
            {
                auto args = getArgs();
                auto i = args.begin();
                while (i != args.end())
                {
                    if ("-resize" == *i)
                    {
                        i = args.erase(i);
                        if (args.size() >= 2)
                        {
                            AV::Image::Size resize;
                            resize.w = std::stoi(*i);
                            i = args.erase(i);
                            resize.h = std::stoi(*i);
                            i = args.erase(i);
                            _resize.reset(new AV::Image::Size(resize));
                        }
                        else
                        {
                            std::stringstream ss;
                            ss << DJV_TEXT("Cannot parse the option '-resize'");
                            throw std::invalid_argument(ss.str());
                        }
                    }
                    else
                    {
                        ++i;
                    }
                }
                if (args.size() != 3)
                {
                    throw std::invalid_argument(DJV_TEXT("Usage: djv_convert (input) (output)"));
                }
                _input = args[1];
                _input = args[2];
            }

            std::string _input;
            std::string _output;
            std::unique_ptr<AV::Image::Size> _resize;
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
