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
                void _init(const std::shared_ptr<Core::Context>&);
                Grid();

            public:
                virtual ~Grid();

                static std::shared_ptr<Grid> create(const std::shared_ptr<Core::Context>&);

                glm::ivec2 getGridSize() const;

                glm::ivec2 getGridPos(const std::shared_ptr<Widget>&);
                void setGridPos(const std::shared_ptr<Widget>&, const glm::ivec2&, GridStretch = GridStretch::None);
                void setGridPos(const std::shared_ptr<Widget>&, int x, int y, GridStretch = GridStretch::None);

                const Spacing& getSpacing() const;
                void setSpacing(const Spacing&);

                GridStretch getStretch(const std::shared_ptr<Widget>&) const;
                void setStretch(const std::shared_ptr<Widget>&, GridStretch);

                void setRowBackgroundRole(int, ColorRole);
                void setColumnBackgroundRole(int, ColorRole);

                float getHeightForWidth(float) const override;

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;

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

