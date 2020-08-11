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
                void _init(const std::shared_ptr<Core::Context>&);
                Overlay();

            public:
                ~Overlay() override;
                static std::shared_ptr<Overlay> create(const std::shared_ptr<Core::Context>&);

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
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;
                void _pointerEnterEvent(Core::Event::PointerEnter&) override;
                void _pointerLeaveEvent(Core::Event::PointerLeave&) override;
                void _pointerMoveEvent(Core::Event::PointerMove&) override;
                void _buttonPressEvent(Core::Event::ButtonPress&) override;
                void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;
                void _keyPressEvent(Core::Event::KeyPress&) override;

            private:
                void _doCloseCallback();

                DJV_PRIVATE();
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv

