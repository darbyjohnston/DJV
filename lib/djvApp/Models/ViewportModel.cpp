// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Models/ViewportModel.h>

#include <feather-tk/ui/Settings.h>

namespace djv
{
    namespace app
    {
        struct ViewportModel::Private
        {
            std::weak_ptr<feather_tk::Context> context;
            std::shared_ptr<feather_tk::Settings> settings;
            std::shared_ptr<feather_tk::ObservableValue<feather_tk::Color4F> > colorPicker;
            std::shared_ptr<feather_tk::ObservableValue<feather_tk::ImageOptions> > imageOptions;
            std::shared_ptr<feather_tk::ObservableValue<tl::timeline::DisplayOptions> > displayOptions;
            std::shared_ptr<feather_tk::ObservableValue<tl::timeline::BackgroundOptions> > backgroundOptions;
            std::shared_ptr<feather_tk::ObservableValue<tl::timeline::ForegroundOptions> > foregroundOptions;
            std::shared_ptr<feather_tk::ObservableValue<feather_tk::ImageType> > colorBuffer;
            std::shared_ptr<feather_tk::ObservableValue<bool> > hud;
        };

        void ViewportModel::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<feather_tk::Settings>& settings)
        {
            FEATHER_TK_P();

            p.context = context;
            p.settings = settings;

            p.colorPicker = feather_tk::ObservableValue<feather_tk::Color4F>::create();

            feather_tk::ImageOptions imageOptions;
            p.settings->getT("/Viewport/Image", imageOptions);
            p.imageOptions = feather_tk::ObservableValue<feather_tk::ImageOptions>::create(imageOptions);

            tl::timeline::DisplayOptions displayOptions;
            p.settings->getT("/Viewport/Display", displayOptions);
            p.displayOptions = feather_tk::ObservableValue<tl::timeline::DisplayOptions>::create(displayOptions);

            tl::timeline::BackgroundOptions backgroundOptions;
            p.settings->getT("/Viewport/Background", backgroundOptions);
            p.backgroundOptions = feather_tk::ObservableValue<tl::timeline::BackgroundOptions>::create(
                backgroundOptions);

            tl::timeline::ForegroundOptions foregroundOptions;
            p.settings->getT("/Viewport/Foreground", foregroundOptions);
            p.foregroundOptions = feather_tk::ObservableValue<tl::timeline::ForegroundOptions>::create(
                foregroundOptions);

            feather_tk::ImageType colorBuffer =
#if defined(FEATHER_TK_API_GL_4_1)
                feather_tk::ImageType::RGBA_F32;
#elif defined(FEATHER_TK_API_GLES_2)
                feather_tk::ImageType::RGBA_U8;
#endif // FEATHER_TK_API_GL_4_1
            std::string s = feather_tk::to_string(colorBuffer);
            p.settings->get("/Viewport/ColorBuffer", s);
            feather_tk::from_string(s, colorBuffer);
            p.colorBuffer = feather_tk::ObservableValue<feather_tk::ImageType>::create(colorBuffer);

            bool hud = false;
            p.settings->get("/Viewport/HUD/Enabled", hud);
            p.hud = feather_tk::ObservableValue<bool>::create(hud);
        }

        ViewportModel::ViewportModel() :
            _p(new Private)
        {}

        ViewportModel::~ViewportModel()
        {
            FEATHER_TK_P();
            p.settings->setT("/Viewport/Image", p.imageOptions->get());
            p.settings->setT("/Viewport/Display", p.displayOptions->get());
            p.settings->setT("/Viewport/Background", p.backgroundOptions->get());
            p.settings->setT("/Viewport/Foreground", p.foregroundOptions->get());
            p.settings->set("/Viewport/ColorBuffer", feather_tk::to_string(p.colorBuffer->get()));
            p.settings->set("/Viewport/HUD/Enabled", p.hud->get());
        }

        std::shared_ptr<ViewportModel> ViewportModel::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<feather_tk::Settings>& settings)
        {
            auto out = std::shared_ptr<ViewportModel>(new ViewportModel);
            out->_init(context, settings);
            return out;
        }

        const feather_tk::Color4F& ViewportModel::getColorPicker() const
        {
            return _p->colorPicker->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<feather_tk::Color4F> > ViewportModel::observeColorPicker() const
        {
            return _p->colorPicker;
        }

        void ViewportModel::setColorPicker(const feather_tk::Color4F& value)
        {
            _p->colorPicker->setIfChanged(value);
        }

        const feather_tk::ImageOptions& ViewportModel::getImageOptions() const
        {
            return _p->imageOptions->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<feather_tk::ImageOptions> > ViewportModel::observeImageOptions() const
        {
            return _p->imageOptions;
        }

        void ViewportModel::setImageOptions(const feather_tk::ImageOptions& value)
        {
            _p->imageOptions->setIfChanged(value);
        }

        const tl::timeline::DisplayOptions& ViewportModel::getDisplayOptions() const
        {
            return _p->displayOptions->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<tl::timeline::DisplayOptions> > ViewportModel::observeDisplayOptions() const
        {
            return _p->displayOptions;
        }

        void ViewportModel::setDisplayOptions(const tl::timeline::DisplayOptions& value)
        {
            _p->displayOptions->setIfChanged(value);
        }

        const tl::timeline::BackgroundOptions& ViewportModel::getBackgroundOptions() const
        {
            return _p->backgroundOptions->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<tl::timeline::BackgroundOptions> > ViewportModel::observeBackgroundOptions() const
        {
            return _p->backgroundOptions;
        }

        void ViewportModel::setBackgroundOptions(const tl::timeline::BackgroundOptions& value)
        {
            _p->settings->setT("/Viewport/Background", value);
            _p->backgroundOptions->setIfChanged(value);
        }

        const tl::timeline::ForegroundOptions& ViewportModel::getForegroundOptions() const
        {
            return _p->foregroundOptions->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<tl::timeline::ForegroundOptions> > ViewportModel::observeForegroundOptions() const
        {
            return _p->foregroundOptions;
        }

        void ViewportModel::setForegroundOptions(const tl::timeline::ForegroundOptions& value)
        {
            _p->settings->setT("/Viewport/Foreground", value);
            _p->foregroundOptions->setIfChanged(value);
        }

        feather_tk::ImageType ViewportModel::getColorBuffer() const
        {
            return _p->colorBuffer->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<feather_tk::ImageType> > ViewportModel::observeColorBuffer() const
        {
            return _p->colorBuffer;
        }

        void ViewportModel::setColorBuffer(feather_tk::ImageType value)
        {
            _p->colorBuffer->setIfChanged(value);
        }

        bool ViewportModel::getHUD() const
        {
            return _p->hud->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<bool> > ViewportModel::observeHUD() const
        {
            return _p->hud;
        }

        void ViewportModel::setHUD(bool value)
        {
            _p->hud->setIfChanged(value);
        }
    }
}
