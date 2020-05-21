// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MDIWidget.h>

#include <djvAV/Pixel.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the color picker widget.
        class ColorPickerWidget : public MDIWidget
        {
            DJV_NON_COPYABLE(ColorPickerWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorPickerWidget();

        public:
            ~ColorPickerWidget() override;

            static std::shared_ptr<ColorPickerWidget> create(const std::shared_ptr<Core::Context>&);
            
            size_t getSampleSize() const;
            void setSampleSize(size_t);

            AV::Image::Type getLockType() const;
            void setLockType(AV::Image::Type);

            bool getApplyColorSpace() const;
            void setApplyColorSpace(bool);

            const glm::vec2& getPickerPos() const;
            void setPickerPos(const glm::vec2&);

        protected:
            void _initEvent(Core::Event::Init &) override;

        private:
            void _sampleUpdate();
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

