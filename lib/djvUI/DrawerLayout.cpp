// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/DrawerLayout.h>

#include <djvUI/Spacer.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvRender2D/Render.h>

#include <djvSystem/Animation.h>

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
                const Math::FloatRange splitRange = Math::FloatRange(.05F, .95F);
                const size_t animationTime = 100;

            } // namespace

            struct Drawer::Private
            {
                Side side = Side::First;
                bool open = false;
                float openAmount = 0.F;
                float split = .5F;

                std::shared_ptr<Widget> drawerWidget;
                std::shared_ptr<StackLayout> childLayout;

                System::Event::PointerID hover = System::Event::invalidID;
                System::Event::PointerID pressedID = System::Event::invalidID;
                float pressedOffset = 0.F;

                std::function<std::shared_ptr<Widget>(void)> openCallback;
                std::function<void(const std::shared_ptr<Widget>&)> closeCallback;
                std::function<void(float)> splitCallback;

                std::shared_ptr<System::Animation::Animation> openAnimation;
            };

            void Drawer::_init(Side side, const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Layout::Drawer");
                setPointerEnabled(true);

                p.side = side;

                p.childLayout = StackLayout::create(context);
                Widget::addChild(p.childLayout);

                p.openAnimation = System::Animation::Animation::create(context);
                p.openAnimation->setType(System::Animation::Type::SmoothStep);
            }

            Drawer::Drawer() :
                _p(new Private)
            {}

            Drawer::~Drawer()
            {}

            std::shared_ptr<Drawer> Drawer::create(Side side, const std::shared_ptr<System::Context>& context)
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
                if (animate)
                {
                    auto weak = std::weak_ptr<Drawer>(std::dynamic_pointer_cast<Drawer>(shared_from_this()));
                    if (p.open)
                    {
                        if (p.drawerWidget)
                        {
                            Widget::removeChild(p.drawerWidget);
                            p.drawerWidget.reset();
                        }
                        if (p.openCallback)
                        {
                            p.drawerWidget = p.openCallback();
                            Widget::addChild(p.drawerWidget);
                            p.openAnimation->start(
                                p.openAmount,
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
                            p.openAmount,
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
                                        widget->_p->closeCallback(widget->_p->drawerWidget);
                                    }
                                    widget->Widget::removeChild(widget->_p->drawerWidget);
                                    widget->_p->drawerWidget.reset();
                                    widget->_resize();
                                }
                            });
                    }
                }
                else
                {
                    if (p.open)
                    {
                        p.openAmount = 1.F;
                        if (p.openCallback)
                        {
                            p.drawerWidget = p.openCallback();
                            Widget::addChild(p.drawerWidget);
                        }
                    }
                    else
                    {
                        p.openAmount = 0.F;
                        if (p.closeCallback)
                        {
                            p.closeCallback(p.drawerWidget);
                        }
                        Widget::removeChild(p.drawerWidget);
                        p.drawerWidget.reset();
                    }
                }
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

            float Drawer::getSplit() const
            {
                return _p->split;
            }

            void Drawer::setSplit(float value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.split)
                    return;
                p.split = Math::clamp(value, splitRange.getMin(), splitRange.getMax());
                _resize();
            }

            void Drawer::setSplitCallback(const std::function<void(float)>& value)
            {
                _p->splitCallback = value;
            }

            void Drawer::addChild(const std::shared_ptr<IObject>& child)
            {
                _p->childLayout->addChild(child);
            }

            void Drawer::removeChild(const std::shared_ptr<IObject>& child)
            {
                _p->childLayout->removeChild(child);
            }

            void Drawer::_preLayoutEvent(System::Event::PreLayout& event)
            {
                _setMinimumSize(_p->childLayout->getMinimumSize());
            }

            void Drawer::_layoutEvent(System::Event::Layout& event)
            {
                DJV_PRIVATE_PTR();
                p.childLayout->setGeometry(_getChildGeometry());
                if (p.drawerWidget)
                {
                    p.drawerWidget->setGeometry(_getDrawerGeometry());
                }
            }

            void Drawer::_paintEvent(System::Event::Paint& event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float h = style->getMetric(MetricsRole::Handle);
                const auto& render = _getRender();
                render->setFillColor(style->getColor(UI::ColorRole::Border));
                const Math::BBox2f& handleGeometry = _getHandleGeometry();
                render->drawRect(handleGeometry);
                if (p.pressedID != System::Event::invalidID)
                {
                    render->setFillColor(style->getColor(UI::ColorRole::Pressed));
                    render->drawRect(handleGeometry);
                }
                else if (p.hover != System::Event::invalidID)
                {
                    render->setFillColor(style->getColor(UI::ColorRole::Hovered));
                    render->drawRect(handleGeometry);
                }
            }

            void Drawer::_pointerLeaveEvent(System::Event::PointerLeave&)
            {
                DJV_PRIVATE_PTR();
                if (p.hover != System::Event::invalidID)
                {
                    p.hover = System::Event::invalidID;
                    _redraw();
                }
            }

            void Drawer::_pointerMoveEvent(System::Event::PointerMove& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                if (p.pressedID)
                {
                    const Math::BBox2f& g = getGeometry();
                    const auto& pos = event.getPointerInfo().projectedPos;
                    switch (p.side)
                    {
                    case Side::Left:
                    case Side::Right:
                        p.split = Math::clamp((pos.x - g.min.x) / g.w() + p.pressedOffset, splitRange.getMin(), splitRange.getMax());
                        break;
                    case Side::Top:
                    case Side::Bottom:
                        p.split = Math::clamp((pos.y - g.min.y) / g.h() + p.pressedOffset, splitRange.getMin(), splitRange.getMax());
                        break;
                    default: break;
                    }
                    if (p.splitCallback)
                    {
                        p.splitCallback(p.split);
                    }
                    _resize();
                }
                else
                {
                    if (_getHandleGeometry().contains(event.getPointerInfo().projectedPos))
                    {
                        const auto id = event.getPointerInfo().id;
                        if (id != p.hover)
                        {
                            p.hover = id;
                            _redraw();
                        }
                    }
                    else
                    {
                        if (p.hover != System::Event::invalidID)
                        {
                            p.hover = System::Event::invalidID;
                            _redraw();
                        }
                    }
                }
            }

            void Drawer::_buttonPressEvent(System::Event::ButtonPress& event)
            {
                DJV_PRIVATE_PTR();
                if (p.pressedID != System::Event::invalidID)
                    return;
                if (_getHandleGeometry().contains(event.getPointerInfo().projectedPos))
                {
                    event.accept();
                    p.pressedID = event.getPointerInfo().id;
                    const Math::BBox2f& g = getGeometry();
                    const auto& pos = event.getPointerInfo().projectedPos;
                    switch (p.side)
                    {
                    case Side::Left:
                    case Side::Right:
                        p.pressedOffset = p.split - (pos.x - g.min.x) / g.w();
                        break;
                    case Side::Top:
                    case Side::Bottom:
                        p.pressedOffset = p.split - (pos.y - g.min.y) / g.h();
                        break;
                    default: break;
                    }
                    _redraw();
                }
            }

            void Drawer::_buttonReleaseEvent(System::Event::ButtonRelease& event)
            {
                DJV_PRIVATE_PTR();
                if (event.getPointerInfo().id != p.pressedID)
                    return;
                event.accept();
                p.pressedID = System::Event::invalidID;
                _redraw();
            }

            Math::BBox2f Drawer::_getChildGeometry() const
            {
                DJV_PRIVATE_PTR();
                Math::BBox2f out;
                const Math::BBox2f& g = getGeometry();
                const float openAmount = p.drawerWidget && p.drawerWidget->isVisible() ? p.openAmount : 0.F;
                switch (p.side)
                {
                case Side::Left:
                    out.min.x = g.min.x + ceilf(g.w() * Math::lerp(openAmount, 0.F, p.split));
                    out.min.y = g.min.y;
                    out.max.x = g.max.x;
                    out.max.y = g.max.y;
                    break;
                case Side::Top:
                    out.min.x = g.min.x;
                    out.min.y = g.min.y + ceilf(g.h() * Math::lerp(openAmount, 0.F, p.split));
                    out.max.x = g.max.x;
                    out.max.y = g.max.y;
                    break;
                case Side::Right:
                    out.min.x = g.min.x;
                    out.min.y = g.min.y;
                    out.max.x = g.min.x + ceilf(g.w() * Math::lerp(1.F - openAmount, p.split, 1.F));
                    out.max.y = g.max.y;
                    break;
                case Side::Bottom:
                    out.min.x = g.min.x;
                    out.min.y = g.min.y;
                    out.max.x = g.max.x;
                    out.max.y = g.min.y + ceilf(g.h() * Math::lerp(1.F - openAmount, p.split, 1.F));
                    break;
                default: break;
                }
                return out;
            }

            Math::BBox2f Drawer::_getDrawerGeometry() const
            {
                DJV_PRIVATE_PTR();
                Math::BBox2f out;
                const auto& style = _getStyle();
                const float h = style->getMetric(MetricsRole::Handle);
                const Math::BBox2f& g = getGeometry();
                const float openAmount = p.drawerWidget && p.drawerWidget->isVisible() ? p.openAmount : 0.F;
                switch (p.side)
                {
                case Side::Left:
                    out.min.x = g.min.x - ceilf(g.w() * Math::lerp(1.F - openAmount, 0.F, p.split));
                    out.min.y = g.min.y;
                    out.max.x = g.min.x + ceilf(g.w() * Math::lerp(openAmount, 0.F, p.split)) - h;
                    out.max.y = g.max.y;
                    break;
                case Side::Top:
                    out.min.x = g.min.x;
                    out.min.y = g.min.y - ceilf(g.h() * Math::lerp(1.F - openAmount, 0.F, p.split));
                    out.max.x = g.max.x;
                    out.max.y = g.min.y + ceilf(g.h() * Math::lerp(openAmount, 0.F, p.split)) - h;
                    break;
                case Side::Right:
                    out.min.x = g.min.x + ceilf(g.w() * Math::lerp(1.F - openAmount, p.split, 1.F)) + h;
                    out.min.y = g.min.y;
                    out.max.x = g.max.x + ceilf(g.w() * Math::lerp(1.F - openAmount, 0.F, 1.F - p.split));
                    out.max.y = g.max.y;
                    break;
                case Side::Bottom:
                    out.min.x = g.min.x;
                    out.min.y = g.min.y + ceilf(g.h() * Math::lerp(1.F - openAmount, p.split, 1.F)) + h;
                    out.max.x = g.max.x;
                    out.max.y = g.max.y + ceilf(g.h() * Math::lerp(1.F - openAmount, 0.F, 1.F - p.split));
                    break;
                default: break;
                }
                return out;
            }

            Math::BBox2f Drawer::_getHandleGeometry() const
            {
                DJV_PRIVATE_PTR();
                Math::BBox2f out;
                const auto& style = _getStyle();
                const float h = style->getMetric(MetricsRole::Handle);
                const Math::BBox2f& g = getGeometry();
                const float openAmount = p.drawerWidget && p.drawerWidget->isVisible() ? p.openAmount : 0.F;
                switch (p.side)
                {
                case Side::Left:
                    out.min.x = g.min.x + ceilf(g.w() * Math::lerp(openAmount, 0.F, p.split)) - h;
                    out.min.y = g.min.y;
                    out.max.x = out.min.x + h;
                    out.max.y = g.max.y;
                    break;
                case Side::Top:
                    out.min.x = g.min.x;
                    out.min.y = g.min.y + ceilf(g.h() * Math::lerp(openAmount, 0.F, p.split)) - h;
                    out.max.x = g.max.x;
                    out.max.y = out.min.y + h;
                    break;
                case Side::Right:
                    out.min.x = g.min.x + ceilf(g.w() * Math::lerp(1.F - openAmount, p.split, 1.F));
                    out.min.y = g.min.y;
                    out.max.x = out.min.x + h;
                    out.max.y = g.max.y;
                    break;
                case Side::Bottom:
                    out.min.x = g.min.x;
                    out.min.y = g.min.y + ceilf(g.h() * Math::lerp(1.F - openAmount, p.split, 1.F));
                    out.max.x = g.max.x;
                    out.max.y = out.min.y + h;
                    break;
                default: break;
                }
                return out;
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
