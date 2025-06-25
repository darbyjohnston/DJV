// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/InfoTool.h>

#include <djvApp/App.h>

#include <feather-tk/ui/Divider.h>
#include <feather-tk/ui/GridLayout.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/ScrollWidget.h>
#include <feather-tk/ui/SearchBox.h>

#include <feather-tk/core/String.h>

namespace djv
{
    namespace app
    {
        struct InfoTool::Private
        {
            tl::io::Info info;
            std::string search;

            std::shared_ptr<feather_tk::SearchBox> searchBox;
            std::shared_ptr<feather_tk::GridLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<std::shared_ptr<tl::timeline::Player> > > playerObserver;
        };

        void InfoTool::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::Info,
                "djv::app::InfoTool",
                parent);
            FEATHER_TK_P();

            p.searchBox = feather_tk::SearchBox::create(context);
            p.searchBox->setHStretch(feather_tk::Stretch::Expanding);

            p.layout = feather_tk::GridLayout::create(context);
            p.layout->setMarginRole(feather_tk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            auto scrollWidget = feather_tk::ScrollWidget::create(context);
            scrollWidget->setWidget(p.layout);
            scrollWidget->setBorder(false);
            scrollWidget->setVStretch(feather_tk::Stretch::Expanding);

            auto layout = feather_tk::VerticalLayout::create(context);
            layout->setSpacingRole(feather_tk::SizeRole::None);
            scrollWidget->setParent(layout);
            feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, layout);
            auto hLayout = feather_tk::HorizontalLayout::create(context, layout);
            hLayout->setMarginRole(feather_tk::SizeRole::MarginInside);
            hLayout->setSpacingRole(feather_tk::SizeRole::SpacingTool);
            p.searchBox->setParent(hLayout);
            _setWidget(layout);

            p.playerObserver = feather_tk::ValueObserver<std::shared_ptr<tl::timeline::Player> >::create(
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
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<InfoTool>(new InfoTool);
            out->_init(context, app, parent);
            return out;
        }

        void InfoTool::_widgetUpdate()
        {
            FEATHER_TK_P();
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
                        !feather_tk::contains(
                            tag.first,
                            p.search,
                            feather_tk::CaseCompare::Insensitive) &&
                        !feather_tk::contains(
                            tag.second,
                            p.search,
                            feather_tk::CaseCompare::Insensitive))
                    {
                        filter = true;
                    }
                    if (!filter)
                    {
                        auto label = feather_tk::Label::create(context, tag.first + ":", p.layout);
                        p.layout->setGridPos(label, row, 0);
                        label = feather_tk::Label::create(context, tag.second, p.layout);
                        p.layout->setGridPos(label, row, 1);
                        ++row;
                    }
                }
            }
        }
    }
}
