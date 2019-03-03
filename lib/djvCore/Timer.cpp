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

#include <djvCore/Timer.h>

#include <djvCore/Context.h>

#include <algorithm>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            void Timer::_init(Context * context)
            {
                if (auto system = context->getSystemT<TimerSystem>().lock())
                {
                    system->_addTimer(shared_from_this());
                }
            }

            std::shared_ptr<Timer> Timer::create(Context * context)
            {
                auto out = std::shared_ptr<Timer>(new Timer);
                out->_init(context);
                return out;
            }

            size_t Timer::getValue(Value value)
            {
                static const std::vector<size_t> data =
                {
                    10000,
                    1000,
                    100,
                    10,
                    1
                };
                DJV_ASSERT(data.size() == static_cast<size_t>(Value::Count));
                return data[static_cast<size_t>(value)];
            }

            std::chrono::milliseconds Timer::getMilliseconds(Value value)
            {
                return std::chrono::milliseconds(getValue(value));
            }

            void Timer::setRepeating(bool value)
            {
                _repeating = value;
            }

            void Timer::start(std::chrono::milliseconds value, const std::function<void(float)>& callback)
            {
                _active = true;
                _timeout = value;
                _callback = callback;
                _start = std::chrono::system_clock::now();
            }

            void Timer::stop()
            {
                _active = false;
            }

            void Timer::_tick(float dt)
            {
                if (_active)
                {
                    const auto now = std::chrono::system_clock::now();
                    if (now >= (_start + _timeout))
                    {
                        if (_callback)
                        {
                            const std::chrono::duration<float> delta = now - _start;
                            _callback(delta.count());
                        }
                        if (_repeating)
                        {
                            _start = now;
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
            };

            void TimerSystem::_init(Context * context)
            {
                ISystem::_init("djv::Core::TimerSystem", context);
            }

            TimerSystem::TimerSystem() :
                _p(new Private)
            {}

            TimerSystem::~TimerSystem()
            {}

            std::shared_ptr<TimerSystem> TimerSystem::create(Context * context)
            {
                auto out = std::shared_ptr<TimerSystem>(new TimerSystem);
                out->_init(context);
                return out;
            }

            void TimerSystem::tick(float dt)
            {
                DJV_PRIVATE_PTR();
                auto i = p.timers.begin();
                while (i != p.timers.end())
                {
                    if (auto timer = i->lock())
                    {
                        timer->_tick(dt);
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
                _p->timers.push_back(value);
            }

        } // namespace Time
    } // namespace Core
    
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Time::Timer,
        Value,
        DJV_TEXT("Slow"),
        DJV_TEXT("Medium"),
        DJV_TEXT("Fast"),
        DJV_TEXT("VeryFast"));

} // namespace djv
