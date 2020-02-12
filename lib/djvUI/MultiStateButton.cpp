//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include <djvUI/MultiStateButton.h>

#include <djvUI/Icon.h>
#include <djvUI/Style.h>

#include <djvAV/Render2D.h>

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
                int currentIndex = -1;
                std::shared_ptr<Icon> icon;
                std::function<void(int)> callback;
                MetricsRole insideMargin = MetricsRole::MarginSmall;
                Event::PointerID pressedID = Event::invalidID;
                glm::vec2 pressedPos = glm::vec2(0.F, 0.F);
                bool canRejectPressed = true;
            };

            void MultiState::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Button::MultiState");
                setPointerEnabled(true);
                setVAlign(VAlign::Center);

                p.icon = Icon::create(context);
                addChild(p.icon);
            }

            MultiState::MultiState() :
                _p(new Private)
            {}

            MultiState::~MultiState()
            {}

            std::shared_ptr<MultiState> MultiState::create(const std::shared_ptr<Context>& context)
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

            void MultiState::setCallback(const std::function<void(int)>& callback)
            {
                _p->callback = callback;
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
            
            void MultiState::_preLayoutEvent(Event::PreLayout&)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float m = style->getMetric(p.insideMargin);
                glm::vec2 size = p.icon->getMinimumSize();
                size += m * 2.F;
                _setMinimumSize(size + getMargin().getSize(style));
            }

            void MultiState::_layoutEvent(Event::Layout&)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const BBox2f g = getMargin().bbox(getGeometry(), style);
                const float m = style->getMetric(p.insideMargin);
                p.icon->setGeometry(g.margin(-m));
            }

            void MultiState::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const BBox2f g = getMargin().bbox(getGeometry(), style);
                auto render = _getRender();
                if (p.pressedID != Event::invalidID)
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

            void MultiState::_pointerEnterEvent(Event::PointerEnter& event)
            {
                event.accept();
                if (isEnabled(true))
                {
                    _redraw();
                }
            }

            void MultiState::_pointerLeaveEvent(Event::PointerLeave& event)
            {
                event.accept();
                if (isEnabled(true))
                {
                    _redraw();
                }
            }

            void MultiState::_pointerMoveEvent(Event::PointerMove& event)
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
                        p.pressedID = Event::invalidID;
                        _redraw();
                    }
                }
            }

            void MultiState::_buttonPressEvent(Event::ButtonPress& event)
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

            void MultiState::_buttonReleaseEvent(Event::ButtonRelease& event)
            {
                DJV_PRIVATE_PTR();
                const auto& pointerInfo = event.getPointerInfo();
                if (pointerInfo.id == p.pressedID)
                {
                    event.accept();
                    p.pressedID = Event::invalidID;
                    const BBox2f& g = getGeometry();
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
                        if (p.callback)
                        {
                            p.callback(p.currentIndex);
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
                p.icon->setIcon(icon);
            }

        } // namespace Button
    } // namespace UI
} // namespace djv
