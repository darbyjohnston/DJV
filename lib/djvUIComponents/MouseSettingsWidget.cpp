// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/MouseSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/EnumFunc.h>
#include <djvUI/FormLayout.h>
#include <djvUI/MouseSettings.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToggleButton.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct MouseSettingsWidget::Private
        {
            std::shared_ptr<ComboBox> scrollWheelSpeedComboBox;
            std::shared_ptr<ToggleButton> reverseScrollingButton;
            std::shared_ptr<FormLayout> layout;

            std::shared_ptr<Observer::Value<float> > doubleClickTimeObserver;
            std::shared_ptr<Observer::Value<ScrollWheelSpeed> > scrollWheelSpeedObserver;
            std::shared_ptr<Observer::Value<bool> > reverseScrollingObserver;
        };

        void MouseSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::MouseSettingsWidget");

            p.scrollWheelSpeedComboBox = ComboBox::create(context);
            p.reverseScrollingButton = ToggleButton::create(context);

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.scrollWheelSpeedComboBox);
            p.layout->addChild(p.reverseScrollingButton);
            addChild(p.layout);

            auto weak = std::weak_ptr<MouseSettingsWidget>(std::dynamic_pointer_cast<MouseSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.scrollWheelSpeedComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::SettingsSystem>();
                            if (auto mouseSettings = settingsSystem->getSettingsT<Settings::Mouse>())
                            {
                                mouseSettings->setScrollWheelSpeed(static_cast<ScrollWheelSpeed>(value));
                            }
                        }
                    }
                });
            p.reverseScrollingButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto uiSettings = settingsSystem->getSettingsT<Settings::Mouse>();
                            uiSettings->setReverseScrolling(value);
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<Settings::SettingsSystem>();
            auto mouseSettings = settingsSystem->getSettingsT<Settings::Mouse>();
            p.scrollWheelSpeedObserver = Observer::Value<ScrollWheelSpeed>::create(
                mouseSettings->observeScrollWheelSpeed(),
                [weak](ScrollWheelSpeed value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->scrollWheelSpeedComboBox->setCurrentItem(static_cast<int>(value));
                    }
                });
            p.reverseScrollingObserver = Observer::Value<bool>::create(
                mouseSettings->observeReverseScrolling(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->reverseScrollingButton->setChecked(value);
                    }
                });
        }

        MouseSettingsWidget::MouseSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<MouseSettingsWidget> MouseSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MouseSettingsWidget>(new MouseSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string MouseSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_mouse");
        }

        std::string MouseSettingsWidget::getSettingsSortKey() const
        {
            return "a";
        }

        void MouseSettingsWidget::setLabelSizeGroup(const std::weak_ptr<Text::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void MouseSettingsWidget::_initEvent(System::Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.scrollWheelSpeedComboBox, _getText(DJV_TEXT("settings_mouse_scroll_wheel_speed")) + ":");
                p.layout->setText(p.reverseScrollingButton, _getText(DJV_TEXT("settings_mouse_reverse_scrolling")) + ":");
                _widgetUpdate();
            }
        }

        void MouseSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::vector<std::string> items;
                for (auto i : getScrollWheelSpeedEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    items.push_back(_getText(ss.str()));
                }
                auto settingsSystem = context->getSystemT<Settings::SettingsSystem>();
                auto mouseSettings = settingsSystem->getSettingsT<Settings::Mouse>();
                p.scrollWheelSpeedComboBox->setItems(items);
                p.scrollWheelSpeedComboBox->setCurrentItem(static_cast<int>(mouseSettings->observeScrollWheelSpeed()->get()));
            }
        }

    } // namespace ViewApp
} // namespace djv

