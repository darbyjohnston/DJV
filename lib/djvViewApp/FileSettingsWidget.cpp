// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/FileSettingsWidget.h>

#include <djvViewApp/FileSettings.h>

#include <djvUI/FormLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToggleButton.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct SequenceSettingsWidget::Private
        {
            std::shared_ptr<UI::ToggleButton> autoDetectButton;
            std::shared_ptr<UI::ToggleButton> firstFrameButton;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<ValueObserver<bool> > autoDetectObserver;
            std::shared_ptr<ValueObserver<bool> > firstFrameObserver;
        };

        void SequenceSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::SequenceSettingsWidget");

            p.autoDetectButton = UI::ToggleButton::create(context);
            p.firstFrameButton = UI::ToggleButton::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.autoDetectButton);
            p.layout->addChild(p.firstFrameButton);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.autoDetectButton->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                        {
                            fileSettings->setAutoDetectSequences(value);
                        }
                    }
                });

            p.firstFrameButton->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                        {
                            fileSettings->setSequencesFirstFrame(value);
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
            {
                auto weak = std::weak_ptr<SequenceSettingsWidget>(std::dynamic_pointer_cast<SequenceSettingsWidget>(shared_from_this()));
                p.autoDetectObserver = ValueObserver<bool>::create(
                    fileSettings->observeAutoDetectSequences(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->autoDetectButton->setChecked(value);
                        }
                    });

                p.firstFrameObserver = ValueObserver<bool>::create(
                    fileSettings->observeSequencesFirstFrame(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->firstFrameButton->setChecked(value);
                        }
                    });
            }
        }

        SequenceSettingsWidget::SequenceSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<SequenceSettingsWidget> SequenceSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<SequenceSettingsWidget>(new SequenceSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string SequenceSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_file_sequences");
        }

        std::string SequenceSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_file");
        }

        std::string SequenceSettingsWidget::getSettingsSortKey() const
        {
            return "A";
        }

        void SequenceSettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void SequenceSettingsWidget::_initEvent(System::Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.autoDetectButton, _getText(DJV_TEXT("settings_file_sequences_auto-detect")) + ":");
                p.layout->setText(p.firstFrameButton, _getText(DJV_TEXT("settings_file_sequences_start_first_frame")) + ":");
            }
        }

    } // namespace ViewApp
} // namespace djv

