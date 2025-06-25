// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/SystemLogTool.h>

#include <feather-tk/ui/Divider.h>
#include <feather-tk/ui/IClipboard.h>
#include <feather-tk/ui/IWindow.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/ScrollWidget.h>
#include <feather-tk/ui/ToolButton.h>
#include <feather-tk/core/Context.h>
#include <feather-tk/core/Format.h>
#include <feather-tk/core/String.h>

namespace djv
{
    namespace app
    {
        namespace
        {
            const int messagesMax = 100;
        }

        struct SystemLogTool::Private
        {
            std::list<std::string> messages;
            std::shared_ptr<feather_tk::Label> label;
            std::shared_ptr<feather_tk::ScrollWidget> scrollWidget;
            std::shared_ptr<feather_tk::ToolButton> copyButton;
            std::shared_ptr<feather_tk::ToolButton> clearButton;
            std::shared_ptr<feather_tk::VerticalLayout> layout;
            std::shared_ptr<feather_tk::ListObserver<feather_tk::LogItem> > logObserver;
        };

        void SystemLogTool::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::SystemLog,
                "djv::app::SystemLogTool",
                parent);
            FEATHER_TK_P();

            p.label = feather_tk::Label::create(context);
            p.label->setFontRole(feather_tk::FontRole::Mono);
            p.label->setMarginRole(feather_tk::SizeRole::MarginSmall);
            p.label->setVAlign(feather_tk::VAlign::Top);

            p.scrollWidget = feather_tk::ScrollWidget::create(context);
            p.scrollWidget->setWidget(p.label);
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setVStretch(feather_tk::Stretch::Expanding);

            p.copyButton = feather_tk::ToolButton::create(context, "Copy");

            p.clearButton = feather_tk::ToolButton::create(context, "Clear");

            p.layout = feather_tk::VerticalLayout::create(context);
            p.layout->setSpacingRole(feather_tk::SizeRole::None);
            p.scrollWidget->setParent(p.layout);
            feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, p.layout);
            auto hLayout = feather_tk::HorizontalLayout::create(context, p.layout);
            hLayout->setMarginRole(feather_tk::SizeRole::MarginInside);
            hLayout->setSpacingRole(feather_tk::SizeRole::SpacingTool);
            p.copyButton->setParent(hLayout);
            p.clearButton->setParent(hLayout);
            _setWidget(p.layout);

            p.copyButton->setClickedCallback(
                [this]
                {
                    if (auto window = getWindow())
                    {
                        if (auto clipboard = window->getClipboard())
                        {
                            const std::string text = feather_tk::join(
                                { std::begin(_p->messages), std::end(_p->messages) },
                                '\n');
                            clipboard->setText(text);
                        }
                    }
                });

            p.clearButton->setClickedCallback(
                [this]
                {
                    _p->messages.clear();
                    _p->label->setText(std::string());
                });

            p.logObserver = feather_tk::ListObserver<feather_tk::LogItem>::create(
                context->getLogSystem()->observeLogItems(),
                [this](const std::vector<feather_tk::LogItem>& value)
                {
                    for (const auto& i : value)
                    {
                        _p->messages.push_back(feather_tk::toString(i));
                    }
                    while (_p->messages.size() > messagesMax)
                    {
                        _p->messages.pop_front();
                    }
                    _p->label->setText(feather_tk::join(
                        { std::begin(_p->messages), std::end(_p->messages) },
                        '\n'));
                });
        }

        SystemLogTool::SystemLogTool() :
            _p(new Private)
        {}

        SystemLogTool::~SystemLogTool()
        {}

        std::shared_ptr<SystemLogTool> SystemLogTool::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<SystemLogTool>(new SystemLogTool);
            out->_init(context, app, parent);
            return out;
        }
    }
}
