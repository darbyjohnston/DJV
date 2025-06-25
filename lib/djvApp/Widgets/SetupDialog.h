// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <feather-tk/ui/IDialog.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Setup start widget.
        class SetupStartWidget : public feather_tk::IWidget
        {
            FEATHER_TK_NON_COPYABLE(SetupStartWidget);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<feather_tk::IWidget>& parent);

            SetupStartWidget();

        public:
            virtual ~SetupStartWidget();

            static std::shared_ptr<SetupStartWidget> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<feather_tk::IWidget>& parent = nullptr);

            void setGeometry(const feather_tk::Box2I&) override;
            void sizeHintEvent(const feather_tk::SizeHintEvent&) override;

            FEATHER_TK_PRIVATE();
        };

        //! Setup dialog.
        class SetupDialog : public feather_tk::IDialog
        {
            FEATHER_TK_NON_COPYABLE(SetupDialog);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            SetupDialog();

        public:
            virtual ~SetupDialog();

            static std::shared_ptr<SetupDialog> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
