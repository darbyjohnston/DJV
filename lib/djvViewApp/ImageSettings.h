// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/ISettings.h>

#include <djvMath/BBox.h>

#include <djvCore/ValueObserver.h>

#include <map>

namespace djv
{
    namespace ViewApp
    {
        struct ImageData;

        //! This class provides the image settings.
        class ImageSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(ImageSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>&);

            ImageSettings();

        public:
            static std::shared_ptr<ImageSettings> create(const std::shared_ptr<System::Context>&);

            std::map<std::string, bool> getControlsBellowsState() const;
            std::map<std::string, bool> getColorSpaceBellowsState() const;

            void setControlsBellowsState(const std::map<std::string, bool>&);
            void setColorSpaceBellowsState(const std::map<std::string, bool>&);

            std::shared_ptr<Core::Observer::IValueSubject<ImageData> > observeData() const;

            void setData(const ImageData&);

            const std::map<std::string, Math::BBox2f>& getWidgetGeom() const;

            void setWidgetGeom(const std::map<std::string, Math::BBox2f>&);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

