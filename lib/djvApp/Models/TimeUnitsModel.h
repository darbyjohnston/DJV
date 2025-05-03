// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlTimeline/TimeUnits.h>

namespace dtk
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
            DTK_NON_COPYABLE(TimeUnitsModel);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<dtk::Settings>&);

            TimeUnitsModel();

        public:
            ~TimeUnitsModel();

            //! Create a new model.
            static std::shared_ptr<TimeUnitsModel> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<dtk::Settings>&);

        private:
            DTK_PRIVATE();
        };
    }
}
