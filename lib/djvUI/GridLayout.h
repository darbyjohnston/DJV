//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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
        namespace Layout
        {
            //! This enumeration provides how widgets use space in a layout.
            enum class GridStretch
            {
                None,
                Horizontal,
                Vertical,
                Both,

                Count,
                First = None
            };
            DJV_ENUM_HELPERS(GridStretch);

            //! This class provides layout that arranges it's children in a grid.
            //!
            //! \todo Add the ability for child widgets to span multiple rows and columns.
            class Grid : public Widget
            {
                DJV_NON_COPYABLE(Grid);

            protected:
                void _init(Core::Context *);
                Grid();

            public:
                virtual ~Grid();

                static std::shared_ptr<Grid> create(Core::Context *);

                void addWidget(const std::shared_ptr<Widget>&, const glm::ivec2&, GridStretch = GridStretch::None);
                void addWidget(const std::shared_ptr<Widget>&, int x, int y, GridStretch = GridStretch::None);
                void removeWidget(const std::shared_ptr<Widget>&);
                void clearWidgets();

                glm::ivec2 getGridSize() const;

                const Spacing& getSpacing() const;
                void setSpacing(const Spacing&);

                GridStretch getStretch(const std::shared_ptr<Widget>&) const;
                void setStretch(const std::shared_ptr<Widget>&, GridStretch);

                void setRowBackgroundRole(int, ColorRole);
                void setColumnBackgroundRole(int, ColorRole);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;
                void _paintEvent(Core::Event::Paint&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Layout

        typedef Layout::Grid GridLayout;
        using Layout::GridStretch;

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS(UI::Layout::GridStretch);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Layout::GridStretch);

} // namespace Gp

