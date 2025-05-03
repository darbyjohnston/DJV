// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlIO/IO.h>

#include <tlCore/Path.h>

#include <dtk/ui/IWidget.h>

#include <dtk/core/LogSystem.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Status bar widget.
        class StatusBar : public dtk::IWidget
        {
            DTK_NON_COPYABLE(StatusBar);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            StatusBar();

        public:
            virtual ~StatusBar();

            //! Create a new widget.
            static std::shared_ptr<StatusBar> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;
            void mousePressEvent(dtk::MouseClickEvent&) override;
            void mouseReleaseEvent(dtk::MouseClickEvent&) override;

        private:
            void _logUpdate(const std::vector<dtk::LogItem>&);
            void _infoUpdate(const tl::file::Path&, const tl::io::Info&);
            void _deviceUpdate(bool);

            DTK_PRIVATE();
        };
    }
}
