// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Enum.h>
#include <djvCore/ISystem.h>
#include <djvCore/Time.h>

#include <chrono>
#include <functional>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            class TimerSystem;

            //! This enumeration provides common timer values.
            enum class TimerValue
            {
                VerySlow,
                Slow,
                Medium,
                Fast,
                VeryFast,

                Count,
                First = VerySlow
            };
            DJV_ENUM_HELPERS(TimerValue);
            size_t getValue(TimerValue);
            Duration getTime(TimerValue);

            //! This class provides a timer.
            class Timer : public std::enable_shared_from_this<Timer>
            {
                DJV_NON_COPYABLE(Timer);
                void _init(const std::shared_ptr<Context>&);
                Timer();

            public:
                //! Create a new time.
                static std::shared_ptr<Timer> create(const std::shared_ptr<Context>&);

                //! \name Timer Options
                ///@{

                bool isRepeating() const;
                void setRepeating(bool);

                ///@}

                //! Is the timer active?
                bool isActive() const;

                //! Start the timer.
                void start(
                    const Duration&,
                    const std::function<void(const std::chrono::steady_clock::time_point&, const Duration&)>&);

                //! Stop the timer.
                void stop();

            private:
                void _tick();

                bool _repeating = false;
                bool _active = false;
                Duration _timeout = Duration::zero();
                std::function<void(const std::chrono::steady_clock::time_point&, const Duration&)> _callback;
                Time::TimePoint _time;
                Time::TimePoint _start;

                friend class TimerSystem;
            };

            //! This class provides the system that manages timers.
            class TimerSystem : public ISystemBase
            {
                DJV_NON_COPYABLE(TimerSystem);
                void _init(const std::shared_ptr<Context>&);
                TimerSystem();

            public:
                ~TimerSystem() override;

                //! Create a new timer system.
                static std::shared_ptr<TimerSystem> create(const std::shared_ptr<Context>&);

                void tick() override;

            private:
                void _addTimer(const std::weak_ptr<Timer>&);

                DJV_PRIVATE();

                friend class Timer;
            };

        } // namespace Time
    } // namespace Core
    
    DJV_ENUM_SERIALIZE_HELPERS(Core::Time::TimerValue);
    
} // namespace djv

