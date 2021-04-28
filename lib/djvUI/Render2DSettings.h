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
            //! Two-dimensional render settings.
            class Render2D : public ISettings
            {
                DJV_NON_COPYABLE(Render2D);

            protected:
                void _init(const std::shared_ptr<System::Context>&);

                Render2D();

            public:
                ~Render2D() override;

                static std::shared_ptr<Render2D> create(const std::shared_ptr<System::Context>&);

                void load(const rapidjson::Value&) override;
                rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

            private:
                void _currentFontUpdate();

                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv
