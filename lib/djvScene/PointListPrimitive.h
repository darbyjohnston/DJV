// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene/IPrimitive.h>

#include <djvGeom/PointList.h>

namespace djv
{
    namespace Scene
    {
        //! This class provides a point primitive.
        class PointListPrimitive : public IPrimitive
        {
            DJV_NON_COPYABLE(PointListPrimitive);

        protected:
            PointListPrimitive();

        public:
            static std::shared_ptr<PointListPrimitive> create();

            void setPointList(const std::shared_ptr<Geom::PointList>&);

            std::string getClassName() const override;
            bool isShaded() const override;
            const std::shared_ptr<Geom::PointList>& getPointList() const override;
            size_t getPointCount() const override;

        private:
            std::shared_ptr<Geom::PointList> _pointList;
        };

    } // namespace Scene
} // namespace djv

#include <djvScene/PointListPrimitiveInline.h>
