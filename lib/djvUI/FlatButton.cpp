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

#include <djvUI/FlatButton.h>

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
            struct Flat::Private
            {
                std::shared_ptr<Icon> icon;
                std::shared_ptr<Label> label;
                std::shared_ptr<HorizontalLayout> layout;
            };

            void Flat::_init(Context * context)
            {
                IButton::_init(context);

                setClassName("djv::UI::Button::Flat");

                DJV_PRIVATE_PTR();
                p.icon = Icon::create(context);
                p.icon->setVAlign(VAlign::Center);

                p.label = Label::create(context);
                p.label->setTextHAlign(TextHAlign::Left);

                p.layout = HorizontalLayout::create(context);
                p.layout->setMargin(MetricsRole::MarginSmall);
                p.layout->addWidget(p.icon);
                p.layout->addWidget(p.label, RowStretch::Expand);
                p.layout->setParent(shared_from_this());

                _widgetUpdate();
            }

            Flat::Flat() :
                _p(new Private)
            {}

            Flat::~Flat()
            {}

            std::shared_ptr<Flat> Flat::create(Context * context)
            {
                auto out = std::shared_ptr<Flat>(new Flat);
                out->_init(context);
                return out;
            }

            std::shared_ptr<Flat> Flat::create(const std::string& text, Context * context)
            {
                auto out = std::shared_ptr<Flat>(new Flat);
                out->_init(context);
                out->setText(text);
                return out;
            }

            std::shared_ptr<Flat> Flat::create(const std::string& text, const std::string& icon, Context * context)
            {
                auto out = std::shared_ptr<Flat>(new Flat);
                out->_init(context);
                out->setIcon(icon);
                out->setText(text);
                return out;
            }

            const std::string& Flat::getIcon() const
            {
                return _p->icon->getIcon();
            }

            void Flat::setIcon(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.icon->setIcon(value);
                _widgetUpdate();
            }

            const std::string& Flat::getText() const
            {
                return _p->label->getText();
            }

            void Flat::setText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                p.label->setText(value);
                _widgetUpdate();
            }

            TextHAlign Flat::getTextHAlign() const
            {
                return _p->label->getTextHAlign();
            }

            TextVAlign Flat::getTextVAlign() const
            {
                return _p->label->getTextVAlign();
            }

            void Flat::setTextHAlign(TextHAlign value)
            {
                _p->label->setTextHAlign(value);
            }

            void Flat::setTextVAlign(TextVAlign value)
            {
                _p->label->setTextVAlign(value);
            }

			ColorRole Flat::getTextColorRole() const
            {
                return _p->label->getTextColorRole();
            }

            void Flat::setTextColorRole(ColorRole value)
            {
                _p->label->setTextColorRole(value);
            }

            const std::string & Flat::getFont() const
            {
                return _p->label->getFont();
            }

            const std::string & Flat::getFontFace() const
            {
                return _p->label->getFontFace();
            }

			MetricsRole Flat::getFontSizeRole() const
            {
                return _p->label->getFontSizeRole();
            }

            void Flat::setFont(const std::string & value)
            {
                _p->label->setFont(value);
            }

            void Flat::setFontFace(const std::string & value)
            {
                _p->label->setFontFace(value);
            }

            void Flat::setFontSizeRole(MetricsRole value)
            {
                _p->label->setFontSizeRole(value);
            }

            const Layout::Margin& Flat::getInsideMargin() const
            {
                return _p->layout->getMargin();
            }

            void Flat::setInsideMargin(const Layout::Margin& value)
            {
                _p->layout->setMargin(value);
            }

            void Flat::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void Flat::_layoutEvent(Event::Layout&)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void Flat::_widgetUpdate()
            {
                _p->icon->setVisible(!_p->icon->getIcon().empty());
                _p->label->setVisible(!_p->label->getText().empty());
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
