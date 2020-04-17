// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        namespace MDI
        {
            class Canvas;

            //! This enumeration provides the MDI move and resize handles.
            enum class Handle
            {
                None,
                Move,
                ResizeE,
                ResizeN,
                ResizeW,
                ResizeS,
                ResizeNE,
                ResizeNW,
                ResizeSW,
                ResizeSE,

                Count,
                First = None
            };

            //! This class provides the base functionality for MDI widgets.
            class IWidget : public Widget
            {
                DJV_NON_COPYABLE(IWidget);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                IWidget();

            public:
                virtual ~IWidget() = 0;

            protected:
                virtual std::map<Handle, std::vector<Core::BBox2f> > _getHandles() const;
                virtual std::map<Handle, std::vector<Core::BBox2f> > _getHandlesDraw() const;

                float _getMaximize() const;
                virtual void _setMaximize(float);
                void _setHandleHovered(Handle);
                void _setHandlePressed(Handle);

                virtual void _setActiveWidget(bool);

                void _paintEvent(Core::Event::Paint&) override;
                void _paintOverlayEvent(Core::Event::PaintOverlay&) override;

            private:
                DJV_PRIVATE();

                friend class Canvas;
            };
            
        } // namespace MDI
    } // namespace UI
} // namespace djv
