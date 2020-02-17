//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/Widget.h>

#include <djvAV/Color.h>

#include <djvCore/PicoJSON.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Image;
    
        } // namespace Image

        namespace Render
        {
            class ImageOptions;

        } // namespace Render
    } // namespace AV

    namespace ViewApp
    {
        class AnnotatePrimitive;
        class Media;

        //! This class provides grid options.
        class GridOptions
        {
        public:
            GridOptions();

            bool                enabled = false;
            float               size    = 100.F;
            AV::Image::Color    color   = AV::Image::Color(0.F, 0.F, 0.F);
            bool                labels  = true;

            bool operator == (const GridOptions&) const;
        };

        //! This class provides an image view widget.
        class ImageView : public UI::Widget
        {
            DJV_NON_COPYABLE(ImageView);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ImageView();

        public:
            ~ImageView() override;

            static std::shared_ptr<ImageView> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<Core::IValueSubject<std::shared_ptr<AV::Image::Image> > > observeImage() const;
            void setImage(const std::shared_ptr<AV::Image::Image>&);

            std::shared_ptr<Core::IValueSubject<AV::Render::ImageOptions> > observeImageOptions() const;
            void setImageOptions(const AV::Render::ImageOptions&);

            std::shared_ptr<Core::IValueSubject<glm::vec2> > observeImagePos() const;
            std::shared_ptr<Core::IValueSubject<float> > observeImageZoom() const;
            std::shared_ptr<Core::IValueSubject<UI::ImageRotate> > observeImageRotate() const;
            std::shared_ptr<Core::IValueSubject<UI::ImageAspectRatio> > observeImageAspectRatio() const;
            Core::BBox2f getImageBBox() const;
            void setImagePos(const glm::vec2&, bool animate = false);
            void setImageZoom(float, bool animate = false);
            void setImageZoomFocus(float, const glm::vec2&, bool animate = false);
            void setImageZoomFocus(float, bool animate = false);
            void setImagePosAndZoom(const glm::vec2&, float, bool animate = false);
            void setImageRotate(UI::ImageRotate);
            void setImageAspectRatio(UI::ImageAspectRatio);

            void imageFill(bool animate = false);
            void setImageFrame(const Core::BBox2f&);
            void imageFrame(bool animate = false);
            void imageCenter(bool animate = false);

            std::shared_ptr<Core::IValueSubject<GridOptions> > observeGridOptions() const;
            void setGridOptions(const GridOptions&);

            std::shared_ptr<Core::IValueSubject<AV::Image::Color> > observeBackgroundColor() const;
            void setBackgroundColor(const AV::Image::Color&);

            void setAnnotations(const std::vector<std::shared_ptr<AnnotatePrimitive> >&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;
            void _paintEvent(Core::Event::Paint &) override;

            void _initEvent(Core::Event::Init&) override;
            void _updateEvent(Core::Event::Update&) override;

        private:
            std::vector<glm::vec3> _getImagePoints() const;
            static glm::vec2 _getCenter(const std::vector<glm::vec3>&);
            static Core::BBox2f _getBBox(const std::vector<glm::vec3>&);
            void _animate(const glm::vec2&, float);
            void _posAndZoom(const glm::vec2&, float);
            void _drawGrid(float gridSize);
            void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp

    picojson::value toJSON(const ViewApp::GridOptions&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::GridOptions&);

} // namespace djv

