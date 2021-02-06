// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Margin.h>
#include <djvUI/Widget.h>

#include <djvSystem/Timer.h>

#include <djvMath/Range.h>

namespace djv
{
    namespace UI
    {
        //! This enumeration provides the scroll directions.
        enum class ScrollType
        {
            Both,
            Horizontal,
            Vertical,

            Count,
            First = Both
        };

        //! This class provides a scroll widget.
        //!
        //! \todo Add a minimum scroll bar handle size.
        class ScrollWidget : public Widget
        {
            DJV_NON_COPYABLE(ScrollWidget);

        protected:
            void _init(ScrollType, const std::shared_ptr<System::Context>&);
            ScrollWidget();

        public:
            ~ScrollWidget() override;

            //! Create a new scroll widget.
            static std::shared_ptr<ScrollWidget> create(ScrollType, const std::shared_ptr<System::Context>&);

            //! \name Scroll Type
            ///@{

            ScrollType getScrollType() const;

            void setScrollType(ScrollType);

            ///@}

            //! \name Scroll Position
            ///@{

            const glm::vec2& getScrollPos() const;

            void setScrollPos(const glm::vec2&);
            void moveToBegin();
            void moveToEnd();
            void movePageUp();
            void movePageDown();
            void moveUp();
            void moveDown();
            void moveLeft();
            void moveRight();

            ///@}

            //! \name Scroll Bars
            ///@{

            bool hasAutoHideScrollBars() const;

            void setAutoHideScrollBars(bool);

            ///@}

            //! \name Options
            ///@{

            bool hasBorder() const;
            MetricsRole getMinimumSizeRole() const;
            MetricsRole getScrollBarSizeRole() const;

            void setBorder(bool);
            void setMinimumSizeRole(MetricsRole);
            void setScrollBarSizeRole(MetricsRole);

            ///@}

            void addChild(const std::shared_ptr<IObject>&) override;
            void removeChild(const std::shared_ptr<IObject>&) override;
            void clearChildren() override;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;
            void _clipEvent(System::Event::Clip&) override;
            void _keyPressEvent(System::Event::KeyPress&) override;
            void _scrollEvent(System::Event::Scroll&) override;

            bool _eventFilter(const std::shared_ptr<IObject>&, System::Event::Event&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
