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

            //! Create a new image widget.
            static std::shared_ptr<ImageWidget> create(const std::shared_ptr<System::Context>&);

            //! \name Image
            ///@{

            const std::shared_ptr<Image::Image>& getImage() const;

            void setImage(const std::shared_ptr<Image::Image>&);

            ///@}

            //! \name Options
            ///@{

            const Render2D::ImageOptions& getImageOptions() const;
            ImageRotate getImageRotate() const;
            ImageAspectRatio getImageAspectRatio() const;
            ColorRole getImageColorRole() const;
            MetricsRole getImageSizeRole() const;

            void setImageOptions(const Render2D::ImageOptions&);
            void setImageRotate(ImageRotate);
            void setImageAspectRatio(ImageAspectRatio);
            void setImageColorRole(ColorRole);
            void setImageSizeRole(MetricsRole);

            ///@}

            //! \name Utility
            ///@{

            static glm::mat3x3 getXForm(
                const std::shared_ptr<Image::Image>&,
                UI::ImageRotate,
                const glm::vec2& zoom,
                UI::ImageAspectRatio);

            ///@}

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _paintEvent(System::Event::Paint&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

