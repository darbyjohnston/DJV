// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/AudioPopup.h>

#include <djvApp/Models/AudioModel.h>
#include <djvApp/App.h>

#include <feather-tk/ui/IntEditSlider.h>
#include <feather-tk/ui/RowLayout.h>

namespace djv
{
    namespace app
    {
        struct AudioPopup::Private
        {
            std::shared_ptr<feather_tk::IntEditSlider> volumeSlider;

            std::shared_ptr<feather_tk::ValueObserver<float> > volumeObserver;
        };

        void AudioPopup::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidgetPopup::_init(
                context,
                "djv::app::AudioPopup",
                parent);
            FEATHER_TK_P();

            p.volumeSlider = feather_tk::IntEditSlider::create(context);
            p.volumeSlider->setRange(0, 100);
            p.volumeSlider->setStep(1);
            p.volumeSlider->setLargeStep(10);
            p.volumeSlider->setTooltip("Audio volume");

            auto layout = feather_tk::VerticalLayout::create(context);
            layout->setMarginRole(feather_tk::SizeRole::MarginSmall);
            layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
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

            p.volumeObserver = feather_tk::ValueObserver<float>::create(
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
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<AudioPopup>(new AudioPopup);
            out->_init(context, app, parent);
            return out;
        }
    }
}
