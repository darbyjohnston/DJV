// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvGeom/PointList.h>

#include <djvCore/UIDFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace Geom
    {
        PointList::PointList() :
            _uid(createUID())
        {}

        void PointList::clear()
        {
            v.clear();
            c.clear();
        }

        void PointList::bboxUpdate()
        {
            bbox.zero();
            if (!v.empty())
            {
                bbox.min = bbox.max = v[0];
                const size_t size = v.size();
                for (size_t i = 1; i < size; ++i)
                {
                    bbox.expand(v[i]);
                }
            }
        }

    } // namespace Geom
} // namespace djv
