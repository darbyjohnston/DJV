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

#include <djvAV/IO.h>
#include <djvAV/System.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Timer.h>
#include <djvCore/Vector.h>

#include <QGuiApplication>

using namespace djv;

class Context : public Core::Context
{
    DJV_NON_COPYABLE(Context);

protected:
    void _init(int & argc, char ** argv)
    {
        Core::Context::_init(argc, argv);

        if (argc != 3)
        {
            throw std::runtime_error(DJV_TEXT("Usage: djv_convert (input) (output)"));
        }

        auto system = AV::System::create(shared_from_this());
        auto io = getSystemT<AV::IO::System>();
        const auto locale = getSystemT<Core::TextSystem>()->getCurrentLocale();

        _queue = AV::IO::Queue::create();
        _read = io->read(argv[1], _queue);
        const auto info = _read->getInfo().get();
        AV::Duration duration = 0;
        const auto & video = info.getVideo();
        if (video.size())
        {
            duration = video[0].getDuration();
        }

        _statsTimer = Core::Timer::create(shared_from_this());
        _statsTimer->setRepeating(true);
        _statsTimer->start(
            Core::Timer::getMilliseconds(Core::Timer::Value::Slow),
            [this, duration](float)
        {
            AV::Timestamp timestamp = 0;
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

        _write = io->write(argv[2], info, _queue);
    }

    Context()
    {}

public:
    static std::shared_ptr<Context> create(int & argc, char ** argv)
    {
        auto out = std::shared_ptr<Context>(new Context);
        out->_init(argc, argv);
        return out;
    }

    bool isRunning() const { return _write->isRunning(); }
    
private:
    std::shared_ptr<AV::IO::Queue> _queue;    
    std::shared_ptr<AV::IO::IRead> _read;
    std::shared_ptr<Core::Timer> _statsTimer;
    std::shared_ptr<AV::IO::IWrite> _write;   
};

class Application : public QGuiApplication
{
public:
    Application(int & argc, char ** argv) :
        QGuiApplication(argc, argv),
        _context(Context::create(argc, argv))
    {
        _time = std::chrono::system_clock::now();
        _timer = startTimer(Core::Timer::getValue(Core::Timer::Value::Fast), Qt::PreciseTimer);
    }
    ~Application()
    {
        _context->exit();
    }
    
protected:
    void timerEvent(QTimerEvent *) override
    {
        if (_context->isRunning())
        {
            const auto now = std::chrono::system_clock::now();
            const std::chrono::duration<float> delta = now - _time;
            _time = now;
            const float dt = delta.count();
            _context->tick(dt);
        }
        else
        {
            exit();
        }
    }
    
private:
    std::shared_ptr<Context> _context;
    std::chrono::time_point<std::chrono::system_clock> _time;
    int _timer = 0;
};

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        return Application(argc, argv).exec();
    }
    catch (const std::exception & error)
    {
        std::cout << Core::format(error) << std::endl;
    }
    return r;
}
