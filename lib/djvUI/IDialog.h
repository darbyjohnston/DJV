// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

namespace djv
{
    namespace UI
    {
        //! This class provides an interface for dialogs.
        class IDialog : public Window
        {
            DJV_NON_COPYABLE(IDialog);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
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

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

