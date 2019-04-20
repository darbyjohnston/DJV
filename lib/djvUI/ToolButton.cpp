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

#include <djvUI/ToolButton.h>

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
            struct Tool::Private
            {
                std::shared_ptr<Icon> icon;
                std::shared_ptr<Label> label;
                std::shared_ptr<HorizontalLayout> layout;
            };

            void Tool::_init(Context * context)
            {
                IButton::_init(context);

                setClassName("djv::UI::Button::Tool");

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

            Tool::Tool() :
                _p(new Private)
            {}

            Tool::~Tool()
            {}

            std::shared_ptr<Tool> Tool::create(Context * context)
            {
                auto out = std::shared_ptr<Tool>(new Tool);
                out->_init(context);
                return out;
            }

            std::shared_ptr<Tool> Tool::create(const std::string & text, Context * context)
            {
                auto out = std::shared_ptr<Tool>(new Tool);
                out->_init(context);
                out->setText(text);
                return out;
            }

            std::shared_ptr<Tool> Tool::create(const std::string & text, const std::string & icon, Context * context)
            {
                auto out = std::shared_ptr<Tool>(new Tool);
                out->_init(context);
                out->setIcon(icon);
                out->setText(text);
                return out;
            }

            const std::string & Tool::getIcon() const
            {
                return _p->icon->getIcon();
            }

            void Tool::setIcon(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                p.icon->setIcon(value);
                _widgetUpdate();
            }

            const std::string & Tool::getText() const
            {
                return _p->label->getText();
            }

            void Tool::setText(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                p.label->setText(value);
                _widgetUpdate();
            }

            TextHAlign Tool::getTextHAlign() const
            {
                return _p->label->getTextHAlign();
            }

            TextVAlign Tool::getTextVAlign() const
            {
                return _p->label->getTextVAlign();
            }

            void Tool::setTextHAlign(TextHAlign value)
            {
                _p->label->setTextHAlign(value);
            }

            void Tool::setTextVAlign(TextVAlign value)
            {
                _p->label->setTextVAlign(value);
            }

            ColorRole Tool::getTextColorRole() const
            {
                return _p->label->getTextColorRole();
            }

            void Tool::setTextColorRole(ColorRole value)
            {
                _p->label->setTextColorRole(value);
            }

            const std::string & Tool::getFont() const
            {
                return _p->label->getFont();
            }

            const std::string & Tool::getFontFace() const
            {
                return _p->label->getFontFace();
            }

            MetricsRole Tool::getFontSizeRole() const
            {
                return _p->label->getFontSizeRole();
            }

            void Tool::setFont(const std::string & value)
            {
                _p->label->setFont(value);
            }

            void Tool::setFontFace(const std::string & value)
            {
                _p->label->setFontFace(value);
            }

            void Tool::setFontSizeRole(MetricsRole value)
            {
                _p->label->setFontSizeRole(value);
            }

            const Layout::Margin & Tool::getInsideMargin() const
            {
                return _p->layout->getMargin();
            }

            void Tool::setInsideMargin(const Layout::Margin & value)
            {
                _p->layout->setMargin(value);
            }

            void Tool::setChecked(bool value)
            {
                IButton::setChecked(value);
                _p->icon->setIconColorRole(value ? getCheckedColorRole() : getForegroundColorRole());
                _p->label->setTextColorRole(value ? getCheckedColorRole() : getForegroundColorRole());
            }

            void Tool::_preLayoutEvent(Event::PreLayout & event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void Tool::_layoutEvent(Event::Layout &)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void Tool::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const BBox2f& g = getGeometry();
                auto render = _getRender();
                auto style = _getStyle();
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

            void Tool::_widgetUpdate()
            {
                _p->icon->setVisible(!_p->icon->getIcon().empty());
                _p->label->setVisible(!_p->label->getText().empty());
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
