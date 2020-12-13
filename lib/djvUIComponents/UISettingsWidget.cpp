// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/UISettingsWidget.h>

#include <djvUI/CheckBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/UISettings.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            struct TooltipsWidget::Private
            {
                std::shared_ptr<UI::CheckBox> tooltipsCheckBox;
                std::shared_ptr<UI::FormLayout> layout;
                std::shared_ptr<Observer::Value<bool> > tooltipsObserver;
            };

            void TooltipsWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                IWidget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UIComponents::Settings::TooltipsWidget");

                p.tooltipsCheckBox = UI::CheckBox::create(context);

                p.layout = UI::FormLayout::create(context);
                p.layout->addChild(p.tooltipsCheckBox);
                addChild(p.layout);

                auto weak = std::weak_ptr<TooltipsWidget>(std::dynamic_pointer_cast<TooltipsWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.tooltipsCheckBox->setCheckedCallback(
                    [weak, contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                                auto uiSettings = settingsSystem->getSettingsT<UI::Settings::UI>();
                                uiSettings->setTooltips(value);
                            }
                        }
                    });

                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                auto uiSettings = settingsSystem->getSettingsT<UI::Settings::UI>();
                p.tooltipsObserver = Observer::Value<bool>::create(
                    uiSettings->observeTooltips(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->tooltipsCheckBox->setChecked(value);
                        }
                    });
            }

            TooltipsWidget::TooltipsWidget() :
                _p(new Private)
            {}

            std::shared_ptr<TooltipsWidget> TooltipsWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<TooltipsWidget>(new TooltipsWidget);
                out->_init(context);
                return out;
            }

            std::string TooltipsWidget::getSettingsGroup() const
            {
                return DJV_TEXT("settings_title_general");
            }

            std::string TooltipsWidget::getSettingsSortKey() const
            {
                return "0";
            }

            void TooltipsWidget::_initEvent(System::Event::Init& event)
            {
                IWidget::_initEvent(event);
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.layout->setText(p.tooltipsCheckBox, _getText(DJV_TEXT("settings_general_tooltips")) + ":");
                }
            }

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

