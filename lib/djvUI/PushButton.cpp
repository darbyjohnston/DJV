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

#include <djvUI/PushButton.h>

#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/PixelData.h>
#include <djvAV/Render2DSystem.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct PushButton::Private
        {
            std::shared_ptr<Icon> icon;
            std::shared_ptr<Label> label;
            std::shared_ptr<HorizontalLayout> layout;
        };

        void PushButton::_init(Context * context)
        {
            IButton::_init(context);

            setClassName("Gp::UI::PushButton");
            setBackgroundRole(ColorRole::Button);

            _p->icon = Icon::create(context);
            _p->icon->setVAlign(VAlign::Center);
            _p->icon->hide();

            _p->label = Label::create(context);
            _p->label->hide();

            _p->layout = HorizontalLayout::create(context);
            _p->layout->setMargin(Margin(MetricsRole::MarginLarge, MetricsRole::MarginLarge, MetricsRole::MarginSmall, MetricsRole::MarginSmall));
            _p->layout->addWidget(_p->icon);
            _p->layout->addWidget(_p->label, RowLayoutStretch::Expand);
            _p->layout->setParent(shared_from_this());

        }

        PushButton::PushButton() :
            _p(new Private)
        {}

        PushButton::~PushButton()
        {}

        std::shared_ptr<PushButton> PushButton::create(Context * context)
        {
            auto out = std::shared_ptr<PushButton>(new PushButton);
            out->_init(context);
            return out;
        }

        std::shared_ptr<PushButton> PushButton::create(const std::string& text, Context * context)
        {
            auto out = std::shared_ptr<PushButton>(new PushButton);
            out->_init(context);
            out->setText(text);
            return out;
        }

        std::shared_ptr<PushButton> PushButton::create(const std::string& text, const FileSystem::Path& icon, Context * context)
        {
            auto out = std::shared_ptr<PushButton>(new PushButton);
            out->_init(context);
            out->setIcon(icon);
            out->setText(text);
            return out;
        }

        const FileSystem::Path& PushButton::getIcon() const
        {
            return _p->icon->getIcon();
        }

        void PushButton::setIcon(const FileSystem::Path& value)
        {
            _p->icon->setIcon(value);
            _p->icon->setVisible(!value.isEmpty());
        }

        const std::string& PushButton::getText() const
        {
            return _p->label->getText();
        }

        void PushButton::setText(const std::string& value)
        {
            _p->label->setText(value);
            _p->label->setVisible(!value.empty());
        }

        TextHAlign PushButton::getTextHAlign() const
        {
            return _p->label->getTextHAlign();
        }

        TextVAlign PushButton::getTextVAlign() const
        {
            return _p->label->getTextVAlign();
        }

        void PushButton::setTextHAlign(TextHAlign value)
        {
            _p->label->setTextHAlign(value);
        }

        void PushButton::setTextVAlign(TextVAlign value)
        {
            _p->label->setTextVAlign(value);
        }

        const std::string & PushButton::getFontFace() const
        {
            return _p->label->getFontFace();
        }

        MetricsRole PushButton::getFontSizeRole() const
        {
            return _p->label->getFontSizeRole();
        }

        void PushButton::setFontFace(const std::string & value)
        {
            _p->label->setFontFace(value);
        }

        void PushButton::setFontSizeRole(MetricsRole value)
        {
            _p->label->setFontSizeRole(value);
        }

        const Margin& PushButton::getInsideMargin() const
        {
            return _p->layout->getMargin();
        }

        void PushButton::setInsideMargin(const Margin& value)
        {
            _p->layout->setMargin(value);
        }

        float PushButton::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void PushButton::updateEvent(Event::Update& event)
        {
            const auto style = _getStyle();
            const bool enabled = isEnabled(true);
            const ColorRole fg = !enabled ? ColorRole::Disabled : (_isToggled() ? ColorRole::CheckedForeground : ColorRole::Foreground);
            _p->icon->setIconColorRole(fg);
            _p->label->setTextColorRole(fg);
        }

        void PushButton::preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void PushButton::layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void PushButton::paintEvent(Event::Paint& event)
        {
            if (auto render = _getRenderSystem().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& g = getGeometry();
                    const float b = style->getMetric(MetricsRole::Border);

                    // Draw the border.
                    render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Border)));
                    render->drawRectangle(g);

                    // Draw the background.
                    render->setFillColor(_getColorWithOpacity(style->getColor(getBackgroundRole())));
                    render->drawRectangle(g.margin(-b));

                    // Draw the toggled state.
                    if (_isToggled())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(getCheckedColorRole())));
                        render->drawRectangle(g.margin(-b));
                    }

                    // Draw the hovered state.
                    if (_isHovered())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Hover)));
                        render->drawRectangle(g.margin(-b));
                    }
                }
            }
        }

    } // namespace UI
} // namespace Gp
