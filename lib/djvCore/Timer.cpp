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

#include <djvCore/Timer.h>

#include <djvCore/Context.h>

#include <algorithm>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            size_t getValue(TimerValue value)
            {
                const std::vector<size_t> data =
                {
                    10000,
                    1000,
                    100,
                    10,
                    1
                };
                DJV_ASSERT(data.size() == static_cast<size_t>(TimerValue::Count));
                return data[static_cast<size_t>(value)];
            }

            Unit getTime(TimerValue value)
            {
                return Unit(std::chrono::duration_cast<Unit>(std::chrono::milliseconds(getValue(value))));
            }

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

            void Timer::setRepeating(bool value)
            {
                _repeating = value;
            }

            void Timer::start(
                const Unit& value,
                const std::function<void(const std::chrono::steady_clock::time_point&, const Unit&)> & callback)
            {
                _active   = true;
                _timeout  = value;
                _callback = callback;
                _start    = _time;
            }

            void Timer::stop()
            {
                _active = false;
            }

            void Timer::_tick(const std::chrono::steady_clock::time_point& t, const Unit& dt)
            {
                _time = t;
                if (_active)
                {
                    if (_time >= (_start + _timeout))
                    {
                        if (_callback)
                        {
                            const auto v = std::chrono::duration_cast<Unit>(_time - _start);
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
            }

            struct TimerSystem::Private
            {
                std::vector<std::weak_ptr<Timer> > timers;
                std::vector<std::weak_ptr<Timer> > newTimers;
            };

            void TimerSystem::_init(const std::shared_ptr<Context>& context)
            {
                ISystemBase::_init("djv::Core::TimerSystem", context);
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

            void TimerSystem::tick(const std::chrono::steady_clock::time_point& t, const Unit& dt)
            {
                DJV_PRIVATE_PTR();
                p.timers.insert(p.timers.end(), p.newTimers.begin(), p.newTimers.end());
                p.newTimers.clear();
                auto i = p.timers.begin();
                while (i != p.timers.end())
                {
                    if (auto timer = i->lock())
                    {
                        timer->_tick(t, dt);
                        ++i;
                    }
                    else
                    {
                        i = p.timers.erase(i);
                    }
                }
            }

            void TimerSystem::_addTimer(const std::weak_ptr<Timer> & value)
            {
                _p->newTimers.push_back(value);
            }

        } // namespace Time
    } // namespace Core
    
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Time,
        TimerValue,
        DJV_TEXT("timer_slow"),
        DJV_TEXT("timer_medium"),
        DJV_TEXT("timer_fast"),
        DJV_TEXT("timer_very_fast"));

} // namespace djv
