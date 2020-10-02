// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            //! This class provides the general I/O settings.
            class IO : public UI::Settings::ISettings
            {
                DJV_NON_COPYABLE(IO);

            protected:
                void _init(const std::shared_ptr<System::Context>& context);

                IO();

            public:
                ~IO() override;

                static std::shared_ptr<IO> create(const std::shared_ptr<System::Context>&);

                std::shared_ptr<Core::Observer::IValueSubject<size_t> > observeThreadCount() const;

                void setThreadCount(size_t);

                void load(const rapidjson::Value &) override;
                rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv
