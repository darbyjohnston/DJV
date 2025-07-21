// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlDevice/BMDDevicesModel.h>

namespace feather_tk
{
    class Settings;
}

namespace djv
{
    namespace app
    {
        //! BMD devices model.
        class BMDDevicesModel : public tl::bmd::DevicesModel
        {
            FEATHER_TK_NON_COPYABLE(BMDDevicesModel);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<feather_tk::Settings>&);

            BMDDevicesModel();

        public:
            ~BMDDevicesModel();

            //! Create a new model.
            static std::shared_ptr<BMDDevicesModel> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<feather_tk::Settings>&);

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
