// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/PopupLayout.h>

#include <djvUI/LayoutUtil.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Popup::Private
            {
                std::weak_ptr<Widget> button;
            };

            void Popup::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Layout::Popup");
            }

            Popup::Popup() :
                _p(new Private)
            {}

            Popup::~Popup()
            {}

            std::shared_ptr<Popup> Popup::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Popup>(new Popup);
                out->_init(context);
                return out;
            }

            void Popup::setButton(const std::weak_ptr<Widget>& value)
            {
                _p->button = value;
                _resize();
            }

            void Popup::_layoutEvent(Event::Layout&)
            {
                DJV_PRIVATE_PTR();
                const BBox2f& g = getGeometry();
                if (auto button = p.button.lock())
                {
                    for (const auto& i : getChildWidgets())
                    {
                        const auto& buttonBBox = button->getGeometry();
                        const auto& minimumSize = i->getMinimumSize();
                        const UI::Popup popup = Layout::getPopup(UI::Popup::BelowRight, g, buttonBBox, minimumSize);
                        i->setGeometry(Layout::getPopupGeometry(popup, buttonBBox, minimumSize).intersect(g));
                    }
                }
            }

            void Popup::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                const auto& style = _getStyle();
                const float sh = style->getMetric(MetricsRole::Shadow);
                const auto& render = _getRender();
                render->setFillColor(style->getColor(ColorRole::Shadow));
                for (const auto& i : getChildWidgets())
                {
                    BBox2f g = i->getGeometry();
                    g.min.x -= sh;
                    g.max.x += sh;
                    g.max.y += sh;
                    if (g.isValid())
                    {
                        render->drawShadow(g, sh);
                    }
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
