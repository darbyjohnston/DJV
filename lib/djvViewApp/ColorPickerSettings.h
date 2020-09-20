// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
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
        //! This class provides the color picker settings.
        class ColorPickerSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(ColorPickerSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>&);

            ColorPickerSettings();

        public:
            ~ColorPickerSettings() override;

            static std::shared_ptr<ColorPickerSettings> create(const std::shared_ptr<System::Context>&);

            size_t getSampleSize() const;

            void setSampleSize(size_t);

            Image::Type getLockType() const;

            void setLockType(Image::Type);

            bool getApplyColorOperations() const;

            void setApplyColorOperations(bool);

            bool getApplyColorSpace() const;

            void setApplyColorSpace(bool);

            const glm::vec2& getPickerPos() const;

            void setPickerPos(const glm::vec2&);

            const std::map<std::string, Math::BBox2f>& getWidgetGeom() const;

            void setWidgetGeom(const std::map<std::string, Math::BBox2f>&);

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

