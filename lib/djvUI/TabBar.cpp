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

#include <djvUI/TabBar.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/IButton.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2DSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class TabBarButton : public IButton
            {
                DJV_NON_COPYABLE(TabBarButton);

            protected:
                void _init(const std::string& text, Core::Context *);
                TabBarButton();

            public:
                virtual ~TabBarButton();

                static std::shared_ptr<TabBarButton> create(const std::string&, Core::Context *);

                const std::string& getText() const;
                void setText(const std::string&);

                float getHeightForWidth(float) const override;
                void preLayoutEvent(Core::PreLayoutEvent&) override;
                void layoutEvent(Core::LayoutEvent&) override;
                void paintEvent(Core::PaintEvent&) override;

                void updateEvent(Core::UpdateEvent&) override;

            private:
                std::shared_ptr<Label> _label;
                std::shared_ptr<StackLayout> _layout;
            };

            void TabBarButton::_init(const std::string& text, Context * context)
            {
                IButton::_init(context);

                setClassName("Gp::UI::TabBarButton");

                _label = Label::create(text, context);
                _label->setMargin(Margin(MetricsRole::Margin, MetricsRole::Margin, MetricsRole::MarginSmall, MetricsRole::MarginSmall));

                _layout = StackLayout::create(context);
                _layout->addWidget(_label);
                _layout->setParent(shared_from_this());
            }

            TabBarButton::TabBarButton()
            {}

            TabBarButton::~TabBarButton()
            {}

            std::shared_ptr<TabBarButton> TabBarButton::create(const std::string& text, Context * context)
            {
                auto out = std::shared_ptr<TabBarButton>(new TabBarButton);
                out->_init(text, context);
                return out;
            }

            const std::string& TabBarButton::getText() const
            {
                return _label->getText();
            }

            void TabBarButton::setText(const std::string& value)
            {
                _label->setText(value);
            }

            float TabBarButton::getHeightForWidth(float value) const
            {
                return _layout->getHeightForWidth(value);
            }

            void TabBarButton::preLayoutEvent(PreLayoutEvent& event)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void TabBarButton::layoutEvent(LayoutEvent&)
            {
                _layout->setGeometry(getGeometry());
            }

            void TabBarButton::paintEvent(Core::PaintEvent& event)
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

            void TabBarButton::updateEvent(UpdateEvent& event)
            {
                IButton::updateEvent(event);
                _label->setTextColorRole(_isToggled() ? ColorRole::Foreground : ColorRole::ForegroundDim);
            }

        } // namespace

        struct TabBar::Private
        {
            std::shared_ptr<ButtonGroup> buttonGroup;
            std::shared_ptr<HorizontalLayout> layout;
            std::function<void(int)> callback;
        };

        void TabBar::_init(Context * context)
        {
            Widget::_init(context);
            
            setClassName("djv::UI::TabBar");
            setBackgroundRole(ColorRole::Border);

            _p->buttonGroup = ButtonGroup::create(ButtonType::Radio);

            _p->layout = HorizontalLayout::create(context);
            _p->layout->setSpacing(MetricsRole::Border);
            _p->layout->setParent(shared_from_this());

            auto weak = std::weak_ptr<TabBar>(std::dynamic_pointer_cast<TabBar>(shared_from_this()));
            _p->buttonGroup->setRadioCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->callback)
                    {
                        widget->_p->callback(value);
                    }
                }
            });
        }

        TabBar::TabBar() :
            _p(new Private)
        {}

        TabBar::~TabBar()
        {}

        std::shared_ptr<TabBar> TabBar::create(Context * context)
        {
            auto out = std::shared_ptr<TabBar>(new TabBar);
            out->_init(context);
            return out;
        }

        void TabBar::addTab(const std::string & text)
        {
            auto button = TabBarButton::create(text, getContext());
            _p->buttonGroup->addButton(button);
            _p->layout->addWidget(button);
        }

        void TabBar::setCallback(const std::function<void(int)>& value)
        {
            _p->callback = value;
        }

        float TabBar::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void TabBar::preLayoutEvent(PreLayoutEvent& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void TabBar::layoutEvent(LayoutEvent& event)
        {
            if (auto style = _getStyle().lock())
            {
                const BBox2f & g = getGeometry();
                _p->layout->setGeometry(getMargin().bbox(g, style));
            }
        }

    } // namespace UI
} // namespace djv
