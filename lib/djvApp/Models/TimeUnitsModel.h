// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlTimeline/TimeUnits.h>

namespace feather_tk
{
    class Settings;
}

namespace djv
{
    namespace app
    {
        //! Time units model.
        class TimeUnitsModel : public tl::timeline::TimeUnitsModel
        {
            FEATHER_TK_NON_COPYABLE(TimeUnitsModel);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<feather_tk::Settings>&);

            TimeUnitsModel();

        public:
            ~TimeUnitsModel();

            //! Create a new model.
            static std::shared_ptr<TimeUnitsModel> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<feather_tk::Settings>&);

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
