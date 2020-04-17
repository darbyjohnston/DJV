// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene/IPrimitive.h>

namespace djv
{
    namespace Scene
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

    } // namespace Scene
} // namespace djv

#include <djvScene/NullPrimitiveInline.h>
