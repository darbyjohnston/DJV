// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Image;

        } // namespace Image

        namespace Render2D
        {
            class ImageOptions;

        } // namespace Render2D
    } // namespace AV

    namespace UI
    {
        //! This class provides a widget that displays an image.
        class ImageWidget : public Widget
        {
            DJV_NON_COPYABLE(ImageWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ImageWidget();

        public:
            virtual ~ImageWidget();

            static std::shared_ptr<ImageWidget> create(const std::shared_ptr<Core::Context>&);

            const std::shared_ptr<AV::Image::Image>& getImage() const;
            void setImage(const std::shared_ptr<AV::Image::Image>&);
            void setImageOptions(const AV::Render2D::ImageOptions&);
            void setImageRotate(ImageRotate);
            void setImageAspectRatio(ImageAspectRatio);

            ColorRole getImageColorRole() const;
            void setImageColorRole(ColorRole);

            MetricsRole getSizeRole() const;
            void setSizeRole(MetricsRole);

            static glm::mat3x3 getXForm(
                const std::shared_ptr<AV::Image::Image>&,
                UI::ImageRotate,
                const glm::vec2& scale,
                UI::ImageAspectRatio);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _paintEvent(Core::Event::Paint &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

