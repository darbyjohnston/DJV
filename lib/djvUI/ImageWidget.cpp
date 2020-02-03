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

#include <djvUI/ImageWidget.h>

#include <djvUI/Style.h>

#include <djvAV/Image.h>
#include <djvAV/OCIOSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ImageWidget::Private
        {
            std::shared_ptr<AV::Image::Image> image;
            ImageAspectRatio imageAspectRatio = ImageAspectRatio::Default;
            AV::AlphaBlend imageAlphaBlend = AV::AlphaBlend::None;
            ColorRole imageColorRole = ColorRole::None;
            MetricsRole sizeRole = MetricsRole::None;
            AV::OCIO::Config ocioConfig;
            std::string outputColorSpace;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > ocioConfigObserver;
        };

        void ImageWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::ImageWidget");

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            auto weak = std::weak_ptr<ImageWidget>(std::dynamic_pointer_cast<ImageWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.ocioConfigObserver = ValueObserver<AV::OCIO::Config>::create(
                ocioSystem->observeCurrentConfig(),
                [weak, contextWeak](const AV::OCIO::Config& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                            widget->_p->ocioConfig = value;
                            widget->_p->outputColorSpace = ocioSystem->getColorSpace(value.display, value.view);
                            widget->_redraw();
                        }
                    }
                });
        }

        ImageWidget::ImageWidget() :
            _p(new Private)
        {}

        ImageWidget::~ImageWidget()
        {}

        std::shared_ptr<ImageWidget> ImageWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ImageWidget>(new ImageWidget);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<AV::Image::Image> & ImageWidget::getImage() const
        {
            return _p->image;
        }

        void ImageWidget::setImage(const std::shared_ptr<AV::Image::Image> & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.image)
                return;
            p.image = value;
            _resize();
        }

        void ImageWidget::setImageAspectRatio(ImageAspectRatio value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.imageAspectRatio)
                return;
            p.imageAspectRatio = value;
            _resize();
        }

        void ImageWidget::setImageAlphaBlend(AV::AlphaBlend value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.imageAlphaBlend)
                return;
            p.imageAlphaBlend = value;
            _redraw();
        }

        ColorRole ImageWidget::getImageColorRole() const
        {
            return _p->imageColorRole;
        }

        void ImageWidget::setImageColorRole(ColorRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.imageColorRole)
                return;
            p.imageColorRole = value;
            _redraw();
        }

        MetricsRole ImageWidget::getSizeRole() const
        {
            return _p->sizeRole;
        }

        void ImageWidget::setSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.sizeRole)
                return;
            p.sizeRole = value;
            _resize();
        }

        void ImageWidget::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            glm::vec2 size(0.F, 0.F);
            const auto& style = _getStyle();
            if (p.sizeRole != MetricsRole::None)
            {
                size.x = style->getMetric(p.sizeRole);
                size.y = ceilf(size.x / 2.F);
                if (p.image)
                {
                    size.y = size.x / p.image->getAspectRatio();
                }
            }
            else if (p.image)
            {
                size.x = p.image->getWidth();
                size.y = p.image->getHeight();
            }
            if (p.image)
            {
                size.x = ceilf(size.x * UI::getPixelAspectRatio(p.imageAspectRatio, p.image->getInfo().pixelAspectRatio));
                size.y = ceilf(size.y * UI::getAspectRatioScale(p.imageAspectRatio, p.image->getAspectRatio()));
            }
            _setMinimumSize(size + getMargin().getSize(style));
        }

        void ImageWidget::_paintEvent(Event::Paint & event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            if (p.image)
            {
                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                const glm::vec2 c = g.getCenter();
                const auto& info = p.image->getInfo();
                glm::vec2 size(0.F, 0.F);
                if (p.sizeRole != MetricsRole::None)
                {
                    size.x = style->getMetric(p.sizeRole);
                    size.y = size.x / p.image->getAspectRatio();
                }
                else
                {
                    size.x = info.size.w;
                    size.y = info.size.h;
                }
                size.x = ceilf(size.x * UI::getPixelAspectRatio(p.imageAspectRatio, p.image->getInfo().pixelAspectRatio));
                size.y = ceilf(size.y * UI::getAspectRatioScale(p.imageAspectRatio, p.image->getAspectRatio()));
                glm::vec2 pos = glm::vec2(0.F, 0.F);
                switch (getHAlign())
                {
                case HAlign::Center:
                    pos.x = ceilf(c.x - size.x / 2.F);
                    break;
                case HAlign::Fill:
                    pos.x = g.min.x;
                    size.x = g.w();
                    break;
                case HAlign::Left:   pos.x = g.min.x; break;
                case HAlign::Right:  pos.x = g.max.x - size.x; break;
                default: break;
                }
                switch (getVAlign())
                {
                case VAlign::Center:
                    pos.y = ceilf(c.y - size.y / 2.F);
                    break;
                case VAlign::Fill:
                    pos.y = g.min.y;
                    size.y = g.h();
                    break;
                case VAlign::Top:    pos.y = g.min.y; break;
                case VAlign::Bottom: pos.y = g.max.y - size.y; break;
                default: break;
                }

                AV::Render::ImageOptions options;
                options.alphaBlend = p.imageAlphaBlend;
                options.cache = AV::Render::ImageCache::Dynamic;
                glm::mat3x3 m(1.F);
                m = glm::translate(m, pos);
                m = glm::scale(m, glm::vec2(size.x / static_cast<float>(info.size.w), size.y / static_cast<float>(info.size.h)));
                auto i = p.ocioConfig.fileColorSpaces.find(p.image->getPluginName());
                if (i != p.ocioConfig.fileColorSpaces.end())
                {
                    options.colorSpace.input = i->second;
                }
                else
                {
                    i = p.ocioConfig.fileColorSpaces.find(std::string());
                    if (i != p.ocioConfig.fileColorSpaces.end())
                    {
                        options.colorSpace.input = i->second;
                    }
                }
                options.colorSpace.output = p.outputColorSpace;

                auto render = _getRender();
                render->pushTransform(m);
                switch (p.imageColorRole)
                {
                case ColorRole::None:
                    render->setFillColor(AV::Image::Color(1.F, 1.F, 1.F, getOpacity(true)));
                    render->drawImage(p.image, glm::vec2(0.F, 0.F), options);
                    break;
                default:
                    render->setFillColor(style->getColor(p.imageColorRole));
                    render->drawFilledImage(p.image, glm::vec2(0.F, 0.F), options);
                    break;
                }
                render->popTransform();
            }
        }

    } // namespace UI
} // namespace djv
