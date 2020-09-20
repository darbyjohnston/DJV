// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/Animation.h>

#include <djvSystem/AnimationFunc.h>
#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

#include <djvMath/Math.h>
#include <djvMath/MathFunc.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace Animation
        {
            void Animation::_init(const std::shared_ptr<Context>& context)
            {
                _function = getFunction(_type);
                auto system = context->getSystemT<System>();
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

            struct System::Private
            {
                std::vector<std::weak_ptr<Animation> > animations;
                std::vector<std::weak_ptr<Animation> > newAnimations;
            };

            void System::_init(const std::shared_ptr<Context>& context)
            {
                ISystem::_init("djv::System::Animation::System", context);
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

            void System::_addAnimation(const std::weak_ptr<Animation>& value)
            {
                _p->newAnimations.push_back(value);
            }

        } // namespace Animation
    } // namespace System
} // namespace djv

