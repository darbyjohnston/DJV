// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            //! This class provides color space settings.
            class ColorSpace : public ISettings
            {
                DJV_NON_COPYABLE(ColorSpace);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                ColorSpace();

            public:
                virtual ~ColorSpace();

                static std::shared_ptr<ColorSpace> create(const std::shared_ptr<Core::Context>&);

                void load(const rapidjson::Value&) override;
                rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv
