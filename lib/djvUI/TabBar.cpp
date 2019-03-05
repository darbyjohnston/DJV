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

#include <djvUI/TabBar.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/IButton.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class TabBarButton : public Button::IButton
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

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;
                void _paintEvent(Event::Paint&) override;

            private:
                std::shared_ptr<Label> _label;
                std::shared_ptr<StackLayout> _layout;
            };

            void TabBarButton::_init(const std::string& text, Context * context)
            {
                IButton::_init(context);

                setClassName("djv::UI::TabBarButton");

                _label = Label::create(text, context);
                _label->setMargin(Layout::Margin(MetricsRole::Margin, MetricsRole::Margin, MetricsRole::MarginSmall, MetricsRole::MarginSmall));

                _layout = StackLayout::create(context);
                _layout->addChild(_label);
                addChild(_layout);
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

            void TabBarButton::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void TabBarButton::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void TabBarButton::_paintEvent(Event::Paint& event)
            {
                if (auto render = _getRender().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const BBox2f& g = getGeometry();

                        // Draw the toggled state.
                        render->setFillColor(_getColorWithOpacity(style->getColor(_isToggled() ? ColorRole::Background : ColorRole::Border)));
                        render->drawRect(g);

                        // Draw the hovered state.
                        if (_isHovered() && !_isToggled())
                        {
                            render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Hovered)));
                            render->drawRect(g);
                        }
                    }
                }
            }

        } // namespace

        struct TabBar::Private
        {
            std::shared_ptr<ButtonGroup> buttonGroup;
            std::shared_ptr<HorizontalLayout> layout;
            std::function<void(int)> callback;
            std::function<void(size_t)> removedCallback;
        };

        void TabBar::_init(Context * context)
        {
            Widget::_init(context);
            
            setClassName("djv::UI::TabBar");
            setBackgroundRole(ColorRole::Border);

            DJV_PRIVATE_PTR();
            p.buttonGroup = ButtonGroup::create(ButtonType::Radio);

            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(MetricsRole::Border);
            addChild(p.layout);

            auto weak = std::weak_ptr<TabBar>(std::dynamic_pointer_cast<TabBar>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
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

        size_t TabBar::getTabCount() const
        {
            return _p->buttonGroup->getButtonCount();
        }

        size_t TabBar::addTab(const std::string & text)
        {
            DJV_PRIVATE_PTR();
            const size_t out = p.buttonGroup->getButtonCount();
            auto button = TabBarButton::create(text, getContext());
            p.buttonGroup->addButton(button);
            p.layout->addChild(button);
            return out;
        }

        void TabBar::removeTab(size_t value)
        {
            DJV_PRIVATE_PTR();
            const auto buttons = p.buttonGroup->getButtons();
            if (value < buttons.size())
            {
                p.buttonGroup->removeButton(buttons[value]);
                p.layout->removeChild(buttons[value]);
                if (p.removedCallback)
                {
                    p.removedCallback(value);
                }
            }
        }

        int TabBar::getCurrentTab() const
        {
            return _p->buttonGroup->getChecked();
        }

        void TabBar::setCurrentTab(int value)
        {
            _p->buttonGroup->setChecked(value);
        }

        void TabBar::setCurrentTabCallback(const std::function<void(int)>& value)
        {
            _p->callback = value;
        }

        void TabBar::removeCurrentTab()
        {
            const int index = getCurrentTab();
            if (index >= 0)
            {
                removeTab(static_cast<size_t>(index));
            }
        }

        float TabBar::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void TabBar::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void TabBar::_layoutEvent(Event::Layout& event)
        {
            if (auto style = _getStyle().lock())
            {
                const BBox2f & g = getGeometry();
                _p->layout->setGeometry(getMargin().bbox(g, style));
            }
        }

    } // namespace UI
} // namespace djv
