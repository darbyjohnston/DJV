// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/ISystem.h>

#include <djvCore/Time.h>

#include <functional>

namespace djv
{
    namespace System
    {
        //! This namespace provides animation functionality.
        namespace Animation
        {
            class System;

            //! This enumeration provides the animation types.
            enum class Type
            {
                Linear,
                EaseIn,
                EaseOut,
                SmoothStep,
                Sine,

                Count,
                First = Linear,
                Last = Sine
            };

            //! This typedef provides an animation function.
            typedef std::function<float(float)> Function;

            //! This class provides an animated value.
            class Animation : public std::enable_shared_from_this<Animation>
            {
                DJV_NON_COPYABLE(Animation);
                void _init(const std::shared_ptr<Context>&);
                Animation();

            public:
                //! Create a new animation.
                static std::shared_ptr<Animation> create(const std::shared_ptr<Context>&);

                //! \name Animation Type
                ///@{

                Type getType() const;
                
                void setType(Type);

                ///@}

                //! \name Animation Options
                ///@{

                bool isRepeating() const;
                
                void setRepeating(bool);

                ///@}

                //! Get whether the animation is active.
                bool isActive() const;

                typedef std::function<void(float)> Callback;

                //! Start the animation.
                void start(
                    float                begin,
                    float                end,
                    Core::Time::Duration duration,
                    const Callback&      callback,
                    const Callback&      endCallback = nullptr);

                //! Stop the animation.
                void stop();

            private:
                void _tick();

                Type                  _type       = Type::Linear;
                Function              _function;
                bool                  _repeating  = false;
                bool                  _active     = false;
                float                 _begin      = 0.F;
                float                 _end        = 0.F;
                Core::Time::Duration  _duration   = Core::Time::Duration::zero();
                Callback              _callback;
                Callback              _endCallback;
                Core::Time::TimePoint _time;
                Core::Time::TimePoint _start;

                friend class System;
            };

            //! This class provides the system that manages animations.
            class System : public ISystem
            {
                DJV_NON_COPYABLE(System);
                void _init(const std::shared_ptr<Context>&);
                System();

            public:
                ~System() override;

                static std::shared_ptr<System> create(const std::shared_ptr<Context>&);

                void tick() override;

            private:
                void _addAnimation(const std::weak_ptr<Animation>&);

                DJV_PRIVATE();

                friend class Animation;
            };

        } // namespace Animation
    } // namespace System
} // namespace djv

