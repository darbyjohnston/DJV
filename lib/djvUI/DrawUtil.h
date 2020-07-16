// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/UI.h>

#include <djvCore/BBox.h>

#include <memory>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Color;

        } // namespace Image

        namespace Render2D
        {
            class Render;

        } // namespace Render

    } // namespace AV

    namespace UI
    {
        namespace Style
        {
            class Style;

        } // namespace Style

        void drawBorder(
            const std::shared_ptr<AV::Render2D::Render>&,
            const Core::BBox2f& box,
            float width);

        void drawCheckers(
            const std::shared_ptr<AV::Render2D::Render>&,
            const Core::BBox2f& box,
            float size,
            const AV::Image::Color& color0,
            const AV::Image::Color& color1);

        glm::vec2 getCheckBoxSize(const std::shared_ptr<Style::Style>&);
        void drawCheckBox(
            const std::shared_ptr<AV::Render2D::Render>&,
            const std::shared_ptr<Style::Style>&,
            const Core::BBox2f&,
            bool checked);

    } // namespace UI
} // namespace djv
