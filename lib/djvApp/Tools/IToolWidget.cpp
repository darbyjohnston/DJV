// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/IToolWidget.h>

#include <djvApp/App.h>

#include <dtk/ui/Divider.h>
#include <dtk/ui/Icon.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/ToolButton.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/Settings.h>
#include <dtk/core/Format.h>

namespace djv
{
    namespace app
    {
        struct IToolWidget::Private
        {
            std::shared_ptr<dtk::Settings> settings;

            Tool tool;
            
            std::shared_ptr<dtk::Icon> icon;
            std::shared_ptr<dtk::Label> label;
            std::shared_ptr<dtk::ToolButton> closeButton;
            std::shared_ptr<dtk::VerticalLayout> toolLayout;
            std::shared_ptr<dtk::VerticalLayout> layout;
        };

        void IToolWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            Tool tool,
            const std::string& objectName,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(context, objectName, parent);
            DTK_P();

            _app = app;

            p.settings = app->getSettings();

            p.tool = tool;

            p.icon = dtk::Icon::create(context, getIcon(tool));
            p.icon->setMarginRole(dtk::SizeRole::MarginSmall);

            p.label = dtk::Label::create(context, getText(tool));
            p.label->setMarginRole(dtk::SizeRole::MarginSmall);
            p.label->setHStretch(dtk::Stretch::Expanding);

            p.closeButton = dtk::ToolButton::create(context);
            p.closeButton->setIcon("Close");

            p.layout = dtk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(dtk::SizeRole::None);
            auto hLayout = dtk::HorizontalLayout::create(context, p.layout);
            hLayout->setSpacingRole(dtk::SizeRole::None);
            p.icon->setParent(hLayout);
            p.label->setParent(hLayout);
            p.closeButton->setParent(hLayout);
            dtk::Divider::create(context, dtk::Orientation::Vertical, p.layout);
            p.toolLayout = dtk::VerticalLayout::create(context, p.layout);
            p.toolLayout->setSpacingRole(dtk::SizeRole::None);
            p.toolLayout->setHStretch(dtk::Stretch::Expanding);
            p.toolLayout->setVStretch(dtk::Stretch::Expanding);

            auto appWeak = std::weak_ptr<App>(app);
            p.closeButton->setClickedCallback(
                [appWeak, tool]
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getToolsModel()->setActiveTool(Tool::None);
                    }
                });
        }

        IToolWidget::IToolWidget() :
            _p(new Private)
        {}

        IToolWidget::~IToolWidget()
        {}

        void IToolWidget::setGeometry(const dtk::Box2I & value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void IToolWidget::sizeHintEvent(const dtk::SizeHintEvent & event)
        {
            IWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void IToolWidget::_loadSettings(const std::map<std::string, std::shared_ptr<dtk::Bellows> >& value)
        {
            DTK_P();
            nlohmann::json json;
            p.settings->get(dtk::Format("/{0}/Bellows").arg(getLabel(p.tool)), json);
            for (auto i = json.begin(); i != json.end(); ++i)
            {
                auto j = value.find(i.key());
                if (j != value.end() && i.value().is_boolean())
                {
                    j->second->setOpen(i.value().get<bool>());
                }
            }
        }

        void IToolWidget::_saveSettings(const std::map<std::string, std::shared_ptr<dtk::Bellows> >& value)
        {
            DTK_P();
            nlohmann::json json;
            for (const auto& i : value)
            {
                json[i.first] = i.second->isOpen();
            }
            p.settings->set(dtk::Format("/{0}/Bellows").arg(getLabel(p.tool)), json);
        }

        void IToolWidget::_setWidget(const std::shared_ptr<dtk::IWidget>& value)
        {
            value->setHStretch(dtk::Stretch::Expanding);
            value->setVStretch(dtk::Stretch::Expanding);
            value->setParent(_p->toolLayout);
        }
    }
}
