// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            //! This class provides a multi-state button.
            class MultiState : public Widget
            {
                DJV_NON_COPYABLE(MultiState);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                MultiState();

            public:
                virtual ~MultiState();

                static std::shared_ptr<MultiState> create(const std::shared_ptr<Core::Context>&);
                
                void addIcon(const std::string&);
                void clearIcons();

                int getCurrentIndex() const;
                void setCurrentIndex(int);

                void setCallback(const std::function<void(int)>&);

                MetricsRole getInsideMargin() const;
                void setInsideMargin(MetricsRole);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;
                void _paintEvent(Core::Event::Paint&) override;
                void _pointerEnterEvent(Core::Event::PointerEnter&) override;
                void _pointerLeaveEvent(Core::Event::PointerLeave&) override;
                void _pointerMoveEvent(Core::Event::PointerMove&) override;
                void _buttonPressEvent(Core::Event::ButtonPress&) override;
                void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;

            private:
                void _widgetUpdate();

                DJV_PRIVATE();
            };

        } // namespace Button
        
        typedef Button::MultiState MultiStateButton;

    } // namespace UI
} // namespace djv

