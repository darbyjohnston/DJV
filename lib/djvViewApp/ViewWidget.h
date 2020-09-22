// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Image
    {
        class Image;

    } // namespace Image

    namespace Render2D
    {
        class ImageOptions;

    } // namespace Render

    namespace ViewApp
    {
        struct GridOptions;
        struct HUDOptions;
        struct ViewBackgroundOptions;
        class AnnotatePrimitive;
        class Media;

        //! This class provides an image view widget.
        class ViewWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ViewWidget);

        protected:
            void _init(
                const std::shared_ptr<Media>&,
                const std::shared_ptr<System::Context>&);
            ViewWidget();

        public:
            ~ViewWidget() override;

            static std::shared_ptr<ViewWidget> create(
                const std::shared_ptr<Media>&,
                const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::IValueSubject<std::shared_ptr<Image::Image> > > observeImage() const;

            void setImage(const std::shared_ptr<Image::Image>&);

            std::shared_ptr<Core::IValueSubject<Render2D::ImageOptions> > observeImageOptions() const;
            
            void setImageOptions(const Render2D::ImageOptions&);

            std::shared_ptr<Core::IValueSubject<glm::vec2> > observeImagePos() const;
            std::shared_ptr<Core::IValueSubject<float> > observeImageZoom() const;
            std::shared_ptr<Core::IValueSubject<UI::ImageRotate> > observeImageRotate() const;
            std::shared_ptr<Core::IValueSubject<UI::ImageAspectRatio> > observeImageAspectRatio() const;
            
            Math::BBox2f getImageBBox() const;
            
            void setImagePos(const glm::vec2&, bool animate = false);
            void setImageZoom(float, bool animate = false);
            void setImageZoomFocus(float, const glm::vec2&, bool animate = false);
            void setImageZoomFocus(float, bool animate = false);
            void setImagePosAndZoom(const glm::vec2&, float, bool animate = false);
            void setImageRotate(UI::ImageRotate);
            void setImageAspectRatio(UI::ImageAspectRatio);

            void imageFrame(bool animate = false);
            void imageCenter(bool animate = false);

            std::shared_ptr<Core::IValueSubject<GridOptions> > observeGridOptions() const;
            
            void setGridOptions(const GridOptions&);

            std::shared_ptr<Core::IValueSubject<HUDOptions> > observeHUDOptions() const;
            void setHUDOptions(const HUDOptions&);

            std::shared_ptr<Core::IValueSubject<ViewBackgroundOptions> > observeBackgroundOptions() const;
            
            void setBackgroundOptions(const ViewBackgroundOptions&);

            void setAnnotations(const std::vector<std::shared_ptr<AnnotatePrimitive> >&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;
            void _paintEvent(System::Event::Paint &) override;

        private:
            std::vector<glm::vec3> _getImagePoints(bool posAndZoom = false) const;
            static glm::vec2 _getCenter(const std::vector<glm::vec3>&);
            static Math::BBox2f _getBBox(const std::vector<glm::vec3>&);

            void _animatePosAndZoom(const glm::vec2&, float);
            void _setPosAndZoom(const glm::vec2&, float);

            void _hudUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

