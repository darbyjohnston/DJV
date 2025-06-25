// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/IToolWidget.h>

#include <djvApp/App.h>

#include <feather-tk/ui/Divider.h>
#include <feather-tk/ui/Icon.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/ToolButton.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/Settings.h>
#include <feather-tk/core/Format.h>

namespace djv
{
    namespace app
    {
        struct IToolWidget::Private
        {
            std::shared_ptr<feather_tk::Settings> settings;

            Tool tool;
            
            std::shared_ptr<feather_tk::Icon> icon;
            std::shared_ptr<feather_tk::Label> label;
            std::shared_ptr<feather_tk::ToolButton> closeButton;
            std::shared_ptr<feather_tk::VerticalLayout> toolLayout;
            std::shared_ptr<feather_tk::VerticalLayout> layout;
        };

        void IToolWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            Tool tool,
            const std::string& objectName,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(context, objectName, parent);
            FEATHER_TK_P();

            _app = app;

            p.settings = app->getSettings();

            p.tool = tool;

            p.icon = feather_tk::Icon::create(context, getIcon(tool));
            p.icon->setMarginRole(feather_tk::SizeRole::MarginSmall);

            p.label = feather_tk::Label::create(context, getText(tool));
            p.label->setMarginRole(feather_tk::SizeRole::MarginSmall);
            p.label->setHStretch(feather_tk::Stretch::Expanding);

            p.closeButton = feather_tk::ToolButton::create(context);
            p.closeButton->setIcon("Close");

            p.layout = feather_tk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(feather_tk::SizeRole::None);
            auto hLayout = feather_tk::HorizontalLayout::create(context, p.layout);
            hLayout->setSpacingRole(feather_tk::SizeRole::None);
            p.icon->setParent(hLayout);
            p.label->setParent(hLayout);
            p.closeButton->setParent(hLayout);
            feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, p.layout);
            p.toolLayout = feather_tk::VerticalLayout::create(context, p.layout);
            p.toolLayout->setSpacingRole(feather_tk::SizeRole::None);
            p.toolLayout->setHStretch(feather_tk::Stretch::Expanding);
            p.toolLayout->setVStretch(feather_tk::Stretch::Expanding);

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

        void IToolWidget::setGeometry(const feather_tk::Box2I & value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void IToolWidget::sizeHintEvent(const feather_tk::SizeHintEvent & event)
        {
            IWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void IToolWidget::_loadSettings(const std::map<std::string, std::shared_ptr<feather_tk::Bellows> >& value)
        {
            FEATHER_TK_P();
            nlohmann::json json;
            p.settings->get(feather_tk::Format("/{0}/Bellows").arg(getLabel(p.tool)), json);
            for (auto i = json.begin(); i != json.end(); ++i)
            {
                auto j = value.find(i.key());
                if (j != value.end() && i.value().is_boolean())
                {
                    j->second->setOpen(i.value().get<bool>());
                }
            }
        }

        void IToolWidget::_saveSettings(const std::map<std::string, std::shared_ptr<feather_tk::Bellows> >& value)
        {
            FEATHER_TK_P();
            nlohmann::json json;
            for (const auto& i : value)
            {
                json[i.first] = i.second->isOpen();
            }
            p.settings->set(feather_tk::Format("/{0}/Bellows").arg(getLabel(p.tool)), json);
        }

        void IToolWidget::_setWidget(const std::shared_ptr<feather_tk::IWidget>& value)
        {
            value->setHStretch(feather_tk::Stretch::Expanding);
            value->setVStretch(feather_tk::Stretch::Expanding);
            value->setParent(_p->toolLayout);
        }
    }
}
