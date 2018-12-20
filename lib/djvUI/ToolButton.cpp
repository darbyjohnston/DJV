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

#include <djvUI/ToolButton.h>

#include <djvUI/Icon.h>
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
        struct ToolButton::Private
        {
            std::shared_ptr<Icon> icon;
            std::shared_ptr<StackLayout> layout;
        };

        void ToolButton::_init(Context * context)
        {
            IButton::_init(context);

            setClassName("Gp::UI::ToolButton");

            _p->icon = Icon::create(context);
            _p->icon->setVAlign(VAlign::Center);
            _p->icon->hide();

            _p->layout = StackLayout::create(context);
            _p->layout->setMargin(MetricsRole::Margin);
            _p->layout->addWidget(_p->icon);
            _p->layout->setParent(shared_from_this());
        }

        ToolButton::ToolButton() :
            _p(new Private)
        {}

        ToolButton::~ToolButton()
        {}

        std::shared_ptr<ToolButton> ToolButton::create(Context * context)
        {
            auto out = std::shared_ptr<ToolButton>(new ToolButton);
            out->_init(context);
            return out;
        }

        std::shared_ptr<ToolButton> ToolButton::create(const Path& icon, Context * context)
        {
            auto out = std::shared_ptr<ToolButton>(new ToolButton);
            out->_init(context);
            out->setIcon(icon);
            return out;
        }

        const Path& ToolButton::getIcon() const
        {
            return _p->icon->getIcon();
        }

        void ToolButton::setIcon(const Path& value)
        {
            _p->icon->setIcon(value);
            _p->icon->setVisible(!value.isEmpty());
        }

        const Margin& ToolButton::getInsideMargin() const
        {
            return _p->layout->getMargin();
        }

        void ToolButton::setInsideMargin(const Margin& value)
        {
            _p->layout->setMargin(value);
        }

        float ToolButton::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void ToolButton::updateEvent(UpdateEvent& event)
        {
            const auto style = _getStyle();
            const bool enabled = isEnabled(true);
            const ColorRole fg = !enabled ? ColorRole::Disabled : (_isToggled() ? ColorRole::CheckedForeground : ColorRole::Foreground);
            _p->icon->setIconColorRole(fg);
        }

        void ToolButton::preLayoutEvent(PreLayoutEvent& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ToolButton::layoutEvent(LayoutEvent&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ToolButton::paintEvent(PaintEvent& event)
        {
            Widget::paintEvent(event);
            if (auto render = _getRenderSystem().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& g = getGeometry();

                    // Draw the toggled state.
                    if (_isToggled())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(getCheckedColorRole())));
                        render->drawRectangle(g);
                    }

                    // Draw the hovered state.
                    if (_isHovered())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Hover)));
                        render->drawRectangle(g);
                    }
                }
            }
        }

    } // namespace UI
} // namespace Gp
