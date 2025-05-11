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

        //! Setup home widget.
        class SetupHomeWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(SetupHomeWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<dtk::IWidget>& parent);

            SetupHomeWidget();

        public:
            virtual ~SetupHomeWidget();

            static std::shared_ptr<SetupHomeWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<dtk::IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

            DTK_PRIVATE();
        };

        //! Setup dialog.
        class SetupDialog : public dtk::IDialog
        {
            DTK_NON_COPYABLE(SetupDialog);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            SetupDialog();

        public:
            virtual ~SetupDialog();

            static std::shared_ptr<SetupDialog> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

        private:
            DTK_PRIVATE();
        };
    }
}
