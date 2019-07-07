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

#include <djvUI/RowLayout.h>

namespace djv
{
    namespace UI
    {
        //! This class provides an interface for dialogs.
        //!
        //! \todo Actions with shortcuts need to be added to the child widget of
        //! the dialog, not the dialog itself.
        class IDialog : public UI::Widget
        {
            DJV_NON_COPYABLE(IDialog);

        protected:
            void _init(Core::Context *);
            IDialog();

        public:
            ~IDialog() override = 0;

            void setTitle(const std::string &);

            void setStretch(const std::shared_ptr<Widget> &, RowStretch = RowStretch::None);
            void setFillLayout(bool);

            void addTitleBarWidget(const std::shared_ptr<Widget>&);
            void removeTitleBarWidget(const std::shared_ptr<Widget>&);
            void clearTitleBarWidgets();

            void setCloseCallback(const std::function<void(void)> &);

            void setVisible(bool) override;
            float getHeightForWidth(float) const override;

            void addChild(const std::shared_ptr<IObject> &) override;
            void removeChild(const std::shared_ptr<IObject> &) override;
            void clearChildren() override;

        protected:
            void _doCloseCallback();

            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;
            void _pointerEnterEvent(Core::Event::PointerEnter &) override;
            void _pointerLeaveEvent(Core::Event::PointerLeave &) override;
            void _pointerMoveEvent(Core::Event::PointerMove &) override;
            void _keyPressEvent(Core::Event::KeyPress &) override;
            void _keyReleaseEvent(Core::Event::KeyRelease &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

