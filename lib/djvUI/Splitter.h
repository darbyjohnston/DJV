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

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            //! This class provides a splitter widget.
            //!
            //! \bug Add support for more than two child widgets.
            class Splitter : public IContainer
            {
                DJV_NON_COPYABLE(Splitter);

            protected:
                void _init(Core::Context *);
                Splitter();

            public:
                virtual ~Splitter();

                static std::shared_ptr<Splitter> create(Orientation, Core::Context *);

                Orientation getOrientation() const;
                void setOrientation(Orientation);

                float getSplit() const;
                void setSplit(float);
                void setSplitCallback(const std::function<void(float)>&);

                float getHeightForWidth(float) const override;
                void updateEvent(Core::Event::Update&) override;
                void preLayoutEvent(Core::Event::PreLayout&) override;
                void layoutEvent(Core::Event::Layout&) override;
                void paintEvent(Core::Event::Paint&) override;

                void pointerEnterEvent(Core::Event::PointerEnter&) override;
                void pointerLeaveEvent(Core::Event::PointerLeave&) override;
                void pointerMoveEvent(Core::Event::PointerMove&) override;
                void buttonPressEvent(Core::Event::ButtonPress&) override;
                void buttonReleaseEvent(Core::Event::ButtonRelease&) override;

            private:
                float _valueToPos(float) const;
                float _posToValue(float) const;
                Core::BBox2f _getSplitterGeometry() const;
                Core::BBox2f _getHandleGeometry() const;

                struct Private;
                std::unique_ptr<Private> _p;
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv

