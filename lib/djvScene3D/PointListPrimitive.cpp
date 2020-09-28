// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene3D/PointListPrimitive.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene3D
    {
        PointListPrimitive::PointListPrimitive()
        {}

        std::shared_ptr<PointListPrimitive> PointListPrimitive::create()
        {
            return std::shared_ptr<PointListPrimitive>(new PointListPrimitive);
        }

        void PointListPrimitive::setPointList(const std::shared_ptr<Geom::PointList>& value)
        {
            _pointList = value;
            Math::BBox3f bbox = getBBox();
            auto i = _pointList->v.begin();
            if (i != _pointList->v.end())
            {
                bbox.min = bbox.max = *i++;
                for (; i != _pointList->v.end(); ++i)
                {
                    bbox.expand(*i);
                }
            }
            setBBox(bbox);
        }

    } // namespace Scene3D
} // namespace djv
