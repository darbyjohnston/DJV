// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ToolBar.h>

namespace djv
{
    namespace ViewApp
    {
        //! Messages widget.
        class MessagesWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(MessagesWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            MessagesWidget();

        public:
            ~MessagesWidget() override;

            static std::shared_ptr<MessagesWidget> create(const std::shared_ptr<System::Context>&);

            //! \name Text
            ///@{

            void setText(const std::string&);

            ///@}

            float getHeightForWidth(float) const override;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            DJV_PRIVATE();
        };

        //! Messages tool bar widget.
        class MessagesToolBar : public UI::ToolBar
        {
            DJV_NON_COPYABLE(MessagesToolBar);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            MessagesToolBar();

        public:
            ~MessagesToolBar() override;

            static std::shared_ptr<MessagesToolBar> create(const std::shared_ptr<System::Context>&);

            //! \name Options
            ///@{

            void setPopup(bool);

            ///@}

            //! \name Callbacks
            ///@{

            void setPopupCallback(const std::function<void(bool)>&);
            void setCopyCallback(const std::function<void(void)>&);
            void setClearCallback(const std::function<void(void)>&);

            ///@}

        protected:
            void _initEvent(System::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

