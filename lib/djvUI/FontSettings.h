// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

#include <djvCore/MapObserver.h>
#include <djvCore/PicoJSON.h>

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            //! This class provides font settings.
            class Font : public ISettings
            {
                DJV_NON_COPYABLE(Font);

            protected:
                void _init(const std::shared_ptr<Core::Context>& context);
                Font();

            public:
                virtual ~Font();
                static std::shared_ptr<Font> create(const std::shared_ptr<Core::Context>&);

                const std::shared_ptr<Core::MapSubject<std::string, std::string> > & observeLocaleFonts() const;

                void load(const picojson::value &) override;
                picojson::value save() override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv
