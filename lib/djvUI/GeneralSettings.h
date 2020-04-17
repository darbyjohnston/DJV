// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            //! This class provides general settings.
            class General : public ISettings
            {
                DJV_NON_COPYABLE(General);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                General();

            public:
                virtual ~General();

                static std::shared_ptr<General> create(const std::shared_ptr<Core::Context>&);

                void load(const picojson::value&) override;
                picojson::value save() override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv
