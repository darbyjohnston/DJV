// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene
    {
        inline std::string PointListPrimitive::getClassName() const
        {
            return "PointListPrimitive";
        }

        inline bool PointListPrimitive::isShaded() const
        {
            return false;
        }

        inline const std::shared_ptr<Geom::PointList>& PointListPrimitive::getPointList() const
        {
            return _pointList;
        }

        inline size_t PointListPrimitive::getPointCount() const
        {
            return _pointList->v.size();
        }

    } // namespace Scene
} // namespace djv
