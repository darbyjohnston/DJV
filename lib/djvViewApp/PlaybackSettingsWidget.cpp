// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/PlaybackSettingsWidget.h>

#include <djvViewApp/PlaybackSettings.h>

#include <djvUI/CheckBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvAV/AVSystem.h>
#include <djvAV/Speed.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct PlaybackSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckBox> startPlaybackCheckBox;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<Observer::Value<bool> > startPlaybackObserver;
        };

        void PlaybackSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::PlaybackSettingsWidget");

            p.startPlaybackCheckBox = UI::CheckBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->addChild(p.startPlaybackCheckBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<PlaybackSettingsWidget>(std::dynamic_pointer_cast<PlaybackSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.startPlaybackCheckBox->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                            {
                                playbackSettings->setStartPlayback(value);
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
            {
                p.startPlaybackObserver = Observer::Value<bool>::create(
                    playbackSettings->observeStartPlayback(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->startPlaybackCheckBox->setChecked(value);
                        }
                    });
            }
        }

        PlaybackSettingsWidget::PlaybackSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<PlaybackSettingsWidget> PlaybackSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<PlaybackSettingsWidget>(new PlaybackSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string PlaybackSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_playback");
        }

        std::string PlaybackSettingsWidget::getSettingsSortKey() const
        {
            return "G";
        }

        void PlaybackSettingsWidget::_initEvent(System::Event::Init& event)
        {
            IWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.startPlaybackCheckBox, _getText(DJV_TEXT("settings_playback_start_playback")) + ":");
            }
        }

        struct TimelineSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckBox> pipEnabledCheckBox;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<Observer::Value<bool> > pipEnabledObserver;
        };

        void TimelineSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::TimelineSettingsWidget");

            p.pipEnabledCheckBox = UI::CheckBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->addChild(p.pipEnabledCheckBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<TimelineSettingsWidget>(std::dynamic_pointer_cast<TimelineSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.pipEnabledCheckBox->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                            {
                                playbackSettings->setPIPEnabled(value);
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
            {
                p.pipEnabledObserver = Observer::Value<bool>::create(
                    playbackSettings->observePIPEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->pipEnabledCheckBox->setChecked(value);
                        }
                    });
            }
        }

        TimelineSettingsWidget::TimelineSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<TimelineSettingsWidget> TimelineSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<TimelineSettingsWidget>(new TimelineSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string TimelineSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_playback");
        }

        std::string TimelineSettingsWidget::getSettingsSortKey() const
        {
            return "G";
        }

        void TimelineSettingsWidget::_initEvent(System::Event::Init& event)
        {
            IWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.pipEnabledCheckBox, _getText(DJV_TEXT("settings_timeline_pip")) + ":");
            }
        }

    } // namespace ViewApp
} // namespace djv

