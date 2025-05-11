// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/ui/IDialog.h>

namespace djv
{
    namespace app
    {
        class App;

        //! About dialog.
        class AboutDialog : public dtk::IDialog
        {
            DTK_NON_COPYABLE(AboutDialog);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            AboutDialog();

        public:
            virtual ~AboutDialog();

            static std::shared_ptr<AboutDialog> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

        private:
            DTK_PRIVATE();
        };
    }
}
