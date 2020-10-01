// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/Enum.h>
#include <djvSystem/Event.h>

namespace djv
{
    namespace System
    {
        class Context;
        class LogSystem;
        class ResourceSystem;
        class TextSystem;

        namespace Event
        {
            class IEventSystem;

        } // namespace Event

        //! This class provides the base functionality for objects.
        class IObject : public std::enable_shared_from_this<IObject>
        {
            DJV_NON_COPYABLE(IObject);

        protected:
            void _init(const std::shared_ptr<Context>&);

            IObject();

        public:
            virtual ~IObject() = 0;

            //! \name Context
            ///@{

            const std::weak_ptr<Context>& getContext() const;

            ///@}

            //! \name Information
            ///@{

            const std::string& getClassName() const;
            const std::string& getObjectName() const;

            void setClassName(const std::string&);
            void setObjectName(const std::string&);

            ///@}

            //! \name Object Hierarchy
            ///@{

            //! Get the parent object.
            const std::weak_ptr<IObject>& getParent() const;

            //! Walk up the object hierarchy looking for a parent of the given type.
            template<typename T>
            std::shared_ptr<T> getParentRecursiveT() const;

            //! Get the list of child objects.
            const std::vector<std::shared_ptr<IObject> >& getChildren() const;

            //! Get a list of child objects of the given type.
            template<typename T>
            std::vector<std::shared_ptr<T> > getChildrenT() const;

            //! Recursively find all child objects of the given type.
            template<typename T>
            std::vector<std::shared_ptr<T> > getChildrenRecursiveT() const;

            //! Get the first child object of the given type.
            template <typename T>
            std::shared_ptr<T> getFirstChildT() const;

            //! Recursively find the first child object of the given type.
            template <typename T>
            std::shared_ptr<T> getFirstChildRecursiveT() const;

            //! Add a child object.
            virtual void addChild(const std::shared_ptr<IObject>&);

            //! Remove a child object.
            virtual void removeChild(const std::shared_ptr<IObject>&);

            //! Remove all child objects. The child objects are removed in order
            //! from last to first.
            virtual void clearChildren();

            //! Move this object to the front of the child list.
            virtual void moveToFront();

            //! Move this object to the back of the child list.
            virtual void moveToBack();

            ///@}

            //! \name Enabled State
            ///@{

            bool isEnabled(bool parents = false) const;
            
            virtual void setEnabled(bool);

            ///@}

            //! \name Events
            ///@{

            //! Install an event filter.
            void installEventFilter(const std::weak_ptr<IObject>&);

            //! Remove an event filter.
            void removeEventFilter(const std::weak_ptr<IObject>&);

            //! This function receives events for the object.
            virtual bool event(System::Event::Event&);

            ///@}

            //! \name Utility
            ///@{

            //! Get the number of objects that currently exist.
            static size_t getGlobalObjectCount();

            //! Get the number of objects per class name.
            static void getObjectCounts(const std::shared_ptr<IObject>&, std::map<std::string, size_t>&);

            ///@}

        protected:
            //! \name Events
            ///@{

            virtual void _parentChangedEvent(System::Event::ParentChanged&);
            virtual void _childAddedEvent(System::Event::ChildAdded&);
            virtual void _childRemovedEvent(System::Event::ChildRemoved&);
            virtual void _childOrderEvent(System::Event::ChildOrder&);
            virtual void _initEvent(System::Event::Init&);
            virtual void _updateEvent(System::Event::Update&);

            //! Over-ride this function to filter events for other objects.
            virtual bool _eventFilter(const std::shared_ptr<IObject>&, Event::Event&);

            ///@}

            //! \name Convenience Functions
            ///@{

            const std::string& _getText(const std::string& id) const;

            void _log(const std::string& message, LogLevel = LogLevel::Information);

            const std::shared_ptr<ResourceSystem>& _getResourceSystem() const;
            const std::shared_ptr<LogSystem>& _getLogSystem() const;
            const std::shared_ptr<TextSystem>& _getTextSystem() const;

            ///@}

        private:
            void _eventInitRecursive(const std::shared_ptr<IObject>&, Event::Init&);
            bool _eventFilter(System::Event::Event&);

            template<typename T>
            static void _getChildrenRecursiveT(const std::shared_ptr<IObject>&, std::vector<std::shared_ptr<T> >&);
            template<typename T>
            static void _getFirstChildRecursiveT(const std::shared_ptr<IObject>&, std::shared_ptr<T>&);

            std::weak_ptr<Context> _context;

            std::string _className;
            std::string _objectName;

            std::weak_ptr<IObject>                 _parent;
            std::vector<std::shared_ptr<IObject> > _children;

            bool _enabled = true;
            bool _parentsEnabled = true;

            std::vector<std::weak_ptr<IObject> > _filters;

            std::shared_ptr<ResourceSystem> _resourceSystem;
            std::shared_ptr<LogSystem>      _logSystem;
            std::shared_ptr<TextSystem>     _textSystem;

            friend class Event::IEventSystem;
        };

    } // namespace System
} // namespace djv

#include <djvSystem/IObjectInline.h>

