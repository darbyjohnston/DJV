// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/NUXSettingsWidget.h>

#include <djvViewApp/NUXSettings.h>

#include <djvUI/CheckBox.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct NUXSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckBox> nuxCheckBox;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<bool> > nuxObserver;
        };

        void NUXSettingsWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::NUXSettingsWidget");

            p.nuxCheckBox = UI::CheckBox::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->addChild(p.nuxCheckBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<NUXSettingsWidget>(std::dynamic_pointer_cast<NUXSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.nuxCheckBox->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
                            {
                                if (auto nuxSettings = settingsSystem->getSettingsT<NUXSettings>())
                                {
                                    nuxSettings->setNUX(value);
                                }
                            }
                        }
                    }
                });

            if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
            {
                if (auto nuxSettings = settingsSystem->getSettingsT<NUXSettings>())
                {
                    p.nuxObserver = ValueObserver<bool>::create(
                        nuxSettings->observeNUX(),
                        [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->nuxCheckBox->setChecked(value);
                        }
                    });
                }
            }
        }

        NUXSettingsWidget::NUXSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<NUXSettingsWidget> NUXSettingsWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<NUXSettingsWidget>(new NUXSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string NUXSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_general_section_nux");
        }

        std::string NUXSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_general");
        }

        std::string NUXSettingsWidget::getSettingsSortKey() const
        {
            return "0";
        }

        void NUXSettingsWidget::_initEvent(Event::Init & event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.nuxCheckBox->setText(_getText(DJV_TEXT("settings_new_user_ux_startup")));
            }
        }

    } // namespace ViewApp
} // namespace djv

