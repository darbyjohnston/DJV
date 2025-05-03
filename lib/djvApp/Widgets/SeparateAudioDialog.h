// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlCore/Path.h>

#include <dtk/ui/IDialog.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Separate audio dialog.
        class SeparateAudioDialog : public dtk::IDialog
        {
            DTK_NON_COPYABLE(SeparateAudioDialog);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<IWidget>& parent);

            SeparateAudioDialog();

        public:
            virtual ~SeparateAudioDialog();

            static std::shared_ptr<SeparateAudioDialog> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            //! Set the callback.
            void setCallback(const std::function<void(
                const tl::file::Path&,
                const tl::file::Path&)>&);

        private:
            DTK_PRIVATE();
        };
    }
}
