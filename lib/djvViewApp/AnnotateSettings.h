// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

#include <djvCore/BBox.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the annotation settings.
        class AnnotateSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(AnnotateSettings);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);

            AnnotateSettings();

        public:
            virtual ~AnnotateSettings();

            static std::shared_ptr<AnnotateSettings> create(const std::shared_ptr<Core::Context>&);

            const std::map<std::string, Core::BBox2f>& getWidgetGeom() const;
            void setWidgetGeom(const std::map<std::string, Core::BBox2f>&);

            void load(const picojson::value &) override;
            picojson::value save() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

