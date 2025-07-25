// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <feather-tk/ui/Event.h>
#include <feather-tk/core/ObservableValue.h>

namespace feather_tk
{
    class Settings;
}

namespace djv
{
    namespace app
    {
        //! Tools.
        enum class Tool
        {
            None,
            Files,
            Export,
            View,
            Color,
            ColorPicker,
            Info,
            Audio,
            Devices,
            Settings,
            Messages,
            SystemLog,

            Count,
            First = None
        };
        FEATHER_TK_ENUM(Tool);

        //! Get the tool text.
        std::string getText(Tool);

        //! Get the tool icon.
        std::string getIcon(Tool);

        //! Get the tools shown in the toolbar.
        std::vector<Tool> getToolsInToolbar();

        //! Tools model.
        class ToolsModel : public std::enable_shared_from_this<ToolsModel>
        {
            FEATHER_TK_NON_COPYABLE(ToolsModel);

        protected:
            void _init(const std::shared_ptr<feather_tk::Settings>&);

            ToolsModel();

        public:
            ~ToolsModel();

            //! Create a new model.
            static std::shared_ptr<ToolsModel> create(
                const std::shared_ptr<feather_tk::Settings>&);

            //! Get the active tool.
            Tool getActiveTool() const;

            //! Observe the active tool.
            std::shared_ptr<feather_tk::ObservableValue<Tool> > observeActiveTool() const;

            //! Set the active tool.
            void setActiveTool(Tool);

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
