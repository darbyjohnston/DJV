// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
                float           begin,
                float           end,
                Time::Duration  duration,
                const Callback& callback,
                const Callback& endCallback)
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
                _start       = std::chrono::steady_clock::now();
            }

            void Animation::stop()
            {
                _active = false;
            }

            void Animation::_tick()
            {
                const auto now = std::chrono::steady_clock::now();
                _time = now;

                const auto diff = std::chrono::duration<float>(_time - _start);
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

                if (_time > (_start + _duration))
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

            void System::tick()
            {
                DJV_PRIVATE_PTR();
                auto i = p.animations.begin();
                while (i != p.animations.end())
                {
                    if (auto animation = i->lock())
                    {
                        if (animation->isActive())
                        {
                            animation->_tick();
                        }
                        ++i;
                    }
                    else
                    {
                        i = p.animations.erase(i);
                    }
                }
            }

            void System::_addAnimation(const std::weak_ptr<Animation>& value)
            {
                _p->animations.push_back(value);
            }

        } // namespace Animation
    } // namespace Core
    
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Animation,
        Type,
        DJV_TEXT("animation_type_linear"),
        DJV_TEXT("animation_type_ease_in"),
        DJV_TEXT("animation_type_ease_out"),
        DJV_TEXT("animation_type_smooth_step"),
        DJV_TEXT("animation_type_sine"));

} // namespace djv

