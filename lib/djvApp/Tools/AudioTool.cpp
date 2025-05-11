// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/AudioTool.h>

#include <djvApp/Models/AudioModel.h>
#include <djvApp/App.h>

#include <dtk/ui/Bellows.h>
#include <dtk/ui/ButtonGroup.h>
#include <dtk/ui/CheckBox.h>
#include <dtk/ui/ComboBox.h>
#include <dtk/ui/DoubleEditSlider.h>
#include <dtk/ui/FormLayout.h>
#include <dtk/ui/IntEditSlider.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>
#include <dtk/core/Format.h>

namespace djv
{
    namespace app
    {
        struct AudioTool::Private
        {
            std::vector<tl::audio::DeviceID> devices;
            tl::audio::Info info;
            std::vector<bool> channelMute;

            std::shared_ptr<dtk::ComboBox> deviceComboBox;
            std::shared_ptr<dtk::IntEditSlider> volumeSlider;
            std::shared_ptr<dtk::CheckBox> muteCheckBox;
            std::vector<std::shared_ptr<dtk::CheckBox> > channelMuteCheckBoxes;
            std::shared_ptr<dtk::ButtonGroup> channelMuteButtonGroup;
            std::shared_ptr<dtk::DoubleEditSlider> syncOffsetSlider;

            std::shared_ptr<dtk::HorizontalLayout> channelMuteLayout;

            std::shared_ptr<dtk::ListObserver<tl::audio::DeviceID> > devicesObserver;
            std::shared_ptr<dtk::ValueObserver<tl::audio::DeviceID> > deviceObserver;
            std::shared_ptr<dtk::ValueObserver<float> > volumeObserver;
            std::shared_ptr<dtk::ValueObserver<bool> > muteObserver;
            std::shared_ptr<dtk::ValueObserver<std::shared_ptr<tl::timeline::Player> > > playerObserver;
            std::shared_ptr<dtk::ListObserver<bool> > channelMuteObserver;
            std::shared_ptr<dtk::ValueObserver<double> > syncOffsetObserver;
        };

        void AudioTool::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::Audio,
                "djv::app::AudioTool",
                parent);
            DTK_P();

            p.deviceComboBox = dtk::ComboBox::create(context);
            p.deviceComboBox->setTooltip("Audio output device");

            p.volumeSlider = dtk::IntEditSlider::create(context);
            p.volumeSlider->setRange(dtk::RangeI(0, 100));
            p.volumeSlider->setStep(1);
            p.volumeSlider->setLargeStep(10);

            p.muteCheckBox = dtk::CheckBox::create(context);

            p.channelMuteButtonGroup = dtk::ButtonGroup::create(context, dtk::ButtonGroupType::Toggle);

            p.syncOffsetSlider = dtk::DoubleEditSlider::create(context);
            p.syncOffsetSlider->setRange(dtk::RangeD(-1.0, 1.0));
            p.syncOffsetSlider->setDefaultValue(0.0);

            auto formLayout = dtk::FormLayout::create(context);
            formLayout->setMarginRole(dtk::SizeRole::MarginSmall);
            formLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            formLayout->addRow("Device:", p.deviceComboBox);
            formLayout->addRow("Volume:", p.volumeSlider);
            formLayout->addRow("Mute:", p.muteCheckBox);
            p.channelMuteLayout = dtk::HorizontalLayout::create(context);
            p.channelMuteLayout->setSpacingRole(dtk::SizeRole::SpacingTool);
            formLayout->addRow("Channel mute:", p.channelMuteLayout);
            formLayout->addRow("Sync offset:", p.syncOffsetSlider);

            auto scrollWidget = dtk::ScrollWidget::create(context);
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

            p.devicesObserver = dtk::ListObserver<tl::audio::DeviceID>::create(
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

            p.deviceObserver = dtk::ValueObserver<tl::audio::DeviceID>::create(
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

            p.volumeObserver = dtk::ValueObserver<float>::create(
                app->getAudioModel()->observeVolume(),
                [this](float value)
                {
                    _p->volumeSlider->setValue(std::roundf(value * 100.F));
                });

            p.muteObserver = dtk::ValueObserver<bool>::create(
                app->getAudioModel()->observeMute(),
                [this](bool value)
                {
                    _p->muteCheckBox->setChecked(value);
                });

            p.playerObserver = dtk::ValueObserver<std::shared_ptr<tl::timeline::Player> >::create(
                app->observePlayer(),
                [this](const std::shared_ptr<tl::timeline::Player>& value)
                {
                    _p->info = value ? value->getIOInfo().audio : tl::audio::Info();
                    _widgetUpdate();
                });

            p.channelMuteObserver = dtk::ListObserver<bool>::create(
                app->getAudioModel()->observeChannelMute(),
                [this](const std::vector<bool>& value)
                {
                    _p->channelMute = value;
                    _widgetUpdate();
                });

            p.syncOffsetObserver = dtk::ValueObserver<double>::create(
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
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<AudioTool>(new AudioTool);
            out->_init(context, app, parent);
            return out;
        }

        void AudioTool::_widgetUpdate()
        {
            DTK_P();
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
                        auto checkBox = dtk::CheckBox::create(
                            context,
                            dtk::Format("{0}").arg(i),
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
