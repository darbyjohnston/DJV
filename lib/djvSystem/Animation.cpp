// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/Animation.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

#include <djvMath/Math.h>

#include <array>
#include <sstream>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace Animation
        {
            DJV_ENUM_HELPERS_IMPLEMENTATION(Type);

            Function getFunction(Type value)
            {
                const std::array<Function, 5> data =
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
                return data[static_cast<size_t>(value)];
            }

            void Animation::_init(const std::shared_ptr<Context>& context)
            {
                _function = getFunction(_type);
                auto system = context->getSystemT<AnimationSystem>();
                system->_addAnimation(shared_from_this());
            }
            
            Animation::Animation()
            {}

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

            void Animation::start(
                float           begin,
                float           end,
                Time::Duration  duration,
                const Callback& callback,
                const Callback& endCallback)
            {
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
                    if (_repeating)
                    {
                        _start = now;
                    }
                    else
                    {
                        if (_endCallback)
                        {
                            _endCallback(_end);
                        }
                        _active = false;
                    }
                }
            }

            struct AnimationSystem::Private
            {
                std::vector<std::weak_ptr<Animation> > animations;
                std::vector<std::weak_ptr<Animation> > newAnimations;
            };

            void AnimationSystem::_init(const std::shared_ptr<Context>& context)
            {
                ISystem::_init("djv::System::Animation::AnimationSystem", context);

                addDependency(TextSystem::create(context));

                _logInitTime();
            }

            AnimationSystem::AnimationSystem() :
                _p(new Private)
            {}

            AnimationSystem::~AnimationSystem()
            {}

            std::shared_ptr<AnimationSystem> AnimationSystem::create(const std::shared_ptr<Context>& context)
            {
                auto out = context->getSystemT<AnimationSystem>();
                if (!out)
                {
                    out = std::shared_ptr<AnimationSystem>(new AnimationSystem);
                    out->_init(context);
                }
                return out;
            }

            void AnimationSystem::tick()
            {
                DJV_PRIVATE_PTR();
                p.animations.insert(p.animations.end(), p.newAnimations.begin(), p.newAnimations.end());
                p.newAnimations.clear();
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

            void AnimationSystem::_addAnimation(const std::weak_ptr<Animation>& value)
            {
                _p->newAnimations.push_back(value);
            }

        } // namespace Animation
    } // namespace System

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        System::Animation,
        Type,
        DJV_TEXT("animation_type_linear"),
        DJV_TEXT("animation_type_ease_in"),
        DJV_TEXT("animation_type_ease_out"),
        DJV_TEXT("animation_type_smooth_step"),
        DJV_TEXT("animation_type_sine"));

} // namespace djv

