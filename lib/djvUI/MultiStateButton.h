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
            //! Multi-state button.
            class MultiState : public Widget
            {
                DJV_NON_COPYABLE(MultiState);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                MultiState();

            public:
                ~MultiState() override;

                //! Create a new multi-state button.
                static std::shared_ptr<MultiState> create(const std::shared_ptr<System::Context>&);

                //! \name Icons
                ///@{
                
                void addIcon(const std::string&);
                void clearIcons();

                void setIconSizeRole(MetricsRole);

                ///@}

                //! \name Current Index
                ///@{

                int getCurrentIndex() const;

                void setCurrentIndex(int);
                void setCurrentCallback(const std::function<void(int)>&);

                ///@}

                //! \name Options
                ///@{

                MetricsRole getInsideMargin() const;

                void setInsideMargin(MetricsRole);

                ///@}

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;
                void _pointerEnterEvent(System::Event::PointerEnter&) override;
                void _pointerLeaveEvent(System::Event::PointerLeave&) override;
                void _pointerMoveEvent(System::Event::PointerMove&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _buttonReleaseEvent(System::Event::ButtonRelease&) override;

            private:
                void _widgetUpdate();

                DJV_PRIVATE();
            };

        } // namespace Button
        
        typedef Button::MultiState MultiStateButton;

    } // namespace UI
} // namespace djv

