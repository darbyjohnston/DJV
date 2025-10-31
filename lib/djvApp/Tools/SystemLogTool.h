// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the DJV project.

#pragma once

#include <djvApp/Tools/IToolWidget.h>

namespace djv
{
    namespace app
    {
        class App;

        //! System log tool.
        //! 
        //! \todo Add an option to automatically scroll to the bottom when
        //! new log items are received.
        class SystemLogTool : public IToolWidget
        {
            FTK_NON_COPYABLE(SystemLogTool);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            SystemLogTool();

        public:
            virtual ~SystemLogTool();

            static std::shared_ptr<SystemLogTool> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

        private:
            FTK_PRIVATE();
        };
    }
}
