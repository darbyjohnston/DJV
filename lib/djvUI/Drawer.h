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
            //! This class provides a drawer widget.
            class Drawer : public Widget
            {
                DJV_NON_COPYABLE(Drawer);

            protected:
                void _init(Side, const std::shared_ptr<Core::Context>&);
                Drawer();

            public:
                ~Drawer() override;

                static std::shared_ptr<Drawer> create(Side, const std::shared_ptr<Core::Context>&);

                Side getSide() const;

                bool isOpen() const;
                void setOpen(bool, bool animate = true);
                void open();
                void close();

                void setOpenCallback(const std::function<std::shared_ptr<Widget>(void)>&);
                void setCloseCallback(const std::function<void(const std::shared_ptr<Widget>&)>&);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Layout

        typedef Layout::Drawer Drawer;

    } // namespace UI
} // namespace djv

