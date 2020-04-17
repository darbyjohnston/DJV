// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene
    {
        inline PointListPrimitive::PointListPrimitive()
        {}

        inline std::shared_ptr<PointListPrimitive> PointListPrimitive::create()
        {
            return std::shared_ptr<PointListPrimitive>(new PointListPrimitive);
        }

        inline void  PointListPrimitive::setPointList(const std::shared_ptr<AV::Geom::PointList>& value)
        {
            _pointList = value;
            Core::BBox3f bbox = getBBox();
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

        inline std::string PointListPrimitive::getClassName() const
        {
            return "PointListPrimitive";
        }

        inline bool PointListPrimitive::isShaded() const
        {
            return false;
        }

        inline const std::shared_ptr<AV::Geom::PointList>& PointListPrimitive::getPointList() const
        {
            return _pointList;
        }

        inline size_t PointListPrimitive::getPointCount() const
        {
            return _pointList->v.size();
        }

    } // namespace Scene
} // namespace djv
