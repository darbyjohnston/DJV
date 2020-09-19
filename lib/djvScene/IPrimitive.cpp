// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene/IPrimitive.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene
    {
        std::vector<std::shared_ptr<Geom::TriangleMesh> > IPrimitive::_meshesDummy;
        std::vector<std::shared_ptr<Geom::PointList> > IPrimitive::_polyLinesDummy;
        std::shared_ptr<Geom::PointList> IPrimitive::_pointListDummy;

        void IPrimitive::addChild(const std::shared_ptr<IPrimitive>& value)
        {
            if (auto prevParent = value->getParent().lock())
            {
                prevParent->removeChild(value);
            }
            value->_parent = std::dynamic_pointer_cast<IPrimitive>(shared_from_this());
            _children.push_back(value);
        }

        void IPrimitive::removeChild(const std::shared_ptr<IPrimitive>& value)
        {
            const auto i = std::find(_children.begin(), _children.end(), value);
            if (i != _children.end())
            {
                (*i)->_parent.reset();
                _children.erase(i);
            }
        }

        void IPrimitive::clearChildren()
        {
            for (auto& i : _children)
            {
                i->_parent.reset();
            }
            _children.clear();
        }

    } // namespace Scene
} // namespace djv

