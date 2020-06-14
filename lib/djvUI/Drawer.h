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
                virtual ~Drawer();

                static std::shared_ptr<Drawer> create(Side, const std::shared_ptr<Core::Context>&);

                Side getSide() const;

                bool isOpen() const;
                void setOpen(bool);
                void open();
                void close();

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;
                void clearChildren() override;

            protected:
                virtual void _openStart(void) {}
                virtual void _openEnd(void) {}
                virtual void _closeStart(void) {}
                virtual void _closeEnd(void) {}

                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;
                void _paintEvent(Core::Event::Paint&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Layout

        typedef Layout::Drawer Drawer;

    } // namespace UI
} // namespace djv

