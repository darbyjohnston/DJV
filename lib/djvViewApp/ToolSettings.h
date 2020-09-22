// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

#include <djvImage/Pixel.h>

#include <djvMath/BBox.h>

#include <djvCore/ValueObserver.h>

#include <map>

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

            std::shared_ptr<Core::IValueSubject<bool> > observeMessagesPopup() const;

            void setMessagesPopup(bool);

            std::map<std::string, bool> getDebugBellowsState() const;

            void setDebugBellowsState(const std::map<std::string, bool>&);

            const std::map<std::string, Math::BBox2f>& getWidgetGeom() const;

            void setWidgetGeom(const std::map<std::string, Math::BBox2f>&);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

