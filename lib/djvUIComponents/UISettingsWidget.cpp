// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/UISettingsWidget.h>

#include <djvUI/FormLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToggleButton.h>
#include <djvUI/UISettings.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct TooltipsSettingsWidget::Private
        {
            std::shared_ptr<ToggleButton> tooltipsButton;
            std::shared_ptr<FormLayout> layout;
            std::shared_ptr<ValueObserver<bool> > tooltipsObserver;
        };

        void TooltipsSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::TooltipsSettingsWidget");

            p.tooltipsButton = ToggleButton::create(context);

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.tooltipsButton);
            addChild(p.layout);

            auto weak = std::weak_ptr<TooltipsSettingsWidget>(std::dynamic_pointer_cast<TooltipsSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.tooltipsButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto uiSettings = settingsSystem->getSettingsT<Settings::UI>();
                            uiSettings->setTooltips(value);
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto uiSettings = settingsSystem->getSettingsT<Settings::UI>();
            p.tooltipsObserver = ValueObserver<bool>::create(
                uiSettings->observeTooltips(),
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->tooltipsButton->setChecked(value);
                }
            });
        }

        TooltipsSettingsWidget::TooltipsSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<TooltipsSettingsWidget> TooltipsSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<TooltipsSettingsWidget>(new TooltipsSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string TooltipsSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_general");
        }

        std::string TooltipsSettingsWidget::getSettingsSortKey() const
        {
            return "0";
        }

        void TooltipsSettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void TooltipsSettingsWidget::_initEvent(System::Event::Init & event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.tooltipsButton, _getText(DJV_TEXT("settings_general_tooltips")) + ":");
            }
        }

        struct ScrollSettingsWidget::Private
        {
            std::shared_ptr<ToggleButton> reverseScrollingButton;
            std::shared_ptr<FormLayout> layout;
            std::shared_ptr<ValueObserver<bool> > reverseScrollingObserver;
        };

        void ScrollSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ScrollSettingsWidget");

            p.reverseScrollingButton = ToggleButton::create(context);

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.reverseScrollingButton);
            addChild(p.layout);

            auto weak = std::weak_ptr<ScrollSettingsWidget>(std::dynamic_pointer_cast<ScrollSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.reverseScrollingButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto uiSettings = settingsSystem->getSettingsT<Settings::UI>();
                            uiSettings->setReverseScrolling(value);
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto uiSettings = settingsSystem->getSettingsT<Settings::UI>();
            p.reverseScrollingObserver = ValueObserver<bool>::create(
                uiSettings->observeReverseScrolling(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->reverseScrollingButton->setChecked(value);
                    }
                });
        }

        ScrollSettingsWidget::ScrollSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ScrollSettingsWidget> ScrollSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ScrollSettingsWidget>(new ScrollSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ScrollSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_general");
        }

        std::string ScrollSettingsWidget::getSettingsSortKey() const
        {
            return "0";
        }

        void ScrollSettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void ScrollSettingsWidget::_initEvent(System::Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.reverseScrollingButton, _getText(DJV_TEXT("settings_general_reverse_scrolling")) + ":");
            }
        }

    } // namespace UI
} // namespace djv

