// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/Enum.h>
#include <djvUI/ISettings.h>

#include <djvCore/BBox.h>
#include <djvCore/MapObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the image settings.
        class ImageSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(ImageSettings);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);

            ImageSettings();

        public:
            static std::shared_ptr<ImageSettings> create(const std::shared_ptr<Core::Context>&);

            std::map<std::string, bool> getControlsBellowsState() const;
            void setControlsBellowsState(const std::map<std::string, bool>&);

            std::map<std::string, bool> getColorSpaceBellowsState() const;
            void setColorSpaceBellowsState(const std::map<std::string, bool>&);

            std::shared_ptr<Core::IValueSubject<UI::ImageRotate> > observeRotate() const;
            std::shared_ptr<Core::IValueSubject<UI::ImageAspectRatio> > observeAspectRatio() const;
            void setRotate(UI::ImageRotate);
            void setAspectRatio(UI::ImageAspectRatio);

            const std::map<std::string, Core::BBox2f>& getWidgetGeom() const;
            void setWidgetGeom(const std::map<std::string, Core::BBox2f>&);

            void load(const picojson::value &) override;
            picojson::value save() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

