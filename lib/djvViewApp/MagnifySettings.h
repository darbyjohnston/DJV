// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

#include <djvMath/BBox.h>
#include <djvCore/ValueObserver.h>

#include <map>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the magnify settings.
        class MagnifySettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(MagnifySettings);

        protected:
            void _init(const std::shared_ptr<System::Context>&);

            MagnifySettings();

        public:
            ~MagnifySettings() override;

            static std::shared_ptr<MagnifySettings> create(const std::shared_ptr<System::Context>&);

            size_t getMagnify() const;
            const glm::vec2& getMagnifyPos() const;
            void setMagnify(size_t);
            void setMagnifyPos(const glm::vec2&);

            const std::map<std::string, Math::BBox2f>& getWidgetGeom() const;
            void setWidgetGeom(const std::map<std::string, Math::BBox2f>&);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

