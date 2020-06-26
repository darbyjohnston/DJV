// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/InputSettingsWidget.h>

#include <djvViewApp/InputSettings.h>

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
        struct ScrollWheelSettingsWidget::Private
        {
            std::shared_ptr<UI::ComboBox> scrollWheelSpeedComboBox;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<ValueObserver<ScrollWheelSpeed> > scrollWheelSpeedObserver;
        };

        void ScrollWheelSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ScrollWheelSettingsWidget");

            p.scrollWheelSpeedComboBox = UI::ComboBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->addChild(p.scrollWheelSpeedComboBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<ScrollWheelSettingsWidget>(std::dynamic_pointer_cast<ScrollWheelSettingsWidget>(shared_from_this()));
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

        ScrollWheelSettingsWidget::ScrollWheelSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ScrollWheelSettingsWidget> ScrollWheelSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ScrollWheelSettingsWidget>(new ScrollWheelSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ScrollWheelSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_input_section_scroll_wheel");
        }

        std::string ScrollWheelSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_input");
        }

        std::string ScrollWheelSettingsWidget::getSettingsSortKey() const
        {
            return "Z";
        }

        void ScrollWheelSettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void ScrollWheelSettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.scrollWheelSpeedComboBox, _getText(DJV_TEXT("settings_input_scroll_wheel_speed")) + ":");
                _widgetUpdate();
            }
        }

        void ScrollWheelSettingsWidget::_widgetUpdate()
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

        struct ShortcutsSettingsWidget::Private
        {
            std::shared_ptr<UI::ShortcutsWidget> widget;
            std::shared_ptr<MapObserver<std::string, UI::ShortcutDataPair> > shortcutsObserver;
        };

        void ShortcutsSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ShortcutsSettingsWidget");

            p.widget = UI::ShortcutsWidget::create(context);
            addChild(p.widget);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.widget->setShortcutsCallback(
                [contextWeak](const UI::ShortcutDataMap& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto inputSettings = settingsSystem->getSettingsT<InputSettings>();
                        inputSettings->setShortcuts(value);
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto inputSettings = settingsSystem->getSettingsT<InputSettings>();
            auto weak = std::weak_ptr<ShortcutsSettingsWidget>(std::dynamic_pointer_cast<ShortcutsSettingsWidget>(shared_from_this()));
            p.shortcutsObserver = MapObserver<std::string, UI::ShortcutDataPair>::create(
                inputSettings->observeShortcuts(),
                [weak](const std::map<std::string, UI::ShortcutDataPair>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->widget->setShortcuts(value);
                    }
                });
        }

        ShortcutsSettingsWidget::ShortcutsSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ShortcutsSettingsWidget> ShortcutsSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ShortcutsSettingsWidget>(new ShortcutsSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ShortcutsSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_input_section_shortcuts");
        }

        std::string ShortcutsSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_input");
        }

        std::string ShortcutsSettingsWidget::getSettingsSortKey() const
        {
            return "Z";
        }

    } // namespace ViewApp
} // namespace djv

