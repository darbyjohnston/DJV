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

#include <djvUI/ListButton.h>

#include <djvUI/Action.h>
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
            struct List::Private
            {
                std::shared_ptr<Icon> icon;
                std::shared_ptr<Label> label;
                std::shared_ptr<HorizontalLayout> layout;
            };

            void List::_init(Context * context)
            {
                IButton::_init(context);

                setClassName("djv::UI::Button::List");

                DJV_PRIVATE_PTR();
                p.icon = Icon::create(context);
                p.icon->setVAlign(VAlign::Center);

                p.label = Label::create(context);
                p.label->setTextHAlign(TextHAlign::Left);

                p.layout = HorizontalLayout::create(context);
                p.layout->setMargin(MetricsRole::MarginSmall);
                p.layout->addChild(p.icon);
                p.layout->addChild(p.label);
                p.layout->setStretch(p.label, RowStretch::Expand);
                addChild(p.layout);

                _widgetUpdate();
            }

            List::List() :
                _p(new Private)
            {}

            List::~List()
            {}

            std::shared_ptr<List> List::create(Context * context)
            {
                auto out = std::shared_ptr<List>(new List);
                out->_init(context);
                return out;
            }

            std::shared_ptr<List> List::create(const std::string & text, Context * context)
            {
                auto out = std::shared_ptr<List>(new List);
                out->_init(context);
                out->setText(text);
                return out;
            }

            std::shared_ptr<List> List::create(const std::string & text, const std::string & icon, Context * context)
            {
                auto out = std::shared_ptr<List>(new List);
                out->_init(context);
                out->setIcon(icon);
                out->setText(text);
                return out;
            }

            const std::string & List::getIcon() const
            {
                return _p->icon->getIcon();
            }

            void List::setIcon(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                p.icon->setIcon(value);
                _widgetUpdate();
            }

            const std::string & List::getText() const
            {
                return _p->label->getText();
            }

            void List::setText(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                p.label->setText(value);
                _widgetUpdate();
            }

            TextHAlign List::getTextHAlign() const
            {
                return _p->label->getTextHAlign();
            }

            TextVAlign List::getTextVAlign() const
            {
                return _p->label->getTextVAlign();
            }

            void List::setTextHAlign(TextHAlign value)
            {
                _p->label->setTextHAlign(value);
            }

            void List::setTextVAlign(TextVAlign value)
            {
                _p->label->setTextVAlign(value);
            }

            ColorRole List::getTextColorRole() const
            {
                return _p->label->getTextColorRole();
            }

            void List::setTextColorRole(ColorRole value)
            {
                _p->label->setTextColorRole(value);
            }

            const std::string & List::getFont() const
            {
                return _p->label->getFont();
            }

            const std::string & List::getFontFace() const
            {
                return _p->label->getFontFace();
            }

            MetricsRole List::getFontSizeRole() const
            {
                return _p->label->getFontSizeRole();
            }

            void List::setFont(const std::string & value)
            {
                _p->label->setFont(value);
            }

            void List::setFontFace(const std::string & value)
            {
                _p->label->setFontFace(value);
            }

            void List::setFontSizeRole(MetricsRole value)
            {
                _p->label->setFontSizeRole(value);
            }

            const Layout::Margin & List::getInsideMargin() const
            {
                return _p->layout->getMargin();
            }

            void List::setInsideMargin(const Layout::Margin & value)
            {
                _p->layout->setMargin(value);
            }

            void List::_preLayoutEvent(Event::PreLayout & event)
            {
                auto style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                glm::vec2 size = _p->layout->getMinimumSize();
                const ButtonType buttonType = getButtonType();
                if (ButtonType::Toggle    == buttonType ||
                    ButtonType::Radio     == buttonType ||
                    ButtonType::Exclusive == buttonType)
                {
                    size.x += m;
                }
                _setMinimumSize(size);
            }

            void List::_layoutEvent(Event::Layout &)
            {
                auto style = _getStyle();
                BBox2f g = getGeometry();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const ButtonType buttonType = getButtonType();
                if (ButtonType::Toggle == buttonType ||
                    ButtonType::Radio == buttonType ||
                    ButtonType::Exclusive == buttonType)
                {
                    g.min.x += m;
                }
                _p->layout->setGeometry(g);
            }

            void List::_paintEvent(Event::Paint& event)
            {
                IButton::_paintEvent(event);
                DJV_PRIVATE_PTR();
                auto render = _getRender();
                auto style = _getStyle();
                const BBox2f& g = getGeometry();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const ColorRole checkedColorRole = getCheckedColorRole();
                if (_isToggled() && checkedColorRole != ColorRole::None)
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(checkedColorRole)));
                    render->drawRect(BBox2f(g.min.x, g.min.y, m, g.h()));
                }
                if (_isPressed())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(getPressedColorRole())));
                    render->drawRect(g);
                }
                else if (_isHovered())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(getHoveredColorRole())));
                    render->drawRect(g);
                }
            }

            void List::_widgetUpdate()
            {
                _p->icon->setVisible(!_p->icon->getIcon().empty());
                _p->label->setVisible(!_p->label->getText().empty());
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
