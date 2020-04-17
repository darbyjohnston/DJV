// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

#include <djvCore/BBox.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the magnify settings.
        class MagnifySettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(MagnifySettings);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);

            MagnifySettings();

        public:
            virtual ~MagnifySettings();

            static std::shared_ptr<MagnifySettings> create(const std::shared_ptr<Core::Context>&);

            size_t getMagnify() const;
            const glm::vec2& getMagnifyPos() const;
            void setMagnify(size_t);
            void setMagnifyPos(const glm::vec2&);

            const std::map<std::string, Core::BBox2f>& getWidgetGeom() const;
            void setWidgetGeom(const std::map<std::string, Core::BBox2f>&);

            void load(const picojson::value &) override;
            picojson::value save() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

