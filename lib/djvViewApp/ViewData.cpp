// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewData.h>

#include <djvViewApp/EnumFunc.h>

#include <djvImage/ColorFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        bool GridOptions::operator == (const GridOptions& other) const
        {
            return enabled == other.enabled &&
                size == other.size &&
                color == other.color &&
                labels == other.labels &&
                labelsColor == other.labelsColor;
        }

        bool HUDData::operator == (const HUDData& other) const
        {
            return
                fileName == other.fileName &&
                layer == other.layer &&
                size == other.size &&
                type == other.type &&
                isSequence == other.isSequence &&
                currentFrame == other.currentFrame &&
                speed == other.speed &&
                realSpeed == other.realSpeed;
        }

        bool HUDOptions::operator == (const HUDOptions& other) const
        {
            return
                enabled == other.enabled &&
                color == other.color &&
                background == other.background;
        }

        bool ViewBackgroundOptions::operator == (const ViewBackgroundOptions& other) const
        {
            return
                background == other.background &&
                color == other.color &&
                checkersSize == other.checkersSize &&
                checkersColors[0] == other.checkersColors[0] &&
                checkersColors[1] == other.checkersColors[1] &&
                border == other.border &&
                borderWidth == other.borderWidth &&
                borderColor == other.borderColor;
        }

    } // namespace ViewApp
} // namespace djv

