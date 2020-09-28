// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene3D/NullPrimitive.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene3D
    {
        NullPrimitive::NullPrimitive()
        {}

        std::shared_ptr<NullPrimitive> NullPrimitive::create()
        {
            return std::shared_ptr<NullPrimitive>(new NullPrimitive);
        }

    } // namespace Scene3D
} // namespace djv

