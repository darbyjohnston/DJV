// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Models/BMDDevicesModel.h>

#include <feather-tk/ui/Settings.h>

namespace djv
{
    namespace app
    {
        struct BMDDevicesModel::Private
        {
            std::shared_ptr<feather_tk::Settings> settings;
        };

        void BMDDevicesModel::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<feather_tk::Settings>& settings)
        {
            tl::bmd::DevicesModel::_init(context);
            FEATHER_TK_P();

            p.settings = settings;

            tl::bmd::DevicesModelData data;
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
            FEATHER_TK_P();
            p.settings->setT("/BMD", observeData()->get());
        }

        std::shared_ptr<BMDDevicesModel> BMDDevicesModel::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<feather_tk::Settings>& settings)
        {
            auto out = std::shared_ptr<BMDDevicesModel>(new BMDDevicesModel);
            out->_init(context, settings);
            return out;
        }
    }
}
