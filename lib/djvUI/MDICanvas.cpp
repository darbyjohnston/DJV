//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvUI/MDICanvas.h>

#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/MDIWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2D.h>

#include <set>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace MDI
        {
            namespace
            {
                struct Hovered
                {
                    std::shared_ptr<IWidget> widget;
                    Handle handle = Handle::None;
                };

                struct Pressed
                {
                    std::shared_ptr<IWidget> widget;
                    glm::vec2 pointer = glm::vec2(0.f, 0.f);
                    Handle handle = Handle::None;
                    glm::vec2 pos = glm::vec2(0.f, 0.f);
                    glm::vec2 size = glm::vec2(0.f, 0.f);
                };

            } // namespace

            struct Canvas::Private
            {
                glm::vec2 canvasSize = glm::vec2(10000.f, 10000.f);
                std::vector<std::shared_ptr<IWidget> > widgets;
				std::map<std::shared_ptr<IWidget>, glm::vec2> widgetToPos;
				std::map<std::shared_ptr<IWidget>, bool> widgetResized;
                std::map<Event::PointerID, Hovered> hovered;
                std::map<Event::PointerID, Pressed> pressed;
                std::function<void(const std::shared_ptr<IWidget> &)> activeCallback;
            };

            void Canvas::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::MDI::Canvas");
            }

            Canvas::Canvas() :
                _p(new Private)
            {}

            Canvas::~Canvas()
            {}

            std::shared_ptr<Canvas> Canvas::create(Context * context)
            {
                auto out = std::shared_ptr<Canvas>(new Canvas);
                out->_init(context);
                return out;
            }

            const glm::vec2 & Canvas::getCanvasSize() const
            {
                return _p->canvasSize;
            }

            void Canvas::setCanvasSize(const glm::vec2 & size)
            {
                DJV_PRIVATE_PTR();
                if (size == p.canvasSize)
                    return;
                p.canvasSize = size;
                _resize();
            }

            std::shared_ptr<IWidget> Canvas::getActiveWidget() const
            {
                DJV_PRIVATE_PTR();
                return p.widgets.size() ? p.widgets.back() : nullptr;
            }

            void Canvas::nextWidget()
            {
                DJV_PRIVATE_PTR();
                const size_t size = p.widgets.size();
                if (size > 1)
                {
                    p.widgets.back()->moveToBack();
                }
            }

            void Canvas::prevWidget()
            {
                DJV_PRIVATE_PTR();
                const size_t size = p.widgets.size();
                if (size > 1)
                {
                    p.widgets.front()->moveToFront();
                }
            }

            void Canvas::setActiveCallback(const std::function<void(const std::shared_ptr<IWidget> &)> & value)
            {
                _p->activeCallback = value;
            }

            const glm::vec2 & Canvas::getWidgetPos(const std::shared_ptr<IWidget> & widget) const
            {
                static const glm::vec2 empty(0.f, 0.f);
                const auto i = _p->widgetToPos.find(widget);
                return i != _p->widgetToPos.end() ? i->second : empty;
            }

            void Canvas::setWidgetPos(const std::shared_ptr<IWidget> & widget, const glm::vec2 & pos)
            {
                const auto i = _p->widgetToPos.find(widget);
                if (i != _p->widgetToPos.end())
                {
                    _p->widgetToPos[widget] = pos;
                    _resize();
                }
            }

            void Canvas::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_p->canvasSize);
            }

            void Canvas::_layoutEvent(Event::Layout&)
            {
				if (auto style = _getStyle().lock())
				{
					const BBox2f & g = getGeometry();
					const glm::vec2 c = g.getCenter();
					const float m = style->getMetric(Style::MetricsRole::MarginDialog);
					for (auto & i : _p->widgetToPos)
					{
						if (i.first->isVisible())
						{
							const glm::vec2 & widgetSize = i.first->getSize();
							const glm::vec2 & widgetMinimumSize = i.first->getMinimumSize();
							BBox2f widgetGeometry;
							widgetGeometry.min.x = Math::clamp(g.min.x + i.second.x, 0.f, g.max.x - widgetMinimumSize.x);
							widgetGeometry.min.y = Math::clamp(g.min.y + i.second.y, 0.f, g.max.y - widgetMinimumSize.y);
							widgetGeometry.max.x = Math::clamp(widgetGeometry.min.x + widgetSize.x, widgetGeometry.min.x + widgetMinimumSize.x, g.max.x);
							widgetGeometry.max.y = Math::clamp(widgetGeometry.min.y + widgetSize.y, widgetGeometry.min.y + widgetMinimumSize.y, g.max.y);
							i.first->setGeometry(widgetGeometry);
						}
					}
				}
            }

            void Canvas::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                if (auto render = _getRender().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float h = style->getMetric(Style::MetricsRole::Handle);
                        const float sh = style->getMetric(Style::MetricsRole::Shadow);

                        render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Shadow)));
                        for (const auto & i : p.widgets)
                        {
                            if (i->isVisible())
                            {
                                BBox2f g = i->getGeometry().margin(-h);
                                g.min.x += sh;
                                g.min.y += sh;
                                g.max.x += sh;
                                g.max.y += sh;
                                render->drawRect(g);
                            }
                        }
                    }
                }
            }

            void Canvas::_paintOverlayEvent(Event::PaintOverlay& event)
            {
                DJV_PRIVATE_PTR();
                if (auto render = _getRender().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        /*const auto & children = getChildrenT<IWidget>();
                        if (children.size())
                        {
                            const BBox2f g = children.back()->getGeometry().margin(-h).margin(ms);
                            render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Checked)));
                            render->drawRect(BBox2f(
                                glm::vec2(g.min.x, g.min.y),
                                glm::vec2(g.max.x, g.min.y + ms)));
                            render->drawRect(BBox2f(
                                glm::vec2(g.min.x, g.max.y - ms),
                                glm::vec2(g.max.x, g.max.y)));
                            render->drawRect(BBox2f(
                                glm::vec2(g.min.x, g.min.y + ms),
                                glm::vec2(g.min.x + ms, g.max.y - ms)));
                            render->drawRect(BBox2f(
                                glm::vec2(g.max.x - ms, g.min.y + ms),
                                glm::vec2(g.max.x, g.max.y - ms)));
                        }*/

                        auto hovered = p.hovered;
                        render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Pressed)));
                        for (const auto & i : p.pressed)
                        {
                            for (const auto & j : i.second.widget->getHandleBBox(i.second.handle))
                            {
                                switch (i.second.handle)
                                {
                                case Handle::Move:
                                case Handle::None: break;
                                default:
                                    render->drawRect(j);
                                    break;
                                }
                            }
                            const auto j = hovered.find(i.first);
                            if (j != hovered.end())
                            {
                                hovered.erase(j);
                            }
                        }

                        render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Hovered)));
                        for (const auto & i : hovered)
                        {
                            for (const auto & j : i.second.widget->getHandleBBox(i.second.handle))
                            {
                                switch (i.second.handle)
                                {
                                case Handle::Move:
                                case Handle::None: break;
                                default:
                                    render->drawRect(j);
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            void Canvas::_childAddedEvent(Event::ChildAdded & value)
            {
                DJV_PRIVATE_PTR();
                if (auto widget = std::dynamic_pointer_cast<IWidget>(value.getChild()))
                {
                    widget->installEventFilter(shared_from_this());
                    p.widgets.push_back(widget);
                    p.widgetToPos[widget] = glm::vec2(0.f, 0.f);
                    _resize();
                    if (p.activeCallback)
                    {
                        p.activeCallback(widget);
                    }
                }
            }

            void Canvas::_childRemovedEvent(Event::ChildRemoved & value)
            {
                DJV_PRIVATE_PTR();
                if (auto widget = std::dynamic_pointer_cast<IWidget>(value.getChild()))
                {
                    widget->removeEventFilter(shared_from_this());
                    {
                        const auto j = std::find(p.widgets.begin(), p.widgets.end(), widget);
                        if (j != p.widgets.end())
                        {
                            const bool active = j == p.widgets.begin();
                            p.widgets.erase(j);
                            if (active)
                            {
                                if (p.widgets.size())
                                {
                                    if (p.activeCallback)
                                    {
                                        p.activeCallback(p.widgets.back());
                                    }
                                }
                                else
                                {
                                    if (p.activeCallback)
                                    {
                                        p.activeCallback(nullptr);
                                    }
                                }
                            }
                        }
                    }
                    {
                        const auto j = p.widgetToPos.find(widget);
                        if (j != p.widgetToPos.end())
                        {
                            p.widgetToPos.erase(j);
                        }
                    }
					_resize();
                }
            }

            bool Canvas::_eventFilter(const std::shared_ptr<IObject>& object, Event::IEvent& event)
            {
                DJV_PRIVATE_PTR();
                /*{
                    std::stringstream ss;
                    ss << event.getEventType();
                    _log(ss.str());
                }*/
                switch (event.getEventType())
                {
                case Event::Type::PointerEnter:
                {
                    event.accept();
                    Event::PointerEnter& pointerEnterEvent = static_cast<Event::PointerEnter&>(event);
                    const auto & pointerInfo = pointerEnterEvent.getPointerInfo();
                    if (auto widget = std::dynamic_pointer_cast<IWidget>(object))
                    {
                        const Handle handle = widget->getHandle(pointerInfo.projectedPos);
                        if (handle != Handle::None)
                        {
                            Hovered hovered;
                            hovered.widget = widget;
                            hovered.handle = handle;
                            p.hovered[pointerInfo.id] = hovered;
                            _redraw();
                            break;
                        }
                    }
                    break;
                }
                case Event::Type::PointerLeave:
                {
                    event.accept();
                    Event::PointerLeave& pointerLeaveEvent = static_cast<Event::PointerLeave&>(event);
                    const auto & pointerInfo = pointerLeaveEvent.getPointerInfo();
                    const auto i = p.hovered.find(pointerInfo.id);
                    if (i != p.hovered.end())
                    {
                        p.hovered.erase(i);
                        _redraw();
                    }
                    break;
                }
                case Event::Type::PointerMove:
                {
                    event.accept();
                    Event::PointerMove& pointerMoveEvent = static_cast<Event::PointerMove&>(event);
                    const auto & pointerInfo = pointerMoveEvent.getPointerInfo();
                    if (auto widget = std::dynamic_pointer_cast<IWidget>(object))
                    {
                        const auto i = p.pressed.find(pointerInfo.id);
                        if (i != p.pressed.end())
                        {
                            const auto j = p.widgetToPos.find(widget);
                            if (j != p.widgetToPos.end())
                            {
                                glm::vec2 size = widget->getSize();
                                const glm::vec2 & minimumSize = widget->getMinimumSize();
                                const glm::vec2 d = pointerInfo.projectedPos - i->second.pointer;
                                const glm::vec2 d2(
                                    d.x - std::max(0.f, minimumSize.x - (i->second.size.x - d.x)),
                                    d.y - std::max(0.f, minimumSize.y - (i->second.size.y - d.y)));
                                switch (i->second.handle)
                                {
                                case Handle::Move:
                                    j->second = i->second.pos + pointerInfo.projectedPos - i->second.pointer;
                                    break;
                                case Handle::ResizeE:
                                {
                                    j->second.x = i->second.pos.x + d2.x;
                                    size.x = i->second.size.x - d2.x;
                                    break;
                                }
                                case Handle::ResizeN:
                                    j->second.y = i->second.pos.y + d2.y;
                                    size.y = i->second.size.y - d2.y;
                                    break;
                                case Handle::ResizeW:
                                    size.x = i->second.size.x + d.x;
                                    break;
                                case Handle::ResizeS:
                                    size.y = i->second.size.y + d.y;
                                    break;
                                case Handle::ResizeNE:
                                    j->second = i->second.pos + d2;
                                    size = i->second.size - d2;
                                    break;
                                case Handle::ResizeNW:
                                    j->second.y = i->second.pos.y + d2.y;
                                    size.x = i->second.size.x + d.x;
                                    size.y = i->second.size.y - d2.y;
                                    break;
                                case Handle::ResizeSW:
                                    size = i->second.size + pointerInfo.projectedPos - i->second.pointer;
                                    break;
                                case Handle::ResizeSE:
                                    j->second.x = i->second.pos.x + d2.x;
                                    size.x = i->second.size.x - d2.x;
                                    size.y = i->second.size.y + d.y;
                                    break;
                                default: break;
                                }
                                widget->resize(size);
                                _resize();
                            }
                        }
                        else
                        {
                            const Handle handle = widget->getHandle(pointerInfo.projectedPos);
                            if (handle != Handle::None)
                            {
                                Hovered hovered;
                                hovered.widget = widget;
                                hovered.handle = handle;
                                p.hovered[pointerInfo.id] = hovered;
                                _redraw();
                            }
                        }
                    }
                    return true;
                }
                case Event::Type::ButtonPress:
                {
                    event.accept();
                    Event::ButtonPress& buttonPressEvent = static_cast<Event::ButtonPress&>(event);
                    const auto & pointerInfo = buttonPressEvent.getPointerInfo();
                    if (auto widget = std::dynamic_pointer_cast<IWidget>(object))
                    {
                        const auto i = p.widgetToPos.find(widget);
                        if (i != p.widgetToPos.end())
                        {
                            Pressed pressed;
                            pressed.widget = widget;
                            pressed.pointer = pointerInfo.projectedPos;
                            pressed.handle = widget->getHandle(pointerInfo.projectedPos);
                            pressed.pos = i->second;
                            pressed.size = widget->getSize();
                            p.pressed[pointerInfo.id] = pressed;
                            widget->moveToFront();
                        }
                    }
                    return true;
                }
                case Event::Type::ButtonRelease:
                {
                    event.accept();
                    Event::ButtonRelease& buttonReleaseEvent = static_cast<Event::ButtonRelease&>(event);
                    const auto & pointerInfo = buttonReleaseEvent.getPointerInfo();
                    const auto i = p.pressed.find(pointerInfo.id);
                    if (i != p.pressed.end())
                    {
                        p.pressed.erase(i);
                    }
                    return true;
                }
                default: break;
                }
                return false;
            }

        } // namespace MDI
    } // namespace UI
} // namespace djdv
