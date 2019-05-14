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

#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>

#include <djvAV/Render2D.h>

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
                std::shared_ptr<HorizontalLayout> layout;
                std::function<void(bool)> checkedCallback;
            };

            void Menu::_init(Context * context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Button::Menu");
                setPointerEnabled(true);

                p.icon = Icon::create(context);
                p.icon->setVAlign(VAlign::Center);
                p.icon->hide();

                p.label = Label::create(context);
                p.label->setMargin(MetricsRole::MarginSmall);
                p.label->setHAlign(HAlign::Left);
                p.label->hide();

                p.popupIcon = Icon::create(context);
                p.popupIcon->setVAlign(VAlign::Center);
                p.popupIcon->hide();

                p.layout = HorizontalLayout::create(context);
                p.layout->addChild(p.icon);
                p.layout->addChild(p.label);
                p.layout->setStretch(p.label, RowStretch::Expand);
                p.layout->addChild(p.popupIcon);
                addChild(p.layout);

                _widgetUpdate();
            }

            Menu::Menu() :
                _p(new Private)
            {}

            Menu::~Menu()
            {}

            std::shared_ptr<Menu> Menu::create(Context * context)
            {
                auto out = std::shared_ptr<Menu>(new Menu);
                out->_init(context);
                return out;
            }

            std::shared_ptr<Menu> Menu::create(const std::string & text, Context * context)
            {
                auto out = std::shared_ptr<Menu>(new Menu);
                out->_init(context);
                out->setText(text);
                return out;
            }

            std::shared_ptr<Menu> Menu::create(const std::string & text, const std::string & icon, Context * context)
            {
                auto out = std::shared_ptr<Menu>(new Menu);
                out->_init(context);
                out->setIcon(icon);
                out->setText(text);
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
                p.label->setVisible(!value.empty());
            }

            void Menu::setFontSizeRole(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                p.label->setFontSizeRole(value);
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

            void Menu::setMenuStyle(MenuStyle value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.menuStyle)
                    return;
                p.menuStyle = value;
                _widgetUpdate();
                _resize();
            }

            void Menu::_preLayoutEvent(Event::PreLayout &)
            {
                DJV_PRIVATE_PTR();
                glm::vec2 size = p.layout->getMinimumSize();
                if (MenuStyle::Round == p.menuStyle)
                {
                    size.x += size.y;
                }
                auto style = _getStyle();
                _setMinimumSize(size + getMargin().getSize(style));
            }

            void Menu::_layoutEvent(Event::Layout &)
            {
                DJV_PRIVATE_PTR();
                auto style = _getStyle();
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                switch (p.menuStyle)
                {
                case MenuStyle::Flat:
                    _p->layout->setGeometry(g);
                    break;
                case MenuStyle::Round:
                {
                    const float h = g.h();
                    const float radius = h / 2.f;
                    _p->layout->setGeometry(BBox2f(
                        floorf(g.min.x + radius),
                        g.min.y,
                        ceilf(g.w() - h),
                        h));
                    break;
                }
                default: break;
                }
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
                DJV_PRIVATE_PTR();
                auto style = _getStyle();
                const float b = style->getMetric(MetricsRole::Border);
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                const BBox2f& g2 = g.margin(-b);
                auto render = _getRender();
                switch (p.menuStyle)
                {
                case MenuStyle::Flat:
                    render->setFillColor(_getColorWithOpacity(style->getColor(getBackgroundRole())));
                    render->drawRect(g);
                    if (p.checked)
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Pressed)));
                        render->drawRect(g);
                    }
                    else if (_isHovered())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Hovered)));
                        render->drawRect(g);
                    }
                    break;
                case MenuStyle::Round:
                    render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Border)));
                    render->drawPill(g);
                    render->setFillColor(_getColorWithOpacity(style->getColor(getBackgroundRole())));
                    render->drawPill(g2);
                    if (p.checked)
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Pressed)));
                        render->drawPill(g2);
                    }
                    else if (_isHovered())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Hovered)));
                        render->drawPill(g2);
                    }
                    break;
                default: break;
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
                if (isEnabled(true))
                {
                    event.accept();
                    setChecked(!p.checked);
                    if (p.checkedCallback)
                    {
                        p.checkedCallback(p.checked);
                    }
                }
            }

            bool Menu::_isHovered() const
            {
                return isEnabled(true) && _getPointerHover().size();
            }

            void Menu::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                switch (p.menuStyle)
                {
                case MenuStyle::Flat:
                    p.layout->setMargin(Layout::Margin(MetricsRole::MarginSmall, MetricsRole::MarginSmall, MetricsRole::None, MetricsRole::None));
                    break;
                case MenuStyle::Round:
                    p.layout->setMargin(MetricsRole::None);
                    break;
                default: break;
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
