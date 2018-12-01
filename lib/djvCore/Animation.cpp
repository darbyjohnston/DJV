//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <algorithm>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        AnimationFunction getAnimationFunction(AnimationType value)
        {
            static const std::vector<AnimationFunction> data =
            {
                [](float t) { return t; },
                [](float t) { return powf(t, 2.f); },
                [](float t) { return powf(t, .5f); },
                [](float t) { return Math::smoothStep(t, 0.f, 1.f); },
                [](float t)
            {
                return (sinf(t * Math::pi2 - Math::pi / 2.f) + 1.f) * .5f;
            }
            };
            DJV_ASSERT(data.size() == static_cast<size_t>(AnimationType::Count));
            return data[static_cast<size_t>(value)];
        }

        struct Animation::Private
        {
            AnimationType type = AnimationType::Linear;
            AnimationFunction function;
            bool repeating = false;
            bool active = false;
            float begin = 0.f;
            float end = 0.f;
            std::chrono::milliseconds timeout;
            Callback callback;
            Callback endCallback;
            std::chrono::time_point<std::chrono::system_clock> start;
        };
        
        void Animation::_init(const std::shared_ptr<Context>& context)
        {
            _p->function = getAnimationFunction(_p->type);

            context->getSystemT<AnimationSystem>()->_addAnimation(shared_from_this());
        }

        Animation::Animation() :
            _p(new Private)
        {}
        
        Animation::~Animation()
        {}

        std::shared_ptr<Animation> Animation::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<Animation>(new Animation);
            out->_init(context);
            return out;
        }

        AnimationType Animation::getType() const
        {
            return _p->type;
        }

        void Animation::setType(AnimationType type)
        {
            _p->type     = type;
            _p->function = getAnimationFunction(_p->type);
        }

        bool Animation::isRepeating() const
        {
            return _p->repeating;
        }
        
        void Animation::setRepeating(bool value)
        {
            _p->repeating = value;
        }
        
        bool Animation::isActive() const
        {
            return _p->active;
        }

        void Animation::start(
            float                     begin,
            float                     end,
            std::chrono::milliseconds timeout,
            const Callback&           callback,
            const Callback&           endCallback)
        {
            /*if (_p->active && _p->endCallback)
            {
                _p->endCallback(_p->end);
            }*/

            _p->active = true;
            _p->begin = begin;
            _p->end = end;
            _p->timeout = timeout;
            _p->callback = callback;
            _p->endCallback = endCallback;
            _p->start = std::chrono::system_clock::now();
        }

        void Animation::_tick(float dt)
        {
            if (_p->active)
            {
                const auto now = std::chrono::system_clock::now();
                const auto diff = std::chrono::duration<float>(now - _p->start);

                const float t = Math::clamp(diff.count() / std::chrono::duration<float>(_p->timeout).count(), 0.f, 1.f);

                float v = 0.f;
                if (_p->begin < _p->end)
                {
                    v = Math::lerp(_p->function(t), _p->begin, _p->end);
                }
                else
                {
                    v = Math::lerp(_p->function(1.f - t), _p->end, _p->begin);
                }
                _p->callback(v);

                if (now > (_p->start + _p->timeout))
                {
                    if (_p->callback)
                    {
                        if (_p->endCallback)
                        {
                            _p->endCallback(_p->end);
                        }
                        _p->active = false;
                    }

                    if (_p->repeating)
                    {
                        _p->active = true;
                        _p->start = now;
                    }
                }
            }
        }

        struct AnimationSystem::Private
        {
            std::vector<std::weak_ptr<Animation> > animations;
        };

        void AnimationSystem::_init(const std::shared_ptr<Context>& context)
        {
            ISystem::_init("djv::Core::AnimationSystem", context);
        }
        
        AnimationSystem::AnimationSystem() :
            _p(new Private)
        {}
        
        AnimationSystem::~AnimationSystem()
        {}

        std::shared_ptr<AnimationSystem> AnimationSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<AnimationSystem>(new AnimationSystem);
            out->_init(context);
            return out;
        }

        void AnimationSystem::_tick(float dt)
        {
            std::vector<std::weak_ptr<Animation> > zombies;
            for (const auto& a : _p->animations)
            {
                if (auto animation = a.lock())
                {
                    animation->_tick(dt);
                }
                else
                {
                    zombies.push_back(a);
                }
            }
            for (const auto& zombie : zombies)
            {
                const auto i = std::find_if(
                    _p->animations.begin(),
                    _p->animations.end(),
                    [zombie](const std::weak_ptr<Animation>& other)
                {
                    return zombie.lock() == other.lock();
                });
                if (i != _p->animations.end())
                {
                    _p->animations.erase(i);
                }
            }
        }

        void AnimationSystem::_exit()
        {
            ISystem::_exit();
            _p->animations.clear();
        }

        void AnimationSystem::_addAnimation(const std::weak_ptr<Animation>& value)
        {
            _p->animations.push_back(value);
        }

    } // namespace Core
    
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::,
        AnimationType,
        DJV_TEXT("Linear"),
        DJV_TEXT("Ease In"),
        DJV_TEXT("Ease Out"),
        DJV_TEXT("Smooth Step"),
        DJV_TEXT("Sine"));

} // namespace djv

