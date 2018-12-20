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

#include <djvUI/IContainerWidget.h>
#include <djvUI/Margin.h>

namespace djv
{
    namespace UI
    {
        class Window;

        //! This class provides a dialog widget.
        class Dialog : public IContainerWidget
        {
            DJV_NON_COPYABLE(Dialog);

        protected:
            void _init(Core::Context *);
            Dialog();

        public:
            virtual ~Dialog();

            static std::shared_ptr<Dialog> create(Core::Context *);

            void close();

            void addWidget(const std::shared_ptr<Widget>&) override;
            void removeWidget(const std::shared_ptr<Widget>&) override;
            void clearWidgets() override;

            void setVisible(bool) override;
            float getHeightForWidth(float) const override;
            void preLayoutEvent(Core::PreLayoutEvent&) override;
            void layoutEvent(Core::LayoutEvent&) override;

            void buttonPressEvent(Core::ButtonPressEvent&) override;
            void keyPressEvent(Core::KeyPressEvent&) override;
            void keyReleaseEvent(Core::KeyReleaseEvent&) override;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! Show a message dialog.
        void messageDialog(
            const std::string & text,
            const std::string & closeText,
            const std::shared_ptr<Window> & window);

        //! Show a confirmation dialog.
        void confirmationDialog(
            const std::string & text,
            const std::string & acceptText,
            const std::string & cancelText,
            const std::shared_ptr<Window> & window,
            const std::function<void(bool)> & callback);

    } // namespace UI
} // namespace djv

