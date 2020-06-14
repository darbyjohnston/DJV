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
            //! This class provides the A/V settings.
            class AV : public ISettings
            {
                DJV_NON_COPYABLE(AV);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);

                AV();

            public:
                virtual ~AV();

                static std::shared_ptr<AV> create(const std::shared_ptr<Core::Context>&);

                void load(const rapidjson::Value &) override;
                rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

            private:
                void _currentFontUpdate();

                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv
