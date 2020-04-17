// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene/IPrimitive.h>

#include <djvAV/PointList.h>

namespace djv
{
    namespace Scene
    {
        //! This class provides a poly-line primitive.
        class PolyLinePrimitive : public IPrimitive
        {
            DJV_NON_COPYABLE(PolyLinePrimitive);

        protected:
            PolyLinePrimitive();

        public:
            static std::shared_ptr<PolyLinePrimitive> create();

            void setPointLists(const std::vector<std::shared_ptr<AV::Geom::PointList> >&);
            void addPointList(const std::shared_ptr<AV::Geom::PointList>&);

            std::string getClassName() const override;
            bool isShaded() const override;
            const std::vector<std::shared_ptr<AV::Geom::PointList> >& getPolyLines() const override;
            size_t getPointCount() const override;

        private:
            std::vector<std::shared_ptr<AV::Geom::PointList> > _pointLists;
            size_t _pointCount = 0;
        };

    } // namespace Scene
} // namespace djv

#include <djvScene/PolyLinePrimitiveInline.h>
