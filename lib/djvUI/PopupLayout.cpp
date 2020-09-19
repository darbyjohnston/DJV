// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/PopupLayout.h>

#include <djvUI/LayoutUtil.h>

#include <djvRender2D/Render.h>

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
                UI::Popup popupDefault = UI::Popup::BelowRight;
                std::map<std::shared_ptr<IObject>, UI::Popup> popups;
            };

            void Popup::_init(const std::shared_ptr<System::Context>& context)
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

            std::shared_ptr<Popup> Popup::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Popup>(new Popup);
                out->_init(context);
                return out;
            }

            void Popup::setButton(const std::weak_ptr<Widget>& value)
            {
                DJV_PRIVATE_PTR();
                p.button = value;
                _resize();
            }

            void Popup::setPopupDefault(UI::Popup value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.popupDefault)
                    return;
                p.popupDefault = value;
                _resize();
            }

            void Popup::clearPopups()
            {
                _p->popups.clear();
            }

            void Popup::_layoutEvent(System::Event::Layout&)
            {
                DJV_PRIVATE_PTR();
                const Math::BBox2f& g = getGeometry();
                if (auto button = p.button.lock())
                {
                    for (const auto& i : getChildWidgets())
                    {
                        const auto& buttonBBox = button->getGeometry();
                        const auto& minimumSize = i->getMinimumSize();
                        UI::Popup popup = p.popupDefault;
                        auto j = p.popups.find(i);
                        if (j == p.popups.end())
                        {
                            popup = Layout::getPopup(popup, g, buttonBBox, minimumSize);
                            p.popups[i] = popup;
                        }
                        else
                        {
                            popup = j->second;
                        }
                        i->setGeometry(Layout::getPopupGeometry(popup, buttonBBox, minimumSize).intersect(g));
                    }
                }
            }

            void Popup::_paintEvent(System::Event::Paint& event)
            {
                Widget::_paintEvent(event);
                const auto& style = _getStyle();
                const float sh = style->getMetric(MetricsRole::Shadow);
                const auto& render = _getRender();
                render->setFillColor(style->getColor(ColorRole::Shadow));
                for (const auto& i : getChildWidgets())
                {
                    Math::BBox2f g = i->getGeometry();
                    g.min.x -= sh;
                    g.max.x += sh;
                    g.max.y += sh;
                    if (g.isValid())
                    {
                        render->drawShadow(g, sh);
                    }
                }
            }

            void Popup::_childRemovedEvent(System::Event::ChildRemoved& value)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.popups.find(value.getChild());
                if (i != p.popups.end())
                {
                    p.popups.erase(i);
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
