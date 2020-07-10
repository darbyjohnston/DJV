// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceViewModel.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceViewModel::Private
        {
            std::shared_ptr<AV::OCIO::System> ocioSystem;

            AV::OCIO::ConfigType configType = AV::OCIO::ConfigType::First;
            AV::OCIO::UserConfigs userConfigs;
            AV::OCIO::Config envConfig;
            AV::OCIO::Config cmdLineConfig;
            AV::OCIO::Config currentConfig;
            AV::OCIO::Displays displays;
            AV::OCIO::Views views;

            std::shared_ptr<ValueSubject<ColorSpaceConfigData> > userConfigDataSubject;
            std::shared_ptr<ValueSubject<ColorSpaceConfigData::Item> > envConfigDataSubject;
            std::shared_ptr<ValueSubject<ColorSpaceConfigData::Item> > cmdLineConfigDataSubject;
            std::shared_ptr<ValueSubject<ColorSpaceDisplayData> > displayDataSubject;
            std::shared_ptr<ValueSubject<ColorSpaceViewData> > viewDataSubject;
            std::shared_ptr<ListSubject<std::string> > colorSpacesSubject;
            std::shared_ptr<MapSubject<std::string, std::string> > imageColorSpacesSubject;

            std::shared_ptr<ValueObserver<AV::OCIO::ConfigType> > configTypeObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::UserConfigs> > userConfigsObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > envConfigObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > cmdLineConfigObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > currentConfigObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Displays> > displaysObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Views> > viewsObserver;
            std::shared_ptr<ListObserver<std::string> > colorSpacesObserver;

            void dataUpdate();
        };

        void ColorSpaceViewModel::_init(const std::shared_ptr<Context>& context)
        {
            DJV_PRIVATE_PTR();

            p.ocioSystem = context->getSystemT<AV::OCIO::System>();

            p.userConfigDataSubject = ValueSubject<ColorSpaceConfigData>::create();
            p.envConfigDataSubject = ValueSubject<ColorSpaceConfigData::Item>::create();
            p.cmdLineConfigDataSubject = ValueSubject<ColorSpaceConfigData::Item>::create();
            p.displayDataSubject = ValueSubject<ColorSpaceDisplayData>::create();
            p.viewDataSubject = ValueSubject<ColorSpaceViewData>::create();

            auto weak = std::weak_ptr<ColorSpaceViewModel>(shared_from_this());
            p.configTypeObserver = ValueObserver<AV::OCIO::ConfigType>::create(
                p.ocioSystem->observeConfigType(),
                [weak](AV::OCIO::ConfigType value)
                {
                    if (auto viewModel = weak.lock())
                    {
                        viewModel->_p->configType = value;
                        viewModel->_p->dataUpdate();
                    }
                });

            p.userConfigsObserver = ValueObserver<AV::OCIO::UserConfigs>::create(
                p.ocioSystem->observeUserConfigs(),
                [weak](const AV::OCIO::UserConfigs& value)
                {
                    if (auto viewModel = weak.lock())
                    {
                        viewModel->_p->userConfigs = value;
                        viewModel->_p->dataUpdate();
                    }
                });

            p.envConfigObserver = ValueObserver<AV::OCIO::Config>::create(
                p.ocioSystem->observeEnvConfig(),
                [weak](const AV::OCIO::Config& value)
                {
                    if (auto viewModel = weak.lock())
                    {
                        viewModel->_p->envConfig = value;
                        viewModel->_p->dataUpdate();
                    }
                });

            p.cmdLineConfigObserver = ValueObserver<AV::OCIO::Config>::create(
                p.ocioSystem->observeCmdLineConfig(),
                [weak](const AV::OCIO::Config& value)
                {
                    if (auto viewModel = weak.lock())
                    {
                        viewModel->_p->cmdLineConfig = value;
                        viewModel->_p->dataUpdate();
                    }
                });

            p.currentConfigObserver = ValueObserver<AV::OCIO::Config>::create(
                p.ocioSystem->observeCurrentConfig(),
                [weak](const AV::OCIO::Config& value)
                {
                    if (auto viewModel = weak.lock())
                    {
                        viewModel->_p->currentConfig = value;
                        viewModel->_p->dataUpdate();
                    }
                });

            p.displaysObserver = ValueObserver<AV::OCIO::Displays>::create(
                p.ocioSystem->observeDisplays(),
                [weak](const AV::OCIO::Displays& value)
                {
                    if (auto viewModel = weak.lock())
                    {
                        viewModel->_p->displays = value;
                        viewModel->_p->dataUpdate();
                    }
                });

            p.viewsObserver = ValueObserver<AV::OCIO::Views>::create(
                p.ocioSystem->observeViews(),
                [weak](const AV::OCIO::Views& value)
                {
                    if (auto viewModel = weak.lock())
                    {
                        viewModel->_p->views = value;
                        viewModel->_p->dataUpdate();
                    }
                });

            p.colorSpacesObserver = ListObserver<std::string>::create(
                p.ocioSystem->observeColorSpaces(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto viewModel = weak.lock())
                    {
                        viewModel->_p->colorSpacesSubject->setIfChanged(value);
                    }
                });
        }

        ColorSpaceViewModel::ColorSpaceViewModel()
        {}

        ColorSpaceViewModel::~ColorSpaceViewModel()
        {}

        std::shared_ptr<ColorSpaceViewModel> ColorSpaceViewModel::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceViewModel>(new ColorSpaceViewModel);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<ColorSpaceConfigData> > ColorSpaceViewModel::observeUserConfigData() const
        {
            return _p->userConfigDataSubject;
        }

        std::shared_ptr<IValueSubject<ColorSpaceConfigData::Item> > ColorSpaceViewModel::observeEnvConfigData() const
        {
            return _p->envConfigDataSubject;
        }

        std::shared_ptr<IValueSubject<ColorSpaceConfigData::Item> > ColorSpaceViewModel::observeCmdLineConfigData() const
        {
            return _p->cmdLineConfigDataSubject;
        }

        std::shared_ptr<IValueSubject<ColorSpaceDisplayData> > ColorSpaceViewModel::observeDisplayData() const
        {
            return _p->displayDataSubject;
        }

        std::shared_ptr<IValueSubject<ColorSpaceViewData> > ColorSpaceViewModel::observeViewData() const
        {
            return _p->viewDataSubject;
        }

        void ColorSpaceViewModel::setImageColorSpaces(const AV::OCIO::ImageColorSpaces& value)
        {
            _p->ocioSystem->setImageColorSpaces(value);
        }

        void ColorSpaceViewModel::Private::dataUpdate()
        {
            ColorSpaceConfigData userConfigData;
            ColorSpaceConfigData::Item envConfigData;
            ColorSpaceConfigData::Item cmdLineConfigData;
            ColorSpaceDisplayData displayData;
            ColorSpaceViewData viewData;
            AV::OCIO::ImageColorSpaces imageColorSpaces;

            for (const auto& config : userConfigs.first)
            {
                userConfigData.items.push_back({ config.fileName, config.name });
            }
            userConfigData.current = userConfigs.second;
            envConfigData.fileName = envConfig.fileName;
            envConfigData.name = envConfig.name;

            displayData.names = displays.first;
            displayData.current = displays.second;

            userConfigDataSubject->setIfChanged(userConfigData);
            envConfigDataSubject->setIfChanged(envConfigData);
            cmdLineConfigDataSubject->setIfChanged(cmdLineConfigData);
            displayDataSubject->setIfChanged(displayData);
            viewDataSubject->setIfChanged(viewData);
            imageColorSpacesSubject->setIfChanged(imageColorSpaces);
        }

    } // namespace ViewApp
} // namespace djv

