// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/InputSettingsWidget.h>

#include <djvViewApp/InputSettings.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct InputSettingsWidget::Private
        {
            std::shared_ptr<UI::ComboBox> scrollWheelSpeedComboBox;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<ValueObserver<ScrollWheelSpeed> > scrollWheelSpeedObserver;
        };

        void InputSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::InputSettingsWidget");

            p.scrollWheelSpeedComboBox = UI::ComboBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->addChild(p.scrollWheelSpeedComboBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<InputSettingsWidget>(std::dynamic_pointer_cast<InputSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.scrollWheelSpeedComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto inputSettings = settingsSystem->getSettingsT<InputSettings>())
                            {
                                inputSettings->setScrollWheelSpeed(static_cast<ScrollWheelSpeed>(value));
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto inputSettings = settingsSystem->getSettingsT<InputSettings>())
            {
                p.scrollWheelSpeedObserver = ValueObserver<ScrollWheelSpeed>::create(
                    inputSettings->observeScrollWheelSpeed(),
                    [weak](ScrollWheelSpeed value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->scrollWheelSpeedComboBox->setCurrentItem(static_cast<int>(value));
                        }
                    });
            }
        }

        InputSettingsWidget::InputSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<InputSettingsWidget> InputSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<InputSettingsWidget>(new InputSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string InputSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_input_section_scroll_wheel");
        }

        std::string InputSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_input");
        }

        std::string InputSettingsWidget::getSettingsSortKey() const
        {
            return "X";
        }

        void InputSettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void InputSettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.scrollWheelSpeedComboBox, _getText(DJV_TEXT("settings_input_scroll_wheel_speed")) + ":");
                _widgetUpdate();
            }
        }

        void InputSettingsWidget::_widgetUpdate()
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
                p.scrollWheelSpeedComboBox->setItems(items);
                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                if (auto inputSettings = settingsSystem->getSettingsT<InputSettings>())
                {
                    p.scrollWheelSpeedComboBox->setCurrentItem(static_cast<int>(inputSettings->observeScrollWheelSpeed()->get()));
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

