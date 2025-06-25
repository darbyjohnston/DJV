// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Models/ToolsModel.h>

#include <feather-tk/ui/Settings.h>
#include <feather-tk/core/Error.h>
#include <feather-tk/core/String.h>

#include <array>
#include <iostream>
#include <sstream>

namespace djv
{
    namespace app
    {
        FEATHER_TK_ENUM_IMPL(
            Tool,
            "None",
            "Files",
            "Export",
            "View",
            "ColorPicker",
            "ColorControls",
            "Info",
            "Audio",
            "Devices",
            "Settings",
            "Messages",
            "SystemLog");

        std::string getText(Tool value)
        {
            const std::array<std::string, static_cast<size_t>(Tool::Count)> data =
            {
                "",
                "Files",
                "Export",
                "View",
                "Color Picker",
                "Color Controls",
                "Information",
                "Audio",
                "Devices",
                "Settings",
                "Messages",
                "System Log"
            };
            return data[static_cast<size_t>(value)];
        }

        std::string getIcon(Tool value)
        {
            const std::array<std::string, static_cast<size_t>(Tool::Count)> data =
            {
                "",
                "Files",
                "Export",
                "View",
                "ColorPicker",
                "ColorControls",
                "Info",
                "Audio",
                "Devices",
                "Settings",
                "Messages",
                ""
            };
            return data[static_cast<size_t>(value)];
        }

        std::vector<Tool> getToolsInToolbar()
        {
            const std::vector<Tool> out
            {
                Tool::Files,
                Tool::Export,
                Tool::View,
                Tool::ColorPicker,
                Tool::ColorControls,
                Tool::Info,
                Tool::Audio,
                Tool::Devices,
                Tool::Settings,
                Tool::Messages
            };
            return out;
        }

        struct ToolsModel::Private
        {
            std::shared_ptr<feather_tk::Settings> settings;

            std::shared_ptr<feather_tk::ObservableValue<Tool> > activeTool;
        };

        void ToolsModel::_init(const std::shared_ptr<feather_tk::Settings>& settings)
        {
            FEATHER_TK_P();

            p.settings = settings;

            std::string s;
            p.settings->get("/Tools/Tool", s);
            Tool tool = Tool::None;
            from_string(s, tool);
            p.activeTool = feather_tk::ObservableValue<Tool>::create(tool);
        }

        ToolsModel::ToolsModel() :
            _p(new Private)
        {}

        ToolsModel::~ToolsModel()
        {
            FEATHER_TK_P();
            p.settings->set("/Tools/Tool", to_string(p.activeTool->get()));
        }

        std::shared_ptr<ToolsModel> ToolsModel::create(const std::shared_ptr<feather_tk::Settings>& settings)
        {
            auto out = std::shared_ptr<ToolsModel>(new ToolsModel);
            out->_init(settings);
            return out;
        }

        Tool ToolsModel::getActiveTool() const
        {
            return _p->activeTool->get();
        }

        std::shared_ptr<feather_tk::ObservableValue<Tool> > ToolsModel::observeActiveTool() const
        {
            return _p->activeTool;
        }

        void ToolsModel::setActiveTool(Tool value)
        {
            _p->activeTool->setIfChanged(value);
        }
    }
}
