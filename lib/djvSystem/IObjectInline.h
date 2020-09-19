// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace System
    {
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

    } // namespace System
} // namespace djv
