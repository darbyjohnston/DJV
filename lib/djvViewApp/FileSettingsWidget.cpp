// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/FileSettingsWidget.h>

#include <djvViewApp/FileSettings.h>

#include <djvUIComponents/ShortcutsWidget.h>

#include <djvUI/CheckBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>
#include <djvUI/SettingsSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/Timer.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct SequenceSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckBox> autoDetectCheckBox;
            std::shared_ptr<UI::CheckBox> firstFrameCheckBox;
            std::shared_ptr<UI::FormLayout> layout;

            std::shared_ptr<Observer::Value<bool> > autoDetectObserver;
            std::shared_ptr<Observer::Value<bool> > firstFrameObserver;
        };

        void SequenceSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::SequenceSettingsWidget");

            p.autoDetectCheckBox = UI::CheckBox::create(context);
            p.firstFrameCheckBox = UI::CheckBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.autoDetectCheckBox);
            p.layout->addChild(p.firstFrameCheckBox);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.autoDetectCheckBox->setCheckedCallback(
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

            p.firstFrameCheckBox->setCheckedCallback(
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
                p.autoDetectObserver = Observer::Value<bool>::create(
                    fileSettings->observeAutoDetectSequences(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->autoDetectCheckBox->setChecked(value);
                        }
                    });

                p.firstFrameObserver = Observer::Value<bool>::create(
                    fileSettings->observeSequencesFirstFrame(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->firstFrameCheckBox->setChecked(value);
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

        void SequenceSettingsWidget::_initEvent(System::Event::Init& event)
        {
            IWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.autoDetectCheckBox, _getText(DJV_TEXT("settings_file_sequences_auto-detect")) + ":");
                p.layout->setText(p.firstFrameCheckBox, _getText(DJV_TEXT("settings_file_sequences_start_first_frame")) + ":");
            }
        }

        struct RecentFilesSettingsWidget::Private
        {
            std::shared_ptr<UI::Numeric::IntSlider> maxSlider;
            std::shared_ptr<UI::FormLayout> layout;

            std::shared_ptr<Observer::Value<size_t> > maxObserver;
        };

        void RecentFilesSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::RecentFilesSettingsWidget");

            p.maxSlider = UI::Numeric::IntSlider::create(context);
            p.maxSlider->setRange(Math::IntRange(5, 100));
            p.maxSlider->setDelay(System::getTimerDuration(System::TimerValue::Medium));

            p.layout = UI::FormLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.maxSlider);
            addChild(p.layout);

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto fileSettings = settingsSystem->getSettingsT<FileSettings>();
            auto weak = std::weak_ptr<RecentFilesSettingsWidget>(std::dynamic_pointer_cast<RecentFilesSettingsWidget>(shared_from_this()));
            p.maxObserver = Observer::Value<size_t>::create(
                fileSettings->observeRecentFilesMax(),
                [weak](size_t value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->maxSlider->setValue(value);
                    }
                });

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.maxSlider->setValueCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        auto fileSettings = settingsSystem->getSettingsT<FileSettings>();
                        fileSettings->setRecentFilesMax(value);
                    }
                });
        }

        RecentFilesSettingsWidget::RecentFilesSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<RecentFilesSettingsWidget> RecentFilesSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<RecentFilesSettingsWidget>(new RecentFilesSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string RecentFilesSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_file_recent_files");
        }

        std::string RecentFilesSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_file");
        }

        std::string RecentFilesSettingsWidget::getSettingsSortKey() const
        {
            return "A";
        }

        void RecentFilesSettingsWidget::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.maxSlider, _getText(DJV_TEXT("settings_file_recent_files_max")) + ":");
            }
        }

    } // namespace ViewApp
} // namespace djv

