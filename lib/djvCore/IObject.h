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

#pragma once

#include <djvCore/Core.h>

#include <memory>

namespace djv
{
    namespace Core
    {
        class ButtonPressEvent;
        class ButtonReleaseEvent;
        class Context;
        class EnterEvent;
        class IEvent;
        class IEventSystem;
        class LocaleEvent;
        class LogSystem;
        class PointerEnterEvent;
        class PointerLeaveEvent;
        class PointerMoveEvent;
        class ResourceSystem;
        class TextSystem;
        class UpdateEvent;

        class IObject : public std::enable_shared_from_this<IObject>
        {
            DJV_NON_COPYABLE(IObject);

        protected:
            void _init(const std::string& className, Context *);

            IObject();

        public:
            virtual ~IObject() = 0;

            inline Context * getContext() const;
            inline const std::string& getClassName() const;
            inline const std::string& getName() const;
            void setName(const std::string&);

            inline const std::weak_ptr<IObject>& getParent() const;
            template<typename T>
            inline std::shared_ptr<T> getParentRecursiveT() const;
            inline const std::vector<std::shared_ptr<IObject> >& getChildren() const;
            template<typename T>
            inline std::vector<std::shared_ptr<T> > getChildrenT() const;
            template<typename T>
            inline std::vector<std::shared_ptr<T> > getChildrenRecursiveT() const;
            template <typename T>
            inline std::shared_ptr<T> getFirstChildT() const;
            template <typename T>
            inline std::shared_ptr<T> getFirstChildRecursiveT() const;
            virtual void setParent(const std::shared_ptr<IObject>&, int insert = -1);
            virtual void raiseToTop();
            virtual void lowerToBottom();

            inline bool isEnabled(bool parents = false) const;
            void setEnabled(bool);

            void installEventFilter(const std::weak_ptr<IObject>&);
            void removeEventFilter(const std::weak_ptr<IObject>&);

        protected:
            virtual void _updateEvent(UpdateEvent&) {}
            virtual void _localeEvent(LocaleEvent&) {}
            virtual void _pointerEnterEvent(PointerEnterEvent&) {}
            virtual void _pointerLeaveEvent(PointerLeaveEvent&) {}
            virtual void _pointerMoveEvent(PointerMoveEvent&) {}
            virtual void _buttonPressEvent(ButtonPressEvent&) {}
            virtual void _buttonReleaseEvent(ButtonReleaseEvent&) {}
            virtual void _event(IEvent&);
            
            bool _eventFilter(IEvent&);
            virtual bool _eventFilter(const std::shared_ptr<IObject>&, IEvent&) { return false; }

            std::weak_ptr<ResourceSystem> _getResourceSystem() const { return _resourceSystem; }
            std::weak_ptr<LogSystem> _getLogSystem() const { return _logSystem; }
            std::weak_ptr<TextSystem> _getTextSystem() const { return _textSystem; }

            void _log(const std::string& message, Core::LogLevel = Core::LogLevel::Information);

        private:
            template<typename T>
            inline static void _getChildrenRecursiveT(const std::shared_ptr<IObject>&, std::vector<std::shared_ptr<T> >&);
            template<typename T>
            inline static void _getFirstChildRecursiveT(const std::shared_ptr<IObject>&, std::shared_ptr<T>&);

            Context * _context = nullptr;
            std::string _className;
            std::string _name;
            bool _firstTick = true;
            std::weak_ptr<IObject> _parent;
            std::vector<std::shared_ptr<IObject> > _children;
            bool _enabled = true;
            bool _parentsEnabled = true;
            std::vector<std::weak_ptr<IObject> > _filters;
            std::weak_ptr<ResourceSystem> _resourceSystem;
            std::weak_ptr<LogSystem> _logSystem;
            std::weak_ptr<TextSystem> _textSystem;

            friend class IEventSystem;
        };

    } // namespace Core
} // namespace djv

#include <djvCore/IObjectInline.h>

