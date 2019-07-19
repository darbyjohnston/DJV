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

#include <djvUI/Border.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>

#include <djvAV/Render2D.h>

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
                bool checked = false;
                MenuStyle menuStyle = MenuStyle::Flat;
                std::shared_ptr<Icon> icon;
                std::shared_ptr<Label> label;
                std::shared_ptr<Icon> popupIcon;
                std::shared_ptr<Border> border;
                std::shared_ptr<HorizontalLayout> layout;
                std::function<void(bool)> checkedCallback;
            };

            void Menu::_init(MenuStyle menuStyle, Context * context)
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
                p.label->setMargin(MetricsRole::MarginSmall);
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
                switch (menuStyle)
                {
                case MenuStyle::Flat:
                    p.layout->setMargin(Layout::Margin(MetricsRole::MarginSmall, MetricsRole::MarginSmall, MetricsRole::None, MetricsRole::None));
                    break;
                case MenuStyle::Tool:
                    p.layout->setMargin(MetricsRole::MarginSmall);
                    break;
                case MenuStyle::ComboBox:
                    p.layout->setMargin(MetricsRole::None);
                    break;
                default: break;
                }
                p.layout->addChild(p.icon);
                p.layout->addChild(p.label);
                p.layout->setStretch(p.label, RowStretch::Expand);
                p.layout->addChild(p.popupIcon);

                p.border = Border::create(context);
                switch (menuStyle)
                {
                case MenuStyle::Flat:
                case MenuStyle::Tool:
                    p.border->setBorderSize(MetricsRole::None);
                    break;
                default: break;
                }
                p.border->addChild(p.layout);
                addChild(p.border);
            }

            Menu::Menu() :
                _p(new Private)
            {}

            Menu::~Menu()
            {}

            std::shared_ptr<Menu> Menu::create(MenuStyle menuStyle, Context * context)
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

            void Menu::setFontSizeRole(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                p.label->setFontSizeRole(value);
            }

            void Menu::setBorderColorRole(ColorRole value)
            {
                _p->border->setBorderColorRole(value);
            }

            bool Menu::isChecked() const
            {
                return _p->checked;
            }

            void Menu::setChecked(bool value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.checked)
                    return;
                p.checked = value;
                _redraw();
            }

            void Menu::setCheckedCallback(const std::function<void(bool)> & callback)
            {
                _p->checkedCallback = callback;
            }

            MenuStyle Menu::getMenuStyle() const
            {
                return _p->menuStyle;
            }

            void Menu::_preLayoutEvent(Event::PreLayout &)
            {
                DJV_PRIVATE_PTR();
                glm::vec2 size = p.border->getMinimumSize();
                const auto& style = _getStyle();
                _setMinimumSize(size + getMargin().getSize(style));
            }

            void Menu::_layoutEvent(Event::Layout &)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                _p->border->setGeometry(g);
            }

            void Menu::_clipEvent(Event::Clip& event)
            {
                if (isClipped())
                {
                    setChecked(false);
                }
            }

            void Menu::_paintEvent(Event::Paint & event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                auto render = _getRender();
                switch (p.menuStyle)
                {
                case MenuStyle::Flat:
                case MenuStyle::Tool:
                    if (p.checked)
                    {
                        render->setFillColor(style->getColor(ColorRole::Pressed));
                        render->drawRect(g);
                    }
                    else if (_isHovered())
                    {
                        render->setFillColor(style->getColor(ColorRole::Hovered));
                        render->drawRect(g);
                    }
                    break;
                case MenuStyle::ComboBox:
                    if (p.checked)
                    {
                        render->setFillColor(style->getColor(ColorRole::Pressed));
                        render->drawRect(g);
                    }
                    else if (_isHovered())
                    {
                        render->setFillColor(style->getColor(ColorRole::Hovered));
                        render->drawRect(g);
                    }
                    break;
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
                setChecked(!p.checked);
                if (p.checkedCallback)
                {
                    p.checkedCallback(p.checked);
                }
            }

            void Menu::_buttonReleaseEvent(Event::ButtonRelease& event)
            {
                event.accept();
            }

            void Menu::_keyPressEvent(Event::KeyPress& event)
            {
                DJV_PRIVATE_PTR();
                if (p.checked)
                {
                    event.accept();
                    if (GLFW_KEY_ESCAPE == event.getKey())
                    {
                        setChecked(false);
                        if (p.checkedCallback)
                        {
                            p.checkedCallback(p.checked);
                        }
                    }
                }
            }

            bool Menu::_isHovered() const
            {
                return isEnabled(true) && _getPointerHover().size();
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
