// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <tlPlayApp/Models/BMDDevicesModel.h>

#include <dtk/ui/Settings.h>

namespace tl
{
    namespace play
    {
        struct BMDDevicesModel::Private
        {
            std::shared_ptr<dtk::Settings> settings;
        };

        void BMDDevicesModel::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<dtk::Settings>& settings)
        {
            bmd::DevicesModel::_init(context);
            DTK_P();

            p.settings = settings;

            bmd::DevicesModelData data;
            p.settings->getT("/BMD", data);
            setDeviceIndex(data.deviceIndex);
            setDisplayModeIndex(data.displayModeIndex);
            setPixelTypeIndex(data.pixelTypeIndex);
            setDeviceEnabled(data.deviceEnabled);
            setBoolOptions(data.boolOptions);
            setHDRMode(data.hdrMode);
            setHDRData(data.hdrData);
        }

        BMDDevicesModel::BMDDevicesModel() :
            _p(new Private)
        {}

        BMDDevicesModel::~BMDDevicesModel()
        {
            DTK_P();
            p.settings->setT("/BMD", observeData()->get());
        }

        std::shared_ptr<BMDDevicesModel> BMDDevicesModel::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<dtk::Settings>& settings)
        {
            auto out = std::shared_ptr<BMDDevicesModel>(new BMDDevicesModel);
            out->_init(context, settings);
            return out;
        }
    }
}
