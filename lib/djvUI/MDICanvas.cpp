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

#include <djvUI/Button.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/MDIWindow.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/StackLayout.h>

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
                class CanvasWidget : public Widget
                {
                    DJV_NON_COPYABLE(CanvasWidget);

                protected:
                    void _init(Context * context)
                    {
                        Widget::_init(context);
                        
                        setClassName("djv::UI::MDI::CanvasWidget");
                    }

                    CanvasWidget()
                    {}

                public:
                    static std::shared_ptr<CanvasWidget> create(Context * context)
                    {
                        auto out = std::shared_ptr<CanvasWidget>(new CanvasWidget);
                        out->_init(context);
                        return out;
                    }

                protected:
                    void preLayoutEvent(PreLayoutEvent&) override
                    {
                        _setMinimumSize(glm::vec2(2000.f, 2000.f));
                    }
                };

            } // namespace

            struct Canvas::Private
            {
                std::shared_ptr<CanvasWidget> canvasWidget;
                //std::shared_ptr<ScrollWidget> scrollWidget;
                std::shared_ptr<StackLayout> stackLayout;
                std::map<std::shared_ptr<Widget>, glm::vec2> windows;
                std::set<std::shared_ptr<Widget> > layoutInit;
                std::map<std::shared_ptr<Widget>, std::shared_ptr<Widget> > widgetToWindow;
                std::map<std::shared_ptr<IObject>, std::shared_ptr<Widget> > titleBarToWindow;
                std::map<std::shared_ptr<IObject>, std::shared_ptr<Widget> > bottomBarToWindow;
                std::map<std::shared_ptr<IObject>, std::shared_ptr<Widget> > resizeHandleToWindow;
                std::map<std::shared_ptr<Widget>, std::shared_ptr<IObject> > windowToTitleBar;
                std::map<std::shared_ptr<Widget>, std::shared_ptr<IObject> > windowToBottomBar;
                std::map<std::shared_ptr<Widget>, std::shared_ptr<IObject> > windowToResizeHandle;
                uint32_t pressed = 0;
                glm::vec2 pressedPos;
                glm::vec2 pressedOffset;
            };

            void Canvas::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::MDI::Canvas");

                _p->canvasWidget = CanvasWidget::create(context);

                //_p->scrollWidget = ScrollWidget::create(ScrollType::Both, context);
                //_p->scrollWidget->addWidget(_p->canvasWidget);
                //_p->scrollWidget->setParent(shared_from_this());
                _p->stackLayout = StackLayout::create(context);
                _p->stackLayout->addWidget(_p->canvasWidget);
                _p->stackLayout->setParent(shared_from_this());
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

            std::shared_ptr<Window> Canvas::addWidget(const std::string & title, const std::shared_ptr<Widget>& value, const glm::vec2& pos)
            {
                auto context = getContext();
                auto window = MDI::Window::create(context);
                window->setTitle(title);
                window->addWidget(value);
                window->setParent(_p->canvasWidget);
                window->move(pos);

                auto titleBar = window->getTitleBar();
                auto bottomBar = window->getBottomBar();
                auto resizeHandle = window->getResizeHandle();
                titleBar->installEventFilter(shared_from_this());
                bottomBar->installEventFilter(shared_from_this());
                resizeHandle->installEventFilter(shared_from_this());

                _p->windows[window] = pos;
                _p->layoutInit.insert(window);
                _p->widgetToWindow[value] = window;
                _p->titleBarToWindow[titleBar] = window;
                _p->bottomBarToWindow[bottomBar] = window;
                _p->resizeHandleToWindow[resizeHandle] = window;
                _p->windowToTitleBar[window] = titleBar;
                _p->windowToBottomBar[window] = bottomBar;
                _p->windowToResizeHandle[window] = resizeHandle;

                auto weak = std::weak_ptr<Canvas>(std::dynamic_pointer_cast<Canvas>(shared_from_this()));
                window->setClosedCallback(
                    [weak, value]
                {
                    if (auto canvas = weak.lock())
                    {
                        canvas->removeWidget(value);
                    }
                });
                
                return window;
            }

            void Canvas::removeWidget(const std::shared_ptr<Widget>& value)
            {
                const auto i = _p->widgetToWindow.find(value);
                if (i != _p->widgetToWindow.end())
                {
                    {
                        const auto j = _p->windows.find(i->second);
                        if (j != _p->windows.end())
                        {
                            _p->windows.erase(j);
                        }
                    }
                    {
                        const auto j = _p->layoutInit.find(i->second);
                        if (j != _p->layoutInit.end())
                        {
                            _p->layoutInit.erase(j);
                        }
                    }
                    {
                        const auto j = _p->windowToTitleBar.find(i->second);
                        if (j != _p->windowToTitleBar.end())
                        {
                            const auto k = _p->titleBarToWindow.find(j->second);
                            if (k != _p->titleBarToWindow.end())
                            {
                                _p->titleBarToWindow.erase(k);
                            }
                            _p->windowToTitleBar.erase(j);
                        }
                    }
                    {
                        const auto j = _p->windowToBottomBar.find(i->second);
                        if (j != _p->windowToBottomBar.end())
                        {
                            const auto k = _p->bottomBarToWindow.find(j->second);
                            if (k != _p->bottomBarToWindow.end())
                            {
                                _p->bottomBarToWindow.erase(k);
                            }
                            _p->windowToBottomBar.erase(j);
                        }
                    }
                    {
                        const auto j = _p->windowToResizeHandle.find(i->second);
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
                    i->first->setParent(nullptr);
                    i->second->setParent(nullptr);
                    _p->widgetToWindow.erase(i);
                }
            }

            void Canvas::clearWidgets()
            {
                for (auto i : _p->canvasWidget->getChildrenT<Widget>())
                {
                    i->setParent(nullptr);
                }
                _p->layoutInit.clear();
                _p->widgetToWindow.clear();
                _p->titleBarToWindow.clear();
                _p->bottomBarToWindow.clear();
                _p->resizeHandleToWindow.clear();
                _p->windowToTitleBar.clear();
                _p->windowToBottomBar.clear();
                _p->windowToResizeHandle.clear();
            }

            void Canvas::preLayoutEvent(PreLayoutEvent&)
            {
                //_setMinimumSize(_p->scrollWidget->getMinimumSize());
                _setMinimumSize(_p->stackLayout->getMinimumSize());
            }

            void Canvas::layoutEvent(LayoutEvent&)
            {
                //_p->scrollWidget->setGeometry(getGeometry());
                _p->stackLayout->setGeometry(getGeometry());

                for (auto& i : _p->layoutInit)
                {
                    const glm::vec2& size = i->getMinimumSize();
                    i->resize(size);
                }
                _p->layoutInit.clear();

                const BBox2f& g = _p->canvasWidget->getGeometry();
                {
                    for (auto& i : _p->windows)
                    {
                        const glm::vec2 pos = g.min + i.second;
                        i.first->move(pos);
                    }
                }
            }
            
            bool Canvas::eventFilter(const std::shared_ptr<IObject>& object, IEvent& event)
            {
                /*{
                    std::stringstream ss;
                    ss << event.getEventType();
                    _log(ss.str());
                }*/
                switch (event.getEventType())
                {
                case EventType::PointerMove:
                {
                    PointerMoveEvent& pointerMoveEvent = static_cast<PointerMoveEvent&>(event);
                    pointerMoveEvent.accept();
                    if (pointerMoveEvent.getPointerInfo().id == _p->pressed)
                    {
                        if (auto style = _getStyle().lock())
                        {
                            const float shadow = style->getMetric(MetricsRole::Shadow);
                            const BBox2f& canvasGeometry = _p->canvasWidget->getGeometry();
                            const auto titleBarToWindow = _p->titleBarToWindow.find(object);
                            const auto bottomBarToWindow = _p->bottomBarToWindow.find(object);
                            const auto resizeHandleToWindow = _p->resizeHandleToWindow.find(object);
                            if (titleBarToWindow != _p->titleBarToWindow.end())
                            {
                                const auto window = _p->windows.find(titleBarToWindow->second);
                                if (window != _p->windows.end())
                                {
                                    const glm::vec2 pos = pointerMoveEvent.getPointerInfo().projectedPos + _p->pressedOffset - canvasGeometry.min;
                                    window->second.x = Math::clamp(pos.x, -shadow, canvasGeometry.w() - window->first->getWidth() + shadow);
                                    window->second.y = Math::clamp(pos.y, -shadow, canvasGeometry.h() - window->first->getHeight() + shadow);
                                }
                            }
                            else if (bottomBarToWindow != _p->bottomBarToWindow.end())
                            {
                                const auto window = _p->windows.find(bottomBarToWindow->second);
                                if (window != _p->windows.end())
                                {
                                    const glm::vec2 pos = pointerMoveEvent.getPointerInfo().projectedPos + _p->pressedOffset - canvasGeometry.min;
                                    window->second.x = Math::clamp(pos.x, -shadow, canvasGeometry.w() - window->first->getWidth() + shadow);
                                    window->second.y = Math::clamp(pos.y, -shadow, canvasGeometry.h() - window->first->getHeight() + shadow);
                                }
                            }
                            else if (resizeHandleToWindow != _p->resizeHandleToWindow.end())
                            {
                                const auto window = _p->windows.find(resizeHandleToWindow->second);
                                if (window != _p->windows.end())
                                {
                                    const glm::vec2 min = window->first->getMinimumSize();
                                    const glm::vec2 pos = pointerMoveEvent.getPointerInfo().projectedPos + _p->pressedOffset - canvasGeometry.min;
                                    glm::vec2 pos2;
                                    pos2.x = Math::clamp(pos.x, window->second.x + min.x, canvasGeometry.w() + shadow);
                                    pos2.y = Math::clamp(pos.y, window->second.y + min.y, canvasGeometry.h() + shadow);
                                    window->first->resize(pos2 - window->first->getGeometry().min + canvasGeometry.min);
                                }
                            }
                        }
                    }
                    return true;
                }
                case EventType::ButtonPress:
                {
                    ButtonPressEvent& buttonPressEvent = static_cast<ButtonPressEvent&>(event);
                    if (!_p->pressed)
                    {
                        if (_p->titleBarToWindow.find(object) != _p->titleBarToWindow.end())
                        {
                            event.accept();
                            _p->pressed = buttonPressEvent.getPointerInfo().id;
                            _p->pressedPos = buttonPressEvent.getPointerInfo().projectedPos;
                            auto window = _p->titleBarToWindow[object];
                            window->raiseToTop();
                            _p->pressedOffset = window->getGeometry().min - _p->pressedPos;
                        }
                        else if (_p->bottomBarToWindow.find(object) != _p->bottomBarToWindow.end())
                        {
                            event.accept();
                            _p->pressed = buttonPressEvent.getPointerInfo().id;
                            _p->pressedPos = buttonPressEvent.getPointerInfo().projectedPos;
                            auto window = _p->bottomBarToWindow[object];
                            window->raiseToTop();
                            _p->pressedOffset = window->getGeometry().min - _p->pressedPos;
                        }
                        else if (_p->resizeHandleToWindow.find(object) != _p->resizeHandleToWindow.end())
                        {
                            event.accept();
                            _p->pressed = buttonPressEvent.getPointerInfo().id;
                            _p->pressedPos = buttonPressEvent.getPointerInfo().projectedPos;
                            auto window = _p->resizeHandleToWindow[object];
                            window->raiseToTop();
                            _p->pressedOffset = window->getGeometry().max - _p->pressedPos;
                        }
                    }
                    return true;
                }
                case EventType::ButtonRelease:
                {
                    ButtonReleaseEvent& buttonReleaseEvent = static_cast<ButtonReleaseEvent&>(event);
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
