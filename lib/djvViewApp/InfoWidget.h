// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MDIWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the information widget.
        class InfoWidget : public MDIWidget
        {
            DJV_NON_COPYABLE(InfoWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            InfoWidget();

        public:
            ~InfoWidget() override;

            static std::shared_ptr<InfoWidget> create(const std::shared_ptr<Core::Context>&);

            bool getBellowsState() const;
            void setBellowsState(bool);

        protected:
            void _initLayoutEvent(djv::Core::Event::InitLayout&) override;

            void _initEvent(Core::Event::Init &) override;

        private:
            void _expandAll();
            void _collapseAll();
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

