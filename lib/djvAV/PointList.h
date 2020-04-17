// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/AV.h>

#include <djvCore/BBox.h>
#include <djvCore/UID.h>

namespace djv
{
    namespace AV
    {
        namespace Geom
        {
            //! This struct provides a point list.
            class PointList
            {
            public:
                PointList();

                Core::UID getUID() const;

                //! \name Components
                ///@{

                std::vector<glm::vec3> v;
                std::vector<glm::vec3> c;

                Core::BBox3f bbox = Core::BBox3f(0.F, 0.F, 0.F, 0.F, 0.F, 0.F);

                void clear();

                ///@}

                //! \name Utilities
                ///@{

                //! Compute the bounding-box.
                void bboxUpdate();

                ///@}

            private:
                Core::UID _uid = 0;
            };

        } // namespace Geom
    } // namespace AV
} // namespace djv

#include <djvAV/PointListInline.h>
