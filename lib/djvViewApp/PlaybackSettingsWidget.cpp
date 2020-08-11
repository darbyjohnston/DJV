// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/PlaybackSettingsWidget.h>

#include <djvViewApp/PlaybackSettings.h>

#include <djvUI/CheckBox.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Speed.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct PlaybackSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckBox> startPlaybackButton;
            std::shared_ptr<ValueObserver<bool> > startPlaybackObserver;
        };

        void PlaybackSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::PlaybackSettingsWidget");

            p.startPlaybackButton = UI::CheckBox::create(context);

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::MetricsRole::None);
            layout->addChild(p.startPlaybackButton);
            addChild(layout);

            auto weak = std::weak_ptr<PlaybackSettingsWidget>(std::dynamic_pointer_cast<PlaybackSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.startPlaybackButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                            {
                                playbackSettings->setStartPlayback(value);
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
            {
                p.startPlaybackObserver = ValueObserver<bool>::create(
                    playbackSettings->observeStartPlayback(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->startPlaybackButton->setChecked(value);
                        }
                    });
            }
        }

        PlaybackSettingsWidget::PlaybackSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<PlaybackSettingsWidget> PlaybackSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<PlaybackSettingsWidget>(new PlaybackSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string PlaybackSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_playback_section_playback");
        }

        std::string PlaybackSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_playback");
        }

        std::string PlaybackSettingsWidget::getSettingsSortKey() const
        {
            return "G";
        }

        void PlaybackSettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.startPlaybackButton->setText(_getText(DJV_TEXT("automatically_start_playback")));
            }
        }

        struct TimelineSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckBox> pipEnabledButton;
            std::shared_ptr<ValueObserver<bool> > pipEnabledObserver;
        };

        void TimelineSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::TimelineSettingsWidget");

            p.pipEnabledButton = UI::CheckBox::create(context);

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::MetricsRole::None);
            layout->addChild(p.pipEnabledButton);
            addChild(layout);

            auto weak = std::weak_ptr<TimelineSettingsWidget>(std::dynamic_pointer_cast<TimelineSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.pipEnabledButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                            {
                                playbackSettings->setPIPEnabled(value);
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
            {
                p.pipEnabledObserver = ValueObserver<bool>::create(
                    playbackSettings->observePIPEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->pipEnabledButton->setChecked(value);
                        }
                    });
            }
        }

        TimelineSettingsWidget::TimelineSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<TimelineSettingsWidget> TimelineSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TimelineSettingsWidget>(new TimelineSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string TimelineSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_playback_section_timeline");
        }

        std::string TimelineSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_playback");
        }

        std::string TimelineSettingsWidget::getSettingsSortKey() const
        {
            return "G";
        }

        void TimelineSettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.pipEnabledButton->setText(_getText(DJV_TEXT("show_pip_picture_in_picture")));
            }
        }

    } // namespace ViewApp
} // namespace djv

