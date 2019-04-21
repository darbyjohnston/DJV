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

#pragma once

#include <djvCore/Enum.h>
#include <djvCore/ISystem.h>

#include <chrono>
#include <functional>

namespace djv
{
    namespace Core
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
            DJV_ENUM_HELPERS(Type);

            //! This typedef provides an animation function.
            typedef std::function<float(float)> Function;

            //! Get an animation function.
            Function getFunction(Type);

            //! This class provides an animated value.
            class Animation : public std::enable_shared_from_this<Animation>
            {
                DJV_NON_COPYABLE(Animation);
                void _init(Context *);
                inline Animation();

            public:
                //! Create a new animation.
                static std::shared_ptr<Animation> create(Context *);

                //! \name Animation Type
                ///@{

                inline Type getType() const;
                void setType(Type);

                ///@}

                //! \name Animation Options
                ///@{

                inline bool isRepeating() const;
                void setRepeating(bool);

                ///@}

                //! Get whether the animation is active.
                inline bool isActive() const;

                typedef std::function<void(float)> Callback;

                //! Start the animation.
                void start(
                    float                     begin,
                    float                     end,
                    std::chrono::milliseconds timeout,
                    const Callback &          callback,
                    const Callback &          endCallback = nullptr);

            private:
                void _tick(float dt);

                Type                      _type        = Type::Linear;
                Function                  _function;
                bool                      _repeating   = false;
                bool                      _active      = false;
                float                     _begin       = 0.f;
                float                     _end         = 0.f;
                std::chrono::milliseconds _timeout     = std::chrono::milliseconds(0);
                Callback                  _callback;
                Callback                  _endCallback;
                std::chrono::time_point<std::chrono::system_clock> _start;

                friend class System;
            };

            //! This class provides an animation system.
            class System : public ISystem
            {
                DJV_NON_COPYABLE(System);
                void _init(Context *);
                System();

            public:
                virtual ~System();

                static std::shared_ptr<System> create(Context *);

                void tick(float dt) override;

            private:
                void _addAnimation(const std::weak_ptr<Animation> &);

                DJV_PRIVATE();

                friend class Animation;
            };

        } // namespace Animation
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::Animation::Type);
    DJV_ENUM_SERIALIZE_HELPERS(Core::Animation::Type);

} // namespace djv

#include <djvCore/AnimationInline.h>

