// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Icon.h>

#include <djvUI/IconSystem.h>

#include <djvRender2D/Render.h>

#include <djvAV/IO.h>

#include <djvImage/Image.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Icon::Private
        {
            std::shared_ptr<IconSystem> iconSystem;

            std::string name;
            ColorRole iconColorRole = ColorRole::Foreground;
            MetricsRole iconSizeRole = MetricsRole::Icon;

            std::future<std::shared_ptr<Image::Image> > imageFuture;
            std::shared_ptr<Image::Image> image;

            Math::BBox2f paintGeometry = Math::BBox2f(0.F, 0.F, 0.F, 0.F);
            glm::vec2 paintCenter = glm::vec2(0.F, 0.F);
        };

        void Icon::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            setClassName("djv::UI::Icon");
            _p->iconSystem = context->getSystemT<IconSystem>();
        }

        Icon::Icon() :
            _p(new Private)
        {}

        Icon::~Icon()
        {}

        std::shared_ptr<Icon> Icon::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<Icon>(new Icon);
            out->_init(context);
            return out;
        }

        const std::string& Icon::getIcon() const
        {
            return _p->name;
        }

        void Icon::setIcon(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (value == p.name)
                    return;
                p.imageFuture = std::future<std::shared_ptr<Image::Image> >();
                p.name = value;
                if (!p.name.empty())
                {
                    auto iconSystem = context->getSystemT<IconSystem>();
                    const auto& style = _getStyle();
                    p.imageFuture = iconSystem->getIcon(p.name, style->getMetric(MetricsRole::Icon));
                }
                else
                {
                    p.image.reset();
                }
                _resize();
            }
        }

        ColorRole Icon::getIconColorRole() const
        {
            return _p->iconColorRole;
        }

        void Icon::setIconColorRole(ColorRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.iconColorRole)
                return;
            p.iconColorRole = value;
            _redraw();
        }

        MetricsRole Icon::getIconSizeRole() const
        {
            return _p->iconSizeRole;
        }

        void Icon::setIconSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.iconSizeRole)
                return;
            p.iconSizeRole = value;
            _resize();
        }

        void Icon::_preLayoutEvent(System::Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float i = style->getMetric(p.iconSizeRole);
            glm::vec2 size(0.F, 0.F);
            if (p.image)
            {
                size.x = p.image->getWidth();
                size.y = p.image->getHeight();
            }
            size.x = std::max(size.x, i);
            size.y = std::max(size.y, i);
            _setMinimumSize(size + getMargin().getSize(style));
        }

        void Icon::_layoutEvent(System::Event::Layout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            p.paintGeometry = getMargin().bbox(getGeometry(), style);
            bool center = false;
            switch (getHAlign())
            {
            case HAlign::Center:
            case HAlign::Fill: center = true; break;
            default: break;
            }
            switch (getVAlign())
            {
            case VAlign::Center:
            case VAlign::Fill: center = true; break;
            default: break;
            }
            if (center)
            {
                p.paintCenter = p.paintGeometry.getCenter();
            }
        }

        void Icon::_paintEvent(System::Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();

            const auto& render = _getRender();
            //render->setFillColor(Image::Color(1.F, 0.F, 0.f));
            //render->drawRect(p.paintGeometry);

            if (p.image && p.image->isValid())
            {
                const uint16_t w = p.image->getWidth();
                const uint16_t h = p.image->getHeight();
                glm::vec2 pos = glm::vec2(0.F, 0.F);
                switch (getHAlign())
                {
                case HAlign::Center:
                case HAlign::Fill:   pos.x = ceilf(p.paintCenter.x - w / 2.F); break;
                case HAlign::Left:   pos.x = p.paintGeometry.min.x; break;
                case HAlign::Right:  pos.x = p.paintGeometry.max.x - w; break;
                default: break;
                }
                switch (getVAlign())
                {
                case VAlign::Center:
                case VAlign::Fill:   pos.y = ceilf(p.paintCenter.y - h / 2.F); break;
                case VAlign::Top:    pos.y = p.paintGeometry.min.y; break;
                case VAlign::Bottom: pos.y = p.paintGeometry.max.y - h; break;
                default: break;
                }
                if (p.iconColorRole != ColorRole::None)
                {
                    render->setFillColor(style->getColor(p.iconColorRole));
                    render->drawFilledImage(p.image, pos);
                }
                else
                {
                    render->setFillColor(Image::Color(1.F, 1.F, 1.F));
                    render->drawImage(p.image, pos);
                }
            }
        }

        void Icon::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().resize)
            {
                if (!p.name.empty())
                {
                    if (auto context = getContext().lock())
                    {
                        auto iconSystem = context->getSystemT<IconSystem>();
                        const auto& style = _getStyle();
                        p.imageFuture = iconSystem->getIcon(p.name, style->getMetric(MetricsRole::Icon));
                    }
                }
            }
        }

        void Icon::_updateEvent(System::Event::Update&)
        {
            DJV_PRIVATE_PTR();
            if (p.imageFuture.valid() &&
                p.imageFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.image = p.imageFuture.get();
                }
                catch (const std::exception& e)
                {
                    p.image.reset();
                    _log(e.what(), System::LogLevel::Error);
                }
                _resize();
            }
        }
            
    } // namespace UI
} // namespace djv
