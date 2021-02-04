// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewAppSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the log system.
        class LogSystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(LogSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            LogSystem();

        public:
            ~LogSystem() override;

            static std::shared_ptr<LogSystem> create(const std::shared_ptr<System::Context>&);

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            ActionData getToolActionData() const override;
            ToolWidgetData createToolWidget(const std::shared_ptr<UI::Action>&) override;

        protected:
            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

