// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <ftk/UI/IDialog.h>

namespace djv
{
    namespace app
    {
        class App;

        //! About dialog.
        class AboutDialog : public ftk::IDialog
        {
            FTK_NON_COPYABLE(AboutDialog);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            AboutDialog();

        public:
            virtual ~AboutDialog();

            static std::shared_ptr<AboutDialog> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

        private:
            FTK_PRIVATE();
        };
    }
}
