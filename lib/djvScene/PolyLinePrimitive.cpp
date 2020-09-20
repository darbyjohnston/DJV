// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene/PolyLinePrimitive.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene
    {
        PolyLinePrimitive::PolyLinePrimitive()
        {}

        std::shared_ptr<PolyLinePrimitive> PolyLinePrimitive::create()
        {
            return std::shared_ptr<PolyLinePrimitive>(new PolyLinePrimitive);
        }

        void PolyLinePrimitive::setPointLists(const std::vector<std::shared_ptr<Geom::PointList> >& value)
        {
            _pointLists = value;
            _pointCount = 0;
            Math::BBox3f bbox = getBBox();
            for (const auto& i : _pointLists)
            {
                auto j = i->v.begin();
                if (j != i->v.end())
                {
                    bbox.min = bbox.max = *j++;
                    for (; j != i->v.end(); ++j)
                    {
                        bbox.expand(*j);
                    }
                }
                _pointCount += i->v.size();
            }
            setBBox(bbox);
        }

        void PolyLinePrimitive::addPointList(const std::shared_ptr<Geom::PointList>& value)
        {
            _pointLists.push_back(value);
            _pointCount += value->v.size();
            Math::BBox3f bbox = getBBox();
            auto i = value->v.begin();
            if (i != value->v.end())
            {
                bbox.min = bbox.max = *i++;
                for (; i != value->v.end(); ++i)
                {
                    bbox.expand(*i);
                }
            }
            setBBox(bbox);
        }

    } // namespace Scene
} // namespace djv

