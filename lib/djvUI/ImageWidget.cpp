// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ImageWidget.h>

#include <djvUI/EnumFunc.h>
#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

#include <djvOCIO/OCIOSystem.h>

#include <djvImage/Image.h>

#include <djvSystem/Context.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ImageWidget::Private
        {
            std::shared_ptr<Image::Image> image;
            Render2D::ImageOptions imageOptions;
            ImageRotate imageRotate = ImageRotate::_0;
            ImageAspectRatio imageAspectRatio = ImageAspectRatio::FromSource;
            ColorRole imageColorRole = ColorRole::None;
            MetricsRole sizeRole = MetricsRole::None;
            OCIO::Config ocioConfig;
            std::string outputColorSpace;
            std::shared_ptr<Observer::Value<OCIO::Config> > ocioConfigObserver;
        };

        void ImageWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::ImageWidget");

            auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
            auto weak = std::weak_ptr<ImageWidget>(std::dynamic_pointer_cast<ImageWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.ocioConfigObserver = Observer::Value<OCIO::Config>::create(
                ocioSystem->observeCurrentConfig(),
                [weak, contextWeak](const OCIO::Config& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
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

        std::shared_ptr<ImageWidget> ImageWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ImageWidget>(new ImageWidget);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<Image::Image>& ImageWidget::getImage() const
        {
            return _p->image;
        }

        void ImageWidget::setImage(const std::shared_ptr<Image::Image>& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.image)
                return;
            p.image = value;
            _resize();
        }

        void ImageWidget::setImageOptions(const Render2D::ImageOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.imageOptions)
                return;
            p.imageOptions = value;
            _redraw();
        }

        void ImageWidget::setImageRotate(ImageRotate value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.imageRotate)
                return;
            p.imageRotate = value;
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

        glm::mat3x3 ImageWidget::getXForm(
            const std::shared_ptr<Image::Image>& image,
            UI::ImageRotate rotate,
            const glm::vec2& zoom,
            UI::ImageAspectRatio imageAspectRatio)
        {
            glm::mat3x3 m(1.F);
            m = glm::rotate(m, Math::deg2rad(UI::getImageRotate(rotate)));
            const auto& info = image->getInfo();
            m = glm::scale(m, glm::vec2(
                zoom.x * UI::getPixelAspectRatio(imageAspectRatio, info.pixelAspectRatio),
                zoom.y * UI::getAspectRatioScale(imageAspectRatio, image->getAspectRatio())));
            switch (rotate)
            {
            case ImageRotate::_90:
                m = glm::translate(m, -glm::vec2(0.F, info.size.h));
                break;
            case ImageRotate::_180:
                m = glm::translate(m, -glm::vec2(info.size.w, info.size.h));
                break;
            case ImageRotate::_270:
                m = glm::translate(m, -glm::vec2(info.size.w, 0.F));
                break;
            default: break;
            }
            return m;
        }

        void ImageWidget::_preLayoutEvent(System::Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            glm::vec2 size(0.F, 0.F);
            const auto& style = _getStyle();
            float pixelAspectRatio = 1.F;
            float aspectRatioScale = 1.F;
            if (p.image)
            {
                pixelAspectRatio = UI::getPixelAspectRatio(p.imageAspectRatio, p.image->getInfo().pixelAspectRatio);
                aspectRatioScale = UI::getAspectRatioScale(p.imageAspectRatio, p.image->getAspectRatio());
            }
            if (p.sizeRole != MetricsRole::None)
            {
                size.x = style->getMetric(p.sizeRole);
                if (p.image && p.image->getAspectRatio() > 0.F && pixelAspectRatio > 0.F)
                {
                    size.y = ceilf(size.x / p.image->getAspectRatio() / pixelAspectRatio * aspectRatioScale);
                }
                else
                {
                    size.y = ceilf(size.x / 2.F);
                }
            }
            else if (p.image)
            {
                size.x = ceilf(p.image->getWidth() * pixelAspectRatio);
                size.y = ceilf(p.image->getHeight() * aspectRatioScale);
            }
            switch (p.imageRotate)
            {
            case ImageRotate::_90:
            case ImageRotate::_270:
            {
                const float tmp = size.x;
                size.x = size.y;
                size.y = tmp;
                break;
            }
            default: break;
            }
            _setMinimumSize(size + getMargin().getSize(style));
        }

        void ImageWidget::_paintEvent(System::Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            if (p.image)
            {
                const auto& style = _getStyle();
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
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
                case HAlign::Center: pos.x = ceilf(c.x - size.x / 2.F); break;
                case HAlign::Fill:   pos.x = g.min.x; break;
                case HAlign::Left:   pos.x = g.min.x; break;
                case HAlign::Right:  pos.x = g.max.x - size.x; break;
                default: break;
                }
                switch (getVAlign())
                {
                case VAlign::Center: pos.y = ceilf(c.y - size.y / 2.F); break;
                case VAlign::Fill:   pos.y = g.min.y; break;
                case VAlign::Top:    pos.y = g.min.y; break;
                case VAlign::Bottom: pos.y = g.max.y - size.y; break;
                default: break;
                }

                Render2D::ImageOptions options = p.imageOptions;
                options.cache = Render2D::ImageCache::Dynamic;
                glm::mat3x3 m(1.F);
                m = glm::translate(m, pos);
                m *= getXForm(
                    p.image,
                    p.imageRotate,
                    glm::vec2(size.x / static_cast<float>(info.size.w), size.y / static_cast<float>(info.size.h)),
                    UI::ImageAspectRatio::Unscaled);

                auto i = p.ocioConfig.imageColorSpaces.find(p.image->getPluginName());
                if (i != p.ocioConfig.imageColorSpaces.end())
                {
                    options.colorSpace.input = i->second;
                }
                else
                {
                    i = p.ocioConfig.imageColorSpaces.find(std::string());
                    if (i != p.ocioConfig.imageColorSpaces.end())
                    {
                        options.colorSpace.input = i->second;
                    }
                }
                options.colorSpace.output = p.outputColorSpace;

                const auto& render = _getRender();
                render->pushTransform(m);
                switch (p.imageColorRole)
                {
                case ColorRole::None:
                    render->setFillColor(Image::Color(1.F, 1.F, 1.F, getOpacity(true)));
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
