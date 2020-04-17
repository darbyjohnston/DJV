// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! This struct provides tool action data.
        struct ToolActionData
        {
            std::shared_ptr<UI::Action> action;
            std::string sortKey;
        };

        //! This class provides the base functionality for tool systems.
        class IToolSystem : public IViewSystem
        {
            DJV_NON_COPYABLE(IToolSystem);

        protected:
            void _init(const std::string & name, const std::shared_ptr<Core::Context>&);
            IToolSystem();

        public:
            ~IToolSystem() override;

            virtual ToolActionData getToolAction() const = 0;

            virtual void setCurrentTool(bool) = 0;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

