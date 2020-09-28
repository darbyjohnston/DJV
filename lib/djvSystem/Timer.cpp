// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/Timer.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        void Timer::_init(const std::shared_ptr<Context>& context)
        {
            if (auto system = context->getSystemT<TimerSystem>())
            {
                system->_addTimer(shared_from_this());
            }
        }

        std::shared_ptr<Timer> Timer::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<Timer>(new Timer);
            out->_init(context);
            return out;
        }
        
        Timer::Timer()
        {}

        void Timer::start(
            const Time::Duration& value,
            const std::function<void(const std::chrono::steady_clock::time_point&, const Time::Duration&)>& callback)
        {
            _active   = true;
            _timeout  = value;
            _callback = callback;
            _start    = std::chrono::steady_clock::now();
        }

        void Timer::_tick()
        {
            _time = std::chrono::steady_clock::now();
            if (_time >= (_start + _timeout))
            {
                if (_callback)
                {
                    const auto v = std::chrono::duration_cast<Time::Duration>(_time - _start);
                    _callback(_time, v);
                }
                if (_repeating)
                {
                    _start = _time;
                }
                else
                {
                    _active = false;
                }
            }
        }

        struct TimerSystem::Private
        {
            std::vector<std::weak_ptr<Timer> > timers;
            std::vector<std::weak_ptr<Timer> > newTimers;
        };

        void TimerSystem::_init(const std::shared_ptr<Context>& context)
        {
            ISystemBase::_init("djv::System::TimerSystem", context);
        }

        TimerSystem::TimerSystem() :
            _p(new Private)
        {}

        TimerSystem::~TimerSystem()
        {}

        std::shared_ptr<TimerSystem> TimerSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = context->getSystemT<TimerSystem>();
            if (!out)
            {
                out = std::shared_ptr<TimerSystem>(new TimerSystem);
                out->_init(context);
            }
            return out;
        }

        void TimerSystem::tick()
        {
            DJV_PRIVATE_PTR();
            p.timers.insert(p.timers.end(), p.newTimers.begin(), p.newTimers.end());
            p.newTimers.clear();
            auto i = p.timers.begin();
            while (i != p.timers.end())
            {
                if (auto timer = i->lock())
                {
                    if (timer->isActive())
                    {
                        timer->_tick();
                    }
                    ++i;
                }
                else
                {
                    i = p.timers.erase(i);
                }
            }
        }

        void TimerSystem::_addTimer(const std::weak_ptr<Timer>& value)
        {
            _p->newTimers.push_back(value);
        }

    } // namespace System
} // namespace djv

