// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            //! This class provides an overlay widget for capturing pointer and key events.
            class Overlay : public Widget
            {
                DJV_NON_COPYABLE(Overlay);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Overlay();

            public:
                ~Overlay() override;
                static std::shared_ptr<Overlay> create(const std::shared_ptr<System::Context>&);

                bool hasCapturePointer() const;
                bool hasCaptureKeyboard() const;

                void setCapturePointer(bool);
                void setCaptureKeyboard(bool);

                bool hasFadeIn() const;

                void setFadeIn(bool);

                void setCloseCallback(const std::function<void(void)>&);

                void setVisible(bool) override;
                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _pointerEnterEvent(System::Event::PointerEnter&) override;
                void _pointerLeaveEvent(System::Event::PointerLeave&) override;
                void _pointerMoveEvent(System::Event::PointerMove&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _buttonReleaseEvent(System::Event::ButtonRelease&) override;
                void _keyPressEvent(System::Event::KeyPress&) override;

            private:
                void _doCloseCallback();

                DJV_PRIVATE();
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv

