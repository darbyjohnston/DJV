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

#include <djvCore/Timer.h>

#include <djvCore/Context.h>

#include <algorithm>

namespace djv
{
    namespace Core
    {
        struct Timer::Private
        {
            bool repeating = false;
            bool active = false;
            std::chrono::milliseconds timeout;
            std::function<void(float)> callback;
            std::chrono::time_point<std::chrono::system_clock> start;
        };

        void Timer::_init(const std::shared_ptr<Context>& context)
        {
            context->getSystemT<TimerSystem>()->_addTimer(shared_from_this());
        }

        Timer::Timer() :
            _p(new Private)
        {}

        Timer::~Timer()
        {}

        std::shared_ptr<Timer> Timer::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<Timer>(new Timer);
            out->_init(context);
            return out;
        }

        bool Timer::isRepeating() const
        {
            return _p->repeating;
        }

        void Timer::setRepeating(bool value)
        {
            _p->repeating = value;
        }

        bool Timer::isActive() const
        {
            return _p->active;
        }

        void Timer::start(std::chrono::milliseconds value, const std::function<void(float)>& callback)
        {
            _p->active   = true;
            _p->timeout  = value;
            _p->callback = callback;
            _p->start    = std::chrono::system_clock::now();
        }

        void Timer::stop()
        {
            _p->active = false;
        }

        void Timer::_tick(float dt)
        {
            if (_p->active)
            {
                const auto now = std::chrono::system_clock::now();
                if (now >= (_p->start + _p->timeout))
                {
                    if (_p->callback)
                    {
                        const std::chrono::duration<float> delta = now - _p->start;
                        _p->callback(delta.count());
                    }
                    if (_p->repeating)
                    {
                        _p->start = now;
                    }
                    else
                    {
                        _p->active = false;
                    }
                }
            }
        }

        struct TimerSystem::Private
        {
            std::vector<std::weak_ptr<Timer> > timers;
        };

        void TimerSystem::_init(const std::shared_ptr<Context>& context)
        {
            ISystem::_init("djv::Core::TimerSystem", context);
        }
        
        TimerSystem::TimerSystem() :
            _p(new Private)
        {}

        TimerSystem::~TimerSystem()
        {}

        std::shared_ptr<TimerSystem> TimerSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TimerSystem>(new TimerSystem);
            out->_init(context);
            return out;
        }

        void TimerSystem::_tick(float dt)
        {
            std::vector<std::weak_ptr<Timer> > zombies;
            auto timers = _p->timers;
            for (const auto& t : timers)
            {
                if (auto timer = t.lock())
                {
                    timer->_tick(dt);
                }
                else
                {
                    zombies.push_back(t);
                }
            }
            for (const auto& zombie : zombies)
            {
                const auto i = std::find_if(
                    _p->timers.begin(),
                    _p->timers.end(),
                    [zombie](const std::weak_ptr<Timer>& other)
                {
                    return zombie.lock() == other.lock();
                });
                if (i != _p->timers.end())
                {
                    _p->timers.erase(i);
                }
            }
        }

        void TimerSystem::_exit()
        {
            ISystem::_exit();
            _p->timers.clear();
        }

        void TimerSystem::_addTimer(const std::weak_ptr<Timer>& value)
        {
            _p->timers.push_back(value);
        }

    } // namespace Core
} // namespace djv
