// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/NUXSettingsWidget.h>

#include <djvViewApp/NUXSettings.h>

#include <djvUI/CheckBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct NUXSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckBox> checkBox;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<Observer::Value<bool> > nuxObserver;
        };

        void NUXSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::NUXSettingsWidget");

            p.checkBox = UI::CheckBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->addChild(p.checkBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<NUXSettingsWidget>(std::dynamic_pointer_cast<NUXSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.checkBox->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>())
                            {
                                if (auto nuxSettings = settingsSystem->getSettingsT<NUXSettings>())
                                {
                                    nuxSettings->setNUX(value);
                                }
                            }
                        }
                    }
                });

            if (auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>())
            {
                if (auto nuxSettings = settingsSystem->getSettingsT<NUXSettings>())
                {
                    p.nuxObserver = Observer::Value<bool>::create(
                        nuxSettings->observeNUX(),
                        [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->checkBox->setChecked(value);
                        }
                    });
                }
            }
        }

        NUXSettingsWidget::NUXSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<NUXSettingsWidget> NUXSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<NUXSettingsWidget>(new NUXSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string NUXSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_general");
        }

        std::string NUXSettingsWidget::getSettingsSortKey() const
        {
            return "0";
        }

        void NUXSettingsWidget::_initEvent(System::Event::Init & event)
        {
            IWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.checkBox, _getText(DJV_TEXT("settings_new_user_ux")) + ":");
            }
        }

    } // namespace ViewApp
} // namespace djv

