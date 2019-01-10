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
        class Application : public Core::Context
        {
            DJV_NON_COPYABLE(Application);

        protected:
            void _init(int & argc, char ** argv)
            {
                Context::_init(argc, argv);

                _avSystem = AV::AVSystem::create(this);
                _parseArgs();

                Core::Time::Duration duration = 0;
                if (auto io = getSystemT<AV::IO::System>().lock())
                {
                    _queue = AV::IO::Queue::create();
                    _read = io->read(argv[1], _queue);
                    auto info = _read->getInfo().get();
                    auto & video = info.video;
                    if (!video.size())
                    {
                        throw std::runtime_error(DJV_TEXT("djv::Convert", "Nothing to convert"));
                    }
                    auto & videoInfo = video[0];
                    if (_resize)
                    {
                        video[0].info.size = *_resize;
                    }
                    duration = videoInfo.duration;
                    _write = io->write(argv[2], info, _queue);
                }

                _statsTimer = Core::Time::Timer::create(this);
                _statsTimer->setRepeating(true);
                _statsTimer->start(
                    Core::Time::Timer::getMilliseconds(Core::Time::Timer::Value::Slow),
                    [this, duration](float)
                {
                    Core::Time::Timestamp timestamp = 0;
                    {
                        std::lock_guard<std::mutex> lock(_queue->getMutex());
                        if (_queue->hasVideo())
                        {
                            timestamp = _queue->getVideo().first;
                        }
                    }
                    if (timestamp && duration)
                    {
                        std::cout << (timestamp / static_cast<float>(duration) * 100.f) << "%" << std::endl;
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

            int run()
            {
                auto time = std::chrono::system_clock::now();
                while (_write->isRunning())
                {
                    const auto now = std::chrono::system_clock::now();
                    const std::chrono::duration<float> delta = now - time;
                    time = now;
                    const float dt = delta.count();
                    tick(dt);
                }
                return 0;
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
                            glm::ivec2 resize;
                            resize.x = std::stoi(*i);
                            i = args.erase(i);
                            resize.y = std::stoi(*i);
                            i = args.erase(i);
                            _resize.reset(new glm::ivec2(resize));
                        }
                        else
                        {
                            std::stringstream ss;
                            ss << DJV_TEXT("djv::Convert", "Cannot parse option '-resize'");
                            throw std::runtime_error(ss.str());
                        }
                    }
                    else
                    {
                        ++i;
                    }
                }
                if (args.size() != 3)
                {
                    throw std::runtime_error(DJV_TEXT("djv::Convert", "Usage: djv_convert (input) (output)"));
                }
                _input = args[1];
                _input = args[2];
            }

            std::string _input;
            std::string _output;
            std::unique_ptr<glm::ivec2> _resize;
            std::shared_ptr<AV::AVSystem> _avSystem;
            std::shared_ptr<AV::IO::Queue> _queue;
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
