// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/ISettings.h>

#include <djvMath/BBox.h>

#include <djvCore/ValueObserver.h>
#include <djvCore/ListObserver.h>

#include <glm/vec2.hpp>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the tool settings.
        class ToolSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(ToolSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>&);

            ToolSettings();

        public:
            ~ToolSettings() override;

            static std::shared_ptr<ToolSettings> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::Observer::IValueSubject<int> > observeCurrentTool() const;

            void setCurrentTool(int);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

