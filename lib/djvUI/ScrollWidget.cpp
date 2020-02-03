//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)handl
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvUI/ScrollWidget.h>

#include <djvUI/Border.h>
#include <djvUI/DrawUtil.h>
#include <djvUI/GridLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/Window.h>

#include <djvCore/Math.h>

#include <djvAV/Render2D.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/geometric.hpp>

#include <list>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            //! \todo Should this be configurable?
            const float  velocityDecay              = .5F;  // How quickly the velocity decays.
            const float  velocityStopDelta          = 5.F;  // The minimum amount of movement to stop the velocity.
            const size_t pointerAverageCount        = 5;    // The number of pointer samples to average.
            const float  pointerAverageDecay        = 1.F;  // How quickly the pointer samples decay.
            const size_t pointerAverageDecayTimeout = 16;   // The timer resolution for pointer sample decay.
            const float  scrollWheelMult            = 50.F; // The scroll wheel multiplier.

            class ScrollBar : public Widget
            {
                DJV_NON_COPYABLE(ScrollBar);

            protected:
                void _init(Orientation, const std::shared_ptr<Context>&);
                ScrollBar();

            public:
                virtual ~ScrollBar();

                static std::shared_ptr<ScrollBar> create(Orientation, const std::shared_ptr<Context>&);

                void setViewSize(float);
                void setContentsSize(float);

                float getScrollPos() const;
                void setScrollPos(float);
                void setScrollPosCallback(const std::function<void(float)>&);

                MetricsRole getSizeRole() const;
                void setSizeRole(MetricsRole);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _paintEvent(Event::Paint&) override;
                void _pointerEnterEvent(Event::PointerEnter&) override;
                void _pointerLeaveEvent(Event::PointerLeave&) override;
                void _pointerMoveEvent(Event::PointerMove&) override;
                void _buttonPressEvent(Event::ButtonPress&) override;
                void _buttonReleaseEvent(Event::ButtonRelease&) override;

            private:
                float _valueToPos(float) const;
                float _posToValue(float) const;

                void _doScrollPosCallback();

                Orientation _orientation = Orientation::Vertical;
                float _viewSize = 0.F;
                float _contentsSize = 0.F;
                float _scrollPos = 0.F;
                std::function<void(float)> _scrollPosCallback;
                MetricsRole _sizeRole = MetricsRole::ScrollBar;
                std::map<Event::PointerID, bool> _hover;
                Event::PointerID _pressedID = Event::InvalidID;
                float _pressedPos = 0.F;
                float _pressedScrollPos = 0.F;
            };

            void ScrollBar::_init(Orientation orientation, const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::UI::ScrollBar");
                setPointerEnabled(true);

                _orientation = orientation;
            }

            ScrollBar::ScrollBar()
            {}

            ScrollBar::~ScrollBar()
            {}

            std::shared_ptr<ScrollBar> ScrollBar::create(Orientation orientation, const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ScrollBar>(new ScrollBar);
                out->_init(orientation, context);
                return out;
            }

            void ScrollBar::setViewSize(float value)
            {
                if (fuzzyCompare(value, _viewSize))
                    return;
                _viewSize = value;
                _redraw();
            }

            void ScrollBar::setContentsSize(float value)
            {
                if (fuzzyCompare(value, _contentsSize))
                    return;
                _contentsSize = value;
                _redraw();
            }

            float ScrollBar::getScrollPos() const
            {
                return _scrollPos;
            }

            void ScrollBar::setScrollPos(float value)
            {
                if (value == _scrollPos)
                    return;
                _scrollPos = value;
                _redraw();
            }

            void ScrollBar::setScrollPosCallback(const std::function<void(float)>& callback)
            {
                _scrollPosCallback = callback;
            }

            MetricsRole ScrollBar::getSizeRole() const
            {
                return _sizeRole;
            }

            void ScrollBar::setSizeRole(MetricsRole value)
            {
                if (value == _sizeRole)
                    return;
                _sizeRole = value;
                _resize();
            }

            void ScrollBar::_preLayoutEvent(Event::PreLayout&)
            {
                glm::vec2 size = glm::vec2(0.F, 0.F);
                const auto& style = _getStyle();
                size += style->getMetric(_sizeRole);
                _setMinimumSize(size);
            }

            void ScrollBar::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);

                const auto& style = _getStyle();
                const BBox2f& g = getGeometry();
                const float b = style->getMetric(MetricsRole::Border);

                // Draw the background.
                auto render = _getRender();
                render->setFillColor(style->getColor(ColorRole::Trough));
                render->drawRect(g);

                if (_viewSize < _contentsSize)
                {
                    // Draw the scroll bar handle.
                    BBox2f handleGeom;
                    switch (_orientation)
                    {
                    case Orientation::Horizontal:
                    {
                        const float x = _valueToPos(_scrollPos);
                        handleGeom = BBox2f(x, g.y(), _valueToPos(_scrollPos + _viewSize) - x, g.h());
                        break;
                    }
                    case Orientation::Vertical:
                    {
                        const float y = _valueToPos(_scrollPos);
                        handleGeom = BBox2f(g.x(), y, g.w(), _valueToPos(_scrollPos + _viewSize) - y);
                        break;
                    }
                    default: break;
                    }
                    render->setFillColor(style->getColor(ColorRole::Border));
                    drawBorder(render, handleGeom, b);
                    render->setFillColor(style->getColor(ColorRole::Button));
                    render->drawRect(handleGeom.margin(-b));

                    // Draw the pressed and hovered state.
                    if (_pressedID)
                    {
                        render->setFillColor(style->getColor(ColorRole::Pressed));
                        render->drawRect(handleGeom);
                    }
                    else
                    {
                        bool hover = false;
                        for (const auto& h : _hover)
                        {
                            hover |= h.second;
                        }
                        if (hover)
                        {
                            render->setFillColor(style->getColor(ColorRole::Hovered));
                            render->drawRect(handleGeom);
                        }
                    }
                }
            }

            void ScrollBar::_pointerEnterEvent(Event::PointerEnter& event)
            {
                if (!event.isRejected())
                {
                    event.accept();
                    _hover[event.getPointerInfo().id] = true;
                    if (isEnabled(true))
                    {
                        _redraw();
                    }
                }
            }

            void ScrollBar::_pointerLeaveEvent(Event::PointerLeave& event)
            {
                event.accept();
                auto i = _hover.find(event.getPointerInfo().id);
                if (i != _hover.end())
                {
                    _hover.erase(i);
                    if (isEnabled(true))
                    {
                        _redraw();
                    }
                }
            }

            void ScrollBar::_pointerMoveEvent(Event::PointerMove& event)
            {
                event.accept();
                if (event.getPointerInfo().id == _pressedID)
                {
                    // Calculate the new scroll position.
                    const auto& pos = event.getPointerInfo().projectedPos;
                    float p = 0.F;
                    switch (_orientation)
                    {
                    case Orientation::Horizontal:
                        p = pos.x;
                        break;
                    case Orientation::Vertical:
                        p = pos.y;
                        break;
                    default: break;
                    }
                    const float v = _posToValue(p) - _posToValue(_pressedPos);
                    _scrollPos = Math::clamp(_pressedScrollPos + v, 0.F, _contentsSize - _viewSize);
                    _doScrollPosCallback();
                    _redraw();
                }
            }

            void ScrollBar::_buttonPressEvent(Event::ButtonPress& event)
            {
                if (_pressedID)
                    return;
                event.accept();
                _pressedID = event.getPointerInfo().id;
                const auto& pos = event.getPointerInfo().projectedPos;
                switch (_orientation)
                {
                case Orientation::Horizontal:
                    _pressedPos = pos.x;
                    break;
                case Orientation::Vertical:
                    _pressedPos = pos.y;
                    break;
                default: break;
                }
                const float v = _posToValue(_pressedPos);
                const float jump = v < _scrollPos ? -_viewSize : (v > _scrollPos + _viewSize ? _viewSize : 0.F);
                if (jump != 0)
                {
                    _scrollPos = Math::clamp(_scrollPos + jump, 0.F, _contentsSize - _viewSize);
                    _doScrollPosCallback();
                }
                _pressedScrollPos = _scrollPos;
                _redraw();
            }

            void ScrollBar::_buttonReleaseEvent(Event::ButtonRelease& event)
            {
                if (event.getPointerInfo().id != _pressedID)
                    return;
                event.accept();
                _pressedID = Event::InvalidID;
                _redraw();
            }

            float ScrollBar::_valueToPos(float value) const
            {
                const BBox2f& g = getGeometry();
                float out = 0.F;
                const float v = std::min(value / (_contentsSize > 0 ? static_cast<float>(_contentsSize) : 1.F), 1.F);
                switch (_orientation)
                {
                case Orientation::Horizontal:
                    out = g.x() + ceilf(g.w() * v);
                    break;
                case Orientation::Vertical:
                    out = g.y() + ceilf(g.h() * v);
                    break;
                default: break;
                }
                return out;
            }

            float ScrollBar::_posToValue(float value) const
            {
                const BBox2f& g = getGeometry();
                float v = 0.F;
                switch (_orientation)
                {
                case Orientation::Horizontal:
                    v = (value - g.x()) / g.w();
                    break;
                case Orientation::Vertical:
                    v = (value - g.y()) / g.h();
                    break;
                default: break;
                }
                return v * _contentsSize;
            }

            void ScrollBar::_doScrollPosCallback()
            {
                if (_scrollPosCallback)
                {
                    _scrollPosCallback(_scrollPos);
                }
            }

            class ScrollArea : public Widget
            {
                DJV_NON_COPYABLE(ScrollArea);
                
            protected:
                void _init(ScrollType, const std::shared_ptr<Context>&);
                ScrollArea()
                {}

            public:
                virtual ~ScrollArea()
                {}

                static std::shared_ptr<ScrollArea> create(ScrollType, const std::shared_ptr<Context>&);

                ScrollType getScrollType() const { return _scrollType; }
                void setScrollType(ScrollType);

                const glm::vec2 & getContentsSize() const { return _contentsSize; }
                void setContentsSizeCallback(const std::function<void(const glm::vec2 &)> &);

                const glm::vec2 & getScrollPos() const { return _scrollPos; }
                bool setScrollPos(const glm::vec2 &);
                void setScrollPosCallback(const std::function<void(const glm::vec2 &)> &);

                MetricsRole getMinimumSizeRole() const { return _minimumSizeRole; }
                void setMinimumSizeRole(MetricsRole);

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;

            private:
                void _doScrollPosCallback();

                ScrollType _scrollType = ScrollType::Both;
                glm::vec2 _contentsSize = glm::vec2(0.F, 0.F);
                std::function<void(const glm::vec2 &)> _contentsSizeCallback;
                glm::vec2 _scrollPos = glm::vec2(0.F, 0.F);
                std::function<void(const glm::vec2 &)> _scrollPosCallback;
                MetricsRole _minimumSizeRole = MetricsRole::ScrollArea;
            };

            void ScrollArea::_init(ScrollType scrollType, const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::ScrollArea");
                _scrollType = scrollType;
            }

            std::shared_ptr<ScrollArea> ScrollArea::create(ScrollType scrollType, const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ScrollArea>(new ScrollArea);
                out->_init(scrollType, context);
                return out;
            }

            void ScrollArea::setScrollType(ScrollType value)
            {
                if (value == _scrollType)
                    return;
                _scrollType = value;
                _resize();
            }

            void ScrollArea::setContentsSizeCallback(const std::function<void(const glm::vec2 &)> & callback)
            {
                _contentsSizeCallback = callback;
            }

            bool ScrollArea::setScrollPos(const glm::vec2 & value)
            {
                const BBox2f & g = getGeometry();
                const glm::vec2 tmp(
                    floorf(Math::clamp(value.x, 0.F, _contentsSize.x - g.w())),
                    floorf(Math::clamp(value.y, 0.F, _contentsSize.y - g.h())));
                if (tmp == _scrollPos)
                    return false;
                _scrollPos = tmp;
                _doScrollPosCallback();
                _resize();
                return true;
            }

            void ScrollArea::setScrollPosCallback(const std::function<void(const glm::vec2 &)> & callback)
            {
                _scrollPosCallback = callback;
            }

            void ScrollArea::setMinimumSizeRole(MetricsRole value)
            {
                if (value == _minimumSizeRole)
                    return;
                _minimumSizeRole = value;
                _resize();
            }

            void ScrollArea::_preLayoutEvent(Event::PreLayout &)
            {
                glm::vec2 childrenMinimumSize = glm::vec2(0.F, 0.F);
                for (const auto & child : getChildWidgets())
                {
                    if (child->isVisible())
                    {
                        childrenMinimumSize = glm::max(childrenMinimumSize, child->getMinimumSize());
                    }
                }
                const auto& style = _getStyle();
                const glm::vec2 minimumSize = glm::vec2(style->getMetric(MetricsRole::ScrollArea), style->getMetric(_minimumSizeRole));
                glm::vec2 size = minimumSize;
                switch (_scrollType)
                {
                case ScrollType::Horizontal:
                    size.x = std::min(childrenMinimumSize.x, size.x);
                    size.y = std::max(childrenMinimumSize.y, size.y);
                    break;
                case ScrollType::Vertical:
                    size.x = std::max(childrenMinimumSize.x, size.x);
                    size.y = std::min(childrenMinimumSize.y, size.y);
                    break;
                default: break;
                }
                _setMinimumSize(size);
            }

            void ScrollArea::_layoutEvent(Event::Layout & event)
            {
                const BBox2f & g = getGeometry();
                const float gw = g.w();
                const float gh = g.h();

                // Update the contents size.
                glm::vec2 contentsSize = glm::vec2(0.F, 0.F);
                for (const auto & child : getChildWidgets())
                {
                    if (child->isVisible())
                    {
                        const auto & ms = child->getMinimumSize();
                        switch (_scrollType)
                        {
                        case ScrollType::Both:
                            contentsSize = glm::max(contentsSize, ms);
                            contentsSize = glm::max(contentsSize, glm::vec2(gw, gh));
                            break;
                        case ScrollType::Horizontal:
                            contentsSize.x = std::max(contentsSize.x, ms.x);
                            contentsSize.y = gh;
                            break;
                        case ScrollType::Vertical:
                            contentsSize.x = gw;
                            contentsSize.y = std::max(contentsSize.y, child->getHeightForWidth(gw));
                            break;
                        default: break;
                        }
                    }
                }
                if (contentsSize != _contentsSize)
                {
                    _contentsSize = contentsSize;
                    if (_contentsSizeCallback)
                    {
                        _contentsSizeCallback(_contentsSize);
                    }
                }

                // Update the scroll position.
                const glm::vec2 scrollPos(
                    Math::clamp(_scrollPos.x, 0.F, _contentsSize.x - gw),
                    Math::clamp(_scrollPos.y, 0.F, _contentsSize.y - gh));
                if (scrollPos != _scrollPos)
                {
                    _scrollPos = scrollPos;
                    _doScrollPosCallback();
                    _resize();
                }

                // Update the child geometry.
                glm::vec2 pos = g.min;
                pos -= _scrollPos;
                for (const auto & child : getChildWidgets())
                {
                    switch (_scrollType)
                    {
                    case ScrollType::Both:
                        child->setGeometry(BBox2f(pos.x, pos.y, _contentsSize.x, _contentsSize.y));
                        break;
                    case ScrollType::Horizontal:
                        child->setGeometry(BBox2f(pos.x, pos.y, _contentsSize.x, gh));
                        break;
                    case ScrollType::Vertical:
                        child->setGeometry(BBox2f(pos.x, pos.y, gw, _contentsSize.y));
                        break;
                    default: break;
                    }
                }
            }

            void ScrollArea::_doScrollPosCallback()
            {
                if (_scrollPosCallback)
                {
                    _scrollPosCallback(_scrollPos);
                }
            }

        } // namespace
        
        struct ScrollWidget::Private
        {
            ScrollType scrollType = ScrollType::First;
            std::shared_ptr<ScrollArea> scrollArea;
            std::shared_ptr<Widget> scrollAreaSwipe;
            std::map<Orientation, std::shared_ptr<ScrollBar> > scrollBars;
            bool autoHideScrollBars = true;
            std::shared_ptr<Border> border;
            Event::PointerID pointerID = Event::InvalidID;
            glm::vec2 pointerPos = glm::vec2(0.F, 0.F);
            std::list<glm::vec2> pointerAverage;
            std::shared_ptr<Time::Timer> pointerAverageTimer;
            glm::vec2 swipeVelocity = glm::vec2(0.F, 0.F);
            std::shared_ptr<Time::Timer> swipeTimer;
        };

        void ScrollWidget::_init(ScrollType scrollType, const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            setClassName("djv::UI::ScrollWidget");

            DJV_PRIVATE_PTR();
            p.scrollType = scrollType;

            p.scrollArea = ScrollArea::create(scrollType, context);
            p.scrollArea->installEventFilter(shared_from_this());

            p.scrollAreaSwipe = Widget::create(context);
            //p.scrollAreaSwipe->setBackgroundRole(ColorRole::Overlay);
            p.scrollAreaSwipe->setVisible(false);
            p.scrollAreaSwipe->installEventFilter(shared_from_this());

            p.scrollBars[Orientation::Horizontal] = ScrollBar::create(Orientation::Horizontal, context);
            p.scrollBars[Orientation::Vertical] = ScrollBar::create(Orientation::Vertical, context);

            auto stackLayout = StackLayout::create(context);
            stackLayout->addChild(p.scrollArea);
            stackLayout->addChild(p.scrollAreaSwipe);
            
            auto layout = GridLayout::create(context);
            layout->setSpacing(Layout::Spacing(MetricsRole::None));
            layout->addChild(stackLayout);
            layout->setGridPos(stackLayout, glm::ivec2(0, 0), GridStretch::Both);
            layout->addChild(p.scrollBars[Orientation::Horizontal]);
            layout->setGridPos(p.scrollBars[Orientation::Horizontal], glm::ivec2(0, 1));
            layout->addChild(p.scrollBars[Orientation::Vertical]);
            layout->setGridPos(p.scrollBars[Orientation::Vertical], glm::ivec2(1, 0));

            p.border = Border::create(context);
            p.border->addChild(layout);
            Widget::addChild(p.border);

            _updateScrollBars(glm::vec2(0.F, 0.F));

            auto weak = std::weak_ptr<ScrollWidget>(std::dynamic_pointer_cast<ScrollWidget>(shared_from_this()));
            p.scrollArea->setScrollPosCallback(
                [weak](const glm::vec2 & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->scrollBars[Orientation::Horizontal]->setScrollPos(value.x);
                    widget->_p->scrollBars[Orientation::Vertical]->setScrollPos(value.y);

                    const BBox2f & g = widget->_p->scrollArea->getGeometry();
                    const glm::vec2 & contentsSize = widget->_p->scrollArea->getContentsSize();
                    if (value.x <= 0.F || value.x >= contentsSize.x - g.w())
                    {
                        widget->_p->swipeVelocity.x = 0.F;
                    }
                    if (value.y <= 0.F || value.y >= contentsSize.y - g.h())
                    {
                        widget->_p->swipeVelocity.y = 0.F;
                    }
                }
            });
            p.scrollArea->setContentsSizeCallback(
                [weak](const glm::vec2 & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_updateScrollBars(value);
                }
            });

            p.scrollBars[Orientation::Horizontal]->setScrollPosCallback(
                [weak](float value)
            {
                if (auto widget = weak.lock())
                {
                    glm::vec2 scrollPos = widget->_p->scrollArea->getScrollPos();
                    scrollPos.x = value;
                    widget->_p->scrollArea->setScrollPos(scrollPos);
                    widget->_p->swipeVelocity = glm::vec2(0.F, 0.F);
                }
            });

            p.scrollBars[Orientation::Vertical]->setScrollPosCallback(
                [weak](float value)
            {
                if (auto widget = weak.lock())
                {
                    glm::vec2 scrollPos = widget->_p->scrollArea->getScrollPos();
                    scrollPos.y = value;
                    widget->_p->scrollArea->setScrollPos(scrollPos);
                    widget->_p->swipeVelocity = glm::vec2(0.F, 0.F);
                }
            });

            p.pointerAverageTimer = Time::Timer::create(context);
            p.pointerAverageTimer->setRepeating(true);
            p.pointerAverageTimer->start(
                std::chrono::milliseconds(pointerAverageDecayTimeout),
                [weak](const std::chrono::steady_clock::time_point&, const Time::Unit&)
            {
                if (auto widget = weak.lock())
                {
                    for (auto & i : widget->_p->pointerAverage)
                    {
                        if (i.x > 0.F)
                        {
                            i.x -= pointerAverageDecay;
                        }
                        else if (i.x < 0.F)
                        {
                            i.x += pointerAverageDecay;
                        }
                        if (i.y > 0.F)
                        {
                            i.y -= pointerAverageDecay;
                        }
                        else if (i.y < 0.F)
                        {
                            i.y += pointerAverageDecay;
                        }
                    }
                }
            });

            p.swipeTimer = Time::Timer::create(context);
            p.swipeTimer->setRepeating(true);
            p.swipeTimer->start(
                Time::getTime(Time::TimerValue::Fast),
                [weak](const std::chrono::steady_clock::time_point&, const Time::Unit& value)
            {
                if (auto widget = weak.lock())
                {
                    const glm::vec2 & pos = widget->_p->scrollArea->getScrollPos();
                    glm::vec2 scrollPos(ceilf(pos.x + widget->_p->swipeVelocity.x), ceilf(pos.y + widget->_p->swipeVelocity.y));
                    if (widget->_p->scrollArea->setScrollPos(scrollPos))
                    {
                        const float mult = value.count() / static_cast<float>(Time::getTime(Time::TimerValue::Fast).count());
                        const float decay = velocityDecay * mult;
                        if (widget->_p->swipeVelocity.x > 0.F)
                        {
                            widget->_p->swipeVelocity.x -= decay;
                        }
                        else if (widget->_p->swipeVelocity.x < 0.F)
                        {
                            widget->_p->swipeVelocity.x += decay;
                        }
                        if (widget->_p->swipeVelocity.y > 0.F)
                        {
                            widget->_p->swipeVelocity.y -= decay;
                        }
                        else if (widget->_p->swipeVelocity.y < 0.F)
                        {
                            widget->_p->swipeVelocity.y += decay;
                        }
                        const float v = glm::length(widget->_p->swipeVelocity);
                        widget->_p->scrollAreaSwipe->setVisible(v > 1.F);
                        if (v < 1.F)
                        {
                            widget->_p->swipeVelocity = glm::vec2(0.F, 0.F);
                        }
                    }
                    else
                    {
                        widget->_p->scrollAreaSwipe->hide();
                        widget->_p->swipeVelocity = glm::vec2(0.F, 0.F);
                    }
                }
            });
        }

        ScrollWidget::ScrollWidget() :
            _p(new Private)
        {}

        ScrollWidget::~ScrollWidget()
        {}

        std::shared_ptr<ScrollWidget> ScrollWidget::create(ScrollType scrollType, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ScrollWidget>(new ScrollWidget);
            out->_init(scrollType, context);
            return out;
        }

        ScrollType ScrollWidget::getScrollType() const
        {
            return _p->scrollArea->getScrollType();
        }

        void ScrollWidget::setScrollType(ScrollType value)
        {
            _p->scrollArea->setScrollType(value);
        }

        const glm::vec2 & ScrollWidget::getScrollPos() const
        {
            return _p->scrollArea->getScrollPos();
        }

        void ScrollWidget::setScrollPos(const glm::vec2 & value)
        {
            _p->scrollArea->setScrollPos(value);
        }

        void ScrollWidget::moveToBegin()
        {
            DJV_PRIVATE_PTR();
            const glm::vec2& scrollPos = p.scrollArea->getScrollPos();
            setScrollPos(glm::vec2(scrollPos.x, 0.F));
        }

        void ScrollWidget::moveToEnd()
        {
            DJV_PRIVATE_PTR();
            const glm::vec2& scrollPos = p.scrollArea->getScrollPos();
            const glm::vec2& contentsSize = p.scrollArea->getContentsSize();
            setScrollPos(glm::vec2(scrollPos.x, contentsSize.y));
        }

        void ScrollWidget::movePageUp()
        {
            DJV_PRIVATE_PTR();
            const glm::vec2& scrollPos = p.scrollArea->getScrollPos();
            const glm::vec2& scrollSize = p.scrollArea->getSize();
            setScrollPos(glm::vec2(scrollPos.x, scrollPos.y - scrollSize.y));
        }

        void ScrollWidget::movePageDown()
        {
            DJV_PRIVATE_PTR();
            const glm::vec2& scrollPos = p.scrollArea->getScrollPos();
            const glm::vec2& scrollSize = p.scrollArea->getSize();
            setScrollPos(glm::vec2(scrollPos.x, scrollPos.y + scrollSize.y));
        }

        void ScrollWidget::moveUp()
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float m = style->getMetric(MetricsRole::Move);
            const glm::vec2& scrollPos = p.scrollArea->getScrollPos();
            setScrollPos(glm::vec2(scrollPos.x, scrollPos.y - m));
        }

        void ScrollWidget::moveDown()
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float m = style->getMetric(MetricsRole::Move);
            const glm::vec2& scrollPos = p.scrollArea->getScrollPos();
            setScrollPos(glm::vec2(scrollPos.x, scrollPos.y + m));
        }

        void ScrollWidget::moveLeft()
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float m = style->getMetric(MetricsRole::Move);
            const glm::vec2& scrollPos = p.scrollArea->getScrollPos();
            setScrollPos(glm::vec2(scrollPos.x - m, scrollPos.y));
        }

        void ScrollWidget::moveRight()
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float m = style->getMetric(MetricsRole::Move);
            const glm::vec2& scrollPos = p.scrollArea->getScrollPos();
            setScrollPos(glm::vec2(scrollPos.x + m, scrollPos.y));
        }

        bool ScrollWidget::hasAutoHideScrollBars() const
        {
            return _p->autoHideScrollBars;
        }

        void ScrollWidget::setAutoHideScrollBars(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.autoHideScrollBars)
                return;
            p.autoHideScrollBars = value;
            _resize();
        }

        void ScrollWidget::setBorder(bool value)
        {
            _p->border->setBorderSize(value ? MetricsRole::Border : MetricsRole::None);
        }

        MetricsRole ScrollWidget::getMinimumSizeRole() const
        {
            return _p->scrollArea->getMinimumSizeRole();
        }

        MetricsRole ScrollWidget::getScrollBarSizeRole() const
        {
            return _p->scrollBars[Orientation::First]->getSizeRole();
        }

        void ScrollWidget::setMinimumSizeRole(MetricsRole value)
        {
            _p->scrollArea->setMinimumSizeRole(value);
        }

        void ScrollWidget::setScrollBarSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            p.scrollBars[Orientation::Horizontal]->setSizeRole(value);
            p.scrollBars[Orientation::Vertical]->setSizeRole(value);
        }

        void ScrollWidget::addChild(const std::shared_ptr<IObject> & value)
        {
            _p->scrollArea->addChild(value);
        }

        void ScrollWidget::removeChild(const std::shared_ptr<IObject> & value)
        {
            _p->scrollArea->removeChild(value);
        }

        void ScrollWidget::clearChildren()
        {
            _p->scrollArea->clearChildren();
        }

        void ScrollWidget::_preLayoutEvent(Event::PreLayout &)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            _setMinimumSize(p.border->getMinimumSize() + getMargin().getSize(style));
            const glm::vec2 contentsSize = p.scrollArea->getContentsSize();
            _updateScrollBars(contentsSize);
        }

        void ScrollWidget::_layoutEvent(Event::Layout &)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f& g = getMargin().bbox(getGeometry(), style);
            p.border->setGeometry(g);
        }

        void ScrollWidget::_clipEvent(Event::Clip &)
        {
            if (isClipped())
            {
                _p->swipeVelocity = glm::vec2(0.F, 0.F);
            }
        }

        void ScrollWidget::_keyPressEvent(Event::KeyPress& event)
        {
            Widget::_keyPressEvent(event);
            DJV_PRIVATE_PTR();
            if (!event.isAccepted())
            {
                switch (event.getKey())
                {
                case GLFW_KEY_HOME:
                    event.accept();
                    moveToBegin();
                    break;
                case GLFW_KEY_END:
                    event.accept();
                    moveToEnd();
                    break;
                case GLFW_KEY_PAGE_UP:
                    switch (p.scrollType)
                    {
                    case ScrollType::Vertical:
                    case ScrollType::Both:
                        event.accept();
                        movePageUp();
                        break;
                    default: break;
                    }
                    break;
                case GLFW_KEY_PAGE_DOWN:
                    switch (p.scrollType)
                    {
                    case ScrollType::Vertical:
                    case ScrollType::Both:
                        event.accept();
                        movePageDown();
                        break;
                    default: break;
                    }
                    break;
                case GLFW_KEY_UP:
                    switch (p.scrollType)
                    {
                    case ScrollType::Vertical:
                    case ScrollType::Both:
                        event.accept();
                        moveUp();
                        break;
                    default: break;
                    }
                    break;
                case GLFW_KEY_DOWN:
                    switch (p.scrollType)
                    {
                    case ScrollType::Vertical:
                    case ScrollType::Both:
                        event.accept();
                        moveDown();
                        break;
                    default: break;
                    }
                    break;
                case GLFW_KEY_LEFT:
                    switch (p.scrollType)
                    {
                    case ScrollType::Horizontal:
                    case ScrollType::Both:
                        event.accept();
                        moveLeft();
                        break;
                    default: break;
                    }
                    break;
                case GLFW_KEY_RIGHT:
                    switch (p.scrollType)
                    {
                    case ScrollType::Horizontal:
                    case ScrollType::Both:
                        event.accept();
                        moveRight();
                        break;
                    default: break;
                    }
                    break;
                }
            }
        }

        void ScrollWidget::_scrollEvent(Event::Scroll & event)
        {
            event.accept();
            setScrollPos(_p->scrollArea->getScrollPos() - event.getScrollDelta() * scrollWheelMult);
        }

        bool ScrollWidget::_eventFilter(const std::shared_ptr<IObject> & object, Event::Event & event)
        {
            DJV_PRIVATE_PTR();
            switch (event.getEventType())
            {
            case Event::Type::PointerEnter:
                event.accept();
                return true;
            case Event::Type::PointerMove:
            {
                auto & pointerEvent = static_cast<Event::PointerMove &>(event);
                if (pointerEvent.getPointerInfo().id == p.pointerID)
                {
                    pointerEvent.accept();
                    const glm::vec2 & pos = pointerEvent.getPointerInfo().projectedPos;
                    if (pos != p.pointerPos)
                    {
                        const glm::vec2 delta = pos - p.pointerPos;
                        p.pointerPos = pos;
                        _addPointerSample(delta);
                        p.scrollArea->setScrollPos(p.scrollArea->getScrollPos() + delta);
                        if (!Math::haveSameSign(p.swipeVelocity.x, delta.x))
                        {
                            p.swipeVelocity.x = 0.F;
                        }
                        if (!Math::haveSameSign(p.swipeVelocity.y, delta.y))
                        {
                            p.swipeVelocity.y = 0.F;
                        }
                    }
                }
                event.accept();
                return true;
            }
            case Event::Type::ButtonPress:
            {
                auto & pointerEvent = static_cast<Event::ButtonPress &>(event);
                if (!p.pointerID)
                {
                    pointerEvent.accept();
                    p.pointerID = pointerEvent.getPointerInfo().id;
                    p.pointerPos = pointerEvent.getPointerInfo().projectedPos;
                    p.pointerAverage.clear();
                    p.scrollAreaSwipe->setVisible(true);
                    return true;
                }
                break;
            }
            case Event::Type::ButtonRelease:
            {
                auto & pointerEvent = static_cast<Event::ButtonRelease &>(event);
                if (pointerEvent.getPointerInfo().id == p.pointerID)
                {
                    pointerEvent.accept();
                    p.pointerID = Event::InvalidID;
                    p.pointerPos = pointerEvent.getPointerInfo().projectedPos;
                    const auto delta = _getPointerAverage();
                    if (glm::length(delta) < velocityStopDelta)
                    {
                        p.swipeVelocity = glm::vec2(0.F, 0.F);
                    }
                    else
                    {
                        if (Math::haveSameSign(delta.x, p.swipeVelocity.x))
                        {
                            p.swipeVelocity.x += delta.x;
                        }
                        else
                        {
                            p.swipeVelocity.x = delta.x;
                        }
                        if (Math::haveSameSign(delta.y, p.swipeVelocity.y))
                        {
                            p.swipeVelocity.y += delta.y;
                        }
                        else
                        {
                            p.swipeVelocity.y = delta.y;
                        }
                    }
                    return true;
                }
                break;
            }
            default: break;
            }
            return false;
        }

        void ScrollWidget::_updateScrollBars(const glm::vec2 & value)
        {
            DJV_PRIVATE_PTR();
            const BBox2f & g = p.scrollArea->getGeometry();
            const float w = g.w();
            const float h = g.h();

            std::map<ScrollType, bool> visible;
            switch (p.scrollType)
            {
            case ScrollType::Both:
                visible[ScrollType::Horizontal] = visible[ScrollType::Vertical] = true;
                break;
            case ScrollType::Horizontal:
                visible[ScrollType::Horizontal] = true;
                break;
            case ScrollType::Vertical:
                visible[ScrollType::Vertical] = true;
                break;
            default: break;
            }
            if (p.autoHideScrollBars)
            {
                visible[ScrollType::Horizontal] &= w < value.x;
                visible[ScrollType::Vertical]   &= h < value.y;
            }

            p.scrollBars[Orientation::Horizontal]->setViewSize(w);
            p.scrollBars[Orientation::Horizontal]->setContentsSize(value.x);
            p.scrollBars[Orientation::Horizontal]->setVisible(visible[ScrollType::Horizontal]);
            p.scrollBars[Orientation::Horizontal]->setEnabled(w < value.x);

            p.scrollBars[Orientation::Vertical]->setViewSize(h);
            p.scrollBars[Orientation::Vertical]->setContentsSize(value.y);
            p.scrollBars[Orientation::Vertical]->setVisible(visible[ScrollType::Vertical]);
            p.scrollBars[Orientation::Vertical]->setEnabled(h < value.y);
        }

        void ScrollWidget::_addPointerSample(const glm::vec2 & value)
        {
            DJV_PRIVATE_PTR();
            p.pointerAverage.push_back(value);
            while (p.pointerAverage.size() > pointerAverageCount)
            {
                p.pointerAverage.pop_front();
            }
        }

        glm::vec2 ScrollWidget::_getPointerAverage() const
        {
            glm::vec2 out = glm::vec2(0.F, 0.F);
            DJV_PRIVATE_PTR();
            if (p.pointerAverage.size())
            {
                for (const auto & velocity : p.pointerAverage)
                {
                    out += velocity;
                }
                out /= static_cast<float>(p.pointerAverage.size());
            }
            return out;
        }

    } // namespace UI    

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        ScrollType,
        DJV_TEXT("Both"),
        DJV_TEXT("Horizontal"),
        DJV_TEXT("Vertical"));

} // namespace djv
