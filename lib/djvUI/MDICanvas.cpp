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
#include <djvUI/MDIWindow.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolButton.h>

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
                glm::vec2 canvasSize = glm::vec2(2000.f, 2000.f);
                std::vector<std::shared_ptr<Widget> > windows;
                std::map<std::shared_ptr<IObject>, std::shared_ptr<Widget> > moveHandleToWindow;
                std::map<std::shared_ptr<IObject>, std::shared_ptr<Widget> > resizeHandleToWindow;
                std::map<std::shared_ptr<Widget>, std::shared_ptr<IObject> > windowToMoveHandle;
                std::map<std::shared_ptr<Widget>, std::shared_ptr<IObject> > windowToResizeHandle;
                Event::PointerID pressed = 0;
                glm::vec2 pressedPos;
                glm::vec2 pressedOffset;
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
                if (size == _p->canvasSize)
                    return;
                _p->canvasSize = size;
                _resize();
            }

            void Canvas::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_p->canvasSize);
            }

            void Canvas::_layoutEvent(Event::Layout&)
            {

            }

            void Canvas::_childAddedEvent(Event::ChildAdded & value)
            {
                if (auto window = std::dynamic_pointer_cast<IWindow>(value.getChild()))
                {
                    auto moveHandle = window->getMoveHandle();
                    auto resizeHandle = window->getResizeHandle();
                    moveHandle->installEventFilter(shared_from_this());
                    resizeHandle->installEventFilter(shared_from_this());

                    _p->windows.push_back(window);
                    _p->moveHandleToWindow[moveHandle] = window;
                    _p->resizeHandleToWindow[resizeHandle] = window;
                    _p->windowToMoveHandle[window] = moveHandle;
                    _p->windowToResizeHandle[window] = resizeHandle;

                    _redraw();
                }
            }

            void Canvas::_childRemovedEvent(Event::ChildRemoved & value)
            {
                if (auto window = std::dynamic_pointer_cast<IWindow>(value.getChild()))
                {
                    {
                        const auto j = std::find(_p->windows.begin(), _p->windows.end(), window);
                        if (j != _p->windows.end())
                        {
                            _p->windows.erase(j);
                        }
                    }
                    {
                        const auto j = _p->windowToMoveHandle.find(window);
                        if (j != _p->windowToMoveHandle.end())
                        {
                            const auto k = _p->moveHandleToWindow.find(j->second);
                            if (k != _p->moveHandleToWindow.end())
                            {
                                _p->moveHandleToWindow.erase(k);
                            }
                            _p->windowToMoveHandle.erase(j);
                        }
                    }
                    {
                        const auto j = _p->windowToResizeHandle.find(window);
                        if (j != _p->windowToResizeHandle.end())
                        {
                            const auto k = _p->resizeHandleToWindow.find(j->second);
                            if (k != _p->resizeHandleToWindow.end())
                            {
                                _p->resizeHandleToWindow.erase(k);
                            }
                            _p->windowToResizeHandle.erase(j);
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
                    if (pointerMoveEvent.getPointerInfo().id == _p->pressed)
                    {
                        if (auto style = _getStyle().lock())
                        {
                            const float shadow = style->getMetric(Style::MetricsRole::Shadow);
                            const BBox2f& g = getGeometry();
                            const auto moveHandleToWindow = _p->moveHandleToWindow.find(object);
                            const auto resizeHandleToWindow = _p->resizeHandleToWindow.find(object);
                            if (moveHandleToWindow != _p->moveHandleToWindow.end())
                            {
                                const auto window = std::find(_p->windows.begin(), _p->windows.end(), moveHandleToWindow->second);
                                if (window != _p->windows.end())
                                {
                                    const glm::vec2 pos = pointerMoveEvent.getPointerInfo().projectedPos + _p->pressedOffset - g.min;
                                    glm::vec2 windowPos;
                                    windowPos.x = Math::clamp(pos.x, -shadow, g.w() - (*window)->getWidth() + shadow);
                                    windowPos.y = Math::clamp(pos.y, -shadow, g.h() - (*window)->getHeight() + shadow);
                                    (*window)->move(windowPos);
                                }
                            }
                            else if (resizeHandleToWindow != _p->resizeHandleToWindow.end())
                            {
                                const auto window = std::find(_p->windows.begin(), _p->windows.end(), resizeHandleToWindow->second);
                                if (window != _p->windows.end())
                                {
                                    const glm::vec2 min = (*window)->getMinimumSize();
                                    const BBox2f & g = (*window)->getGeometry();
                                    const glm::vec2 pos = pointerMoveEvent.getPointerInfo().projectedPos + _p->pressedOffset - g.min;
                                    glm::vec2 pos2;
                                    pos2.x = Math::clamp(pos.x, g.min.x + min.x, g.w() + shadow);
                                    pos2.y = Math::clamp(pos.y, g.min.y + min.y, g.h() + shadow);
                                    (*window)->resize(pos2 - g.min + g.min);
                                }
                            }
                        }
                    }
                    return true;
                }
                case Event::Type::ButtonPress:
                {
                    Event::ButtonPress& buttonPressEvent = static_cast<Event::ButtonPress&>(event);
                    if (!_p->pressed)
                    {
                        if (_p->moveHandleToWindow.find(object) != _p->moveHandleToWindow.end())
                        {
                            event.accept();
                            _p->pressed = buttonPressEvent.getPointerInfo().id;
                            _p->pressedPos = buttonPressEvent.getPointerInfo().projectedPos;
                            auto window = _p->moveHandleToWindow[object];
                            window->moveToFront();
                            _p->pressedOffset = window->getGeometry().min - _p->pressedPos;
                        }
                        else if (_p->resizeHandleToWindow.find(object) != _p->resizeHandleToWindow.end())
                        {
                            event.accept();
                            _p->pressed = buttonPressEvent.getPointerInfo().id;
                            _p->pressedPos = buttonPressEvent.getPointerInfo().projectedPos;
                            auto window = _p->resizeHandleToWindow[object];
                            window->moveToFront();
                            _p->pressedOffset = window->getGeometry().max - _p->pressedPos;
                        }
                    }
                    return true;
                }
                case Event::Type::ButtonRelease:
                {
                    Event::ButtonRelease& buttonReleaseEvent = static_cast<Event::ButtonRelease&>(event);
                    if (_p->pressed == buttonReleaseEvent.getPointerInfo().id)
                    {
                        event.accept();
                        _p->pressed = 0;
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
