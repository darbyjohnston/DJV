// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/TabBar.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/IButton.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolButton.h>

#include <djvRender2D/Render.h>

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
                void _init(const std::string& text, const std::shared_ptr<System::Context>&);
                TabBarButton();

            public:
                ~TabBarButton() override;

                static std::shared_ptr<TabBarButton> create(const std::string&, const std::shared_ptr<System::Context>&);

                const std::string& getText() const;
                void setText(const std::string&);

                void setChecked(bool) override;

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;

            private:
                std::shared_ptr<Text::Label> _label;
                std::shared_ptr<StackLayout> _layout;
            };

            void TabBarButton::_init(const std::string& text, const std::shared_ptr<System::Context>& context)
            {
                IButton::_init(context);

                setClassName("djv::UI::TabBarButton");

                _label = Text::Label::create(context);
                _label->setText(text);
                _label->setTextColorRole(ColorRole::ForegroundDim);
                _label->setMargin(Layout::Margin(MetricsRole::Margin, MetricsRole::Margin, MetricsRole::MarginSmall, MetricsRole::MarginSmall));

                _layout = StackLayout::create(context);
                _layout->addChild(_label);
                addChild(_layout);
            }

            TabBarButton::TabBarButton()
            {}

            TabBarButton::~TabBarButton()
            {}

            std::shared_ptr<TabBarButton> TabBarButton::create(const std::string& text, const std::shared_ptr<System::Context>& context)
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

            void TabBarButton::setChecked(bool value)
            {
                IButton::setChecked(value);
                _label->setTextColorRole(value ? ColorRole::Foreground : ColorRole::ForegroundDim);
            }

            float TabBarButton::getHeightForWidth(float value) const
            {
                return _layout->getHeightForWidth(value);
            }

            void TabBarButton::_preLayoutEvent(System::Event::PreLayout& event)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void TabBarButton::_layoutEvent(System::Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void TabBarButton::_paintEvent(System::Event::Paint& event)
            {
                const Math::BBox2f& g = getGeometry();
                const auto& render = _getRender();
                const auto& style = _getStyle();
                render->setFillColor(style->getColor(_isToggled() ? ColorRole::BackgroundToolBar : ColorRole::Background));
                render->drawRect(g);
                if (_isHovered() && !_isToggled())
                {
                    render->setFillColor(style->getColor(ColorRole::Hovered));
                    render->drawRect(g);
                }
            }

        } // namespace

        struct TabBar::Private
        {
            std::shared_ptr<ButtonGroup> buttonGroup;
            std::shared_ptr<HorizontalLayout> layout;
            std::shared_ptr<ScrollWidget> scrollWidget;
            std::function<void(int)> callback;
            std::function<void(size_t)> removedCallback;
        };

        void TabBar::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            
            setClassName("djv::UI::TabBar");
            setBackgroundRole(ColorRole::Background);

            DJV_PRIVATE_PTR();
            p.buttonGroup = ButtonGroup::create(ButtonType::Radio);

            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);

            p.scrollWidget = ScrollWidget::create(ScrollType::Horizontal, context);
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setScrollBarSizeRole(UI::MetricsRole::ScrollBarSmall);
            p.scrollWidget->addChild(p.layout);
            addChild(p.scrollWidget);

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

        std::shared_ptr<TabBar> TabBar::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<TabBar>(new TabBar);
            out->_init(context);
            return out;
        }

        size_t TabBar::getTabCount() const
        {
            return _p->buttonGroup->getButtonCount();
        }

        void TabBar::setTabs(const std::vector<std::string>& text)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::vector<std::shared_ptr<Button::IButton> > buttons;
                for (const auto& i : text)
                {
                    auto button = TabBarButton::create(i, context);
                    p.layout->addChild(button);
                    buttons.push_back(button);
                }
                p.buttonGroup->setButtons(buttons);
            }
        }

        void TabBar::clearTabs()
        {
            DJV_PRIVATE_PTR();
            p.buttonGroup->clearButtons();
            p.layout->clearChildren();
        }

        void TabBar::setText(int index, const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (index < static_cast<int>(p.buttonGroup->getButtonCount()))
            {
                std::dynamic_pointer_cast<TabBarButton>(p.buttonGroup->getButtons()[index])->setText(value);
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

        float TabBar::getHeightForWidth(float value) const
        {
            return _p->scrollWidget->getHeightForWidth(value);
        }

        void TabBar::_preLayoutEvent(System::Event::PreLayout& event)
        {
            _setMinimumSize(_p->scrollWidget->getMinimumSize());
        }

        void TabBar::_layoutEvent(System::Event::Layout& event)
        {
            const Math::BBox2f& g = getGeometry();
            const auto& style = _getStyle();
            _p->scrollWidget->setGeometry(getMargin().bbox(g, style));
        }

    } // namespace UI
} // namespace djv
