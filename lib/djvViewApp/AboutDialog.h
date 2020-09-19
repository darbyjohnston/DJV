// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/IDialog.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the about dialog.
        class AboutDialog : public UI::IDialog
        {
            DJV_NON_COPYABLE(AboutDialog);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            AboutDialog();

        public:
            ~AboutDialog() override;

            static std::shared_ptr<AboutDialog> create(const std::shared_ptr<System::Context>&);

        protected:
            void _initEvent(System::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

