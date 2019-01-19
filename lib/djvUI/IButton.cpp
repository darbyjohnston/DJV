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

#include <djvUI/IButton.h>

#include <djvAV/Render2D.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            struct IButton::Private
            {
                ButtonType buttonType = ButtonType::Push;
                bool checked = false;
                Style::ColorRole foregroundColorRole = Style::ColorRole::Foreground;
                Style::ColorRole checkedColorRole = Style::ColorRole::Checked;
                Style::ColorRole checkedForegroundColorRole = Style::ColorRole::CheckedForeground;

                std::function<void(void)> clickedCallback;
                std::function<void(bool)> checkedCallback;

                Event::PointerID pressedId = Event::InvalidID;
                glm::vec2 pressedPos = glm::vec2(0.f, 0.f);
                bool canRejectPressed = true;
            };

            IButton::IButton() :
                _p(new Private)
            {
                setPointerEnabled(true);
            }

            IButton::~IButton()
            {}

            ButtonType IButton::getButtonType() const
            {
                return _p->buttonType;
            }

            void IButton::setButtonType(ButtonType value)
            {
                _p->buttonType = value;
            }

            bool IButton::isChecked() const
            {
                return _p->checked;
            }

            void IButton::setChecked(bool value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.checked)
                    return;
                p.checked = value;
                _redraw();
            }

            Style::ColorRole IButton::getForegroundColorRole() const
            {
                return _p->foregroundColorRole;
            }

            Style::ColorRole IButton::getCheckedColorRole() const
            {
                return _p->checkedColorRole;
            }

            Style::ColorRole IButton::getCheckedForegroundColorRole() const
            {
                return _p->checkedForegroundColorRole;
            }

            void IButton::setForegroundColorRole(Style::ColorRole value)
            {
                if (value == _p->foregroundColorRole)
                    return;
                _p->foregroundColorRole = value;
                _redraw();
            }

            void IButton::setCheckedColorRole(Style::ColorRole value)
            {
                if (_p->checkedColorRole == value)
                    return;
                _p->checkedColorRole = value;
                _redraw();
            }

            void IButton::setCheckedForegroundColorRole(Style::ColorRole value)
            {
                if (value == _p->checkedForegroundColorRole)
                    return;
                _p->checkedForegroundColorRole = value;
                _redraw();
            }

            void IButton::setClickedCallback(const std::function<void(void)>& callback)
            {
                _p->clickedCallback = callback;
            }

            void IButton::setCheckedCallback(const std::function<void(bool)>& callback)
            {
                _p->checkedCallback = callback;
            }

            void IButton::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                if (auto render = _getRender().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const BBox2f& g = getGeometry();

                        // Draw the toggled state.
                        DJV_PRIVATE_PTR();
                        if (_isToggled() && p.checkedColorRole != Style::ColorRole::None)
                        {
                            render->setFillColor(_getColorWithOpacity(style->getColor(p.checkedColorRole)));
                            render->drawRectangle(g);
                        }

                        // Draw the hovered state.
                        if (_isHovered())
                        {
                            render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Hover)));
                            render->drawRectangle(g);
                        }
                    }
                }
            }

            void IButton::_pointerEnterEvent(Event::PointerEnter& event)
            {
                event.accept();
                if (isEnabled())
                {
                    _redraw();
                }
            }

            void IButton::_pointerLeaveEvent(Event::PointerLeave& event)
            {
                event.accept();
                if (isEnabled())
                {
                    _redraw();
                }
            }

            void IButton::_pointerMoveEvent(Event::PointerMove& event)
            {
                event.accept();
                const auto id = event.getPointerInfo().id;
                const auto& pos = event.getPointerInfo().projectedPos;
                DJV_PRIVATE_PTR();
                if (id == p.pressedId)
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float distance = glm::length(pos - p.pressedPos);
                        const bool accepted = p.canRejectPressed ? distance < style->getMetric(Style::MetricsRole::Drag) : true;
                        event.setAccepted(accepted);
                        if (!accepted)
                        {
                            p.pressedId = Event::InvalidID;
                            _redraw();
                        }
                    }
                }
            }

            void IButton::_buttonPressEvent(Event::ButtonPress& event)
            {
                DJV_PRIVATE_PTR();
                if (!isEnabled() || p.pressedId)
                    return;
                event.accept();
                const auto & pointerInfo = event.getPointerInfo();
                p.pressedId = pointerInfo.id;
                p.pressedPos = pointerInfo.projectedPos;
                _redraw();
            }

            void IButton::_buttonReleaseEvent(Event::ButtonRelease& event)
            {
                DJV_PRIVATE_PTR();
                const auto & pointerInfo = event.getPointerInfo();
                if (pointerInfo.id == p.pressedId)
                {
                    event.accept();
                    p.pressedId = Event::InvalidID;
                    const BBox2f& g = getGeometry();
                    const auto & hover = _getPointerHover();
                    const auto i = hover.find(pointerInfo.id);
                    if (i != hover.end() && g.contains(i->second))
                    {
                        _doClickedCallback();
                        switch (p.buttonType)
                        {
                        case ButtonType::Toggle:
                            p.checked = !p.checked;
                            _doCheckedCallback(p.checked);
                            break;
                        case ButtonType::Radio:
                            if (!p.checked)
                            {
                                p.checked = true;
                                _doCheckedCallback(p.checked);
                            }
                            break;
                        default: break;
                        }
                    }
                    _redraw();
                }
            }

            bool IButton::_isToggled() const
            {
                DJV_PRIVATE_PTR();
                bool out = false;
                if (p.pressedId)
                {
                    const auto & hover = _getPointerHover();
                    const auto i = hover.find(p.pressedId);
                    if (i != hover.end())
                    {
                        const BBox2f& g = getGeometry();
                        switch (p.buttonType)
                        {
                        case ButtonType::Radio:
                            if (p.checked)
                            {
                                out = p.checked;
                            }
                            else
                            {
                                out = g.contains(i->second) ? !p.checked : p.checked;
                            }
                            break;
                        default:
                            out = g.contains(i->second) ? !p.checked : p.checked;
                            break;
                        }
                    }
                }
                else
                {
                    out = p.checked;
                }
                return out;
            }

            bool IButton::_isHovered() const
            {
                return isEnabled() && (_getPointerHover().size() || _isPressed());
            }

            bool IButton::_isPressed() const
            {
                return isEnabled() && _p->pressedId != 0 ? true : false;
            }

            const glm::vec2& IButton::_getPressedPos() const
            {
                return _p->pressedPos;
            }

            void IButton::_setCanRejectPressed(bool value)
            {
                _p->canRejectPressed = value;
            }

            Style::ColorRole IButton::_getForegroundColorRole() const
            {
                return !isEnabled(true) ? Style::ColorRole::Disabled :
                    (_isToggled() ? _p->checkedForegroundColorRole : _p->foregroundColorRole);
            }

            void IButton::_doClickedCallback()
            {
                DJV_PRIVATE_PTR();
                if (p.clickedCallback)
                {
                    p.clickedCallback();
                }
            }

            void IButton::_doCheckedCallback(bool value)
            {
                DJV_PRIVATE_PTR();
                if (p.checkedCallback)
                {
                    p.checkedCallback(value);
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace djv

