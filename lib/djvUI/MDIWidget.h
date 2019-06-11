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

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        namespace MDI
        {
            class Canvas;

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

            class IWidget : public Widget
            {
                DJV_NON_COPYABLE(IWidget);

            protected:
                void _init(Core::Context*);
                IWidget();

            public:
                virtual ~IWidget() = 0;

            protected:
                virtual std::map<Handle, std::vector<Core::BBox2f> > _getHandles() const;
                virtual std::map<Handle, std::vector<Core::BBox2f> > _getHandlesDraw() const;

                float _getMaximized() const;
                virtual void _setMaximized(float);
                void _setHandleHovered(Handle);
                void _setHandlePressed(Handle);

                virtual void _activeWidget(bool) {}

                void _paintEvent(Core::Event::Paint&) override;
                void _paintOverlayEvent(Core::Event::PaintOverlay&) override;

            private:
                DJV_PRIVATE();

                friend class Canvas;
            };
            
        } // namespace MDI
    } // namespace UI
} // namespace djv
