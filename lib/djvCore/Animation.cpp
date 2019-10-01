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

#include <djvCore/Animation.h>

#include <djvCore/Context.h>
#include <djvCore/Math.h>
#include <djvCore/TextSystem.h>

#include <algorithm>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace Animation
        {
            Function getFunction(Type value)
            {
                const std::vector<Function> data =
                {
                    [](float t) { return t; },
                    [](float t) { return powf(t, 2.F); },
                    [](float t) { return powf(t, .5F); },
                    [](float t) { return Math::smoothStep(t, 0.F, 1.F); },
                    [](float t)
                {
                    return (sinf(t * Math::pi2 - Math::pi / 2.F) + 1.F) * .5F;
                }
                };
                DJV_ASSERT(data.size() == static_cast<size_t>(Type::Count));
                return data[static_cast<size_t>(value)];
            }

            void Animation::_init(const std::shared_ptr<Context>& context)
            {
                _function = getFunction(_type);
                auto system = context->getSystemT<System>();
                system->_addAnimation(shared_from_this());
            }

            std::shared_ptr<Animation> Animation::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Animation>(new Animation);
                out->_init(context);
                return out;
            }

            void Animation::setType(Type type)
            {
                _type = type;
                _function = getFunction(_type);
            }

            void Animation::setRepeating(bool value)
            {
                _repeating = value;
            }

            void Animation::start(
                float                     begin,
                float                     end,
                std::chrono::milliseconds duration,
                const Callback &          callback,
                const Callback &          endCallback)
            {
                /*if (_active && _endCallback)
                {
                    _endCallback(_end);
                }*/

                _active      = true;
                _begin       = begin;
                _end         = end;
                _duration    = duration;
                _callback    = callback;
                _endCallback = endCallback;
                _start       = std::chrono::system_clock::now();
            }

            void Animation::stop()
            {
                _active = false;
            }

            void Animation::_tick(float)
            {
                if (_active)
                {
                    const auto now = std::chrono::system_clock::now();
                    const auto diff = std::chrono::duration<float>(now - _start);

                    const float t = Math::clamp(diff.count() / std::chrono::duration<float>(_duration).count(), 0.F, 1.F);

                    float v = 0.F;
                    if (_begin < _end)
                    {
                        v = Math::lerp(_function(t), _begin, _end);
                    }
                    else
                    {
                        v = Math::lerp(_function(1.F - t), _end, _begin);
                    }
                    _callback(v);

                    if (now > (_start + _duration))
                    {
                        if (_callback)
                        {
                            if (_endCallback)
                            {
                                _endCallback(_end);
                            }
                            _active = false;
                        }

                        if (_repeating)
                        {
                            _active = true;
                            _start = now;
                        }
                    }
                }
            }

            struct System::Private
            {
                std::vector<std::weak_ptr<Animation> > animations;
            };

            void System::_init(const std::shared_ptr<Context>& context)
            {
                ISystem::_init("djv::Core::Animation::System", context);
                addDependency(context->getSystemT<TextSystem>());
            }

            System::System() :
                _p(new Private)
            {}

            System::~System()
            {}

            std::shared_ptr<System> System::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<System>(new System);
                out->_init(context);
                return out;
            }

            void System::tick(float dt)
            {
                DJV_PRIVATE_PTR();
                auto i = p.animations.begin();
                while (i != p.animations.end())
                {
                    if (auto animation = i->lock())
                    {
                        animation->_tick(dt);
                        ++i;
                    }
                    else
                    {
                        i = p.animations.erase(i);
                    }
                }
            }

            void System::_addAnimation(const std::weak_ptr<Animation> & value)
            {
                _p->animations.push_back(value);
            }

        } // namespace Animation
    } // namespace Core
    
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Animation,
        Type,
        DJV_TEXT("Linear"),
        DJV_TEXT("EaseIn"),
        DJV_TEXT("EaseOut"),
        DJV_TEXT("SmoothStep"),
        DJV_TEXT("Sine"));

} // namespace djv

