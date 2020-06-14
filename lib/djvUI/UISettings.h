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
            //! This class provides UI settings.
            class UI : public ISettings
            {
                DJV_NON_COPYABLE(UI);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                UI();

            public:
                virtual ~UI();

                static std::shared_ptr<UI> create(const std::shared_ptr<Core::Context>&);

                std::shared_ptr<Core::IValueSubject<bool> > observeTooltips() const;
                void setTooltips(bool);

                std::shared_ptr<Core::IValueSubject<bool> > observeReverseScrolling() const;
                void setReverseScrolling(bool);

                void load(const rapidjson::Value &) override;
                rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv
