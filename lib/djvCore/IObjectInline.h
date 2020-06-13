// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        inline IObject::IObject()
        {}

        inline const std::weak_ptr<Context>& IObject::getContext() const
        {
            return _context;
        }

        inline const std::string& IObject::getClassName() const
        {
            return _className;
        }

        inline void IObject::setClassName(const std::string& name)
        {
            _className = name;
        }

        inline const std::string& IObject::getObjectName() const
        {
            return _objectName;
        }

        inline void IObject::setObjectName(const std::string& name)
        {
            _objectName = name;
        }

        inline const std::weak_ptr<IObject>& IObject::getParent() const
        {
            return _parent;
        }

        template<typename T>
        inline std::shared_ptr<T> IObject::getParentRecursiveT() const
        {
            for (auto parent = _parent.lock(); parent.get(); parent = parent->_parent.lock())
            {
                if (auto parentT = std::dynamic_pointer_cast<T>(parent))
                {
                    return parentT;
                }
            }
            return nullptr;
        }

        inline const std::vector<std::shared_ptr<IObject> >& IObject::getChildren() const
        {
            return _children;
        }

        template<typename T>
        inline std::vector<std::shared_ptr<T> > IObject::getChildrenT() const
        {
            std::vector<std::shared_ptr<T> > out;
            for (const auto& child : _children)
            {
                if (auto childT = std::dynamic_pointer_cast<T>(child))
                {
                    out.push_back(childT);
                }
            }
            return out;
        }

        template<typename T>
        inline std::vector<std::shared_ptr<T> > IObject::getChildrenRecursiveT() const
        {
            std::vector<std::shared_ptr<T> > out;
            for (const auto& child : _children)
            {
                _getChildrenRecursiveT(child, out);
            }
            return out;
        }

        template <typename T>
        inline std::shared_ptr<T> IObject::getFirstChildT() const
        {
            for (const auto& child : _children)
            {
                if (auto childT = std::dynamic_pointer_cast<T>(child))
                {
                    return childT;
                }
            }
            return nullptr;
        }

        template<typename T>
        inline std::shared_ptr<T> IObject::getFirstChildRecursiveT() const
        {
            std::shared_ptr<T> out;
            for (const auto& child : _children)
            {
                _getFirstChildRecursiveT(child, out);
                if (out.get())
                {
                    break;
                }
            }
            return out;
        }

        inline bool IObject::isEnabled(bool parents) const
        {
            return parents ? (_parentsEnabled && _enabled) : _enabled;
        }

        inline const std::shared_ptr<ResourceSystem>& IObject::_getResourceSystem() const
        {
            return _resourceSystem;
        }

        inline const std::shared_ptr<LogSystem>& IObject::_getLogSystem() const
        {
            return _logSystem;
        }

        inline const std::shared_ptr<TextSystem>& IObject::_getTextSystem() const
        {
            return _textSystem;
        }

        template<typename T>
        inline void IObject::_getChildrenRecursiveT(const std::shared_ptr<IObject>& value, std::vector<std::shared_ptr<T> >& out)
        {
            if (auto valueT = std::dynamic_pointer_cast<T>(value))
            {
                out.push_back(valueT);
            }
            for (const auto& child : value->_children)
            {
                _getChildrenRecursiveT(child, out);
            }
        }

        template<typename T>
        inline void IObject::_getFirstChildRecursiveT(const std::shared_ptr<IObject>& value, std::shared_ptr<T>& out)
        {
            if (auto valueT = std::dynamic_pointer_cast<T>(value))
            {
                out = valueT;
                return;
            }
            for (const auto& child : value->_children)
            {
                _getFirstChildRecursiveT(child, out);
            }
        }

    } // namespace Core
} // namespace djv
