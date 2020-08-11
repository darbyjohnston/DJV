// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MouseSettingsWidget.h>

#include <djvViewApp/MouseSettings.h>

#include <djvUIComponents/ShortcutsWidget.h>

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
        struct MouseScrollWheelSettingsWidget::Private
        {
            std::shared_ptr<UI::ComboBox> scrollWheelSpeedComboBox;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<ValueObserver<ScrollWheelSpeed> > scrollWheelSpeedObserver;
        };

        void MouseScrollWheelSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MouseScrollWheelSettingsWidget");

            p.scrollWheelSpeedComboBox = UI::ComboBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->addChild(p.scrollWheelSpeedComboBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<MouseScrollWheelSettingsWidget>(std::dynamic_pointer_cast<MouseScrollWheelSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.scrollWheelSpeedComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto mouseSettings = settingsSystem->getSettingsT<MouseSettings>())
                            {
                                mouseSettings->setScrollWheelSpeed(static_cast<ScrollWheelSpeed>(value));
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto mouseSettings = settingsSystem->getSettingsT<MouseSettings>())
            {
                p.scrollWheelSpeedObserver = ValueObserver<ScrollWheelSpeed>::create(
                    mouseSettings->observeScrollWheelSpeed(),
                    [weak](ScrollWheelSpeed value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->scrollWheelSpeedComboBox->setCurrentItem(static_cast<int>(value));
                        }
                    });
            }
        }

        MouseScrollWheelSettingsWidget::MouseScrollWheelSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<MouseScrollWheelSettingsWidget> MouseScrollWheelSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<MouseScrollWheelSettingsWidget>(new MouseScrollWheelSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string MouseScrollWheelSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_mouse_section_scroll_wheel");
        }

        std::string MouseScrollWheelSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_mouse");
        }

        std::string MouseScrollWheelSettingsWidget::getSettingsSortKey() const
        {
            return "ZZ";
        }

        void MouseScrollWheelSettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void MouseScrollWheelSettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.scrollWheelSpeedComboBox, _getText(DJV_TEXT("settings_mouse_scroll_wheel_speed")) + ":");
                _widgetUpdate();
            }
        }

        void MouseScrollWheelSettingsWidget::_widgetUpdate()
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
                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                auto mouseSettings = settingsSystem->getSettingsT<MouseSettings>();
                p.scrollWheelSpeedComboBox->setItems(items);
                p.scrollWheelSpeedComboBox->setCurrentItem(static_cast<int>(mouseSettings->observeScrollWheelSpeed()->get()));
            }
        }

    } // namespace ViewApp
} // namespace djv

