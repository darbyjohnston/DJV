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
                ColorRole foregroundColorRole = ColorRole::Foreground;
                ColorRole hoveredColorRole = ColorRole::Hovered;
                ColorRole pressedColorRole = ColorRole::Pressed;
                ColorRole checkedColorRole = ColorRole::Checked;
                ColorRole disabledColorRole = ColorRole::Disabled;

                std::function<void(void)> clickedCallback;
                std::function<void(bool)> checkedCallback;

                Event::PointerID pressedID = Event::InvalidID;
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
                DJV_PRIVATE_PTR();
                if (value == p.buttonType)
                    return;
                p.buttonType = value;
                _resize();
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

            ColorRole IButton::getForegroundColorRole() const
            {
                return _p->foregroundColorRole;
            }

            ColorRole IButton::getHoveredColorRole() const
            {
                return _p->hoveredColorRole;
            }

            ColorRole IButton::getPressedColorRole() const
            {
                return _p->pressedColorRole;
            }

            ColorRole IButton::getCheckedColorRole() const
            {
                return _p->checkedColorRole;
            }

            ColorRole IButton::getDisabledColorRole() const
            {
                return _p->disabledColorRole;
            }

            void IButton::setForegroundColorRole(ColorRole value)
            {
                if (value == _p->foregroundColorRole)
                    return;
                _p->foregroundColorRole = value;
                _redraw();
            }

            void IButton::setHoveredColorRole(ColorRole value)
            {
                if (_p->hoveredColorRole == value)
                    return;
                _p->hoveredColorRole = value;
                _redraw();
            }

            void IButton::setPressedColorRole(ColorRole value)
            {
                if (_p->pressedColorRole == value)
                    return;
                _p->pressedColorRole = value;
                _redraw();
            }

            void IButton::setCheckedColorRole(ColorRole value)
            {
                if (_p->checkedColorRole == value)
                    return;
                _p->checkedColorRole = value;
                _redraw();
            }

            void IButton::setDisabledColorRole(ColorRole value)
            {
                if (_p->disabledColorRole == value)
                    return;
                _p->disabledColorRole = value;
                _redraw();
            }

            void IButton::setClickedCallback(const std::function<void(void)> & callback)
            {
                _p->clickedCallback = callback;
            }

            void IButton::setCheckedCallback(const std::function<void(bool)> & callback)
            {
                _p->checkedCallback = callback;
            }

            void IButton::_paintEvent(Event::Paint & event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const BBox2f & g = getGeometry();
                auto render = _getRender();
                auto style = _getStyle();
                if (_isToggled() && p.checkedColorRole != ColorRole::None)
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(p.checkedColorRole)));
                    render->drawRect(g);
                }
                if (_isPressed())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(p.pressedColorRole)));
                    render->drawRect(g);
                }
                else if (_isHovered())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(p.hoveredColorRole)));
                    render->drawRect(g);
                }
            }

            void IButton::_pointerEnterEvent(Event::PointerEnter & event)
            {
                event.accept();
                if (isEnabled(true))
                {
                    _redraw();
                }
            }

            void IButton::_pointerLeaveEvent(Event::PointerLeave & event)
            {
                event.accept();
                if (isEnabled(true))
                {
                    _redraw();
                }
            }

            void IButton::_pointerMoveEvent(Event::PointerMove & event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                const auto id = event.getPointerInfo().id;
                const auto & pos = event.getPointerInfo().projectedPos;
                if (id == p.pressedID)
                {
                    const float distance = glm::length(pos - p.pressedPos);
                    auto style = _getStyle();
                    const bool accepted = p.canRejectPressed ? distance < style->getMetric(MetricsRole::Drag) : true;
                    event.setAccepted(accepted);
                    if (!accepted)
                    {
                        p.pressedID = Event::InvalidID;
                        _redraw();
                    }
                }
            }

            void IButton::_buttonPressEvent(Event::ButtonPress & event)
            {
                DJV_PRIVATE_PTR();
                if (!isEnabled(true) || p.pressedID)
                    return;
                event.accept();
                const auto & pointerInfo = event.getPointerInfo();
                p.pressedID  = pointerInfo.id;
                p.pressedPos = pointerInfo.projectedPos;
                _redraw();
            }

            void IButton::_buttonReleaseEvent(Event::ButtonRelease & event)
            {
                DJV_PRIVATE_PTR();
                const auto & pointerInfo = event.getPointerInfo();
                if (pointerInfo.id == p.pressedID)
                {
                    event.accept();
                    p.pressedID = Event::InvalidID;
                    const BBox2f & g = getGeometry();
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
                        case ButtonType::Exclusive:
                            p.checked = !p.checked;
                            _doCheckedCallback(p.checked);
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
                if (p.pressedID)
                {
                    const auto & hover = _getPointerHover();
                    const auto i = hover.find(p.pressedID);
                    if (i != hover.end())
                    {
                        const BBox2f & g = getGeometry();
                        switch (p.buttonType)
                        {
                        case ButtonType::Toggle:
                            out = g.contains(i->second) ? !p.checked : p.checked;
                            break;
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
                        case ButtonType::Exclusive:
                            out = g.contains(i->second) ? !p.checked : p.checked;
                            break;
                        default: break;
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
                return isEnabled(true) && _getPointerHover().size();
            }

            bool IButton::_isPressed() const
            {
                return isEnabled(true) && _p->pressedID != 0;
            }

            const glm::vec2 & IButton::_getPressedPos() const
            {
                return _p->pressedPos;
            }

            void IButton::_setCanRejectPressed(bool value)
            {
                _p->canRejectPressed = value;
            }

            ColorRole IButton::_getForegroundColorRole() const
            {
                DJV_PRIVATE_PTR();
                return !isEnabled(true) ? p.disabledColorRole : p.foregroundColorRole;
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

