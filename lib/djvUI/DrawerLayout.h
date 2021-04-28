// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            //! Drawer layout.
            class Drawer : public Widget
            {
                DJV_NON_COPYABLE(Drawer);

            protected:
                void _init(Side, const std::shared_ptr<System::Context>&);
                Drawer();

            public:
                ~Drawer() override;

                static std::shared_ptr<Drawer> create(Side, const std::shared_ptr<System::Context>&);

                Side getSide() const;

                bool isOpen() const;

                void setOpen(bool, bool animate = true);
                void open();
                void close();

                void setOpenCallback(const std::function<std::shared_ptr<Widget>(void)>&);
                void setCloseCallback(const std::function<void(const std::shared_ptr<Widget>&)>&);

                float getSplit() const;

                void setSplit(float);

                void setSplitCallback(const std::function<void(float)>&);

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;
                void _pointerLeaveEvent(System::Event::PointerLeave&) override;
                void _pointerMoveEvent(System::Event::PointerMove&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _buttonReleaseEvent(System::Event::ButtonRelease&) override;

            private:
                Math::BBox2f _getChildGeometry() const;
                Math::BBox2f _getDrawerGeometry() const;
                Math::BBox2f _getHandleGeometry() const;

                DJV_PRIVATE();
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv
