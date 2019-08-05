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

#include <djvUI/Drawer.h>

#include <djvUI/Spacer.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            namespace
            {
                class DrawerLayout : public Layout::Row
                {
                    DJV_NON_COPYABLE(DrawerLayout);

                protected:
                    void _init(Orientation orientation, Context* context)
                    {
                        Row::_init(orientation, context);
                        setPointerEnabled(true);
                    }

                    DrawerLayout()
                    {}

                public:
                    static std::shared_ptr<DrawerLayout> create(Orientation orientation, Context* context)
                    {
                        auto out = std::shared_ptr<DrawerLayout>(new DrawerLayout);
                        out->_init(orientation, context);
                        return out;
                    }

                protected:
                    void _buttonPressEvent(Event::ButtonPress& event) override
                    {
                        event.accept();
                    }

                    void _buttonReleaseEvent(Event::ButtonRelease& event) override
                    {
                        event.accept();
                    }
                };

            } // namespace

            struct Drawer::Private
            {
                Side side = Side::First;
                bool open = false;
                float size = .3f;
                std::shared_ptr<Stack> childLayout;
                std::shared_ptr<Layout::Spacer> spacer;
                std::shared_ptr<DrawerLayout> layout;
                std::vector<Event::PointerID> pointerHover;
                Event::PointerID pressedID = Event::InvalidID;
                glm::vec2 pressedPos = glm::vec2(0.f, 0.f);
                float pressedSize = 0.f;
            };

            void Drawer::_init(Side side, Context * context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Layout::Drawer");

                p.side = side;

                p.spacer = Layout::Spacer::create(Orientation::Horizontal, context);
                p.spacer->setPointerEnabled(true);
                p.spacer->setShadowOverlay({ side });
                p.spacer->installEventFilter(shared_from_this());

                p.childLayout = Stack::create(context);

                switch (side)
                {
                case Side::Left:
                case Side::Right:
                    p.layout = DrawerLayout::create(Orientation::Horizontal, context);
                    break;
                case Side::Top:
                case Side::Bottom:
                    p.layout = DrawerLayout::create(Orientation::Vertical, context);
                    break;
                default: break;
                }
                p.layout->setSpacing(MetricsRole::None);
                p.layout->setBackgroundRole(ColorRole::Background);
                p.layout->setPointerEnabled(true);
                switch (side)
                {
                case Side::Left:
                case Side::Top:
                    p.layout->addChild(p.childLayout);
                    p.layout->setStretch(p.childLayout, RowStretch::Expand);
                    p.layout->addChild(p.spacer);
                    break;
                case Side::Right:
                case Side::Bottom:
                    p.layout->addChild(p.spacer);
                    p.layout->addChild(p.childLayout);
                    p.layout->setStretch(p.childLayout, RowStretch::Expand);
                    break;
                default: break;
                }
                Widget::addChild(p.layout);
            }

            Drawer::Drawer() :
                _p(new Private)
            {}

            Drawer::~Drawer()
            {}

            std::shared_ptr<Drawer> Drawer::create(Side side, Context * context)
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

            void Drawer::setOpen(bool value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.open)
                    return;
                p.open = value;
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

            void Drawer::addChild(const std::shared_ptr<IObject> & value)
            {
                _p->childLayout->addChild(value);
            }

            void Drawer::removeChild(const std::shared_ptr<IObject> & value)
            {
                _p->childLayout->removeChild(value);
            }

            void Drawer::_preLayoutEvent(Event::PreLayout & event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float sh = style->getMetric(MetricsRole::Shadow);
                glm::vec2 size = p.layout->getMinimumSize();
                switch (p.side)
                {
                case Side::Left:   size.x += sh; break;
                case Side::Top:    size.y += sh; break;
                case Side::Right:  size.x += sh; break;
                case Side::Bottom: size.y += sh; break;
                default: break;
                }
                _setMinimumSize(size);
            }

            void Drawer::_layoutEvent(Event::Layout & event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const BBox2f & g = getGeometry();
                const glm::vec2 & minimumSize = p.layout->getMinimumSize();
                BBox2f childGeometry = g;
                switch (p.side)
                {
                case Side::Left:   childGeometry.max.x = p.open ? (g.min.x + ceilf(std::max(g.w() * p.size, minimumSize.x))) : g.min.x; break;
                case Side::Top:    childGeometry.max.y = p.open ? (g.min.y + ceilf(std::max(g.h() * p.size, minimumSize.y))) : g.min.y; break;
                case Side::Right:  childGeometry.min.x = p.open ? (g.max.x - ceilf(std::max(g.w() * p.size, minimumSize.x))) : g.max.x; break;
                case Side::Bottom: childGeometry.min.y = p.open ? (g.max.y - ceilf(std::max(g.h() * p.size, minimumSize.y))) : g.max.y; break;
                default: break;
                }
                p.layout->setGeometry(childGeometry);
            }

            void Drawer::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                if (p.open)
                {
                    const BBox2f& g = p.layout->getGeometry();
                    const auto& style = _getStyle();
                    const float sh = style->getMetric(MetricsRole::Shadow);
                    auto render = _getRender();
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

            void Drawer::_paintOverlayEvent(Event::PaintOverlay& event)
            {
                DJV_PRIVATE_PTR();
                if (p.open && p.pointerHover.size())
                {
                    const auto& style = _getStyle();
                    auto render = _getRender();
                    render->setFillColor(style->getColor(ColorRole::Hovered));
                    render->drawRect(p.spacer->getGeometry());
                }
            }

            bool Drawer::_eventFilter(const std::shared_ptr<IObject>& object, Event::Event& event)
            {
                DJV_PRIVATE_PTR();
                switch (event.getEventType())
                {
                case Event::Type::PointerEnter:
                {
                    Event::PointerEnter& pointerEnterEvent = static_cast<Event::PointerEnter&>(event);
                    const auto& pointerInfo = pointerEnterEvent.getPointerInfo();
                    if (auto widget = std::dynamic_pointer_cast<Widget>(object))
                    {
                        event.accept();
                        const auto i = std::find(p.pointerHover.begin(), p.pointerHover.end(), pointerInfo.id);
                        if (i == p.pointerHover.end())
                        {
                            p.pointerHover.push_back(pointerInfo.id);
                        }
                        _redraw();
                    }
                    break;
                }
                case Event::Type::PointerLeave:
                {
                    Event::PointerLeave& pointerLeaveEvent = static_cast<Event::PointerLeave&>(event);
                    const auto& pointerInfo = pointerLeaveEvent.getPointerInfo();
                    if (auto widget = std::dynamic_pointer_cast<Widget>(object))
                    {
                        event.accept();
                        const auto i = std::find(p.pointerHover.begin(), p.pointerHover.end(), pointerInfo.id);
                        if (i != p.pointerHover.end())
                        {
                            p.pointerHover.erase(i);
                        }
                        _redraw();
                    }
                    break;
                }
                case Event::Type::PointerMove:
                {
                    Event::PointerMove& pointerMoveEvent = static_cast<Event::PointerMove&>(event);
                    const auto& pointerInfo = pointerMoveEvent.getPointerInfo();
                    if (auto widget = std::dynamic_pointer_cast<Widget>(object))
                    {
                        event.accept();
                        const auto& style = _getStyle();
                        const BBox2f& g = getGeometry();
                        const float w = g.w();
                        const float h = g.h();
                        const glm::vec2& minimumSize = p.layout->getMinimumSize();
                        if (pointerInfo.id == p.pressedID)
                        {
                            switch (p.side)
                            {
                            case Side::Left:
                                p.size = Math::clamp(p.pressedSize * w + (pointerInfo.projectedPos.x - p.pressedPos.x), minimumSize.x, w) / w;
                                break;
                            case Side::Right:
                                p.size = Math::clamp(p.pressedSize * w + (p.pressedPos.x - pointerInfo.projectedPos.x), minimumSize.x, w) / w;
                                break;
                            case Side::Top:
                            case Side::Bottom:
                                p.size = Math::clamp(p.pressedSize * h + (pointerInfo.projectedPos.y - p.pressedPos.y), minimumSize.y, h) / h;
                                break;
                            default: break;
                            }
                            _resize();
                        }
                    }
                    break;
                }
                case Event::Type::ButtonPress:
                {
                    Event::ButtonPress& buttonPressEvent = static_cast<Event::ButtonPress&>(event);
                    const auto& pointerInfo = buttonPressEvent.getPointerInfo();
                    if (auto widget = std::dynamic_pointer_cast<Widget>(object))
                    {
                        if (!p.pressedID)
                        {
                            event.accept();
                            p.pressedID = pointerInfo.id;
                            p.pressedPos = pointerInfo.projectedPos;
                            p.pressedSize = p.size;
                        }
                    }
                    break;
                }
                case Event::Type::ButtonRelease:
                {
                    if (auto widget = std::dynamic_pointer_cast<Widget>(object))
                    {
                        event.accept();
                        p.pressedID = Event::InvalidID;
                    }
                    break;
                }
                default: break;
                }
                return false;
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
