// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/RowLayout.h>

#include <djvCore/Enum.h>

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            DJV_ENUM_HELPERS(RowStretch);

        } // namespace Layout

        typedef Layout::Horizontal HorizontalLayout;
        typedef Layout::Vertical VerticalLayout;
        using Layout::RowStretch;

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS(UI::Layout::RowStretch);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Layout::RowStretch);

} // namespace Gp
