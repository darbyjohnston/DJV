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

#include <djvCore/IObject.h>

#include <djvCore/Context.h>
#include <djvCore/IEventSystem.h>
#include <djvCore/LogSystem.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/TextSystem.h>

#include <algorithm>

namespace djv
{
    namespace Core
    {
        namespace
        {
            size_t globalObjectCount = 0;

        } // namespace

        Context * IObject::_context = nullptr;

        void IObject::_init(Context * context)
        {
            ++globalObjectCount;

            _context   = context;
            _className = "djv::Core::IObject";

            _resourceSystem = context->getSystemT<ResourceSystem>();
            _logSystem = context->getSystemT<LogSystem>();
            _textSystem = context->getSystemT<TextSystem>();
            auto eventSystem = context->getSystemT<Event::IEventSystem>();
            eventSystem->_objectCreated(shared_from_this());
        }

        IObject::~IObject()
        {
            --globalObjectCount;
        }

        void IObject::installEventFilter(const std::weak_ptr<IObject> & value)
        {
            removeEventFilter(value);
            _filters.push_back(value);
        }

        void IObject::removeEventFilter(const std::weak_ptr<IObject> & value)
        {
            const auto i = std::find_if(
                _filters.begin(),
                _filters.end(),
                [value](const std::weak_ptr<IObject> & other)
            {
                auto a = value.lock();
                auto b = other.lock();
                return a && b && a == b;
            });
            if (i != _filters.end())
            {
                _filters.erase(i);
            }
        }

        void IObject::addChild(const std::shared_ptr<IObject> & value)
        {
            std::shared_ptr<IObject> prevParent;
            if (auto parent = value->_parent.lock())
            {
                prevParent = parent;

                const auto i = std::find(parent->_children.begin(), parent->_children.end(), value);
                if (i != parent->_children.end())
                {
                    parent->_children.erase(i);
                }

                Event::ChildRemoved childRemovedEvent(value);
                parent->event(childRemovedEvent);
            }

            value->_parent = shared_from_this();
            _children.push_back(value);
            
            Event::ChildAdded childAddedEvent(value);
            event(childAddedEvent);

            Event::ParentChanged parentChangedEvent(prevParent, shared_from_this());
            value->event(parentChangedEvent);
        }

        void IObject::removeChild(const std::shared_ptr<IObject> & value)
        {
            auto child = value;
            const auto i = std::find(_children.begin(), _children.end(), child);
            if (i != _children.end())
            {
                _children.erase(i);

                child->_parent.reset();

                Event::ChildRemoved childRemovedEvent(child);
                event(childRemovedEvent);

                Event::ParentChanged parentChangedEvent(shared_from_this(), nullptr);
                child->event(parentChangedEvent);
            }
        }

        void IObject::clearChildren()
        {
            while (_children.size())
            {
                removeChild(_children.back());
            }
        }

        void IObject::moveToFront()
        {
            if (auto parent = _parent.lock())
            {
                auto object = shared_from_this();
                auto& siblings = parent->_children;
                const auto i = std::find(siblings.begin(), siblings.end(), object);
                if (i != siblings.end())
                {
                    siblings.erase(i);
                }
                siblings.push_back(object);
                Event::ChildOrder childOrderEvent;
                parent->event(childOrderEvent);
            }
        }

        void IObject::moveToBack()
        {
            if (auto parent = _parent.lock())
            {
                auto object = shared_from_this();
                auto& siblings = parent->_children;
                const auto i = std::find(siblings.begin(), siblings.end(), object);
                if (i != siblings.end())
                {
                    siblings.erase(i);
                }
                siblings.insert(siblings.begin(), object);
                Event::ChildOrder childOrderEvent;
                parent->event(childOrderEvent);
            }
        }

        bool IObject::event(Event::Event & event)
        {
            bool out = _eventFilter(event);
            if (!out)
            {
                switch (event.getEventType())
                {
                case Event::Type::ParentChanged:
                    _parentChangedEvent(static_cast<Event::ParentChanged &>(event));
                    break;
                case Event::Type::ChildAdded:
                    _childAddedEvent(static_cast<Event::ChildAdded &>(event));
                    break;
                case Event::Type::ChildRemoved:
                    _childRemovedEvent(static_cast<Event::ChildRemoved &>(event));
                    break;
                case Event::Type::ChildOrder:
                    _childOrderEvent(static_cast<Event::ChildOrder &>(event));
                    break;
                case Event::Type::Locale:
                    _localeEvent(static_cast<Event::Locale &>(event));
                    break;
                case Event::Type::Update:
                    _updateEvent(static_cast<Event::Update &>(event));
                    break;
                default: break;
                }
                out = event.isAccepted();
            }
            return out;
        }

        size_t IObject::getGlobalObjectCount()
        {
            return globalObjectCount;
        }

        void IObject::getObjectCounts(const std::shared_ptr<IObject>& object, std::map<std::string, size_t>& out)
        {
            const std::string& className = object->getClassName();
            const auto i = out.find(className);
            if (i != out.end())
            {
                i->second++;
            }
            else
            {
                out[className] = 1;
            }
            for (const auto& j : object->getChildren())
            {
                getObjectCounts(j, out);
            }
        }

        std::string IObject::_getText(const std::string & id) const
        {
            return _textSystem->getText(id);
        }

        void IObject::_log(const std::string & message, LogLevel level)
        {
            _logSystem->log(_className, message, level);
        }

        bool IObject::_eventFilter(Event::Event & event)
        {
            bool filtered = false;
            auto i = _filters.begin();
            while (i != _filters.end())
            {
                if (auto object = i->lock())
                {
                    if (object->_eventFilter(shared_from_this(), event))
                    {
                        filtered = true;
                        break;
                    }
                    ++i;
                }
                else
                {
                    i = _filters.erase(i);
                }
            }
            return filtered;
        }

    } // namespace Core
} // namespace djv
