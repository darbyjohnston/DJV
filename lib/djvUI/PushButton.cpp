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

#include <djvUI/PushButton.h>

#include <djvUI/Border.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2D.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            struct Push::Private
            {
                std::shared_ptr<Icon> icon;
                std::shared_ptr<Label> label;
                std::shared_ptr<HorizontalLayout> layout;
                std::shared_ptr<Border> border;
            };

            void Push::_init(Context * context)
            {
                IButton::_init(context);

                setClassName("djv::UI::Button::Push");
                setBackgroundRole(ColorRole::Button);

                DJV_PRIVATE_PTR();
                p.icon = Icon::create(context);
                p.icon->setVAlign(VAlign::Center);
                p.icon->hide();

                p.label = Label::create(context);
                p.label->hide();

                p.layout = HorizontalLayout::create(context);
                p.layout->setMargin(Layout::Margin(MetricsRole::MarginLarge, MetricsRole::MarginLarge, MetricsRole::MarginSmall, MetricsRole::MarginSmall));
                p.layout->addWidget(p.icon);
                p.layout->addWidget(p.label, RowStretch::Expand);

                p.border = Border::create(context);
                p.border->addWidget(p.layout);
                p.border->setParent(shared_from_this());
            }

            Push::Push() :
                _p(new Private)
            {}

            Push::~Push()
            {}

            std::shared_ptr<Push> Push::create(Context * context)
            {
                auto out = std::shared_ptr<Push>(new Push);
                out->_init(context);
                return out;
            }

            std::shared_ptr<Push> Push::create(const std::string& text, Context * context)
            {
                auto out = std::shared_ptr<Push>(new Push);
                out->_init(context);
                out->setText(text);
                return out;
            }

            std::shared_ptr<Push> Push::create(const std::string& text, const std::string& icon, Context * context)
            {
                auto out = std::shared_ptr<Push>(new Push);
                out->_init(context);
                out->setIcon(icon);
                out->setText(text);
                return out;
            }

            const std::string& Push::getIcon() const
            {
                return _p->icon->getIcon();
            }

            void Push::setIcon(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.icon->setIcon(value);
                p.icon->setVisible(!value.empty());
            }

            const std::string& Push::getText() const
            {
                return _p->label->getText();
            }

            void Push::setText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.label->setText(value);
                p.label->setVisible(!value.empty());
            }

            TextHAlign Push::getTextHAlign() const
            {
                return _p->label->getTextHAlign();
            }

            TextVAlign Push::getTextVAlign() const
            {
                return _p->label->getTextVAlign();
            }

            void Push::setTextHAlign(TextHAlign value)
            {
                _p->label->setTextHAlign(value);
            }

            void Push::setTextVAlign(TextVAlign value)
            {
                _p->label->setTextVAlign(value);
            }

            const std::string & Push::getFont() const
            {
                return _p->label->getFont();
            }

            const std::string & Push::getFontFace() const
            {
                return _p->label->getFontFace();
            }

			MetricsRole Push::getFontSizeRole() const
            {
                return _p->label->getFontSizeRole();
            }

            void Push::setFont(const std::string & value)
            {
                _p->label->setFont(value);
            }

            void Push::setFontFace(const std::string & value)
            {
                _p->label->setFontFace(value);
            }

            void Push::setFontSizeRole(MetricsRole value)
            {
                _p->label->setFontSizeRole(value);
            }

            const Layout::Margin& Push::getInsideMargin() const
            {
                return _p->border->getMargin();
            }

            void Push::setInsideMargin(const Layout::Margin& value)
            {
                _p->border->setMargin(value);
            }

            float Push::getHeightForWidth(float value) const
            {
                return _p->border->getHeightForWidth(value);
            }

            void Push::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->border->getMinimumSize());
            }

            void Push::_layoutEvent(Event::Layout&)
            {
                _p->border->setGeometry(getGeometry());
            }

            void Push::_updateEvent(Event::Update& event)
            {
                IButton::_updateEvent(event);
                const ColorRole colorRole = _getForegroundColorRole();
                DJV_PRIVATE_PTR();
                p.icon->setIconColorRole(colorRole);
                p.label->setTextColorRole(colorRole);
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
