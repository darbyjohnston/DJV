// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/InfoTool.h>

#include <djvApp/App.h>

#include <dtk/ui/Divider.h>
#include <dtk/ui/GridLayout.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>
#include <dtk/ui/SearchBox.h>

#include <dtk/core/String.h>

namespace djv
{
    namespace app
    {
        struct InfoTool::Private
        {
            tl::io::Info info;
            std::string search;

            std::shared_ptr<dtk::SearchBox> searchBox;
            std::shared_ptr<dtk::GridLayout> layout;

            std::shared_ptr<dtk::ValueObserver<std::shared_ptr<tl::timeline::Player> > > playerObserver;
        };

        void InfoTool::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::Info,
                "tl::play_app::InfoTool",
                parent);
            DTK_P();

            p.searchBox = dtk::SearchBox::create(context);
            p.searchBox->setHStretch(dtk::Stretch::Expanding);

            p.layout = dtk::GridLayout::create(context);
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            auto scrollWidget = dtk::ScrollWidget::create(context);
            scrollWidget->setWidget(p.layout);
            scrollWidget->setBorder(false);
            scrollWidget->setVStretch(dtk::Stretch::Expanding);

            auto layout = dtk::VerticalLayout::create(context);
            layout->setSpacingRole(dtk::SizeRole::None);
            scrollWidget->setParent(layout);
            dtk::Divider::create(context, dtk::Orientation::Vertical, layout);
            auto hLayout = dtk::HorizontalLayout::create(context, layout);
            hLayout->setMarginRole(dtk::SizeRole::MarginInside);
            hLayout->setSpacingRole(dtk::SizeRole::SpacingTool);
            p.searchBox->setParent(hLayout);
            _setWidget(layout);

            p.playerObserver = dtk::ValueObserver<std::shared_ptr<tl::timeline::Player> >::create(
                app->observePlayer(),
                [this](const std::shared_ptr<tl::timeline::Player>& value)
                {
                    _p->info = value ? value->getIOInfo() : tl::io::Info();
                    _widgetUpdate();
                });

            p.searchBox->setCallback(
                [this](const std::string& value)
                {
                    _p->search = value;
                    _widgetUpdate();
                });
        }

        InfoTool::InfoTool() :
            _p(new Private)
        {}

        InfoTool::~InfoTool()
        {}

        std::shared_ptr<InfoTool> InfoTool::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<InfoTool>(new InfoTool);
            out->_init(context, app, parent);
            return out;
        }

        void InfoTool::_widgetUpdate()
        {
            DTK_P();
            auto children = p.layout->getChildren();
            for (const auto& child : children)
            {
                child->setParent(nullptr);
            }
            if (auto context = getContext())
            {
                int row = 0;
                for (const auto& tag : p.info.tags)
                {
                    bool filter = false;
                    if (!p.search.empty() &&
                        !dtk::contains(
                            tag.first,
                            p.search,
                            dtk::CaseCompare::Insensitive) &&
                        !dtk::contains(
                            tag.second,
                            p.search,
                            dtk::CaseCompare::Insensitive))
                    {
                        filter = true;
                    }
                    if (!filter)
                    {
                        auto label = dtk::Label::create(context, tag.first + ":", p.layout);
                        p.layout->setGridPos(label, row, 0);
                        label = dtk::Label::create(context, tag.second, p.layout);
                        p.layout->setGridPos(label, row, 1);
                        ++row;
                    }
                }
            }
        }
    }
}
