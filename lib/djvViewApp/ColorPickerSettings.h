// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
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
        //! This class provides the color picker settings.
        class ColorPickerSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(ColorPickerSettings);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);

            ColorPickerSettings();

        public:
            virtual ~ColorPickerSettings();

            static std::shared_ptr<ColorPickerSettings> create(const std::shared_ptr<Core::Context>&);

            size_t getSampleSize() const;
            void setSampleSize(size_t);

            AV::Image::Type getLockType() const;
            void setLockType(AV::Image::Type);

            bool getApplyColorSpace() const;
            void setApplyColorSpace(bool);

            const glm::vec2& getPickerPos() const;
            void setPickerPos(const glm::vec2&);

            const std::map<std::string, Core::BBox2f>& getWidgetGeom() const;
            void setWidgetGeom(const std::map<std::string, Core::BBox2f>&);

            void load(const picojson::value &) override;
            picojson::value save() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

