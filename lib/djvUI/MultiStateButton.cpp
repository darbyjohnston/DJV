// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvUI/MultiStateButton.h>

#include <djvUI/IconWidget.h>
#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            struct MultiState::Private
            {
                std::vector<std::string> icons;
                std::shared_ptr<IconWidget> iconWidget;
                int currentIndex = -1;
                std::function<void(int)> currentCallback;
                MetricsRole insideMargin = MetricsRole::MarginInside;
                System::Event::PointerID pressedID = System::Event::invalidID;
                glm::vec2 pressedPos = glm::vec2(0.F, 0.F);
                bool canRejectPressed = true;
            };

            void MultiState::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Button::MultiState");
                setPointerEnabled(true);
                setVAlign(VAlign::Center);

                p.iconWidget = IconWidget::create(context);
                addChild(p.iconWidget);
            }

            MultiState::MultiState() :
                _p(new Private)
            {}

            MultiState::~MultiState()
            {}

            std::shared_ptr<MultiState> MultiState::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<MultiState>(new MultiState);
                out->_init(context);
                return out;
            }

            int MultiState::getCurrentIndex() const
            {
                return _p->currentIndex;
            }

            void MultiState::setCurrentIndex(int value)
            {
                DJV_PRIVATE_PTR();
                const size_t size = p.icons.size();
                if (size)
                {
                    value = std::min(value, static_cast<int>(size) - 1);
                }
                else
                {
                    value = -1;
                }
                if (value == p.currentIndex)
                    return;
                p.currentIndex = value;
                _widgetUpdate();
            }

            void MultiState::addIcon(const std::string& name)
            {
                DJV_PRIVATE_PTR();
                p.icons.push_back(name);
                if (-1 == p.currentIndex)
                {
                    setCurrentIndex(0);
                }
                _widgetUpdate();
            }

            void MultiState::clearIcons()
            {
                _p->icons.clear();
                setCurrentIndex(-1);
                _widgetUpdate();
            }

            void MultiState::setIconSizeRole(MetricsRole value)
            {
                _p->iconWidget->setIconSizeRole(value);
            }

            void MultiState::setCurrentCallback(const std::function<void(int)>& callback)
            {
                _p->currentCallback = callback;
            }

            MetricsRole MultiState::getInsideMargin() const
            {
                return _p->insideMargin;
            }

            void MultiState::setInsideMargin(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.insideMargin)
                    return;
                p.insideMargin = value;
                _resize();
            }
            
            void MultiState::_preLayoutEvent(System::Event::PreLayout&)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float m = style->getMetric(p.insideMargin);
                glm::vec2 size = p.iconWidget->getMinimumSize();
                size += m * 2.F;
                _setMinimumSize(size + getMargin().getSize(style));
            }

            void MultiState::_layoutEvent(System::Event::Layout&)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
                const float m = style->getMetric(p.insideMargin);
                p.iconWidget->setGeometry(g.margin(-m));
            }

            void MultiState::_paintEvent(System::Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
                const auto& render = _getRender();
                if (p.pressedID != System::Event::invalidID)
                {
                    render->setFillColor(style->getColor(ColorRole::Pressed));
                    render->drawRect(g);
                }
                else if (isEnabled(true) && _getPointerHover().size())
                {
                    render->setFillColor(style->getColor(ColorRole::Hovered));
                    render->drawRect(g);
                }
            }

            void MultiState::_pointerEnterEvent(System::Event::PointerEnter& event)
            {
                event.accept();
                if (isEnabled(true))
                {
                    _redraw();
                }
            }

            void MultiState::_pointerLeaveEvent(System::Event::PointerLeave& event)
            {
                event.accept();
                if (isEnabled(true))
                {
                    _redraw();
                }
            }

            void MultiState::_pointerMoveEvent(System::Event::PointerMove& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                const auto id = event.getPointerInfo().id;
                const auto& pos = event.getPointerInfo().projectedPos;
                if (id == p.pressedID)
                {
                    const float distance = glm::length(pos - p.pressedPos);
                    const auto& style = _getStyle();
                    const bool accepted = p.canRejectPressed ? distance < style->getMetric(MetricsRole::Drag) : true;
                    event.setAccepted(accepted);
                    if (!accepted)
                    {
                        p.pressedID = System::Event::invalidID;
                        _redraw();
                    }
                }
            }

            void MultiState::_buttonPressEvent(System::Event::ButtonPress& event)
            {
                DJV_PRIVATE_PTR();
                if (p.pressedID)
                    return;
                event.accept();
                const auto& pointerInfo = event.getPointerInfo();
                p.pressedID = pointerInfo.id;
                p.pressedPos = pointerInfo.projectedPos;
                _redraw();
            }

            void MultiState::_buttonReleaseEvent(System::Event::ButtonRelease& event)
            {
                DJV_PRIVATE_PTR();
                const auto& pointerInfo = event.getPointerInfo();
                if (pointerInfo.id == p.pressedID)
                {
                    event.accept();
                    p.pressedID = System::Event::invalidID;
                    const Math::BBox2f& g = getGeometry();
                    const auto& hover = _getPointerHover();
                    const auto i = hover.find(pointerInfo.id);
                    if (i != hover.end() && g.contains(i->second))
                    {
                        int index = p.currentIndex + 1;
                        if (index > static_cast<int>(p.icons.size()) - 1)
                        {
                            index = 0;
                        }
                        setCurrentIndex(index);
                        _redraw();
                        if (p.currentCallback)
                        {
                            p.currentCallback(p.currentIndex);
                        }
                    }
                }
            }

            void MultiState::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                std::string icon;
                if (p.currentIndex >= 0 && p.currentIndex < static_cast<int>(p.icons.size()))
                {
                    icon = p.icons[p.currentIndex];
                }
                p.iconWidget->setIcon(icon);
            }

        } // namespace Button
    } // namespace UI
} // namespace djv
