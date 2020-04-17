// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MDIWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the debugging widget.
        class DebugWidget : public MDIWidget
        {
            DJV_NON_COPYABLE(DebugWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            DebugWidget();

        public:
            ~DebugWidget() override;

            static std::shared_ptr<DebugWidget> create(const std::shared_ptr<Core::Context>&);

            std::map<std::string, bool> getBellowsState() const;
            void setBellowsState(const std::map<std::string, bool>&);

        protected:
            void _initEvent(Core::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

