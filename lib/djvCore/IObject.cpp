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

#include <djvCore/IObject.h>

#include <djvCore/Context.h>
#include <djvCore/Event.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/TextSystem.h>

#include <algorithm>

namespace djv
{
    namespace Core
    {
        namespace
        {
            static size_t currentObjectCount = 0;

        } // namespace

        void IObject::_init(const std::string& className, const std::shared_ptr<Context>& context)
        {
            _context = context;
            _className = className;
            _resourceSystem = context->getSystemT<ResourceSystem>();
            _textSystem = context->getSystemT<TextSystem>();

            ++currentObjectCount;
        }

        IObject::IObject()
        {}

        IObject::~IObject()
        {
            --currentObjectCount;
        }

        void IObject::setName(const std::string& name)
        {
            _name = name;
        }

        void IObject::raiseToTop()
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
            }
        }

        void IObject::lowerToBottom()
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
            }
        }

        void IObject::setEnabled(bool value)
        {
            _enabled = value;
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

        void IObject::setParent(const std::shared_ptr<IObject>& value, int insert)
        {
            if (auto parent = _parent.lock())
            {
                const auto i = std::find(parent->_children.begin(), parent->_children.end(), shared_from_this());
                if (i != parent->_children.end())
                {
                    parent->_children.erase(i);
                }
                _parent.reset();
            }
            if (value)
            {
                if (-1 == insert)
                {
                    insert = static_cast<int>(value->_children.size());
                }
                auto i = value->_children.begin();
                for (int j = 0; i != value->_children.end() && j < insert; ++i, ++j)
                    ;
                value->_children.insert(i, shared_from_this());
                _parent = value;
            }
        }

        void IObject::_event(IEvent& event)
        {
            if (!_eventFilter(event))
            {
                switch (event.getEventType())
                {
                case EventType::Update: _updateEvent(static_cast<UpdateEvent&>(event)); break;
                case EventType::Locale: _localeEvent(static_cast<LocaleEvent&>(event)); break;
                case EventType::PointerEnter: _pointerEnterEvent(static_cast<PointerEnterEvent&>(event)); break;
                case EventType::PointerLeave: _pointerLeaveEvent(static_cast<PointerLeaveEvent&>(event)); break;
                case EventType::PointerMove: _pointerMoveEvent(static_cast<PointerMoveEvent&>(event)); break;
                case EventType::ButtonPress: _buttonPressEvent(static_cast<ButtonPressEvent&>(event)); break;
                case EventType::ButtonRelease: _buttonReleaseEvent(static_cast<ButtonReleaseEvent&>(event)); break;
                default: break;
                }
            }
        }

        bool IObject::_eventFilter(IEvent& event)
        {
            bool filtered = false;
            std::vector<std::weak_ptr<IObject> > zombies;
            for (const auto& filter : _filters)
            {
                if (auto object = filter.lock())
                {
                    if (object->_eventFilter(shared_from_this(), event))
                    {
                        filtered = true;
                        break;
                    }
                }
                else
                {
                    zombies.push_back(filter);
                }
            }
            for (const auto& zombie : zombies)
            {
                removeEventFilter(zombie);
            }
            return filtered;
        }

        void IObject::_log(const std::string& message, LogLevel level)
        {
            if (auto context = _context.lock())
            {
                context->log(_className, message, level);
            }
        }

    } // namespace Core
} // namespace djv
