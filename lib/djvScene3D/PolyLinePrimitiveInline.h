// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene3D
    {
        inline std::string PolyLinePrimitive::getClassName() const
        {
            return "PolyLinePrimitive";
        }

        inline bool PolyLinePrimitive::isShaded() const
        {
            return false;
        }

        inline const std::vector<std::shared_ptr<Geom::PointList> >& PolyLinePrimitive::getPolyLines() const
        {
            return _pointLists;
        }

        inline size_t PolyLinePrimitive::getPointCount() const
        {
            return _pointCount;
        }

    } // namespace Scene3D
} // namespace djv

