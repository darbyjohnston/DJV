// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene3D/IPrimitive.h>

namespace djv
{
    namespace Scene3D
    {
        //! This class provides a null primitive.
        class NullPrimitive : public IPrimitive
        {
            DJV_NON_COPYABLE(NullPrimitive);

        protected:
            NullPrimitive();

        public:
            static std::shared_ptr<NullPrimitive> create();

            std::string getClassName() const override;
        };

    } // namespace Scene3D
} // namespace djv

#include <djvScene3D/NullPrimitiveInline.h>
