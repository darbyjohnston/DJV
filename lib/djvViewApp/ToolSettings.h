// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

#include <djvAV/Pixel.h>

#include <djvCore/BBox.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the tool settings.
        class ToolSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(ToolSettings);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);

            ToolSettings();

        public:
            virtual ~ToolSettings();

            static std::shared_ptr<ToolSettings> create(const std::shared_ptr<Core::Context>&);

            bool getInfoBellowsState() const;
            void setInfoBellowsState(bool);

            std::shared_ptr<Core::IValueSubject<bool> > observeMessagesPopup() const;
            void setMessagesPopup(bool);

            std::map<std::string, bool> getDebugBellowsState() const;
            void setDebugBellowsState(const std::map<std::string, bool>&);

            const std::map<std::string, Core::BBox2f>& getWidgetGeom() const;
            void setWidgetGeom(const std::map<std::string, Core::BBox2f>&);

            void load(const picojson::value &) override;
            picojson::value save() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

