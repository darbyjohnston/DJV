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
        class StatusBar : public feather_tk::IWidget
        {
            FEATHER_TK_NON_COPYABLE(StatusBar);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            StatusBar();

        public:
            virtual ~StatusBar();

            //! Create a new widget.
            static std::shared_ptr<StatusBar> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const feather_tk::Box2I&) override;
            void sizeHintEvent(const feather_tk::SizeHintEvent&) override;
            void mousePressEvent(feather_tk::MouseClickEvent&) override;
            void mouseReleaseEvent(feather_tk::MouseClickEvent&) override;

        private:
            void _logUpdate(const std::vector<feather_tk::LogItem>&);
            void _infoUpdate(const tl::file::Path&, const tl::io::Info&);
            void _deviceUpdate(bool);

            FEATHER_TK_PRIVATE();
        };
    }
}
