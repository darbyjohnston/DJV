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
            struct Canvas::Private
            {
                glm::vec2 canvasSize = glm::vec2(10000.f, 10000.f);
                std::vector<std::shared_ptr<Widget> > widgets;
                std::map<std::shared_ptr<Widget>, glm::vec2> widgetToPos;
                std::map<std::shared_ptr<IObject>, std::shared_ptr<Widget> > moveHandleToWidget;
                std::map<std::shared_ptr<IObject>, std::shared_ptr<Widget> > resizeHandleToWidget;
                std::map<std::shared_ptr<Widget>, std::shared_ptr<IObject> > widgetToMoveHandle;
                std::map<std::shared_ptr<Widget>, std::shared_ptr<IObject> > widgetToResizeHandle;
                Event::PointerID pressed = Event::InvalidID;
                glm::vec2 pressedPos;
                glm::vec2 pressedOffset;
                std::function<void(const std::shared_ptr<Widget> &)> activeCallback;
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

            std::shared_ptr<Widget> Canvas::getActiveWidget() const
            {
                const auto & children = getChildrenT<Widget>();
                return children.size() ? children.back() : nullptr;
            }

            void Canvas::nextWidget()
            {
                const auto & children = getChildrenT<Widget>();
                const size_t size = children.size();
                if (size > 1)
                {
                    children.back()->moveToBack();
                }
            }

            void Canvas::prevWidget()
            {
                const auto & children = getChildrenT<Widget>();
                const size_t size = children.size();
                if (size > 1)
                {
                    children.front()->moveToFront();
                }
            }

            void Canvas::setActiveCallback(const std::function<void(const std::shared_ptr<Widget> &)> & value)
            {
                _p->activeCallback = value;
            }

            const glm::vec2 & Canvas::getWidgetPos(const std::shared_ptr<Widget> & widget) const
            {
                static const glm::vec2 empty(0.f, 0.f);
                const auto i = _p->widgetToPos.find(widget);
                return i != _p->widgetToPos.end() ? i->second : empty;
            }

            void Canvas::setWidgetPos(const std::shared_ptr<Widget> & widget, const glm::vec2 & pos)
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
                const BBox2f & g = getGeometry();
                for (const auto i : _p->widgetToPos)
                {
                    const glm::vec2 & widgetMinimumSize = i.first->getMinimumSize();
                    const glm::vec2 & widgetSize = i.first->getSize();
                    BBox2f widgetGeometry;
                    widgetGeometry.min = g.min + i.second;
                    widgetGeometry.max.x = Math::clamp(widgetGeometry.min.x + widgetSize.x, widgetGeometry.min.x + widgetMinimumSize.x, g.max.x);
                    widgetGeometry.max.y = Math::clamp(widgetGeometry.min.y + widgetSize.y, widgetGeometry.min.y + widgetMinimumSize.y, g.max.y);
                    i.first->setGeometry(widgetGeometry);
                }
            }

            void Canvas::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                /*if (auto render = _getRender().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float s = style->getMetric(Style::MetricsRole::Shadow);
                        render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Shadow)));
                        for (const auto & i : getChildrenT<Widget>())
                        {
                            BBox2f g = i->getGeometry();
                            g.min.x += s;
                            g.min.y += s;
                            g.max.x += s;
                            g.max.y += s;
                            render->drawRect(g);
                        }
                    }
                }*/
            }

            void Canvas::_paintOverlayEvent(Event::PaintOverlay& event)
            {
                if (auto render = _getRender().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float b = style->getMetric(Style::MetricsRole::Border);
                        const auto & children = getChildrenT<Widget>();
                        if (children.size())
                        {
                            const BBox2f g = children.back()->getGeometry().margin(b);
                            render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Checked)));
                            render->drawRect(BBox2f(g.min, glm::vec2(g.max.x, g.min.y + b)));
                            render->drawRect(BBox2f(glm::vec2(g.min.x, g.min.y + b), glm::vec2(g.min.x + b, g.max.y - b)));
                            render->drawRect(BBox2f(glm::vec2(g.max.x - b, g.min.y + b), glm::vec2(g.max.x, g.max.y - b)));
                            render->drawRect(BBox2f(glm::vec2(g.min.x, g.max.y - b), glm::vec2(g.max.x, g.max.y)));
                        }
                    }
                }
            }

            void Canvas::_childAddedEvent(Event::ChildAdded & value)
            {
                if (auto widget = std::dynamic_pointer_cast<IWidget>(value.getChild()))
                {
                    DJV_PRIVATE_PTR();
                    if (auto moveHandle = widget->getMoveHandle())
                    {
                        moveHandle->installEventFilter(shared_from_this());
                        p.moveHandleToWidget[moveHandle] = widget;
                        p.widgetToMoveHandle[widget] = moveHandle;
                    }
                    if (auto resizeHandle = widget->getResizeHandle())
                    {
                        resizeHandle->installEventFilter(shared_from_this());
                        p.resizeHandleToWidget[resizeHandle] = widget;
                        p.widgetToResizeHandle[widget] = resizeHandle;
                    }

                    p.widgets.push_back(widget);
                    p.widgetToPos[widget] = glm::vec2(0.f, 0.f);

                    _redraw();

                    if (p.activeCallback)
                    {
                        p.activeCallback(widget);
                    }
                }
            }

            void Canvas::_childRemovedEvent(Event::ChildRemoved & value)
            {
                if (auto widget = std::dynamic_pointer_cast<IWidget>(value.getChild()))
                {
                    DJV_PRIVATE_PTR();
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
                    {
                        const auto j = p.widgetToMoveHandle.find(widget);
                        if (j != p.widgetToMoveHandle.end())
                        {
                            const auto k = p.moveHandleToWidget.find(j->second);
                            if (k != p.moveHandleToWidget.end())
                            {
                                p.moveHandleToWidget.erase(k);
                            }
                            p.widgetToMoveHandle.erase(j);
                        }
                    }
                    {
                        const auto j = p.widgetToResizeHandle.find(widget);
                        if (j != p.widgetToResizeHandle.end())
                        {
                            const auto k = p.resizeHandleToWidget.find(j->second);
                            if (k != p.resizeHandleToWidget.end())
                            {
                                p.resizeHandleToWidget.erase(k);
                            }
                            p.widgetToResizeHandle.erase(j);
                        }
                    }
                    _redraw();
                }
            }

            bool Canvas::_eventFilter(const std::shared_ptr<IObject>& object, Event::IEvent& event)
            {
                /*{
                    std::stringstream ss;
                    ss << event.getEventType();
                    _log(ss.str());
                }*/
                switch (event.getEventType())
                {
                case Event::Type::PointerMove:
                {
                    Event::PointerMove& pointerMoveEvent = static_cast<Event::PointerMove&>(event);
                    pointerMoveEvent.accept();
                    DJV_PRIVATE_PTR();
                    if (pointerMoveEvent.getPointerInfo().id == p.pressed)
                    {
                        if (auto style = _getStyle().lock())
                        {
                            const BBox2f& g = getGeometry();
                            const float shadow = style->getMetric(Style::MetricsRole::Shadow);
                            const auto moveHandleToWidget = p.moveHandleToWidget.find(object);
                            const auto resizeHandleToWidget = p.resizeHandleToWidget.find(object);
                            if (moveHandleToWidget != p.moveHandleToWidget.end())
                            {
                                const auto widget = std::find(p.widgets.begin(), p.widgets.end(), moveHandleToWidget->second);
                                if (widget != p.widgets.end())
                                {
                                    const glm::vec2 pos = pointerMoveEvent.getPointerInfo().projectedPos + p.pressedOffset - g.min;
                                    glm::vec2 widgetPos;
                                    widgetPos.x = Math::clamp(pos.x, 0.f, g.w() - (*widget)->getWidth());
                                    widgetPos.y = Math::clamp(pos.y, 0.f, g.h() - (*widget)->getHeight());
                                    p.widgetToPos[*widget] = widgetPos;
                                    _resize();
                                }
                            }
                            else if (resizeHandleToWidget != p.resizeHandleToWidget.end())
                            {
                                const auto widget = std::find(p.widgets.begin(), p.widgets.end(), resizeHandleToWidget->second);
                                if (widget != p.widgets.end())
                                {
                                    const BBox2f & g = getGeometry();
                                    BBox2f widgetGeometry = (*widget)->getGeometry();
                                    const glm::vec2 & widgetMinimumSize = (*widget)->getMinimumSize();
                                    const glm::vec2 pointerPos = pointerMoveEvent.getPointerInfo().projectedPos + p.pressedOffset;
                                    widgetGeometry.max.x = Math::clamp(pointerPos.x, widgetGeometry.min.x + widgetMinimumSize.x, g.max.x);
                                    widgetGeometry.max.y = Math::clamp(pointerPos.y, widgetGeometry.min.y + widgetMinimumSize.y, g.max.y);
                                    (*widget)->setGeometry(widgetGeometry);
                                }
                            }
                        }
                    }
                    return true;
                }
                case Event::Type::ButtonPress:
                {
                    Event::ButtonPress& buttonPressEvent = static_cast<Event::ButtonPress&>(event);
                    DJV_PRIVATE_PTR();
                    if (!p.pressed)
                    {
                        const auto i = p.moveHandleToWidget.find(object);
                        const auto j = p.resizeHandleToWidget.find(object);
                        if (i != p.moveHandleToWidget.end())
                        {
                            event.accept();
                            p.pressed = buttonPressEvent.getPointerInfo().id;
                            p.pressedPos = buttonPressEvent.getPointerInfo().projectedPos;
                            i->second->moveToFront();
                            p.pressedOffset = i->second->getGeometry().min - p.pressedPos;
                            if (p.activeCallback)
                            {
                                p.activeCallback(i->second);
                            }
                        }
                        else if (j != p.resizeHandleToWidget.end())
                        {
                            event.accept();
                            p.pressed = buttonPressEvent.getPointerInfo().id;
                            p.pressedPos = buttonPressEvent.getPointerInfo().projectedPos;
                            j->second->moveToFront();
                            p.pressedOffset = j->second->getGeometry().max - p.pressedPos;
                            if (p.activeCallback)
                            {
                                p.activeCallback(j->second);
                            }
                        }
                    }
                    return true;
                }
                case Event::Type::ButtonRelease:
                {
                    Event::ButtonRelease& buttonReleaseEvent = static_cast<Event::ButtonRelease&>(event);
                    DJV_PRIVATE_PTR();
                    if (p.pressed == buttonReleaseEvent.getPointerInfo().id)
                    {
                        event.accept();
                        p.pressed = 0;
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
