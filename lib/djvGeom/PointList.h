// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <djvMath/BBox.h>

#include <djvCore/UID.h>

#include <vector>

namespace djv
{
    namespace Geom
    {
        //! Point list.
        class PointList
        {
        public:
            PointList();

            Core::UID getUID() const;

            std::vector<glm::vec3> v;
            std::vector<glm::vec3> c;

            Math::BBox3f bbox = Math::BBox3f(0.F, 0.F, 0.F, 0.F, 0.F, 0.F);

            //! \name Utility
            ///@{

            void clear();

            //! Compute the bounding-box.
            void bboxUpdate();

            ///@}

        private:
            Core::UID _uid = 0;
        };

    } // namespace Geom
} // namespace djv

#include <djvGeom/PointListInline.h>
