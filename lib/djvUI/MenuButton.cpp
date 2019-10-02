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

#include <djvUI/MenuButton.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>

#include <djvAV/Render2D.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            struct Menu::Private
            {
                bool open = false;
                MenuStyle menuStyle = MenuStyle::Flat;
                bool textFocusEnabled = true;
                std::shared_ptr<Icon> icon;
                std::shared_ptr<Label> label;
                std::shared_ptr<Icon> popupIcon;
                std::shared_ptr<HorizontalLayout> layout;
                std::function<void(bool)> openCallback;
            };

            void Menu::_init(MenuStyle menuStyle, const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                p.menuStyle = menuStyle;

                setClassName("djv::UI::Button::Menu");
                setPointerEnabled(true);

                p.icon = Icon::create(context);
                p.icon->setVAlign(VAlign::Center);
                p.icon->hide();

                p.label = Label::create(context);
                p.label->setHAlign(HAlign::Left);
                p.label->setMargin(Layout::Margin(MetricsRole::MarginSmall));
                switch (menuStyle)
                {
                case MenuStyle::Flat:
                case MenuStyle::Tool: p.label->hide(); break;
                default: break;
                }

                p.popupIcon = Icon::create(context);
                p.popupIcon->setVAlign(VAlign::Center);
                p.popupIcon->hide();

                p.layout = HorizontalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                switch (menuStyle)
                {
                case MenuStyle::Flat:
                    p.layout->setMargin(Layout::Margin(MetricsRole::MarginSmall, MetricsRole::MarginSmall, MetricsRole::None, MetricsRole::None));
                    break;
                case MenuStyle::Tool:
                    p.layout->setMargin(Layout::Margin(MetricsRole::MarginSmall));
                    break;
                case MenuStyle::ComboBox:
                    p.layout->setMargin(Layout::Margin(MetricsRole::None));
                    break;
                default: break;
                }
                p.layout->addChild(p.icon);
                p.layout->addChild(p.label);
                p.layout->setStretch(p.label, RowStretch::Expand);
                p.layout->addChild(p.popupIcon);

                addChild(p.layout);
            }

            Menu::Menu() :
                _p(new Private)
            {}

            Menu::~Menu()
            {}

            std::shared_ptr<Menu> Menu::create(MenuStyle menuStyle, const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Menu>(new Menu);
                out->_init(menuStyle, context);
                return out;
            }

            const std::string& Menu::getIcon() const
            {
                return _p->icon->getIcon();
            }

            const std::string& Menu::getPopupIcon() const
            {
                return _p->popupIcon->getIcon();
            }

            void Menu::setIcon(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.icon->setIcon(value);
                p.icon->setVisible(!value.empty());
            }

            void Menu::setPopupIcon(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.popupIcon->setIcon(value);
                p.popupIcon->setVisible(!value.empty());
            }

            const std::string & Menu::getText() const
            {
                return _p->label->getText();
            }

            void Menu::setText(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                p.label->setText(value);
                switch (p.menuStyle)
                {
                case MenuStyle::Flat:
                case MenuStyle::Tool:       p.label->setVisible(!value.empty());    break;
                case MenuStyle::ComboBox:   p.label->show();                        break;
                default: break;
                }
            }

            const std::string& Menu::getFont() const
            {
                return _p->label->getFont();
            }

            const std::string& Menu::getFontFace() const
            {
                return _p->label->getFontFace();
            }

            MetricsRole Menu::getFontSizeRole() const
            {
                return _p->label->getFontSizeRole();
            }

            void Menu::setFont(const std::string& value)
            {
                _p->label->setFont(value);
            }

            void Menu::setFontFace(const std::string& value)
            {
                _p->label->setFontFace(value);
            }

            void Menu::setFontSizeRole(MetricsRole value)
            {
                _p->label->setFontSizeRole(value);
            }

            bool Menu::isOpen() const
            {
                return _p->open;
            }

            void Menu::setOpen(bool value)
            {
                if (value == _p->open)
                    return;
                _p->open = value;
                _redraw();
            }

            void Menu::setOpenCallback(const std::function<void(bool)>& callback)
            {
                _p->openCallback = callback;
            }

            void Menu::setTextFocusEnabled(bool value)
            {
                _p->textFocusEnabled = value;
            }

            MenuStyle Menu::getMenuStyle() const
            {
                return _p->menuStyle;
            }

            bool Menu::acceptFocus(TextFocusDirection)
            {
                bool out = false;
                if (_p->textFocusEnabled && isEnabled(true) && isVisible(true) && !isClipped())
                {
                    takeTextFocus();
                    out = true;
                }
                return out;
            }

            void Menu::_preLayoutEvent(Event::PreLayout &)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float b = style->getMetric(MetricsRole::Border);
                glm::vec2 size = p.layout->getMinimumSize();
                _setMinimumSize(size + b * 2.F + getMargin().getSize(style));
            }

            void Menu::_layoutEvent(Event::Layout &)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                const float b = style->getMetric(MetricsRole::Border);
                _p->layout->setGeometry(g.margin(-b));
            }

            void Menu::_paintEvent(Event::Paint & event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                const float b = style->getMetric(MetricsRole::Border);
                auto render = _getRender();

                if (p.open)
                {
                    render->setFillColor(style->getColor(ColorRole::Checked));
                    render->drawRect(g);
                }
                if (_isHovered())
                {
                    render->setFillColor(style->getColor(ColorRole::Hovered));
                    render->drawRect(g);
                }

                if (hasTextFocus())
                {
                    render->setFillColor(style->getColor(ColorRole::TextFocus));
                    drawBorder(render, g, b);
                }
                else
                {
                    switch (p.menuStyle)
                    {
                    case MenuStyle::ComboBox:
                        render->setFillColor(style->getColor(ColorRole::BorderButton));
                        drawBorder(render, g, b);
                        break;
                    default: break;
                    }
                }
            }

            void Menu::_pointerEnterEvent(Event::PointerEnter & event)
            {
                event.accept();
                if (isEnabled(true))
                {
                    _redraw();
                }
            }

            void Menu::_pointerLeaveEvent(Event::PointerLeave & event)
            {
                event.accept();
                if (isEnabled(true))
                {
                    _redraw();
                }
            }

            void Menu::_pointerMoveEvent(Event::PointerMove & event)
            {
                event.accept();
            }

            void Menu::_buttonPressEvent(Event::ButtonPress & event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                if (p.textFocusEnabled)
                {
                    takeTextFocus();
                }
                p.open = !p.open;
                _doOpenCallback();
            }

            void Menu::_buttonReleaseEvent(Event::ButtonRelease& event)
            {
                event.accept();
            }

            void Menu::_keyPressEvent(Event::KeyPress& event)
            {
                Widget::_keyPressEvent(event);
                DJV_PRIVATE_PTR();
                if (!event.isAccepted())
                {
                    switch (event.getKey())
                    {
                    case GLFW_KEY_ENTER:
                    case GLFW_KEY_SPACE:
                        event.accept();
                        p.open = !p.open;
                        _doOpenCallback();
                        break;
                    case GLFW_KEY_ESCAPE:
                        event.accept();
                        if (p.open)
                        {
                            p.open = false;
                            _doOpenCallback();
                        }
                        else
                        {
                            releaseTextFocus();
                        }
                        break;
                    default: break;
                    }
                }
            }

            void Menu::_textFocusEvent(Event::TextFocus&)
            {
                _redraw();
            }

            void Menu::_textFocusLostEvent(Event::TextFocusLost&)
            {
                _redraw();
            }

            bool Menu::_isHovered() const
            {
                return isEnabled(true) && _getPointerHover().size();
            }

            void Menu::_doOpenCallback()
            {
                DJV_PRIVATE_PTR();
                if (p.openCallback)
                {
                    p.openCallback(p.open);
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
