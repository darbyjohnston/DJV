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

#include <djvCore/IEventSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Event.h>
#include <djvCore/IObject.h>
#include <djvCore/TextSystem.h>

namespace djv
{
    namespace Core
    {
        namespace Event
        {
            struct IEventSystem::Private
            {
                std::weak_ptr<TextSystem> textSystem;
                std::shared_ptr<IObject> rootObject;
            };

            void IEventSystem::_init(const std::string& systemName, Context * context)
            {
                ISystem::_init(systemName, context);
                _p->textSystem = context->getSystemT<TextSystem>();
            }

            IEventSystem::IEventSystem() :
                _p(new Private)
            {}

            IEventSystem::~IEventSystem()
            {}

            const std::shared_ptr<IObject>& IEventSystem::getRootObject() const
            {
                return _p->rootObject;
            }

            void IEventSystem::setRootObject(const std::shared_ptr<IObject>& value)
            {
                _p->rootObject = value;
            }

            void IEventSystem::_locale(Locale& event)
            {
                if (_p->rootObject)
                {
                    _localeRecursive(_p->rootObject, event);
                }
            }

            void IEventSystem::_tick(float dt)
            {
                DJV_PRIVATE_PTR();
                if (p.rootObject)
                {
                    std::vector<std::shared_ptr<IObject> > firstTick;
                    _getFirstTick(p.rootObject, firstTick);
                    for (auto& object : firstTick)
                    {
                        object->_firstTick = false;
                    }

                    Update update(dt);
                    _updateRecursive(p.rootObject, update);

                    if (firstTick.size())
                    {
                        if (auto textSystem = _p->textSystem.lock())
                        {
                            Locale locale(textSystem->getCurrentLocale());
                            for (auto& object : firstTick)
                            {
                                object->event(locale);
                            }
                        }
                    }
                }
            }

            void IEventSystem::_getFirstTick(const std::shared_ptr<IObject>& object, std::vector<std::shared_ptr<IObject> >& out)
            {
                if (object->_firstTick)
                {
                    out.push_back(object);
                }
                for (const auto& child : object->_children)
                {
                    _getFirstTick(child, out);
                }
            }

            void IEventSystem::_updateRecursive(const std::shared_ptr<IObject>& object, Update& event)
            {
                object->event(event);
                for (const auto& child : object->_children)
                {
                    _updateRecursive(child, event);
                }
            }

            void IEventSystem::_localeRecursive(const std::shared_ptr<IObject>& object, Locale& event)
            {
                object->event(event);
                for (const auto& child : object->_children)
                {
                    _localeRecursive(child, event);
                }
            }

        } // namespace Event
    } // namespace Core
} // namespace djv

