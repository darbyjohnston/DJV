// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvAV/Color.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a color swatch widget.
        class ColorSwatch : public Widget
        {
            DJV_NON_COPYABLE(ColorSwatch);
            
        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorSwatch();

        public:
            ~ColorSwatch() override;

            static std::shared_ptr<ColorSwatch> create(const std::shared_ptr<Core::Context>&);

            const AV::Image::Color& getColor() const;
            void setColor(const AV::Image::Color&);

            MetricsRole getSwatchSizeRole() const;
            void setSwatchSizeRole(MetricsRole);

            void setBorder(bool);

            void setClickedCallback(const std::function<void(void)>&);

            bool acceptFocus(TextFocusDirection) override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _paintEvent(Core::Event::Paint&) override;
            void _pointerEnterEvent(Core::Event::PointerEnter&) override;
            void _pointerLeaveEvent(Core::Event::PointerLeave&) override;
            void _pointerMoveEvent(Core::Event::PointerMove&) override;
            void _buttonPressEvent(Core::Event::ButtonPress&) override;
            void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;
            void _keyPressEvent(Core::Event::KeyPress&) override;
            void _textFocusEvent(Core::Event::TextFocus&) override;
            void _textFocusLostEvent(Core::Event::TextFocusLost&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
