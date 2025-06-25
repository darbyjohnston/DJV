// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/TabBar.h>

#include <djvApp/Models/FilesModel.h>
#include <djvApp/App.h>

#include <feather-tk/ui/TabBar.h>
#include <feather-tk/core/String.h>

namespace djv
{
    namespace app
    {
        struct TabBar::Private
        {
            int aIndex = -1;
            std::shared_ptr<feather_tk::TabBar> tabBar;
            std::shared_ptr<feather_tk::ListObserver<std::shared_ptr<FilesModelItem> > > filesObserver;
            std::shared_ptr<feather_tk::ValueObserver<int> > aIndexObserver;
        };

        void TabBar::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(context, "tl::play::TabBar", parent);
            FEATHER_TK_P();

            p.tabBar = feather_tk::TabBar::create(context, shared_from_this());
            p.tabBar->setTabsClosable(true);

            std::weak_ptr<App> appWeak(app);
            p.tabBar->setCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getFilesModel()->setA(value);
                    }
                });
            p.tabBar->setTabCloseCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getFilesModel()->close(value);
                    }
                });

            p.filesObserver = feather_tk::ListObserver<std::shared_ptr<FilesModelItem> >::create(
                app->getFilesModel()->observeFiles(),
                [this](const std::vector<std::shared_ptr<FilesModelItem> >& value)
                {
                    FEATHER_TK_P();
                    p.tabBar->clearTabs();
                    for (const auto& item : value)
                    {
                        p.tabBar->addTab(
                            feather_tk::elide(item->path.get(-1, tl::file::PathType::FileName)),
                            item->path.get());
                    }
                    p.tabBar->setCurrentTab(p.aIndex);
                });

            p.aIndexObserver = feather_tk::ValueObserver<int>::create(
                app->getFilesModel()->observeAIndex(),
                [this](int value)
                {
                    FEATHER_TK_P();
                    p.aIndex = value;
                    p.tabBar->setCurrentTab(value);
                });
        }

        TabBar::TabBar() :
            _p(new Private)
        {}

        TabBar::~TabBar()
        {}

        std::shared_ptr<TabBar> TabBar::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<TabBar>(new TabBar);
            out->_init(context, app, parent);
            return out;
        }

        void TabBar::setGeometry(const feather_tk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->tabBar->setGeometry(value);
        }

        void TabBar::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            IWidget::sizeHintEvent(event);
            _setSizeHint(_p->tabBar->getSizeHint());
        }
    }
}
