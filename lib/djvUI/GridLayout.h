// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
            //! Grid layout stretch.
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

            //! Grid layout.
            //!
            //! This layout arranges it's children in a grid.
            //!
            //! \todo Add the ability for child widgets to span multiple rows and columns.
            class Grid : public Widget
            {
                DJV_NON_COPYABLE(Grid);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Grid();

            public:
                ~Grid() override;

                static std::shared_ptr<Grid> create(const std::shared_ptr<System::Context>&);

                //! \name Layout
                ///@{

                glm::ivec2 getGridSize() const;
                glm::ivec2 getGridPos(const std::shared_ptr<Widget>&);
                const Spacing& getSpacing() const;
                GridStretch getStretch(const std::shared_ptr<Widget>&) const;

                void setGridPos(const std::shared_ptr<Widget>&, const glm::ivec2&, GridStretch = GridStretch::None);
                void setGridPos(const std::shared_ptr<Widget>&, int x, int y, GridStretch = GridStretch::None);
                void setSpacing(const Spacing&);
                void setStretch(const std::shared_ptr<Widget>&, GridStretch);

                ///@}

                //! \name Options
                ///@{

                void setRowColorRole(int, ColorRole);
                void setColumnColorRole(int, ColorRole);

                ///@}

                float getHeightForWidth(float) const override;

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;

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

