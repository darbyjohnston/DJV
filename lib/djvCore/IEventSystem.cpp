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

#include <map>

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
                float t = 0.f;
                Event::PointerInfo pointerInfo;
                std::shared_ptr<IObject> hover;
                std::shared_ptr<IObject> grab;
                std::shared_ptr<IObject> focus;
                std::shared_ptr<IObject> keyGrab;
                std::shared_ptr<ValueObserver<std::string> > localeObserver;
            };

            void IEventSystem::_init(const std::string& systemName, Context * context)
            {
                ISystem::_init(systemName, context);
                _p->textSystem = context->getSystemT<TextSystem>();
                if (auto textSystem = _p->textSystem.lock())
                {
                    auto weak = std::weak_ptr<IEventSystem>(std::dynamic_pointer_cast<IEventSystem>(shared_from_this()));
                    _p->localeObserver = ValueObserver<std::string>::create(
                        textSystem->observeCurrentLocale(),
                        [weak](const std::string & value)
                    {
                        if (auto system = weak.lock())
                        {
                            LocaleChanged localeChangedEvent(value);
                            system->_localeChanged(localeChangedEvent);
                        }
                    });
                }
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

            const std::shared_ptr<IObject>& IEventSystem::getHover() const
            {
                return _p->hover;
            }

            void IEventSystem::setHover(const std::shared_ptr<IObject>& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.hover)
                    return;
                if (p.hover)
                {
                    Event::PointerLeave leaveEvent(_p->pointerInfo);
                    p.hover->event(leaveEvent);
                }
                p.hover = value;
                if (p.hover)
                {
                    Event::PointerEnter enterEvent(_p->pointerInfo);
                    p.hover->event(enterEvent);
                }
            }

            void IEventSystem::_pointerMove(const Event::PointerInfo& info)
            {
                DJV_PRIVATE_PTR();
                p.pointerInfo = info;
            }

            void IEventSystem::_buttonPress(int button)
            {
                DJV_PRIVATE_PTR();
                auto info = _p->pointerInfo;
                info.buttons[button] = true;
                Event::ButtonPress event(info);
                if (p.hover)
                {
                    p.hover->event(event);
                    if (event.isAccepted())
                    {
                        p.grab = p.hover;
                    }
                }
            }

            void IEventSystem::_buttonRelease(int button)
            {
                DJV_PRIVATE_PTR();
                auto info = _p->pointerInfo;
                info.buttons[button] = false;
                Event::ButtonRelease event(info);
                if (p.grab)
                {
                    p.grab->event(event);
                    p.grab = nullptr;
                }
            }

            void IEventSystem::_keyPress(int key, int mods)
            {
                DJV_PRIVATE_PTR();
                if (p.hover)
                {
                    Event::KeyPress event(key, mods, _p->pointerInfo);
                    auto object = p.hover;
                    while (object)
                    {
                        object->event(event);
                        if (event.isAccepted())
                        {
                            p.keyGrab = object;
                            break;
                        }
                        object = object->getParent().lock();
                    }
                }
            }

            void IEventSystem::_keyRelease(int key, int mods)
            {
                DJV_PRIVATE_PTR();
                Event::KeyRelease event(key, mods, _p->pointerInfo);
                if (p.keyGrab)
                {
                    p.keyGrab->event(event);
                    p.keyGrab = nullptr;
                }
                else
                {
                    auto object = p.hover;
                    while (object)
                    {
                        object->event(event);
                        if (event.isAccepted())
                        {
                            break;
                        }
                        object = object->getParent().lock();
                    }
                }
            }

            void IEventSystem::_drop(const std::vector<std::string> & list)
            {
                DJV_PRIVATE_PTR();
                if (p.hover)
                {
                    Event::Drop event(list, _p->pointerInfo);
                    auto object = p.hover;
                    while (object)
                    {
                        object->event(event);
                        if (event.isAccepted())
                        {
                            break;
                        }
                        object = object->getParent().lock();
                    }
                }
            }

            void IEventSystem::_tick(float dt)
            {
                DJV_PRIVATE_PTR();
                p.t += dt;
                if (p.rootObject)
                {
                    std::vector<std::shared_ptr<IObject> > firstTick;
                    _getFirstTick(p.rootObject, firstTick);
                    if (firstTick.size())
                    {
                        if (auto textSystem = _p->textSystem.lock())
                        {
                            LocaleChanged localeChangedEvent(textSystem->getCurrentLocale());
                            for (auto& object : firstTick)
                            {
                                object->_firstTick = false;
                                object->event(localeChangedEvent);
                            }
                        }
                    }

                    Update updateEvent(p.t, dt);
                    _updateRecursive(p.rootObject, updateEvent);

                    Event::PointerMove moveEvent(_p->pointerInfo);
                    if (p.grab)
                    {
                        p.grab->event(moveEvent);
                        if (!moveEvent.isAccepted())
                        {
                            // If the grabbed object did not accept the event then see if
                            // any of the parent objects want it.
                            moveEvent.reject();
                            for (auto parent = p.grab->getParent().lock(); parent; parent = parent->getParent().lock())
                            {
                                parent->event(moveEvent);
                                if (moveEvent.isAccepted())
                                {
                                    setHover(parent);
                                    if (p.hover)
                                    {
                                        auto info = _p->pointerInfo;
                                        info.buttons[info.id] = true;
                                        Event::ButtonPress buttonPressEvent(info);
                                        p.hover->event(buttonPressEvent);
                                        if (buttonPressEvent.isAccepted())
                                        {
                                            p.grab = p.hover;
                                        }
                                        else
                                        {
                                            p.grab = nullptr;
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    else if (auto root = getRootObject())
                    {
                        std::shared_ptr<IObject> hover;
                        _hover(moveEvent, hover);
                        /*if (hover)
                        {
                            std::stringstream ss;
                            ss << "Hover: " << hover->getClassName();
                            getContext()->log("djv::Desktop::EventSystem", ss.str());
                        }*/
                        setHover(hover);
                    }
                }
            }

            void IEventSystem::_getFirstTick(const std::shared_ptr<IObject>& object, std::vector<std::shared_ptr<IObject> >& out)
            {
                if (object->_firstTick)
                {
                    out.push_back(object);
                }
                auto children = object->_children;
                for (const auto& child : children)
                {
                    _getFirstTick(child, out);
                }
            }

            void IEventSystem::_updateRecursive(const std::shared_ptr<IObject>& object, Update& event)
            {
                object->event(event);
                auto children = object->_children;
                for (const auto& child : children)
                {
                    _updateRecursive(child, event);
                }
            }

            void IEventSystem::_localeChanged(LocaleChanged& event)
            {
                if (_p->rootObject)
                {
                    _localeChangedRecursive(_p->rootObject, event);
                }
            }

            void IEventSystem::_localeChangedRecursive(const std::shared_ptr<IObject>& object, LocaleChanged& event)
            {
                object->event(event);
                auto children = object->_children;
                for (const auto& child : children)
                {
                    _localeChangedRecursive(child, event);
                }
            }

        } // namespace Event
    } // namespace Core
} // namespace djv

