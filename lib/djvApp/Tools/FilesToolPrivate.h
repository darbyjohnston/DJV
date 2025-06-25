// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Tools/FilesTool.h>

#include <feather-tk/ui/IButton.h>

namespace djv
{
    namespace app
    {
        class FileButton : public feather_tk::IButton
        {
            FEATHER_TK_NON_COPYABLE(FileButton);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<FilesModelItem>&,
                const std::shared_ptr<IWidget>& parent);

            FileButton();

        public:
            virtual ~FileButton();

            static std::shared_ptr<FileButton> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<FilesModelItem>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void tickEvent(
                bool,
                bool,
                const feather_tk::TickEvent&) override;
            void sizeHintEvent(const feather_tk::SizeHintEvent&) override;
            void clipEvent(const feather_tk::Box2I&, bool) override;
            void drawEvent(const feather_tk::Box2I&, const feather_tk::DrawEvent&) override;
            void keyPressEvent(feather_tk::KeyEvent&) override;
            void keyReleaseEvent(feather_tk::KeyEvent&) override;

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
