//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include <djvUI/IContainer.h>
#include <djvUI/Margin.h>

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
        //! \bug [1.0 M] Add a minimum scroll bar handle size.
        //! \todo Add keyboard shortcuts (page up, page down, etc.).
        class ScrollWidget : public Layout::IContainer
        {
            DJV_NON_COPYABLE(ScrollWidget);

        protected:
            void _init(ScrollType, Core::Context *);
            ScrollWidget();

        public:
            virtual ~ScrollWidget();

            static std::shared_ptr<ScrollWidget> create(ScrollType, Core::Context *);

            ScrollType getScrollType() const;
            void setScrollType(ScrollType);

            const glm::vec2& getScrollPos() const;
            void setScrollPos(const glm::vec2&);

            bool hasBorder() const;
            void setBorder(bool);

            void addWidget(const std::shared_ptr<Widget>&) override;
            void removeWidget(const std::shared_ptr<Widget>&) override;
            void clearWidgets() override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _clipEvent(Core::Event::Clip&) override;
            void _scrollEvent(Core::Event::Scroll&) override;

            bool _eventFilter(const std::shared_ptr<IObject>&, Core::Event::IEvent&) override;

        private:
            void _updateScrollBars(const glm::vec2&);
            void _addPointerSample(const glm::vec2&);
            glm::vec2 _getPointerAverage() const;

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
    
    DJV_ENUM_SERIALIZE_HELPERS(UI::ScrollType);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ScrollType);

} // namespace djv
