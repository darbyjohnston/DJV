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

#include <djvCore/EventSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Event.h>
#include <djvCore/IObject.h>
#include <djvCore/TextSystem.h>

namespace djv
{
    namespace Core
    {
        struct IEventSystem::Private
        {
            std::shared_ptr<IObject> rootObject;
            std::shared_ptr<IObject> hover;
            std::shared_ptr<IObject> grab;
            std::shared_ptr<IObject> focus;
        };

        void IEventSystem::_init(const std::string& systemName, const std::shared_ptr<Context>& context)
        {
            ISystem::_init(systemName, context);
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

        void IEventSystem::_locale(LocaleEvent& event)
        {
            if (_p->rootObject)
            {
                _localeRecursive(_p->rootObject, event);
            }
        }

        void IEventSystem::_pointerMove(const PointerInfo& info)
        {
            PointerMoveEvent moveEvent(info);
            if (_p->grab)
            {
                _p->grab->_event(moveEvent);
            }
            else if (_p->rootObject)
            {
                std::shared_ptr<IObject> hover;
                _hover(_p->rootObject, moveEvent, hover);
                if (hover != _p->hover)
                {
                    if (_p->hover)
                    {
                        PointerLeaveEvent leaveEvent(info);
                        _p->hover->_event(leaveEvent);
                    }
                    _p->hover = hover;
                    if (_p->hover)
                    {
                        PointerEnterEvent enterEvent(info);
                        _p->hover->_event(enterEvent);
                    }
                }
            }
        }

        void IEventSystem::_buttonPress(const PointerInfo& info)
        {
            ButtonPressEvent event(info);
            if (_p->hover)
            {
                _p->hover->_event(event);
                if (event.isAccepted())
                {
                    _p->grab = _p->hover;
                }
            }
        }

        void IEventSystem::_buttonRelease(const PointerInfo& info)
        {
            ButtonReleaseEvent event(info);
            if (_p->grab)
            {
                _p->grab->_event(event);
                _p->grab = nullptr;
            }
        }

        void IEventSystem::_tick(float dt)
        {
            if (_p->rootObject)
            {
                std::vector<std::shared_ptr<IObject> > firstTick;
                _getFirstTick(_p->rootObject, firstTick);
                for (auto& object : firstTick)
                {
                    object->_firstTick = false;
                }

                UpdateEvent updateEvent(dt);
                _updateRecursive(_p->rootObject, updateEvent);

                if (firstTick.size())
                {
                    if (auto context = getContext().lock())
                    {
                        if (auto textSystem = context->getSystemT<TextSystem>())
                        {
                            LocaleEvent localeEvent(textSystem->getCurrentLocale());
                            for (auto& object : firstTick)
                            {
                                object->_event(localeEvent);
                            }
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

        void IEventSystem::_updateRecursive(const std::shared_ptr<IObject>& object, UpdateEvent& event)
        {
            object->_event(event);
            for (const auto& child : object->_children)
            {
                _updateRecursive(child, event);
            }
        }
       
        void IEventSystem::_localeRecursive(const std::shared_ptr<IObject>& object, LocaleEvent& event)
        {
            object->_event(event);
            for (const auto& child : object->_children)
            {
                _localeRecursive(child, event);
            }
        }

        void IEventSystem::_hover(const std::shared_ptr<IObject>& object, PointerMoveEvent& event, std::shared_ptr<IObject>& hover)
        {
            object->_event(event);
            if (event.isAccepted())
            {
                hover = object;
            }
            else
            {
                for (const auto& child : object->_children)
                {
                    _hover(child, event, hover);
                }
            }
        }

    } // namespace Core
} // namespace djv
