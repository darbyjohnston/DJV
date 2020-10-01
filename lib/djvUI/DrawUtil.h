// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/BBox.h>

#include <memory>

namespace djv
{
    namespace Image
    {
        class Color;

    } // namespace Image

    namespace Render2D
    {
        class Render;

    } // namespace Render

    namespace UI
    {
        namespace Style
        {
            class Style;

        } // namespace Style

        //! \name Drawing
        ///@{

        void drawBorder(
            const std::shared_ptr<Render2D::Render>&,
            const Math::BBox2f& box,
            float width);

        void drawCheckers(
            const std::shared_ptr<Render2D::Render>&,
            const Math::BBox2f& box,
            float size,
            const Image::Color& color0,
            const Image::Color& color1);

        glm::vec2 getCheckBoxSize(const std::shared_ptr<Style::Style>&);

        void drawCheckBox(
            const std::shared_ptr<Render2D::Render>&,
            const std::shared_ptr<Style::Style>&,
            const Math::BBox2f&,
            bool checked);

        ///@}

    } // namespace UI
} // namespace djv
