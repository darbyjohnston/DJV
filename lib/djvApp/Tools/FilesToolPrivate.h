// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Tools/FilesTool.h>

#include <dtk/ui/IButton.h>

namespace djv
{
    namespace app
    {
        class FileButton : public dtk::IButton
        {
            DTK_NON_COPYABLE(FileButton);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<FilesModelItem>&,
                const std::shared_ptr<IWidget>& parent);

            FileButton();

        public:
            virtual ~FileButton();

            static std::shared_ptr<FileButton> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<FilesModelItem>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void tickEvent(
                bool,
                bool,
                const dtk::TickEvent&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;
            void clipEvent(const dtk::Box2I&, bool) override;
            void drawEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;
            void keyPressEvent(dtk::KeyEvent&) override;
            void keyReleaseEvent(dtk::KeyEvent&) override;

        private:
            DTK_PRIVATE();
        };
    }
}
