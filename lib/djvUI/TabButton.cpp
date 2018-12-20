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

#include <djvUI/TabButton.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2DSystem.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct TabButton::Private
        {
            std::shared_ptr<Label> label;
            std::shared_ptr<StackLayout> layout;
        };

        void TabButton::_init(const std::string& text, Context * context)
        {
            IButton::_init(context);
            
            setClassName("Gp::UI::TabButton");

            _p->label = Label::create(text, context);
            _p->label->setMargin(MetricsRole::Margin);
            _p->label->setVisible(!text.empty());
            
            _p->layout = StackLayout::create(context);
            _p->layout->addWidget(_p->label);
            _p->layout->setParent(shared_from_this());
        }

        TabButton::TabButton() :
            _p(new Private)
        {}

        TabButton::~TabButton()
        {}

        std::shared_ptr<TabButton> TabButton::create(const std::string& text, Context * context)
        {
            auto out = std::shared_ptr<TabButton>(new TabButton);
            out->_init(text, context);
            return out;
        }

        const std::string& TabButton::getText() const
        {
            return _p->label->getText();
        }

        void TabButton::setText(const std::string& value)
        {
            _p->label->setText(value);
            _p->label->setVisible(!value.empty());
        }

        TextHAlign TabButton::getTextHAlign() const
        {
            return _p->label->getTextHAlign();
        }
        
        TextVAlign TabButton::getTextVAlign() const
        {
            return _p->label->getTextVAlign();
        }
        
        void TabButton::setTextHAlign(TextHAlign value)
        {
            _p->label->setTextHAlign(value);
        }
        
        void TabButton::setTextVAlign(TextVAlign value)
        {
            _p->label->setTextVAlign(value);
        }

        const std::string & TabButton::getFontFace() const
        {
            return _p->label->getFontFace();
        }

        MetricsRole TabButton::getFontSizeRole() const
        {
            return _p->label->getFontSizeRole();
        }

        void TabButton::setFontFace(const std::string & value)
        {
            _p->label->setFontFace(value);
        }

        void TabButton::setFontSizeRole(MetricsRole value)
        {
            _p->label->setFontSizeRole(value);
        }

        float TabButton::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void TabButton::preLayoutEvent(PreLayoutEvent& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void TabButton::layoutEvent(LayoutEvent&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void TabButton::paintEvent(Core::PaintEvent& event)
        {
            if (auto render = _getRenderSystem().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& g = getGeometry();

                    // Draw the toggled state.
                    render->setFillColor(_getColorWithOpacity(style->getColor(_isToggled() ? ColorRole::Background : ColorRole::Border)));
                    render->drawRectangle(g);

                    // Draw the hovered state.
                    if (_isHovered() && !_isToggled())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Hover)));
                        render->drawRectangle(g);
                    }
                }
            }
        }

        void TabButton::updateEvent(UpdateEvent& event)
        {
            IButton::updateEvent(event);
            _p->label->setTextColorRole(_isToggled() ? ColorRole::Foreground : ColorRole::ForegroundDim);
        }

    } // namespace UI
} // namespace Gp
