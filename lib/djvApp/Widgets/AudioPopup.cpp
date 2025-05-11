// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/AudioPopup.h>

#include <djvApp/Models/AudioModel.h>
#include <djvApp/App.h>

#include <dtk/ui/IntEditSlider.h>
#include <dtk/ui/RowLayout.h>

namespace djv
{
    namespace app
    {
        struct AudioPopup::Private
        {
            std::shared_ptr<dtk::IntEditSlider> volumeSlider;

            std::shared_ptr<dtk::ValueObserver<float> > volumeObserver;
        };

        void AudioPopup::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidgetPopup::_init(
                context,
                "djv::app::AudioPopup",
                parent);
            DTK_P();

            p.volumeSlider = dtk::IntEditSlider::create(context);
            p.volumeSlider->setRange(dtk::RangeI(0, 100));
            p.volumeSlider->setStep(1);
            p.volumeSlider->setLargeStep(10);
            p.volumeSlider->setTooltip("Audio volume");

            auto layout = dtk::VerticalLayout::create(context);
            layout->setMarginRole(dtk::SizeRole::MarginSmall);
            layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.volumeSlider->setParent(layout);
            setWidget(layout);

            auto appWeak = std::weak_ptr<App>(app);
            p.volumeSlider->setCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getAudioModel()->setVolume(value / 100.F);
                    }
                });

            p.volumeObserver = dtk::ValueObserver<float>::create(
                app->getAudioModel()->observeVolume(),
                [this](float value)
                {
                    _p->volumeSlider->setValue(std::roundf(value * 100.F));
                });
        }

        AudioPopup::AudioPopup() :
            _p(new Private)
        {}

        AudioPopup::~AudioPopup()
        {}

        std::shared_ptr<AudioPopup> AudioPopup::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<AudioPopup>(new AudioPopup);
            out->_init(context, app, parent);
            return out;
        }
    }
}
