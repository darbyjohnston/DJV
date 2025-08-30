// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlIO/IO.h>

#include <tlCore/Path.h>

#include <feather-tk/ui/IWidget.h>

#include <feather-tk/core/LogSystem.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Status bar widget.
        class StatusBar : public ftk::IWidget
        {
            FTK_NON_COPYABLE(StatusBar);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            StatusBar();

        public:
            virtual ~StatusBar();

            //! Create a new widget.
            static std::shared_ptr<StatusBar> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;
            void mousePressEvent(ftk::MouseClickEvent&) override;
            void mouseReleaseEvent(ftk::MouseClickEvent&) override;

        private:
            void _logUpdate(const std::vector<ftk::LogItem>&);
            void _infoUpdate(const tl::file::Path&, const tl::io::Info&);
            void _deviceUpdate(bool);

            FTK_PRIVATE();
        };
    }
}
