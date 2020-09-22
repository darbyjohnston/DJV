// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <glm/mat3x3.hpp>

namespace djv
{
    namespace Image
    {
        class Image;

    } // namespace Image

    namespace Render2D
    {
        class ImageOptions;

    } // namespace Render2D

    namespace UI
    {
        //! This class provides a widget that displays an image.
        class ImageWidget : public Widget
        {
            DJV_NON_COPYABLE(ImageWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ImageWidget();

        public:
            ~ImageWidget() override;

            static std::shared_ptr<ImageWidget> create(const std::shared_ptr<System::Context>&);

            const std::shared_ptr<Image::Image>& getImage() const;

            void setImage(const std::shared_ptr<Image::Image>&);
            void setImageOptions(const Render2D::ImageOptions&);
            void setImageRotate(ImageRotate);
            void setImageAspectRatio(ImageAspectRatio);

            ColorRole getImageColorRole() const;

            void setImageColorRole(ColorRole);

            MetricsRole getSizeRole() const;

            void setSizeRole(MetricsRole);

            static glm::mat3x3 getXForm(
                const std::shared_ptr<Image::Image>&,
                UI::ImageRotate,
                const glm::vec2& scale,
                UI::ImageAspectRatio);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _paintEvent(System::Event::Paint&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

