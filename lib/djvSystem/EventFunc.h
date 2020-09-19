// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/Event.h>

#include <djvCore/Enum.h>

namespace djv
{
    namespace System
    {
        namespace Event
        {
            DJV_ENUM_HELPERS(Type);

        } // namespace Event
    } // namespace System

    DJV_ENUM_SERIALIZE_HELPERS(System::Event::Type);

} // namespace djv

