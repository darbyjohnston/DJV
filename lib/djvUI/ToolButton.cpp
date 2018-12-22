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

#include <djvAV/Render2DSystem.h>

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
                std::shared_ptr<Layout::Stack> layout;
            };

            void Tool::_init(Context * context)
            {
                IButton::_init(context);

                setClassName("Gp::UI::Button::Tool");

                _p->icon = Icon::create(context);
                _p->icon->setVAlign(VAlign::Center);
                _p->icon->hide();

                _p->layout = Layout::Stack::create(context);
                _p->layout->setMargin(Style::MetricsRole::Margin);
                _p->layout->addWidget(_p->icon);
                _p->layout->setParent(shared_from_this());
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

            std::shared_ptr<Tool> Tool::create(const FileSystem::Path& icon, Context * context)
            {
                auto out = std::shared_ptr<Tool>(new Tool);
                out->_init(context);
                out->setIcon(icon);
                return out;
            }

            const FileSystem::Path& Tool::getIcon() const
            {
                return _p->icon->getIcon();
            }

            void Tool::setIcon(const FileSystem::Path& value)
            {
                _p->icon->setIcon(value);
                _p->icon->setVisible(!value.isEmpty());
            }

            const Layout::Margin& Tool::getInsideMargin() const
            {
                return _p->layout->getMargin();
            }

            void Tool::setInsideMargin(const Layout::Margin& value)
            {
                _p->layout->setMargin(value);
            }

            float Tool::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }

            void Tool::updateEvent(Event::Update& event)
            {
                const auto style = _getStyle();
                const bool enabled = isEnabled(true);
                const Style::ColorRole fg = !enabled ? Style::ColorRole::Disabled : (_isToggled() ? Style::ColorRole::CheckedForeground : Style::ColorRole::Foreground);
                _p->icon->setIconColorRole(fg);
            }

            void Tool::preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void Tool::layoutEvent(Event::Layout&)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void Tool::paintEvent(Event::Paint& event)
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
                            render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Hover)));
                            render->drawRectangle(g);
                        }
                    }
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
