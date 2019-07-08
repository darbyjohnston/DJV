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

#include <djvViewApp/ImageView.h>

#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/ImageViewSettings.h>

#include <djvUI/Action.h>
#include <djvUI/ColorSpaceSettings.h>
#include <djvUI/SettingsSystem.h>

#include <djvAV/Image.h>
#include <djvAV/Render2D.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageView::Private
        {
            std::shared_ptr<AV::Image::Image> image;
            std::shared_ptr<ValueSubject<AV::Render::ImageOptions> > imageOptions;
            std::shared_ptr<ValueSubject<std::string> > colorDisplay;
            std::shared_ptr<ValueSubject<std::string> > colorView;
            std::shared_ptr<ValueSubject<glm::vec2> > imagePos;
            std::shared_ptr<ValueSubject<float> > imageZoom;
            std::shared_ptr<ValueSubject<ImageRotate> > imageRotate;
            std::shared_ptr<ValueSubject<ImageAspectRatio> > imageAspectRatio;
            ImageViewLock lock = ImageViewLock::None;
            AV::Image::Color backgroundColor = AV::Image::Color(0.f, 0.f, 0.f);
            glm::vec2 pressedImagePos = glm::vec2(0.f, 0.f);
            std::shared_ptr<ValueObserver<ImageViewLock> > lockObserver;
            std::shared_ptr<ValueObserver<AV::Image::Color> > backgroundColorObserver;
        };

        void ImageView::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::ViewApp::ImageView");

            DJV_PRIVATE_PTR();
            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto colorSpaceSettings = settingsSystem->getSettingsT<UI::Settings::ColorSpace>();
            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
            AV::Render::ImageOptions imageOptions;
            imageOptions.colorSpace = AV::OCIO::Convert(
                colorSpaceSettings->observeColorSpace()->get(),
                colorSpaceSettings->observeOutputColorSpace()->get());
            p.imageOptions = ValueSubject<AV::Render::ImageOptions>::create(imageOptions);
            p.colorDisplay = ValueSubject<std::string>::create(colorSpaceSettings->observeDisplay()->get());
            p.colorView = ValueSubject<std::string>::create(colorSpaceSettings->observeView()->get());
            p.imagePos = ValueSubject<glm::vec2>::create();
            p.imageZoom = ValueSubject<float>::create();
            p.imageRotate = ValueSubject<ImageRotate>::create(imageSettings->observeImageRotate()->get());
            p.imageAspectRatio = ValueSubject<ImageAspectRatio>::create(imageSettings->observeImageAspectRatio()->get());

            auto weak = std::weak_ptr<ImageView>(std::dynamic_pointer_cast<ImageView>(shared_from_this()));
            auto imageViewSettings = settingsSystem->getSettingsT<ImageViewSettings>();
            p.lockObserver = ValueObserver<ImageViewLock>::create(
                imageViewSettings->observeLock(),
                [weak](ImageViewLock value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->lock = value;
                        if (widget->isVisible() && !widget->isClipped())
                        {
                            widget->_resize();
                        }
                    }
                });

            p.backgroundColorObserver = ValueObserver<AV::Image::Color>::create(
                imageViewSettings->observeBackgroundColor(),
                [weak](const AV::Image::Color& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundColor = value;
                        if (widget->isVisible() && !widget->isClipped())
                        {
                            widget->_redraw();
                        }
                    }
                });
        }

        ImageView::ImageView() :
            _p(new Private)
        {}

        ImageView::~ImageView()
        {}

        std::shared_ptr<ImageView> ImageView::create(Context * context)
        {
            auto out = std::shared_ptr<ImageView>(new ImageView);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<AV::Image::Image> & ImageView::getImage() const
        {
            return _p->image;
        }

        void ImageView::setImage(const std::shared_ptr<AV::Image::Image>& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.image)
                return;
            p.image = value;
            if (isVisible() && !isClipped())
            {
                _resize();
            }
        }

        std::shared_ptr<IValueSubject<AV::Render::ImageOptions> > ImageView::observeImageOptions() const
        {
            return _p->imageOptions;
        }

        std::shared_ptr<Core::IValueSubject<std::string> > ImageView::observeColorDisplay() const
        {
            return _p->colorDisplay;
        }

        std::shared_ptr<Core::IValueSubject<std::string> > ImageView::observeColorView() const
        {
            return _p->colorView;
        }

        void ImageView::setImageOptions(const AV::Render::ImageOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageOptions->setIfChanged(value))
            {
                if (isVisible() && !isClipped())
                {
                    _resize();
                }
            }
        }

        void ImageView::setColorDisplay(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            p.colorDisplay->setIfChanged(value);
        }

        void ImageView::setColorView(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            p.colorView->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<glm::vec2> > ImageView::observeImagePos() const
        {
            return _p->imagePos;
        }

        std::shared_ptr<IValueSubject<float> > ImageView::observeImageZoom() const
        {
            return _p->imageZoom;
        }

        std::shared_ptr<IValueSubject<ImageRotate> > ImageView::observeImageRotate() const
        {
            return _p->imageRotate;
        }

        std::shared_ptr<IValueSubject<ImageAspectRatio> > ImageView::observeImageAspectRatio() const
        {
            return _p->imageAspectRatio;
        }

        void ImageView::setImagePos(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            if (p.imagePos->setIfChanged(value))
            {
                _resize();
            }
        }

        void ImageView::setImageZoom(float value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageZoom->setIfChanged(value))
            {
                _resize();
            }
        }

        void ImageView::setImageZoomFocus(float value, const glm::vec2& mouse)
        {
            DJV_PRIVATE_PTR();
            setImagePosAndZoom(
                mouse + (p.imagePos->get() - mouse) * (value / p.imageZoom->get()),
                value);
        }

        void ImageView::setImagePosAndZoom(const glm::vec2& pos, float zoom)
        {
            DJV_PRIVATE_PTR();
            const bool posChanged = p.imagePos->setIfChanged(pos);
            const bool zoomChanged = p.imageZoom->setIfChanged(zoom);
            if (posChanged || zoomChanged)
            {
                _resize();
            }
        }

        void ImageView::setImageRotate(ImageRotate value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageRotate->setIfChanged(value))
            {
                _resize();
            }
        }

        void ImageView::setImageAspectRatio(ImageAspectRatio value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageAspectRatio->setIfChanged(value))
            {
                _resize();
            }
        }

        void ImageView::imageFit()
        {
            DJV_PRIVATE_PTR();
            if (p.image)
            {
                const BBox2f& g = getGeometry();
                const auto pts = _getXFormPoints();
                const glm::vec2 c = _getCenter(pts);
                const BBox2f bbox = _getBBox(pts);
                float zoom = g.w() / static_cast<float>(bbox.w());
                if (zoom * bbox.h() > g.h())
                {
                    zoom = g.h() / static_cast<float>(bbox.h());
                }
                setImagePosAndZoom(
                    glm::vec2(
                        g.w() / 2.f - c.x * zoom,
                        g.h() / 2.f - c.y * zoom),
                    zoom);
            }
        }

        void ImageView::imageCenter()
        {
            DJV_PRIVATE_PTR();
            if (p.image)
            {
                const BBox2f& g = getGeometry();
                const glm::vec2 c = _getCenter(_getXFormPoints());
                setImagePosAndZoom(
                    glm::vec2(
                        g.w() / 2.f - c.x,
                        g.h() / 2.f - c.y),
                    1.f);
            }
        }

        void ImageView::_preLayoutEvent(Event::PreLayout & event)
        {
            const auto& style = _getStyle();
            const float sa = style->getMetric(UI::MetricsRole::ScrollArea);
            _setMinimumSize(glm::vec2(sa, sa));
        }

        void ImageView::_layoutEvent(Event::Layout &)
        {
            DJV_PRIVATE_PTR();
            switch (p.lock)
            {
            case ImageViewLock::Fit:    imageFit();    break;
            case ImageViewLock::Center: imageCenter(); break;
            default: break;
            }
        }

        void ImageView::_paintEvent(Event::Paint &)
        {
            DJV_PRIVATE_PTR();
            if (p.image)
            {
                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                auto render = _getRender();
                render->setFillColor(p.backgroundColor);
                render->drawRect(g);
                render->setFillColor(AV::Image::Color(1.f, 1.f, 1.f));

                glm::mat3x3 m(1.f);
                m = glm::translate(m, g.min + p.imagePos->get());
                m = glm::rotate(m, Math::deg2rad(getImageRotate(p.imageRotate->get())));
                m = glm::scale(m, glm::vec2(p.imageZoom->get(), p.imageZoom->get() * _getImageAspectRatioScale()));
                render->pushTransform(m);
                AV::Render::ImageOptions options(p.imageOptions->get());
                options.cache = AV::Render::ImageCache::Dynamic;
                render->drawImage(p.image, glm::vec2(0.f, 0.f), options);
                render->popTransform();
            }
        }

        float ImageView::_getImageAspectRatioScale() const
        {
            DJV_PRIVATE_PTR();
            float out = 1.f;
            switch (p.imageAspectRatio->get())
            {
            case ImageAspectRatio::Auto:
                //! \todo Automatic aspect ratio.
                break;
            case ImageAspectRatio::_16_9:
            case ImageAspectRatio::_1_85:
            case ImageAspectRatio::_2_35:
                out = p.image->getAspectRatio() / getImageAspectRatio(p.imageAspectRatio->get());
                break;
            default: break;
            }
            return out;
        }

        std::vector<glm::vec3> ImageView::_getXFormPoints() const
        {
            DJV_PRIVATE_PTR();
            std::vector<glm::vec3> out;
            if (p.image)
            {
                const AV::Image::Size& imageSize = p.image->getSize();
                glm::mat3x3 m(1.f);
                m = glm::rotate(m, Math::deg2rad(getImageRotate(p.imageRotate->get())));
                m = glm::scale(m, glm::vec2(1.f, _getImageAspectRatioScale()));
                out.resize(4);
                out[0].x = 0.f;
                out[0].y = 0.f;
                out[0].z = 1.f;
                out[1].x = 0.f + imageSize.w;
                out[1].y = 0.f;
                out[1].z = 1.f;
                out[2].x = 0.f + imageSize.w;
                out[2].y = 0.f + imageSize.h;
                out[2].z = 1.f;
                out[3].x = 0.f;
                out[3].y = 0.f + imageSize.h;
                out[3].z = 1.f;
                for (auto& i : out)
                {
                    i = m * i;
                }
            }
            return out;
        }

        glm::vec2 ImageView::_getCenter(const std::vector<glm::vec3>& value)
        {
            glm::vec2 out(0.f, 0.f);
            if (value.size())
            {
                for (const auto& i : value)
                {
                    out.x += i.x;
                    out.y += i.y;
                }
                out.x /= static_cast<float>(value.size());
                out.y /= static_cast<float>(value.size());
            }
            return out;
        }
        
        BBox2f ImageView::_getBBox(const std::vector<glm::vec3>& value)
        {
            BBox2f out(0.f, 0.f, 0.f, 0.f);
            if (value.size())
            {
                out.min = out.max = value[0];
                for (size_t i = 1; i < 4; ++i)
                {
                    out.min.x = std::min(out.min.x, value[i].x);
                    out.max.x = std::max(out.max.x, value[i].x);
                    out.min.y = std::min(out.min.y, value[i].y);
                    out.max.y = std::max(out.max.y, value[i].y);
                }
            }
            return out;
        }

    } // namespace ViewApp
} // namespace djv

