// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/ToolsWidget.h>

#include <djvApp/Tools/AudioTool.h>
#include <djvApp/Tools/ColorControlsTool.h>
#include <djvApp/Tools/ColorPickerTool.h>
#include <djvApp/Tools/DevicesTool.h>
#include <djvApp/Tools/ExportTool.h>
#include <djvApp/Tools/FilesTool.h>
#include <djvApp/Tools/InfoTool.h>
#include <djvApp/Tools/MessagesTool.h>
#include <djvApp/Tools/SettingsTool.h>
#include <djvApp/Tools/SystemLogTool.h>
#include <djvApp/Tools/ViewTool.h>
#include <djvApp/App.h>

#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/StackLayout.h>

namespace djv
{
    namespace app
    {
        struct ToolsWidget::Private
        {
            std::map<Tool, std::shared_ptr<IToolWidget> > toolWidgets;
            std::shared_ptr<feather_tk::StackLayout> layout;
            std::shared_ptr<feather_tk::ValueObserver<Tool> > activeObserver;
        };

        void ToolsWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(
                context,
                "djv::app::ToolsWidget",
                parent);
            FEATHER_TK_P();

            p.toolWidgets[Tool::Audio] = AudioTool::create(context, app);
            p.toolWidgets[Tool::ColorPicker] = ColorPickerTool::create(context, app);
            p.toolWidgets[Tool::ColorControls] = ColorControlsTool::create(context, app);
            p.toolWidgets[Tool::Devices] = DevicesTool::create(context, app);
            p.toolWidgets[Tool::Export] = ExportTool::create(context, app);
            p.toolWidgets[Tool::Files] = FilesTool::create(context, app);
            p.toolWidgets[Tool::Info] = InfoTool::create(context, app);
            p.toolWidgets[Tool::Messages] = MessagesTool::create(context, app);
            p.toolWidgets[Tool::Settings] = SettingsTool::create(context, app);
            p.toolWidgets[Tool::SystemLog] = SystemLogTool::create(context, app);
            p.toolWidgets[Tool::View] = ViewTool::create(context, app);

            p.layout = feather_tk::StackLayout::create(context, shared_from_this());
            for (const auto& widget : p.toolWidgets)
            {
                widget.second->setParent(p.layout);
            }

            p.activeObserver = feather_tk::ValueObserver<Tool>::create(
                app->getToolsModel()->observeActiveTool(),
                [this](Tool value)
                {
                    FEATHER_TK_P();
                    auto i = p.toolWidgets.find(value);
                    p.layout->setCurrentWidget(i != p.toolWidgets.end() ? i->second : nullptr);
                    setVisible(value != Tool::None);
                });
        }

        ToolsWidget::ToolsWidget() :
            _p(new Private)
        {}

        ToolsWidget::~ToolsWidget()
        {}

        std::shared_ptr<ToolsWidget> ToolsWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ToolsWidget>(new ToolsWidget);
            out->_init(context, app, mainWindow, parent);
            return out;
        }

        void ToolsWidget::setGeometry(const feather_tk::Box2I & value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void ToolsWidget::sizeHintEvent(const feather_tk::SizeHintEvent & event)
        {
            IWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }
    }
}
