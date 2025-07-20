// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/AudioTool.h>

#include <djvApp/Models/AudioModel.h>
#include <djvApp/App.h>

#include <feather-tk/ui/Bellows.h>
#include <feather-tk/ui/ButtonGroup.h>
#include <feather-tk/ui/CheckBox.h>
#include <feather-tk/ui/ComboBox.h>
#include <feather-tk/ui/DoubleEditSlider.h>
#include <feather-tk/ui/FormLayout.h>
#include <feather-tk/ui/IntEditSlider.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/ScrollWidget.h>
#include <feather-tk/core/Format.h>

namespace djv
{
    namespace app
    {
        struct AudioTool::Private
        {
            std::vector<tl::audio::DeviceID> devices;
            tl::audio::Info info;
            std::vector<bool> channelMute;

            std::shared_ptr<feather_tk::ComboBox> deviceComboBox;
            std::shared_ptr<feather_tk::IntEditSlider> volumeSlider;
            std::shared_ptr<feather_tk::CheckBox> muteCheckBox;
            std::vector<std::shared_ptr<feather_tk::CheckBox> > channelMuteCheckBoxes;
            std::shared_ptr<feather_tk::ButtonGroup> channelMuteButtonGroup;
            std::shared_ptr<feather_tk::DoubleEditSlider> syncOffsetSlider;

            std::shared_ptr<feather_tk::HorizontalLayout> channelMuteLayout;

            std::shared_ptr<feather_tk::ListObserver<tl::audio::DeviceID> > devicesObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::audio::DeviceID> > deviceObserver;
            std::shared_ptr<feather_tk::ValueObserver<float> > volumeObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > muteObserver;
            std::shared_ptr<feather_tk::ValueObserver<std::shared_ptr<tl::timeline::Player> > > playerObserver;
            std::shared_ptr<feather_tk::ListObserver<bool> > channelMuteObserver;
            std::shared_ptr<feather_tk::ValueObserver<double> > syncOffsetObserver;
        };

        void AudioTool::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::Audio,
                "djv::app::AudioTool",
                parent);
            FEATHER_TK_P();

            p.deviceComboBox = feather_tk::ComboBox::create(context);
            p.deviceComboBox->setTooltip("Audio output device");

            p.volumeSlider = feather_tk::IntEditSlider::create(context);
            p.volumeSlider->setRange(0, 100);
            p.volumeSlider->setStep(1);
            p.volumeSlider->setLargeStep(10);

            p.muteCheckBox = feather_tk::CheckBox::create(context);

            p.channelMuteButtonGroup = feather_tk::ButtonGroup::create(context, feather_tk::ButtonGroupType::Toggle);

            p.syncOffsetSlider = feather_tk::DoubleEditSlider::create(context);
            p.syncOffsetSlider->setRange(-1.0, 1.0);
            p.syncOffsetSlider->setDefaultValue(0.0);

            auto formLayout = feather_tk::FormLayout::create(context);
            formLayout->setMarginRole(feather_tk::SizeRole::MarginSmall);
            formLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            formLayout->addRow("Device:", p.deviceComboBox);
            formLayout->addRow("Volume:", p.volumeSlider);
            formLayout->addRow("Mute:", p.muteCheckBox);
            p.channelMuteLayout = feather_tk::HorizontalLayout::create(context);
            p.channelMuteLayout->setSpacingRole(feather_tk::SizeRole::SpacingTool);
            formLayout->addRow("Channel mute:", p.channelMuteLayout);
            formLayout->addRow("Sync offset:", p.syncOffsetSlider);

            auto scrollWidget = feather_tk::ScrollWidget::create(context);
            scrollWidget->setBorder(false);
            scrollWidget->setWidget(formLayout);
            _setWidget(scrollWidget);

            auto appWeak = std::weak_ptr<App>(app);
            p.deviceComboBox->setIndexCallback(
                [this, appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        if (value >= 0 && value < _p->devices.size())
                        {
                            app->getAudioModel()->setDevice(
                                0 == value ? tl::audio::DeviceID() : _p->devices[value]);
                        }
                    }
                });

            p.volumeSlider->setCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getAudioModel()->setVolume(value / 100.F);
                    }
                });

            p.muteCheckBox->setCheckedCallback(
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getAudioModel()->setMute(value);
                    }
                });

            p.channelMuteButtonGroup->setCheckedCallback(
                [this, appWeak](int index, bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        std::vector<bool> channelMute = _p->channelMute;
                        if (index >= channelMute.size())
                        {
                            channelMute.resize(index + 1);
                        }
                        channelMute[index] = value;
                        app->getAudioModel()->setChannelMute(channelMute);
                    }
                });

            p.syncOffsetSlider->setCallback(
                [appWeak](double value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getAudioModel()->setSyncOffset(value);
                    }
                });

            p.devicesObserver = feather_tk::ListObserver<tl::audio::DeviceID>::create(
                app->getAudioModel()->observeDevices(),
                [this](const std::vector<tl::audio::DeviceID>& devices)
                {
                    _p->devices.clear();
                    _p->devices.push_back(tl::audio::DeviceID());
                    _p->devices.insert(_p->devices.end(), devices.begin(), devices.end());
                    std::vector<std::string> names;
                    names.push_back("Default");
                    for (const auto& device : devices)
                    {
                        names.push_back(device.name);
                    }
                    _p->deviceComboBox->setItems(names);
                });

            p.deviceObserver = feather_tk::ValueObserver<tl::audio::DeviceID>::create(
                app->getAudioModel()->observeDevice(),
                [this](const tl::audio::DeviceID& value)
                {
                    int index = 0;
                    const auto i = std::find(_p->devices.begin(), _p->devices.end(), value);
                    if (i != _p->devices.end())
                    {
                        index = i - _p->devices.begin();
                    }
                    _p->deviceComboBox->setCurrentIndex(index);
                });

            p.volumeObserver = feather_tk::ValueObserver<float>::create(
                app->getAudioModel()->observeVolume(),
                [this](float value)
                {
                    _p->volumeSlider->setValue(std::roundf(value * 100.F));
                });

            p.muteObserver = feather_tk::ValueObserver<bool>::create(
                app->getAudioModel()->observeMute(),
                [this](bool value)
                {
                    _p->muteCheckBox->setChecked(value);
                });

            p.playerObserver = feather_tk::ValueObserver<std::shared_ptr<tl::timeline::Player> >::create(
                app->observePlayer(),
                [this](const std::shared_ptr<tl::timeline::Player>& value)
                {
                    _p->info = value ? value->getIOInfo().audio : tl::audio::Info();
                    _widgetUpdate();
                });

            p.channelMuteObserver = feather_tk::ListObserver<bool>::create(
                app->getAudioModel()->observeChannelMute(),
                [this](const std::vector<bool>& value)
                {
                    _p->channelMute = value;
                    _widgetUpdate();
                });

            p.syncOffsetObserver = feather_tk::ValueObserver<double>::create(
                app->getAudioModel()->observeSyncOffset(),
                [this](double value)
                {
                    _p->syncOffsetSlider->setValue(value);
                });
        }

        AudioTool::AudioTool() :
            _p(new Private)
        {}

        AudioTool::~AudioTool()
        {}

        std::shared_ptr<AudioTool> AudioTool::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<AudioTool>(new AudioTool);
            out->_init(context, app, parent);
            return out;
        }

        void AudioTool::_widgetUpdate()
        {
            FEATHER_TK_P();
            if (p.channelMuteCheckBoxes.size() != p.info.channelCount)
            {
                for (const auto& checkBox : p.channelMuteCheckBoxes)
                {
                    checkBox->setParent(nullptr);
                }
                p.channelMuteCheckBoxes.clear();
                p.channelMuteButtonGroup->clearButtons();
                if (auto context = getContext())
                {
                    for (size_t i = 0; i < p.info.channelCount; ++i)
                    {
                        auto checkBox = feather_tk::CheckBox::create(
                            context,
                            feather_tk::Format("{0}").arg(i),
                            p.channelMuteLayout);
                        p.channelMuteCheckBoxes.push_back(checkBox);
                        p.channelMuteButtonGroup->addButton(checkBox);
                    }
                }
            }
            for (size_t i = 0; i < p.channelMute.size() && i < p.channelMuteCheckBoxes.size(); ++i)
            {
                p.channelMuteCheckBoxes[i]->setChecked(p.channelMute[i]);
            }
        }
    }
}
