// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/SoloLayout.h>

#include <djvUI/LayoutUtil.h>

#include <djvCore/Animation.h>

#include <glm/glm.hpp>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            namespace
            {
                //! \todo Should this be configurable?
                const size_t animationTime = 150;

            } // namespace

            struct Solo::Private
            {
                std::shared_ptr<Widget> currentWidget;
                SoloMinimumSize soloMinimumSize = SoloMinimumSize::Both;
                Side side = Side::First;
                std::shared_ptr<Widget> prevWidget;
                float animationValue = 0.F;
                std::shared_ptr<Animation::Animation> animation;
            };

            void Solo::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Layout::Solo");

                _widgetUpdate();

                p.animation = Animation::Animation::create(context);
                p.animation->setType(Animation::Type::SmoothStep);
            }

            Solo::Solo() :
                _p(new Private)
            {}

            Solo::~Solo()
            {}

            std::shared_ptr<Solo> Solo::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Solo>(new Solo);
                out->_init(context);
                return out;
            }

            const std::shared_ptr<Widget>& Solo::getCurrentWidget() const
            {
                return _p->currentWidget;
            }

            void Solo::setCurrentWidget(const std::shared_ptr<Widget>& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.currentWidget)
                    return;
                p.prevWidget.reset();
                if (p.currentWidget)
                {
                    p.currentWidget->hide();
                }
                p.currentWidget = value;
                _widgetUpdate();
            }

            void Solo::setCurrentWidget(const std::shared_ptr<Widget>& value, Side side)
            {
                DJV_PRIVATE_PTR();
                const auto& childWidgets = getChildWidgets();
                const auto i = std::find(childWidgets.begin(), childWidgets.end(), value);
                if (i != childWidgets.end() && *i != p.currentWidget)
                {
                    p.side = side;
                    if (p.prevWidget)
                    {
                        p.prevWidget->hide();
                    }
                    p.prevWidget = p.currentWidget;
                    p.currentWidget = *i;
                    p.currentWidget->moveToFront();
                    p.currentWidget->show();
                    if (p.animationValue > 0.F)
                    {
                        p.animationValue = 1.F - p.animationValue;
                    }
                    auto weak = std::weak_ptr<Solo>(std::dynamic_pointer_cast<Solo>(shared_from_this()));
                    p.animation->start(
                        p.animationValue,
                        1.F,
                        std::chrono::milliseconds(static_cast<size_t>(animationTime * std::max(0.F, 1.F - p.animationValue))),
                        [weak](float value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->animationValue = value;
                                widget->_resize();
                            }
                        },
                        [weak](float value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->prevWidget->hide();
                                widget->_p->prevWidget.reset();
                                widget->_p->animationValue = value;
                            }
                        });
                }
            }

            SoloMinimumSize Solo::getSoloMinimumSize() const
            {
                return _p->soloMinimumSize;
            }

            void Solo::setSoloMinimumSize(SoloMinimumSize value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.soloMinimumSize)
                    return;
                p.soloMinimumSize = value;
                _resize();
            }

            float Solo::getHeightForWidth(float value) const
            {
                DJV_PRIVATE_PTR();
                float out = 0.F;
                const auto& style = _getStyle();
                const glm::vec2 m = getMargin().getSize(style);
                for (const auto& child : getChildWidgets())
                {
                    if (child->isVisible() ||
                        SoloMinimumSize::Vertical == p.soloMinimumSize ||
                        SoloMinimumSize::Both == p.soloMinimumSize)
                    {
                        out = std::max(out, child->getHeightForWidth(value - m.x));
                    }
                }
                out += m.y;
                return out;
            }

            void Solo::addChild(const std::shared_ptr<IObject>& value)
            {
                Widget::addChild(value);
                DJV_PRIVATE_PTR();
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    if (!p.currentWidget)
                    {
                        p.prevWidget.reset();
                        p.currentWidget = widget;
                    }
                    else
                    {
                        widget->hide();
                    }
                }
                _widgetUpdate();
            }

            void Solo::removeChild(const std::shared_ptr<IObject>& value)
            {
                const auto& childWidgets = getChildWidgets();
                const auto i = std::find(childWidgets.begin(), childWidgets.end(), value);
                std::shared_ptr<Widget> currentWidget;
                if (i != childWidgets.end() && i != childWidgets.begin())
                {
                    currentWidget = *(i - 1);
                }
                Widget::removeChild(value);
                setCurrentWidget(currentWidget);
            }

            void Solo::_preLayoutEvent(Event::PreLayout&)
            {
                DJV_PRIVATE_PTR();
                glm::vec2 minimumSize = glm::vec2(0.F, 0.F);
                for (const auto& child : getChildWidgets())
                {
                    if (child->isVisible() || p.soloMinimumSize != SoloMinimumSize::None)
                    {
                        const glm::vec2& childMinimumSize = child->getMinimumSize();
                        switch (p.soloMinimumSize)
                        {
                        case SoloMinimumSize::Horizontal: minimumSize.x = glm::max(minimumSize.x, childMinimumSize.x); break;
                        case SoloMinimumSize::Vertical:   minimumSize.y = glm::max(minimumSize.y, childMinimumSize.y); break;
                        case SoloMinimumSize::Both:       minimumSize   = glm::max(minimumSize, child->getMinimumSize()); break;
                        default: break;
                        }
                    }
                }
                const auto& style = _getStyle();
                _setMinimumSize(minimumSize + getMargin().getSize(style));
            }

            void Solo::_layoutEvent(Event::Layout&)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                const auto& childWidgets = getChildWidgets();
                for (const auto& child : childWidgets)
                {
                    child->setGeometry(getAlign(g, child->getMinimumSize(), child->getHAlign(), child->getVAlign()));
                }
                if (p.currentWidget && p.prevWidget)
                {
                    const float w = g.w();
                    const float h = g.h();
                    BBox2f currentG(g);
                    BBox2f prevG(g);
                    switch (p.side)
                    {
                    case Side::Left:
                        currentG = BBox2f(floorf(g.min.x + w - w * p.animationValue), g.min.y, w, h);
                        prevG = BBox2f(floorf(g.min.x - w * p.animationValue), g.min.y, w, h);
                        break;
                    case Side::Top: break;
                    case Side::Right:
                        currentG = BBox2f(floorf(g.min.x - w + w * p.animationValue), g.min.y, w, h);
                        prevG = BBox2f(floorf(g.min.x + w * p.animationValue), g.min.y, w, h);
                        break;
                    case Side::Bottom: break;
                    default: break;
                    }
                    p.currentWidget->setGeometry(getAlign(
                        currentG,
                        p.currentWidget->getMinimumSize(),
                        p.currentWidget->getHAlign(),
                        p.currentWidget->getVAlign()));
                    p.prevWidget->setGeometry(getAlign(
                        prevG,
                        p.prevWidget->getMinimumSize(),
                        p.prevWidget->getHAlign(),
                        p.prevWidget->getVAlign()));
                }
            }

            void Solo::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                if (p.currentWidget)
                {
                    p.currentWidget->moveToFront();
                    p.currentWidget->show();
                }
                _resize();
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
