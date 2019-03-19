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

#include <djvCore/IEventSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Event.h>
#include <djvCore/IObject.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Timer.h>

#include <map>

namespace djv
{
    namespace Core
    {
        namespace Event
        {
            struct IEventSystem::Private
            {
                std::vector<std::shared_ptr<IObject> > objectsCreated;
                std::weak_ptr<TextSystem> textSystem;
                float t = 0.f;
                Event::PointerInfo pointerInfo;
                std::shared_ptr<IObject> hover;
                std::shared_ptr<IObject> grab;
                std::shared_ptr<IObject> keyGrab;
                std::shared_ptr<IObject> textFocus;
                std::string locale;
                bool localeInit = false;
                std::shared_ptr<ValueObserver<std::string> > localeObserver;
                std::shared_ptr<Time::Timer> statsTimer;
            };

            void IEventSystem::_init(const std::string & systemName, Context * context)
            {
                ISystem::_init(systemName, context);

                DJV_PRIVATE_PTR();

                p.textSystem = context->getSystemT<TextSystem>();
                if (auto textSystem = p.textSystem.lock())
                {
                    auto weak = std::weak_ptr<IEventSystem>(std::dynamic_pointer_cast<IEventSystem>(shared_from_this()));
                    p.localeObserver = ValueObserver<std::string>::create(
                        textSystem->observeCurrentLocale(),
                        [weak, context](const std::string & value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->locale = value;
                            system->_p->localeInit = true;
                        }
                    });
                }

                p.statsTimer = Time::Timer::create(context);
                p.statsTimer->setRepeating(true);
                p.statsTimer->start(
                    Time::getMilliseconds(Time::TimerValue::VerySlow),
                    [this](float)
                {
                    DJV_PRIVATE_PTR();
                    std::stringstream s;
                    s << "Global object count: " << IObject::getGlobalObjectCount();
                    _log(s.str());
                });
            }

            IEventSystem::IEventSystem() :
                _p(new Private)
            {}

            IEventSystem::~IEventSystem()
            {}

            const std::shared_ptr<IObject> & IEventSystem::getTextFocus() const
            {
                return _p->textFocus;
            }

            void IEventSystem::setTextFocus(const std::shared_ptr<IObject> & value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.textFocus)
                    return;
                auto textFocus = p.textFocus;
                p.textFocus = value;
                if (textFocus)
                {
                    Event::TextFocusLost event;
                    textFocus->event(event);
                }
                if (p.textFocus)
                {
                    Event::TextFocus event;
                    p.textFocus->event(event);
                }
            }

            void IEventSystem::tick(float dt)
            {
                DJV_PRIVATE_PTR();
                p.t += dt;

                auto objectsCreated = std::move(p.objectsCreated);
                for (const auto & i : objectsCreated)
                {
                    _initObject(i);
                }
                objectsCreated.clear();

                auto rootObject = getContext()->getRootObject();
                if (p.localeInit)
                {
                    p.localeInit = false;
                    Event::Locale localeEvent(p.locale);
                    _localeRecursive(rootObject, localeEvent);
                }

                Update updateEvent(p.t, dt);
                _updateRecursive(rootObject, updateEvent);

                Event::PointerMove moveEvent(_p->pointerInfo);
                if (p.grab)
                {
                    /*{
                        std::stringstream ss;
                        ss << "Grab: " << p.grab->getClassName();
                        _log(ss.str());
                    }*/
                    p.grab->event(moveEvent);
                    if (!moveEvent.isAccepted())
                    {
                        // Release the grabbed object if it did not accept the move event.
                        auto object = p.grab;
                        p.grab = nullptr;
                        moveEvent.reject();

                        // See if a parent wants the event.
                        for (auto parent = object->getParent().lock(); parent; parent = parent->getParent().lock())
                        {
                            parent->event(moveEvent);
                            if (moveEvent.isAccepted())
                            {
                                _setHover(parent);
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
                                }
                                break;
                            }
                        }

                        // If none of the parents wanted the event, see if the original
                        // object wants it back.
                        if (!moveEvent.isAccepted())
                        {
                            object->event(moveEvent);
                            if (moveEvent.isAccepted())
                            {
                                _setHover(object);
                            }
                        }
                    }
                }
                else
                {
                    std::shared_ptr<IObject> hover;
                    _hover(moveEvent, hover);
                    /*if (hover)
                    {
                        std::stringstream ss;
                        ss << "Hover: " << hover->getClassName();
                        _log(ss.str());
                    }*/
                    _setHover(hover);
                }
            }

            void IEventSystem::_initObject(const std::shared_ptr<IObject> & object)
            {
                DJV_PRIVATE_PTR();
                Event::Locale localeEvent(p.locale);
                object->event(localeEvent);
            }

            void IEventSystem::_pointerMove(const Event::PointerInfo & info)
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
                auto object = p.hover;
                while (object)
                {
                    object->event(event);
                    if (event.isAccepted())
                    {
                        p.grab = object;
                        break;
                    }
                    object = object->getParent().lock();
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

            void IEventSystem::_keyPress(int key, int modifiers)
            {
                DJV_PRIVATE_PTR();
                if (p.textFocus || p.hover)
                {
                    Event::KeyPress event(key, modifiers, _p->pointerInfo);
                    auto object = p.textFocus ? p.textFocus : p.hover;
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

            void IEventSystem::_keyRelease(int key, int modifiers)
            {
                DJV_PRIVATE_PTR();
                Event::KeyRelease event(key, modifiers, _p->pointerInfo);
                if (p.keyGrab)
                {
                    p.keyGrab->event(event);
                    p.keyGrab.reset();
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

            void IEventSystem::_text(const std::string & text, int modifiers)
            {
                DJV_PRIVATE_PTR();
                if (p.textFocus)
                {
                    Event::Text event(text, modifiers);
                    p.textFocus->event(event);
                }
            }

            void IEventSystem::_objectCreated(const std::shared_ptr<IObject> & object)
            {
                _p->objectsCreated.push_back(object);
            }

            void IEventSystem::_localeRecursive(const std::shared_ptr<IObject> & object, Locale & event)
            {
                object->event(event);
                for (const auto & child : object->_children)
                {
                    _localeRecursive(child, event);
                }
            }

            void IEventSystem::_updateRecursive(const std::shared_ptr<IObject> & object, Update & event)
            {
                object->event(event);
                auto children = object->_children;
                for (const auto & child : children)
                {
                    child->_parentsEnabled = object->_enabled && object->_parentsEnabled;
                    _updateRecursive(child, event);
                }
            }

            void IEventSystem::_setHover(const std::shared_ptr<IObject> & value)
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

        } // namespace Event
    } // namespace Core
} // namespace djv

