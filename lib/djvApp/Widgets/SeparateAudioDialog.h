// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlCore/Path.h>

#include <ftk/UI/IDialog.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Separate audio widget.
        class SeparateAudioWidget : public ftk::IMouseWidget
        {
            FTK_NON_COPYABLE(SeparateAudioWidget);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<IWidget>& parent);

            SeparateAudioWidget();

        public:
            virtual ~SeparateAudioWidget();

            static std::shared_ptr<SeparateAudioWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setCallback(const std::function<void(
                const tl::file::Path&,
                const tl::file::Path&)>&);

            void setCancelCallback(const std::function<void(void)>&);

            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;

        private:
            FTK_PRIVATE();
        };

        //! Separate audio dialog.
        class SeparateAudioDialog : public ftk::IDialog
        {
            FTK_NON_COPYABLE(SeparateAudioDialog);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<IWidget>& parent);

            SeparateAudioDialog();

        public:
            virtual ~SeparateAudioDialog();

            static std::shared_ptr<SeparateAudioDialog> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            //! Set the callback.
            void setCallback(const std::function<void(
                const tl::file::Path&,
                const tl::file::Path&)>&);

        private:
            FTK_PRIVATE();
        };
    }
}
