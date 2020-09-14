// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Enum.h>
#include <djvCore/Event.h>

namespace djv
{
    namespace Core
    {
        namespace Event
        {
            DJV_ENUM_HELPERS(Type);

        } // namespace Event
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::Event::Type);

} // namespace djv

