// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/Animation.h>

#include <djvCore/Enum.h>

namespace djv
{
    namespace System
    {
        namespace Animation
        {
            DJV_ENUM_HELPERS(Type);

            //! Get an animation function.
            Function getFunction(Type);

        } // namespace Animation
    } // namespace System

    DJV_ENUM_SERIALIZE_HELPERS(System::Animation::Type);
    DJV_ENUM_SERIALIZE_HELPERS(System::Animation::Type);

} // namespace djv

