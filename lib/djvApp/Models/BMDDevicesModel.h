// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlDevice/BMDDevicesModel.h>

namespace dtk
{
    class Settings;
}

namespace tl
{
    namespace play
    {
        //! BMD devices model.
        class BMDDevicesModel : public bmd::DevicesModel
        {
            DTK_NON_COPYABLE(BMDDevicesModel);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<dtk::Settings>&);

            BMDDevicesModel();

        public:
            ~BMDDevicesModel();

            //! Create a new model.
            static std::shared_ptr<BMDDevicesModel> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<dtk::Settings>&);

        private:
            DTK_PRIVATE();
        };
    }
}
