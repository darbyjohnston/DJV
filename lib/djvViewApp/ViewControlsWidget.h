// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MDIWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the view controls widget.
        class ViewControlsWidget : public MDIWidget
        {
            DJV_NON_COPYABLE(ViewControlsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ViewControlsWidget();

        public:
            ~ViewControlsWidget() override;

            static std::shared_ptr<ViewControlsWidget> create(const std::shared_ptr<Core::Context>&);

            std::map<std::string, bool> getBellowsState() const;
            void setBellowsState(const std::map<std::string, bool>&);

        protected:
            void _initLayoutEvent(Core::Event::InitLayout&) override;

            void _initEvent(Core::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

