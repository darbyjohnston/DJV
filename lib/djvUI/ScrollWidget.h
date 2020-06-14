// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Margin.h>
#include <djvUI/Widget.h>

#include <djvCore/Range.h>
#include <djvCore/Timer.h>

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
        DJV_ENUM_HELPERS(ScrollType);

        //! This class provides a scroll widget.
        //!
        //! \todo Add a minimum scroll bar handle size.
        class ScrollWidget : public Widget
        {
            DJV_NON_COPYABLE(ScrollWidget);

        protected:
            void _init(ScrollType, const std::shared_ptr<Core::Context>&);
            ScrollWidget();

        public:
            virtual ~ScrollWidget();

            static std::shared_ptr<ScrollWidget> create(ScrollType, const std::shared_ptr<Core::Context>&);

            ScrollType getScrollType() const;
            void setScrollType(ScrollType);

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

            bool hasAutoHideScrollBars() const;
            void setAutoHideScrollBars(bool);

            void setBorder(bool);

            MetricsRole getMinimumSizeRole() const;
            MetricsRole getScrollBarSizeRole() const;
            void setMinimumSizeRole(MetricsRole);
            void setScrollBarSizeRole(MetricsRole);

            void addChild(const std::shared_ptr<IObject>&) override;
            void removeChild(const std::shared_ptr<IObject>&) override;
            void clearChildren() override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _clipEvent(Core::Event::Clip&) override;
            void _keyPressEvent(Core::Event::KeyPress&) override;
            void _scrollEvent(Core::Event::Scroll&) override;

            bool _eventFilter(const std::shared_ptr<IObject>&, Core::Event::Event&) override;

        private:
            void _updateScrollBars(const glm::vec2&);
            void _addPointerSample(const glm::vec2&);
            glm::vec2 _getPointerAverage() const;

            DJV_PRIVATE();
        };

    } // namespace UI
    
    DJV_ENUM_SERIALIZE_HELPERS(UI::ScrollType);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ScrollType);

} // namespace djv
