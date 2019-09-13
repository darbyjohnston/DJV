//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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
            struct ImageOptions;

        } // namespace Render
    } // namespace AV

    namespace ViewApp
    {
        class Media;

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

            const std::shared_ptr<AV::Image::Image>& getImage() const;
            void setImage(const std::shared_ptr<AV::Image::Image>&);

            std::shared_ptr<Core::IValueSubject<AV::Render::ImageOptions> > observeImageOptions() const;
            std::shared_ptr<Core::IValueSubject<std::string> > observeColorDisplay() const;
            std::shared_ptr<Core::IValueSubject<std::string> > observeColorView() const;
            void setImageOptions(const AV::Render::ImageOptions&);
            void setColorDisplay(const std::string&);
            void setColorView(const std::string&);

            std::shared_ptr<Core::IValueSubject<glm::vec2> > observeImagePos() const;
            std::shared_ptr<Core::IValueSubject<float> > observeImageZoom() const;
            std::shared_ptr<Core::IValueSubject<ImageRotate> > observeImageRotate() const;
            std::shared_ptr<Core::IValueSubject<ImageAspectRatio> > observeImageAspectRatio() const;
            Core::BBox2f getImageBBox() const;
            float getPixelAspectRatio() const;
            float getAspectRatioScale() const;
            void setImagePos(const glm::vec2&);
            void setImageZoom(float);
            void setImageZoomFocus(float, const glm::vec2 &);
            void setImagePosAndZoom(const glm::vec2&, float);
            void setImageRotate(ImageRotate);
            void setImageAspectRatio(ImageAspectRatio);

            void imageFull();
            void setImageFrame(const Core::BBox2f&);
            void imageFrame();
            void imageCenter();

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;
            void _paintEvent(Core::Event::Paint &) override;

        private:
            std::vector<glm::vec3> _getImagePoints() const;
            static glm::vec2 _getCenter(const std::vector<glm::vec3>&);
            static Core::BBox2f _getBBox(const std::vector<glm::vec3>&);
            void _colorSpaceUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

