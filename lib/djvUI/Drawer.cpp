// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Drawer.h>

#include <djvUI/Spacer.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2D.h>

#include <djvCore/Animation.h>

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
                const size_t animationTime = 100;

            } // namespace

            struct Drawer::Private
            {
                Side side = Side::First;
                bool open = false;
                float openAmount = 0.F;
                std::shared_ptr<Widget> widget;
                std::shared_ptr<Stack> childLayout;
                std::function<std::shared_ptr<Widget>(void)> openCallback;
                std::function<void(const std::shared_ptr<Widget>&)> closeCallback;
                std::shared_ptr<Animation::Animation> openAnimation;
            };

            void Drawer::_init(Side side, const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Layout::Drawer");

                p.side = side;

                p.childLayout = Stack::create(context);
                Widget::addChild(p.childLayout);

                p.openAnimation = Animation::Animation::create(context);
                p.openAnimation->setType(Animation::Type::SmoothStep);
            }

            Drawer::Drawer() :
                _p(new Private)
            {}

            Drawer::~Drawer()
            {}

            std::shared_ptr<Drawer> Drawer::create(Side side, const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Drawer>(new Drawer);
                out->_init(side, context);
                return out;
            }

            Side Drawer::getSide() const
            {
                return _p->side;
            }

            bool Drawer::isOpen() const
            {
                return _p->open;
            }

            void Drawer::setOpen(bool value, bool animate)
            {
                DJV_PRIVATE_PTR();
                if (value == p.open)
                    return;
                p.open = value;
                auto weak = std::weak_ptr<Drawer>(std::dynamic_pointer_cast<Drawer>(shared_from_this()));
                if (p.open)
                {
                    if (p.openCallback)
                    {
                        p.widget = p.openCallback();
                        p.childLayout->addChild(p.widget);
                        p.openAnimation->start(
                            0.F,
                            1.F,
                            std::chrono::milliseconds(animationTime),
                            [weak](float value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    widget->_p->openAmount = value;
                                    widget->_resize();
                                }
                            },
                            [weak](float value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    widget->_p->openAmount = value;
                                    widget->_resize();
                                }
                            });
                    }
                }
                else
                {
                    p.openAnimation->start(
                        1.F,
                        0.F,
                        std::chrono::milliseconds(animationTime),
                        [weak](float value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->openAmount = value;
                                widget->_resize();
                            }
                        },
                        [weak](float value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->openAmount = value;
                                if (widget->_p->closeCallback)
                                {
                                    widget->_p->closeCallback(widget->_p->widget);
                                }
                                widget->_p->childLayout->removeChild(widget->_p->widget);
                                widget->_p->widget.reset();
                                widget->_resize();
                            }
                        });
                }
                _resize();
            }

            void Drawer::open()
            {
                setOpen(true);
            }

            void Drawer::close()
            {
                setOpen(false);
            }

            void Drawer::setOpenCallback(const std::function<std::shared_ptr<Widget>(void)>& value)
            {
                _p->openCallback = value;
            }

            void Drawer::setCloseCallback(const std::function<void(const std::shared_ptr<Widget>&)>& value)
            {
                _p->closeCallback = value;
            }

            void Drawer::_preLayoutEvent(Event::PreLayout& event)
            {
                DJV_PRIVATE_PTR();
                const glm::vec2& minimumSize = p.childLayout->getMinimumSize();
                glm::vec2 size(0.F, 0.F);
                if (p.open)
                {
                    switch (p.side)
                    {
                    case Side::Left:
                    case Side::Right:
                        size.x = minimumSize.x * p.openAmount;
                        size.y = minimumSize.y;
                        break;
                    case Side::Top:
                    case Side::Bottom:
                        size.x = minimumSize.x;
                        size.y = minimumSize.y * p.openAmount;
                        break;
                    default: break;
                    }
                }
                _setMinimumSize(size);
            }

            void Drawer::_layoutEvent(Event::Layout& event)
            {
                DJV_PRIVATE_PTR();
                const BBox2f& g = getGeometry();
                const glm::vec2& minimumSize = p.childLayout->getMinimumSize();
                BBox2f childGeometry(0.F, 0.F, 0.F, 0.F);
                switch (p.side)
                {
                case Side::Left:
                    childGeometry.min.x = g.min.x;
                    childGeometry.min.y = g.min.y;
                    childGeometry.max.x = g.min.x + (minimumSize.x * p.openAmount);
                    childGeometry.max.y = g.max.y;
                    break;
                case Side::Top:
                    childGeometry.min.x = g.min.x;
                    childGeometry.min.y = g.min.y;
                    childGeometry.max.x = g.max.x;
                    childGeometry.max.y = g.min.y + (minimumSize.y * p.openAmount);
                    break;
                case Side::Right:
                    childGeometry.min.x = g.max.x - (minimumSize.x * p.openAmount);
                    childGeometry.min.y = g.min.y;
                    childGeometry.max.x = g.max.x;
                    childGeometry.max.y = g.max.y;
                    break;
                case Side::Bottom:
                    childGeometry.min.x = g.min.x;
                    childGeometry.min.y = g.max.y - (minimumSize.x * p.openAmount);
                    childGeometry.max.x = g.max.x;
                    childGeometry.max.y = g.max.y;
                    break;
                default: break;
                }
                p.childLayout->setGeometry(childGeometry);
            }

            void Drawer::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                if (p.open)
                {
                    const BBox2f& g = p.childLayout->getGeometry();
                    const auto& style = _getStyle();
                    const float sh = style->getMetric(MetricsRole::Shadow);
                    const auto& render = _getRender();
                    render->setFillColor(style->getColor(ColorRole::Shadow));
                    switch (p.side)
                    {
                    case Side::Left:   render->drawShadow(BBox2f(g.max.x, g.min.y, sh, g.h()), AV::Side::Right);     break;
                    case Side::Top:    render->drawShadow(BBox2f(g.min.x, g.max.y, g.w(), sh), AV::Side::Bottom);    break;
                    case Side::Right:  render->drawShadow(BBox2f(g.min.x - sh, g.min.y, sh, g.h()), AV::Side::Left); break;
                    case Side::Bottom: render->drawShadow(BBox2f(g.min.x, g.min.y - sh, g.w(), sh), AV::Side::Top);  break;
                    default: break;
                    }
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
