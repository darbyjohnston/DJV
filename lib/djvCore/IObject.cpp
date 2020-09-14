// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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

        void IObject::_init(const std::shared_ptr<Context>& context)
        {
            ++globalObjectCount;

            _context   = context;
            _className = "djv::Core::IObject";

            _resourceSystem = context->getSystemT<ResourceSystem>();
            _logSystem = context->getSystemT<LogSystem>();
            _textSystem = context->getSystemT<TextSystem>();
            auto eventSystem = context->getSystemT<Event::IEventSystem>();
        }
        
        IObject::IObject()
        {}

        IObject::~IObject()
        {
            --globalObjectCount;
        }

        const std::weak_ptr<Context>& IObject::getContext() const
        {
            return _context;
        }

        const std::string& IObject::getClassName() const
        {
            return _className;
        }

        const std::string& IObject::getObjectName() const
        {
            return _objectName;
        }

        void IObject::setClassName(const std::string& name)
        {
            _className = name;
        }

        void IObject::setObjectName(const std::string& name)
        {
            _objectName = name;
        }

        const std::weak_ptr<IObject>& IObject::getParent() const
        {
            return _parent;
        }

        const std::vector<std::shared_ptr<IObject> >& IObject::getChildren() const
        {
            return _children;
        }

        bool IObject::isEnabled(bool parents) const
        {
            return parents ? (_parentsEnabled && _enabled) : _enabled;
        }
        
        void IObject::installEventFilter(const std::weak_ptr<IObject>& value)
        {
            removeEventFilter(value);
            _filters.push_back(value);
        }

        void IObject::removeEventFilter(const std::weak_ptr<IObject>& value)
        {
            const auto i = std::find_if(
                _filters.begin(),
                _filters.end(),
                [value](const std::weak_ptr<IObject>& other)
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

        void IObject::addChild(const std::shared_ptr<IObject>& value)
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

        void IObject::removeChild(const std::shared_ptr<IObject>& value)
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

        void IObject::setEnabled(bool value)
        {
            _enabled = value;
        }

        bool IObject::event(Event::Event& event)
        {
            bool out = false;
            if (_filters.size())
            {
                out = _eventFilter(event);
            }
            if (!out)
            {
                switch (event.getEventType())
                {
                case Event::Type::ParentChanged:
                {
                    auto& parentChangedEvent = static_cast<Event::ParentChanged&>(event);
                    _parentChangedEvent(parentChangedEvent);
                    const bool prevParent = parentChangedEvent.getPrevParent() ? true : false;
                    const bool newParent = parentChangedEvent.getNewParent() ? true : false;
                    if (newParent && !prevParent)
                    {
                        Event::InitData initData(true);
                        Event::Init initEvent(initData);
                        _eventInitRecursive(shared_from_this(), initEvent);
                    }
                    break;
                }
                case Event::Type::ChildAdded:
                    _childAddedEvent(static_cast<Event::ChildAdded&>(event));
                    break;
                case Event::Type::ChildRemoved:
                    _childRemovedEvent(static_cast<Event::ChildRemoved&>(event));
                    break;
                case Event::Type::ChildOrder:
                    _childOrderEvent(static_cast<Event::ChildOrder&>(event));
                    break;
                case Event::Type::Init:
                    _initEvent(static_cast<Event::Init&>(event));
                    break;
                case Event::Type::Update:
                    _updateEvent(static_cast<Event::Update&>(event));
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

        void IObject::_parentChangedEvent(Event::ParentChanged&)
        {
            // Default implementation does nothing.
        }
        
        void IObject::_childAddedEvent(Event::ChildAdded&)
        {
            // Default implementation does nothing.
        }
        
        void IObject::_childRemovedEvent(Event::ChildRemoved&)
        {
            // Default implementation does nothing.
        }
        
        void IObject::_childOrderEvent(Event::ChildOrder&)
        {
            // Default implementation does nothing.
        }
        
        void IObject::_initEvent(Event::Init&)
        {
            // Default implementation does nothing.
        }
        
        void IObject::_updateEvent(Event::Update&)
        {
            // Default implementation does nothing.
        }

        bool IObject::_eventFilter(const std::shared_ptr<IObject>&, Event::Event&)
        {
            return false;
        }

        const std::string& IObject::_getText(const std::string& id) const
        {
            return _textSystem->getText(id);
        }

        void IObject::_log(const std::string& message, LogLevel level)
        {
            _logSystem->log(_className, message, level);
        }

        const std::shared_ptr<ResourceSystem>& IObject::_getResourceSystem() const
        {
            return _resourceSystem;
        }

        const std::shared_ptr<LogSystem>& IObject::_getLogSystem() const
        {
            return _logSystem;
        }

        const std::shared_ptr<TextSystem>& IObject::_getTextSystem() const
        {
            return _textSystem;
        }
        
        void IObject::_eventInitRecursive(const std::shared_ptr<IObject>& object, Event::Init& event)
        {
            for (const auto& i : object->_children)
            {
                _eventInitRecursive(i, event);
            }
            object->event(event);
        }
        
        bool IObject::_eventFilter(Event::Event& event)
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
