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

#include <djvUI/Button.h>

#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/PixelData.h>
#include <djvAV/Render2DSystem.h>

#include <glm/geometric.hpp>

//#pragma optimize("", off)

using namespace djv::Core;

namespace
{
    const float toggleWidth  = 2.2f;
    const float toggleHeight = 1.2f;

} // namespace

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

        void IButton::_paintEvent(PaintEvent& event)
        {
            Widget::_paintEvent(event);

            auto renderSystem = _getRenderSystem();
            const auto style = _getStyle();
            const BBox2f& g = getGeometry();

            // Draw the toggled state.
            if (_isToggled())
            {
                renderSystem->setFillColor(style->getColor(_p->checkedColorRole));
                renderSystem->drawRectangle(g);
            }

            // Draw the hovered state.
            if (_isHovered())
            {
                renderSystem->setFillColor(style->getColor(ColorRole::Hover));
                renderSystem->drawRectangle(g);
            }
        }

        void IButton::_pointerEnterEvent(PointerEnterEvent& event)
        {
            event.accept();

            const auto id = event.getPointerInfo().id;
            _p->pointerState[id].hover  = true;
            _p->pointerState[id].inside = true;
        }

        void IButton::_pointerLeaveEvent(PointerLeaveEvent& event)
        {
            event.accept();

            const auto id = event.getPointerInfo().id;
            const auto i = _p->pointerState.find(id);
            if (i != _p->pointerState.end())
            {
                _p->pointerState.erase(i);
            }
        }

        void IButton::_pointerMoveEvent(PointerMoveEvent& event)
        {
            event.accept();

            const auto id = event.getPointerInfo().id;
            const auto& pos = event.getPointerInfo().projectedPos;
            _p->pointerState[id].inside = getGeometry().contains(pos);

            if (id == _p->pressedId)
            {
                const float distance = glm::length(pos - _p->pressedPos);
                const bool accepted = _p->canRejectPressed ? distance < _getStyle()->getMetric(MetricsRole::Drag) : true;
                event.setAccepted(accepted);
                if (!accepted)
                {
                    _p->pressedId = 0;
                }
            }
        }

        void IButton::_buttonPressEvent(ButtonPressEvent& event)
        {
            if (_p->pressedId)
                return;

            event.accept();

            _p->pressedId  = event.getPointerInfo().id;
            _p->pressedPos = event.getPointerInfo().projectedPos;
        }

        void IButton::_buttonReleaseEvent(ButtonReleaseEvent& event)
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

        struct ButtonGroup::Private
        {
            std::vector<std::shared_ptr<IButton> > buttons;
            ButtonType buttonType = ButtonType::Push;
            std::function<void(int)> clickedCallback;
            std::function<void(int, bool)> checkedCallback;
            std::function<void(int)> radioCallback;
        };

        ButtonGroup::ButtonGroup() :
            _p(new Private)
        {}

        ButtonGroup::~ButtonGroup()
        {
            clearButtons();
        }

        std::shared_ptr<ButtonGroup> ButtonGroup::create(ButtonType buttonType)
        {
            auto out = std::shared_ptr<ButtonGroup>(new ButtonGroup);
            out->setButtonType(buttonType);
            return out;
        }

        const std::vector<std::shared_ptr<IButton> >& ButtonGroup::getButtons() const
        {
            return _p->buttons;
        }

        size_t ButtonGroup::getButtonCount() const
        {
            return _p->buttons.size();
        }

        int ButtonGroup::getButtonIndex(const std::shared_ptr<IButton>& value) const
        {
            const auto i = std::find(_p->buttons.begin(), _p->buttons.end(), value);
            if (i != _p->buttons.end())
            {
                return static_cast<int>(i - _p->buttons.begin());
            }
            return -1;
        }
        
        void ButtonGroup::addButton(const std::shared_ptr<IButton>& button)
        {
            button->setButtonType(_p->buttonType);

            if (ButtonType::Radio == _p->buttonType)
            {
                size_t i = 0;
                for (; i < _p->buttons.size(); ++i)
                {
                    if (_p->buttons[i]->isChecked())
                    {
                        break;
                    }
                }
                if (i == _p->buttons.size())
                {
                    button->setChecked(true);
                }
            }

            auto weak = std::weak_ptr<ButtonGroup>(std::dynamic_pointer_cast<ButtonGroup>(shared_from_this()));
            button->setClickedCallback(
                [weak, button]
            {
                if (auto group = weak.lock())
                {
                    const auto i = std::find(group->_p->buttons.begin(), group->_p->buttons.end(), button);
                    if (i != group->_p->buttons.end())
                    {
                        if (group->_p->clickedCallback)
                        {
                            const int index = static_cast<int>(i - group->_p->buttons.begin());
                            group->_p->clickedCallback(index);
                        }
                    }
                }
            });

            button->setCheckedCallback(
                [weak, button](bool value)
            {
                if (auto group = weak.lock())
                {
                    const auto i = std::find(group->_p->buttons.begin(), group->_p->buttons.end(), button);
                    if (i != group->_p->buttons.end())
                    {
                        const int index = static_cast<int>(i - group->_p->buttons.begin());
                        if (ButtonType::Radio == group->_p->buttonType)
                        {
                            for (size_t i = 0; i < group->_p->buttons.size(); ++i)
                            {
                                group->_p->buttons[i]->setChecked(i == index);
                            }

                            if (value && group->_p->radioCallback)
                            {
                                group->_p->radioCallback(index);
                            }
                        }

                        if (group->_p->checkedCallback)
                        {
                            group->_p->checkedCallback(index, true);
                        }
                    }
                }
            });

            _p->buttons.push_back(button);
        }

        void ButtonGroup::removeButton(const std::shared_ptr<IButton>& button)
        {
            const auto i = std::find(_p->buttons.begin(), _p->buttons.end(), button);
            if (i != _p->buttons.end())
            {
                (*i)->setClickedCallback(nullptr);
                (*i)->setCheckedCallback(nullptr);
                _p->buttons.erase(i);
            }
        }

        void ButtonGroup::clearButtons()
        {
            auto buttons = _p->buttons;
            for (const auto& button : buttons)
            {
                button->setClickedCallback(nullptr);
                button->setCheckedCallback(nullptr);
            }
            _p->buttons.clear();
        }

        ButtonType ButtonGroup::getButtonType() const
        {
            return _p->buttonType;
        }

        void ButtonGroup::setButtonType(ButtonType value)
        {
            _p->buttonType = value;

            for (size_t i = 0; i < _p->buttons.size(); ++i)
            {
                _p->buttons[i]->setButtonType(value);
            }

            if (ButtonType::Radio == _p->buttonType)
            {
                setChecked(0);
            }
        }

        int ButtonGroup::getChecked() const
        {
            for (size_t i = 0; i < _p->buttons.size(); ++i)
            {
                if (_p->buttons[i]->isChecked())
                {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        void ButtonGroup::setChecked(int index, bool value)
        {
            if (index >= 0 && index < static_cast<int>(_p->buttons.size()))
            {
                if (_p->buttons[index]->isChecked() != value)
                {
                    if (ButtonType::Radio == _p->buttonType)
                    {
                        for (size_t i = 0; i < _p->buttons.size(); ++i)
                        {
                            _p->buttons[i]->setChecked(i == index);
                        }

                        if (_p->radioCallback)
                        {
                            _p->radioCallback(index);
                        }
                    }
                    else
                    {
                        _p->buttons[index]->setChecked(value);
                    }

                    if (_p->checkedCallback)
                    {
                        _p->checkedCallback(index, value);
                    }
                }
            }
        }

        void ButtonGroup::setClickedCallback(const std::function<void(int)>& callback)
        {
            _p->clickedCallback = callback;
        }

        void ButtonGroup::setCheckedCallback(const std::function<void(int, bool)>& callback)
        {
            _p->checkedCallback = callback;
        }

        void ButtonGroup::setRadioCallback(const std::function<void(int)>& callback)
        {
            _p->radioCallback = callback;
        }

        struct Button::Private
        {
            std::shared_ptr<Icon> icon;
            std::shared_ptr<Label> label;
            std::shared_ptr<HorizontalLayout> layout;
        };

        void Button::_init(Context * context)
        {
            IButton::_init(context);

            setName("Gp::UI::Button");

            _p->icon = Icon::create(context);
            _p->icon->setVAlign(VAlign::Center);
            _p->icon->hide();

            _p->label = Label::create(context);
            _p->label->setMargin(Margin(MetricsRole::MarginSmall, MetricsRole::MarginSmall, MetricsRole::None, MetricsRole::None));
            _p->label->hide();

            _p->layout = HorizontalLayout::create(context);
            _p->layout->setMargin(MetricsRole::Margin);
            _p->layout->addWidget(_p->icon);
            _p->layout->addWidget(_p->label, RowLayoutStretch::Expand);
            _p->layout->setParent(shared_from_this());
        }

        Button::Button() :
            _p(new Private)
        {}

        Button::~Button()
        {}

        std::shared_ptr<Button> Button::create(Context * context)
        {
            auto out = std::shared_ptr<Button>(new Button);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Button> Button::create(const Path& icon, Context * context)
        {
            auto out = std::shared_ptr<Button>(new Button);
            out->_init(context);
            out->setIcon(icon);
            return out;
        }

        std::shared_ptr<Button> Button::create(const Path& icon, const std::string& text, Context * context)
        {
            auto out = std::shared_ptr<Button>(new Button);
            out->_init(context);
            out->setIcon(icon);
            out->setText(text);
            return out;
        }

        const Path& Button::getIcon() const
        {
            return _p->icon->getIcon();
        }

        void Button::setIcon(const Path& value)
        {
            _p->icon->setIcon(value);
            _p->icon->setVisible(!value.isEmpty());
        }

        const std::string& Button::getText() const
        {
            return _p->label->getText();
        }
        
        void Button::setText(const std::string& value)
        {
            _p->label->setText(value);
            _p->label->setVisible(!value.empty());
        }

        TextHAlign Button::getTextHAlign() const
        {
            return _p->label->getTextHAlign();
        }
        
        TextVAlign Button::getTextVAlign() const
        {
            return _p->label->getTextVAlign();
        }
        
        void Button::setTextHAlign(TextHAlign value)
        {
            _p->label->setTextHAlign(value);
        }
        
        void Button::setTextVAlign(TextVAlign value)
        {
            _p->label->setTextVAlign(value);
        }

        FontFace Button::getFontFace() const
        {
            return _p->label->getFontFace();
        }
        
        MetricsRole Button::getFontSizeRole() const
        {
            return _p->label->getFontSizeRole();
        }
        
        void Button::setFontFace(FontFace value)
        {
            _p->label->setFontFace(value);
        }
        
        void Button::setFontSizeRole(MetricsRole value)
        {
            _p->label->setFontSizeRole(value);
        }

        const Margin& Button::getInsideMargin() const
        {
            return _p->layout->getMargin();
        }

        void Button::setInsideMargin(const Margin& value)
        {
            _p->layout->setMargin(value);
        }

        float Button::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void Button::_updateEvent(UpdateEvent& event)
        {
            const auto style = _getStyle();
            const bool enabled = isEnabled(true);
            const ColorRole fg = !enabled ? ColorRole::Disabled : (_isToggled() ? ColorRole::CheckedForeground : ColorRole::Foreground);
            _p->icon->setIconColorRole(fg);
            _p->label->setTextColorRole(fg);
        }

        void Button::_preLayoutEvent(PreLayoutEvent& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void Button::_layoutEvent(LayoutEvent&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void Button::_paintEvent(PaintEvent& event)
        {
            Widget::_paintEvent(event);

            auto renderSystem = _getRenderSystem();
            const auto style = _getStyle();
            const BBox2f& g = getGeometry();

            // Draw the toggled state.
            if (_isToggled())
            {
                renderSystem->setFillColor(style->getColor(getCheckedColorRole()));
                renderSystem->drawRectangle(g);
            }

            // Draw the hovered state.
            if (_isHovered())
            {
                renderSystem->setFillColor(style->getColor(ColorRole::Hover));
                renderSystem->drawRectangle(g);
            }
        }

        struct ListButton::Private
        {
            std::shared_ptr<Label> label;
            std::shared_ptr<StackLayout> layout;
        };

        void ListButton::_init(const std::string& text, Context * context)
        {
            IButton::_init(context);
            
            setName("Gp::UI::ListButton");

            _p->label = Label::create(text, context);
            _p->label->setMargin(MetricsRole::Margin);
            _p->label->setVisible(!text.empty());
            
            _p->layout = StackLayout::create(context);
            _p->layout->addWidget(_p->label);
            _p->layout->setParent(shared_from_this());
        }

        ListButton::ListButton() :
            _p(new Private)
        {}

        ListButton::~ListButton()
        {}

        std::shared_ptr<ListButton> ListButton::create(Context * context)
        {
            auto out = std::shared_ptr<ListButton>(new ListButton);
            out->_init(std::string(), context);
            return out;
        }

        std::shared_ptr<ListButton> ListButton::create(const std::string& text, Context * context)
        {
            auto out = std::shared_ptr<ListButton>(new ListButton);
            out->_init(text, context);
            return out;
        }

        const std::string& ListButton::getText() const
        {
            return _p->label->getText();
        }

        void ListButton::setText(const std::string& value)
        {
            _p->label->setText(value);
            _p->label->setVisible(!value.empty());
        }

        TextHAlign ListButton::getTextHAlign() const
        {
            return _p->label->getTextHAlign();
        }
        
        TextVAlign ListButton::getTextVAlign() const
        {
            return _p->label->getTextVAlign();
        }
        
        void ListButton::setTextHAlign(TextHAlign value)
        {
            _p->label->setTextHAlign(value);
        }
        
        void ListButton::setTextVAlign(TextVAlign value)
        {
            _p->label->setTextVAlign(value);
        }

        FontFace ListButton::getFontFace() const
        {
            return _p->label->getFontFace();
        }

        MetricsRole ListButton::getFontSizeRole() const
        {
            return _p->label->getFontSizeRole();
        }

        void ListButton::setFontFace(FontFace value)
        {
            _p->label->setFontFace(value);
        }

        void ListButton::setFontSizeRole(MetricsRole value)
        {
            _p->label->setFontSizeRole(value);
        }

        float ListButton::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void ListButton::_updateEvent(UpdateEvent& event)
        {
            IButton::_updateEvent(event);

            _p->label->setTextColorRole(_isToggled() ? ColorRole::CheckedForeground : ColorRole::Foreground);
        }

        void ListButton::_preLayoutEvent(PreLayoutEvent& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ListButton::_layoutEvent(LayoutEvent&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        struct ToggleButton::Private
        {
            float lineHeight = 0.f;
        };

        void ToggleButton::_init(Context * context)
        {
            IButton::_init(context);
            
            setName("Gp::UI::ToggleButton");
            setButtonType(ButtonType::Toggle);
            setBackgroundRole(ColorRole::Button);
        }

        ToggleButton::ToggleButton() :
            _p(new Private)
        {}

        ToggleButton::~ToggleButton()
        {}

        std::shared_ptr<ToggleButton> ToggleButton::create(Context * context)
        {
            auto out = std::shared_ptr<ToggleButton>(new ToggleButton);
            out->_init(context);
            return out;
        }

        void ToggleButton::_preLayoutEvent(PreLayoutEvent& event)
        {
            auto fontSystem = _getFontSystem();
            const auto style = _getStyle();
            const float m = style->getMetric(MetricsRole::Margin);

            // Use the font size to determine the size of the button.
            const auto fontMetrics = fontSystem->getMetrics(style->getFont(FontFace::Regular, MetricsRole::FontMedium)).get();
            _p->lineHeight = static_cast<float>(fontMetrics.lineHeight);

            // Set the minimum size.
            glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
            minimumSize.x = _p->lineHeight * toggleWidth + m * 2.f;
            minimumSize.y = _p->lineHeight + m * 2.f;
            _setMinimumSize(minimumSize);
        }

        void ToggleButton::_paintEvent(PaintEvent& event)
        {
            auto renderSystem = _getRenderSystem();
            const auto style = _getStyle();
            const float b = style->getMetric(MetricsRole::Border);
            const float m = style->getMetric(MetricsRole::Margin);
            const BBox2f& g = getGeometry();
            const glm::vec2 c = g.getCenter();

            BBox2f g1;
            g1.min.x = c.x - (_p->lineHeight * toggleWidth) / 2.f;
            g1.min.y = c.y - (_p->lineHeight * toggleHeight) / 2.f;
            g1.max.x = g1.min.x + _p->lineHeight * toggleWidth;
            g1.max.y = g1.min.y + _p->lineHeight * toggleHeight;

            // Draw the background.
            BBox2f g2 = g1.margin(-b / 2.f);
            /*nvg.beginPath();
            nvg.roundedRect(g2, g2.h() / 2.f);
            nvg.fillColor(style->getColor(ColorRole::Trough));
            nvg.fill();
            nvg.strokeColor(style->getColor(ColorRole::Border));
            nvg.stroke();*/

            // Draw the button handle.
            glm::vec2 pos = glm::vec2(0.f, 0.f);
            if (isChecked())
            {
                pos.x = g1.max.x - g1.h() / 2.f;
                pos.y = g1.min.y + g1.h() / 2.f;
            }
            else
            {
                pos.x = g1.min.x + g1.h() / 2.f;
                pos.y = g1.min.y + g1.h() / 2.f;
            }
            /*nvg.beginPath();
            nvg.circle(pos, g1.h() / 2.f);
            nvg.fillColor(style->getColor(isChecked() ? getCheckedColorRole() : ColorRole::Button));
            nvg.fill();*/

            // Draw the hovered state.
            /*if (_isHovered())
            {
                nvg.beginPath();
                nvg.circle(pos, g1.h() / 2.f);
                nvg.fillColor(style->getColor(ColorRole::Hover));
                nvg.fill();
            }*/
        }

    } // namespace UI
} // namespace Gp
