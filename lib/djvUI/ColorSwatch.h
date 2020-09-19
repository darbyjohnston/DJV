// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvImage/Color.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a color swatch widget.
        class ColorSwatch : public Widget
        {
            DJV_NON_COPYABLE(ColorSwatch);
            
        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ColorSwatch();

        public:
            ~ColorSwatch() override;

            static std::shared_ptr<ColorSwatch> create(const std::shared_ptr<System::Context>&);

            const Image::Color& getColor() const;
            void setColor(const Image::Color&);

            MetricsRole getSwatchSizeRole() const;
            void setSwatchSizeRole(MetricsRole);

            void setBorder(bool);

            void setClickedCallback(const std::function<void(void)>&);

            bool acceptFocus(TextFocusDirection) override;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _paintEvent(System::Event::Paint&) override;
            void _pointerEnterEvent(System::Event::PointerEnter&) override;
            void _pointerLeaveEvent(System::Event::PointerLeave&) override;
            void _pointerMoveEvent(System::Event::PointerMove&) override;
            void _buttonPressEvent(System::Event::ButtonPress&) override;
            void _buttonReleaseEvent(System::Event::ButtonRelease&) override;
            void _keyPressEvent(System::Event::KeyPress&) override;
            void _textFocusEvent(System::Event::TextFocus&) override;
            void _textFocusLostEvent(System::Event::TextFocusLost&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
