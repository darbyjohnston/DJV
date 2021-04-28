// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/ISystem.h>

#include <djvCore/Enum.h>
#include <djvCore/Time.h>

#include <chrono>
#include <functional>

namespace djv
{
    namespace System
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

        //! This class provides a timer.
        class Timer : public std::enable_shared_from_this<Timer>
        {
            DJV_NON_COPYABLE(Timer);
            void _init(const std::shared_ptr<Context>&);
            Timer();

        public:
            //! Create a new timer.
            static std::shared_ptr<Timer> create(const std::shared_ptr<Context>&);

            //! \name Options
            ///@{

            bool isRepeating() const;
            
            void setRepeating(bool);

            ///@}

            //! \name Active
            ///@{

            //! Is the timer active?
            bool isActive() const;

            //! Start the timer.
            void start(
                const Core::Time::Duration&,
                const std::function<void(const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)>&);

            //! Stop the timer.
            void stop();

            ///@}

        private:
            void _tick();

            bool _repeating = false;
            bool _active = false;
            Core::Time::Duration _timeout = Core::Time::Duration::zero();
            std::function<void(const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)> _callback;
            Core::Time::TimePoint _time;
            Core::Time::TimePoint _start;

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
        
        //! \name Information
        ///@{

        size_t getTimerValue(TimerValue);
        
        Core::Time::Duration getTimerDuration(TimerValue);

        ///@}

    } // namespace System

    DJV_ENUM_SERIALIZE_HELPERS(System::TimerValue);

} // namespace djv

#include <djvSystem/TimerInline.h>
