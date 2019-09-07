//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvViewApp/PlaybackSettingsWidget.h>

#include <djvViewApp/PlaybackSettings.h>

#include <djvUI/CheckBox.h>
#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
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
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<bool> > startPlaybackObserver;
        };

        void PlaybackSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::PlaybackSettingsWidget");

            p.startPlaybackButton = UI::CheckBox::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->addChild(p.startPlaybackButton);
            addChild(p.layout);

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
            return DJV_TEXT("Playback");
        }

        std::string PlaybackSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("Playback");
        }

        std::string PlaybackSettingsWidget::getSettingsSortKey() const
        {
            return "G";
        }

        void PlaybackSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.startPlaybackButton->setText(_getText(DJV_TEXT("Automatically start playback")));
        }

        struct PlaybackSpeedSettingsWidget::Private
        {
            std::shared_ptr<UI::ComboBox> defaultSpeedComboBox;
            std::shared_ptr<UI::CheckBox> playEveryFrameButton;
            std::shared_ptr<UI::FormLayout> formLayout;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<Time::FPS> > defaultSpeedObserver;
            std::shared_ptr<ValueObserver<bool> > playEveryFrameObserver;
        };

        void PlaybackSpeedSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::PlaybackSpeedSettingsWidget");

            p.defaultSpeedComboBox = UI::ComboBox::create(context);
            p.playEveryFrameButton = UI::CheckBox::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->addChild(p.defaultSpeedComboBox);
            p.layout->addChild(p.formLayout);
            p.layout->addChild(p.playEveryFrameButton);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<PlaybackSpeedSettingsWidget>(std::dynamic_pointer_cast<PlaybackSpeedSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.defaultSpeedComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto avSystem = context->getSystemT<AV::AVSystem>())
                            {
                                avSystem->setDefaultSpeed(static_cast<Time::FPS>(value));
                            }
                        }
                    }
                });

            p.playEveryFrameButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                            {
                                playbackSettings->setPlayEveryFrame(value);
                            }
                        }
                    }
                });

            auto avSystem = context->getSystemT<AV::AVSystem>();
            p.defaultSpeedObserver = ValueObserver<Time::FPS>::create(
                avSystem->observeDefaultSpeed(),
                [weak](Time::FPS value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->defaultSpeedComboBox->setCurrentItem(static_cast<int>(value));
                        widget->_widgetUpdate();
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
            {
                p.playEveryFrameObserver = ValueObserver<bool>::create(
                    playbackSettings->observePlayEveryFrame(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->playEveryFrameButton->setChecked(value);
                        }
                    });
            }
        }

        PlaybackSpeedSettingsWidget::PlaybackSpeedSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<PlaybackSpeedSettingsWidget> PlaybackSpeedSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<PlaybackSpeedSettingsWidget>(new PlaybackSpeedSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string PlaybackSpeedSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Playback Speed");
        }

        std::string PlaybackSpeedSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("Playback");
        }

        std::string PlaybackSpeedSettingsWidget::getSettingsSortKey() const
        {
            return "G";
        }

        void PlaybackSpeedSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.playEveryFrameButton->setText(_getText(DJV_TEXT("Playback every frame")));
            p.formLayout->setText(p.defaultSpeedComboBox, _getText(DJV_TEXT("Default speed")));
        }

        void PlaybackSpeedSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            const int currentItem = p.defaultSpeedComboBox->getCurrentItem();
            p.defaultSpeedComboBox->clearItems();
            for (const auto& i : Time::getFPSEnums())
            {
                std::stringstream ss;
                ss << i;
                p.defaultSpeedComboBox->addItem(ss.str());
            }
            p.defaultSpeedComboBox->setCurrentItem(currentItem);
        }

        struct TimelineSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckBox> pipButton;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<bool> > pipObserver;
        };

        void TimelineSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::TimelineSettingsWidget");

            p.pipButton = UI::CheckBox::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->addChild(p.pipButton);
            addChild(p.layout);

            auto weak = std::weak_ptr<TimelineSettingsWidget>(std::dynamic_pointer_cast<TimelineSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.pipButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                            {
                                playbackSettings->setPIP(value);
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
            {
                p.pipObserver = ValueObserver<bool>::create(
                    playbackSettings->observePIP(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->pipButton->setChecked(value);
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
            return DJV_TEXT("Timeline");
        }

        std::string TimelineSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("Playback");
        }

        std::string TimelineSettingsWidget::getSettingsSortKey() const
        {
            return "G";
        }

        void TimelineSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.pipButton->setText(_getText(DJV_TEXT("Show PIP (picture in picture)")));
        }

    } // namespace ViewApp
} // namespace djv

