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

#include <djvUI/Margin.h>
#include <djvUI/Spacing.h>
#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This enumeration provides how widgets use space in a layout.
        enum class RowLayoutStretch
        {
            None,
            Expand,
            
            Count,
            First = None
        };
        DJV_ENUM_HELPERS(RowLayoutStretch);
        
        //! This class provides a layout that arranges it's children in a row.
        class RowLayout : public Widget
        {
            DJV_NON_COPYABLE(RowLayout);
            
        protected:
            void _init(Orientation, Core::Context *);
            RowLayout();

        public:
            virtual ~RowLayout();

            static std::shared_ptr<RowLayout> create(Orientation, Core::Context *);

            void addWidget(const std::shared_ptr<Widget>&, RowLayoutStretch = RowLayoutStretch::None);
            void insertWidget(const std::shared_ptr<Widget>&, int index, RowLayoutStretch = RowLayoutStretch::None);
            void removeWidget(const std::shared_ptr<Widget>&);
            void clearWidgets();
            
            void addSeparator();
            void addSpacer();
            void addExpander();

            Orientation getOrientation() const;
            void setOrientation(Orientation);

            const Spacing& getSpacing() const;
            void setSpacing(const Spacing&);
            
            RowLayoutStretch getStretch(const std::shared_ptr<Widget>&) const;
            void setStretch(const std::shared_ptr<Widget>&, RowLayoutStretch);
            
            float getHeightForWidth(float) const override;

        protected:
            void _preLayoutEvent(Core::PreLayoutEvent&) override;
            void _layoutEvent(Core::LayoutEvent&) override;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides a layout that arranges it's children in a horizontal row.
        class HorizontalLayout : public RowLayout
        {
            DJV_NON_COPYABLE(HorizontalLayout);

        protected:
            void _init(Core::Context *);
            HorizontalLayout();

        public:
            static std::shared_ptr<HorizontalLayout> create(Core::Context *);
        };

        //! This class provides a layout that arranges it's children in a vertical row.
        class VerticalLayout : public RowLayout
        {
            DJV_NON_COPYABLE(VerticalLayout);

        protected:
            void _init(Core::Context *);
            VerticalLayout();

        public:
            static std::shared_ptr<VerticalLayout> create(Core::Context *);
        };

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS(UI::RowLayoutStretch);
    DJV_ENUM_SERIALIZE_HELPERS(UI::RowLayoutStretch);

} // namespace Gp
