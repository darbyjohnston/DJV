// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/TooltipsSettingsWidget.h>

#include <djvUI/CheckBox.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/UISettings.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct TooltipsSettingsWidget::Private
        {
            std::shared_ptr<CheckBox> tooltipsCheckBox;
            std::shared_ptr<VerticalLayout> layout;
            std::shared_ptr<ValueObserver<bool> > tooltipsObserver;
        };

        void TooltipsSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::TooltipsSettingsWidget");

            p.tooltipsCheckBox = CheckBox::create(context);

            p.layout = VerticalLayout::create(context);
            p.layout->addChild(p.tooltipsCheckBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<TooltipsSettingsWidget>(std::dynamic_pointer_cast<TooltipsSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.tooltipsCheckBox->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto uiSettings = settingsSystem->getSettingsT<Settings::UI>();
                            uiSettings->setTooltips(value);
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto uiSettings = settingsSystem->getSettingsT<Settings::UI>();
            p.tooltipsObserver = ValueObserver<bool>::create(
                uiSettings->observeTooltips(),
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->tooltipsCheckBox->setChecked(value);
                }
            });
        }

        TooltipsSettingsWidget::TooltipsSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<TooltipsSettingsWidget> TooltipsSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TooltipsSettingsWidget>(new TooltipsSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string TooltipsSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_general_section_tooltips");
        }

        std::string TooltipsSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_general");
        }

        std::string TooltipsSettingsWidget::getSettingsSortKey() const
        {
            return "0";
        }

        void TooltipsSettingsWidget::_initEvent(Event::Init & event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.tooltipsCheckBox->setText(_getText(DJV_TEXT("settings_general_enable_tooltips")));
            }
        }

    } // namespace UI
} // namespace djv

