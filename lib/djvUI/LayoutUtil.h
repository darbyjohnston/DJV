// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <djvCore/BBox.h>

namespace djv
{
    namespace UI
    {
        //! This namespace provides layout functionality.
        namespace Layout
        {
            Core::BBox2f getPopupGeometry(
                UI::Popup,
                const glm::vec2& pos,
                const glm::vec2& minimumSize);
            Core::BBox2f getPopupGeometry(
                UI::Popup,
                const Core::BBox2f& button,
                const glm::vec2& minimumSize);

            UI::Popup getPopup(
                UI::Popup,
                const Core::BBox2f& area,
                const glm::vec2& pos,
                const glm::vec2& minimumSize);
            UI::Popup getPopup(
                UI::Popup,
                const Core::BBox2f& area,
                const Core::BBox2f& button,
                const glm::vec2& minimumSize);

        } // namespace Layout
    } // namespace UI
} // namespace djv

