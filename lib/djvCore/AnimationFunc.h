// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Animation.h>

namespace djv
{
    namespace Core
    {
        namespace Animation
        {
            DJV_ENUM_HELPERS(Type);

            //! Get an animation function.
            Function getFunction(Type);

        } // namespace Animation
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::Animation::Type);
    DJV_ENUM_SERIALIZE_HELPERS(Core::Animation::Type);

} // namespace djv

