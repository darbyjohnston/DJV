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

#include <djvAV/Render2DSystem.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct IButton::Private
        {
            ButtonType buttonType = ButtonType::Push;
            bool checked = false;
            ColorRole checkedColorRole = ColorRole::Checked;

            std::function<void(void)> clickedCallback;
            std::function<void(bool)> checkedCallback;

            struct PointerState
            {
                bool hover;
                bool inside;
            };
            std::map<uint32_t, PointerState> pointerState;
            uint32_t pressedId = 0;
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
            _p->checked = value;
        }

        ColorRole IButton::getCheckedColorRole() const
        {
            return _p->checkedColorRole;
        }

        void IButton::setCheckedColorRole(ColorRole value)
        {
            _p->checkedColorRole = value;
        }

        void IButton::setClickedCallback(const std::function<void(void)>& callback)
        {
            _p->clickedCallback = callback;
        }

        void IButton::setCheckedCallback(const std::function<void(bool)>& callback)
        {
            _p->checkedCallback = callback;
        }

        void IButton::paintEvent(PaintEvent& event)
        {
            Widget::paintEvent(event);
            if (auto render = _getRenderSystem().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& g = getGeometry();

                    // Draw the toggled state.
                    if (_isToggled())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(_p->checkedColorRole)));
                        render->drawRectangle(g);
                    }

                    // Draw the hovered state.
                    if (_isHovered())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Hover)));
                        render->drawRectangle(g);
                    }
                }
            }
        }

        void IButton::pointerEnterEvent(PointerEnterEvent& event)
        {
            event.accept();

            const auto id = event.getPointerInfo().id;
            _p->pointerState[id].hover = true;
            _p->pointerState[id].inside = true;
        }

        void IButton::pointerLeaveEvent(PointerLeaveEvent& event)
        {
            event.accept();

            const auto id = event.getPointerInfo().id;
            const auto i = _p->pointerState.find(id);
            if (i != _p->pointerState.end())
            {
                _p->pointerState.erase(i);
            }
        }

        void IButton::pointerMoveEvent(PointerMoveEvent& event)
        {
            event.accept();

            const auto id = event.getPointerInfo().id;
            const auto& pos = event.getPointerInfo().projectedPos;
            _p->pointerState[id].inside = getGeometry().contains(pos);

            if (id == _p->pressedId)
            {
                if (auto style = _getStyle().lock())
                {
                    const float distance = glm::length(pos - _p->pressedPos);
                    const bool accepted = _p->canRejectPressed ? distance < style->getMetric(MetricsRole::Drag) : true;
                    event.setAccepted(accepted);
                    if (!accepted)
                    {
                        _p->pressedId = 0;
                    }
                }
            }
        }

        void IButton::buttonPressEvent(ButtonPressEvent& event)
        {
            if (_p->pressedId)
                return;

            event.accept();

            _p->pressedId = event.getPointerInfo().id;
            _p->pressedPos = event.getPointerInfo().projectedPos;
        }

        void IButton::buttonReleaseEvent(ButtonReleaseEvent& event)
        {
            const auto id = event.getPointerInfo().id;
            if (id != _p->pressedId)
                return;

            event.accept();

            _p->pressedId = 0;

            if (_p->pointerState[id].inside)
            {
                _doClickedCallback();

                switch (_p->buttonType)
                {
                case ButtonType::Toggle:
                    _p->checked = !_p->checked;
                    _doCheckedCallback(_p->checked);
                    break;
                case ButtonType::Radio:
                    if (!_p->checked)
                    {
                        _p->checked = true;
                        _doCheckedCallback(_p->checked);
                    }
                    break;
                default: break;
                }
            }
        }

        bool IButton::_isToggled() const
        {
            bool out = false;
            if (_p->pressedId)
            {
                const auto i = _p->pointerState.find(_p->pressedId);
                if (i != _p->pointerState.end())
                {
                    switch (_p->buttonType)
                    {
                    case ButtonType::Radio:
                        if (_p->checked)
                        {
                            out = _p->checked;
                        }
                        else
                        {
                            out = i->second.inside ? !_p->checked : _p->checked;
                        }
                        break;
                    default:
                        out = i->second.inside ? !_p->checked : _p->checked;
                        break;
                    }
                }
            }
            else
            {
                out = _p->checked;
            }
            return out;
        }
        
        bool IButton::_isHovered() const
        {
            bool out = false;
            for (const auto& pointerState : _p->pointerState)
            {
                out |= pointerState.second.hover;
            }
            return out || _isPressed();
        }

        bool IButton::_isPressed() const
        {
            return _p->pressedId != 0 ? true : false;
        }

        const glm::vec2& IButton::_getPressedPos() const
        {
            return _p->pressedPos;
        }

        void IButton::_setCanRejectPressed(bool value)
        {
            _p->canRejectPressed = value;
        }

        void IButton::_doClickedCallback()
        {
            if (_p->clickedCallback)
            {
                _p->clickedCallback();
            }
        }

        void IButton::_doCheckedCallback(bool value)
        {
            if (_p->checkedCallback)
            {
                _p->checkedCallback(value);
            }
        }

    } // namespace UI
} // namespace Gp
