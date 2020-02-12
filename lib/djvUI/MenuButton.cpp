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
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvUI/MenuButton.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>

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
                bool textFocusEnabled = false;
                std::string font;
                std::string fontFace;
                MetricsRole fontSizeRole = MetricsRole::FontMedium;
                std::shared_ptr<Icon> icon;
                std::shared_ptr<Label> label;
                std::shared_ptr<Icon> popupIcon;
                std::function<void(bool)> openCallback;
            };

            void Menu::_init(MenuStyle menuStyle, const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                p.menuStyle = menuStyle;

                setClassName("djv::UI::Button::Menu");
                setPointerEnabled(true);
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
                if (!value.empty())
                {
                    if (!p.icon)
                    {
                        if (auto context = getContext().lock())
                        {
                            p.icon = Icon::create(context);
                            p.icon->setVAlign(VAlign::Center);
                            addChild(p.icon);
                        }
                    }
                    p.icon->setIcon(value);
                }
                else
                {
                    removeChild(p.icon);
                    p.icon.reset();
                    _resize();
                }
            }

            void Menu::setPopupIcon(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (!p.popupIcon)
                    {
                        if (auto context = getContext().lock())
                        {
                            p.popupIcon = Icon::create(context);
                            p.popupIcon->setVAlign(VAlign::Center);
                            addChild(p.popupIcon);
                        }
                    }
                    p.popupIcon->setIcon(value);
                }
                else
                {
                    removeChild(p.popupIcon);
                    p.popupIcon.reset();
                    _resize();
                }
            }

            const std::string & Menu::getText() const
            {
                return _p->label->getText();
            }

            void Menu::setText(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (!p.label)
                    {
                        if (auto context = getContext().lock())
                        {
                            p.label = Label::create(context);
                            p.label->setTextHAlign(TextHAlign::Left);
                            p.label->setFont(p.font);
                            p.label->setFontFace(p.fontFace);
                            p.label->setFontSizeRole(p.fontSizeRole);
                            p.label->setMargin(Layout::Margin(MetricsRole::MarginSmall));
                            addChild(p.label);
                        }
                    }
                    p.label->setText(value);
                }
                else
                {
                    removeChild(p.label);
                    p.label.reset();
                    _resize();
                }
            }

            const std::string& Menu::getFont() const
            {
                return _p->font;
            }

            const std::string& Menu::getFontFace() const
            {
                return _p->fontFace;
            }

            MetricsRole Menu::getFontSizeRole() const
            {
                return _p->fontSizeRole;
            }

            void Menu::setFont(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.font = value;
                if (p.label)
                {
                    p.label->setFont(value);
                }
            }

            void Menu::setFontFace(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.fontFace = value;
                if (p.label)
                {
                    p.label->setFontFace(value);
                }
            }

            void Menu::setFontSizeRole(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                p.fontSizeRole = value;
                if (p.label)
                {
                    p.label->setFontSizeRole(value);
                }
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
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float b = style->getMetric(MetricsRole::Border);
                glm::vec2 size = glm::vec2(0.F, 0.F);
                if (p.icon)
                {
                    const auto& tmp = p.icon->getMinimumSize();
                    size.x += tmp.x;
                    size.y = std::max(size.y, tmp.y);
                }
                if (p.label)
                {
                    const auto& tmp = p.label->getMinimumSize();
                    size.x += tmp.x;
                    size.y = std::max(size.y, tmp.y);
                }
                if (p.popupIcon)
                {
                    const auto& tmp = p.popupIcon->getMinimumSize();
                    size.x += tmp.x;
                    size.y = std::max(size.y, tmp.y);
                }                
                switch (p.menuStyle)
                {
                case MenuStyle::Flat:     size.x += m * 2.F; break;
                case MenuStyle::Tool:     size += m * 2.F;   break;
                case MenuStyle::ComboBox: size += b * 4.F;   break;
                default: break;
                }
                _setMinimumSize(size + getMargin().getSize(style));
            }

            void Menu::_layoutEvent(Event::Layout &)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float b = style->getMetric(MetricsRole::Border);
                BBox2f g2;
                switch (p.menuStyle)
                {
                case MenuStyle::Flat:     g2 = g.margin(-m, 0, -m, 0); break;
                case MenuStyle::Tool:     g2 = g.margin(-m);           break;
                case MenuStyle::ComboBox: g2 = g.margin(-b * 2.F);     break;
                default: break;
                }
                float x = g2.min.x;
                float y = g2.min.y + g2.h() / 2.F;
                float w = g2.w();
                if (p.icon)
                {
                    const auto& tmp = p.icon->getMinimumSize();
                    p.icon->setGeometry(BBox2f(x, floorf(y - tmp.y / 2.F), tmp.x, tmp.y));
                    x += tmp.x;
                    w -= tmp.x;
                }
                if (p.popupIcon)
                {
                    const auto& tmp = p.popupIcon->getMinimumSize();
                    p.popupIcon->setGeometry(BBox2f(g2.max.x - tmp.x, floorf(y - tmp.y / 2.F), tmp.x, tmp.y));
                    w -= tmp.x;
                }
                if (p.label)
                {
                    const auto& tmp = p.label->getMinimumSize();
                    p.label->setGeometry(BBox2f(x, floorf(y - tmp.y / 2.F), w, tmp.y));
                }
            }

            void Menu::_paintEvent(Event::Paint &)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float b = style->getMetric(MetricsRole::Border);
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                auto render = _getRender();

                BBox2f g2;
                switch (p.menuStyle)
                {
                case MenuStyle::ComboBox:
                    if (hasTextFocus())
                    {
                        render->setFillColor(style->getColor(ColorRole::TextFocus));
                        drawBorder(render, g, b * 2.F);
                    }
                    g2 = g.margin(-b * 2.F);
                    break;
                default:
                    g2 = g;
                    break;
                }

                render->setFillColor(style->getColor(getBackgroundRole()));
                render->drawRect(g2);
                switch (p.menuStyle)
                {
                case MenuStyle::ComboBox:
                    render->setFillColor(style->getColor(ColorRole::BorderButton));
                    drawBorder(render, g2, b);
                    break;
                default: break;
                }
                if (p.open)
                {
                    render->setFillColor(style->getColor(ColorRole::Checked));
                    render->drawRect(g2);
                }

                if (_isHovered())
                {
                    render->setFillColor(style->getColor(ColorRole::Hovered));
                    render->drawRect(g2);
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
                if (!event.isAccepted() && hasTextFocus())
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
