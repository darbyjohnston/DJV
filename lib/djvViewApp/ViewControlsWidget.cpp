// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewControlsWidget.h>

#include <djvViewApp/ViewControlsWidgetPrivate.h>

#include <djvUI/Bellows.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ViewControlsWidget::Private
        {
            std::shared_ptr<ViewControlsViewWidget> viewWidget;
            std::shared_ptr<ViewControlsGridWidget> gridWidget;
            std::shared_ptr<ViewControlsHUDWidget> hudWidget;
            std::shared_ptr<ViewControlsBackgroundWidget> backgroundWidget;
            std::shared_ptr<ViewControlsBorderWidget> borderWidget;
            std::map<std::string, std::shared_ptr<UI::Bellows> > bellows;
            std::shared_ptr<UI::VerticalLayout> layout;
        };

        void ViewControlsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ViewControlsWidget");

            p.viewWidget = ViewControlsViewWidget::create(context);
            p.gridWidget = ViewControlsGridWidget::create(context);
            p.hudWidget = ViewControlsHUDWidget::create(context);
            p.backgroundWidget = ViewControlsBackgroundWidget::create(context);
            p.borderWidget = ViewControlsBorderWidget::create(context);

            p.bellows["View"] = UI::Bellows::create(context);
            p.bellows["View"]->addChild(p.viewWidget);
            p.bellows["Grid"] = UI::Bellows::create(context);
            p.bellows["Grid"]->addButtonWidget(p.gridWidget->getEnabledButton());
            p.bellows["Grid"]->addChild(p.gridWidget);
            p.bellows["HUD"] = UI::Bellows::create(context);
            p.bellows["HUD"]->addButtonWidget(p.hudWidget->getEnabledButton());
            p.bellows["HUD"]->addChild(p.hudWidget);
            p.bellows["Background"] = UI::Bellows::create(context);
            p.bellows["Background"]->addChild(p.backgroundWidget);
            p.bellows["Border"] = UI::Bellows::create(context);
            p.bellows["Border"]->addButtonWidget(p.borderWidget->getEnabledButton());
            p.bellows["Border"]->addChild(p.borderWidget);
            
            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.bellows["View"]);
            p.layout->addChild(p.bellows["Grid"]);
            p.layout->addChild(p.bellows["HUD"]);
            p.layout->addChild(p.bellows["Background"]);
            p.layout->addChild(p.bellows["Border"]);
            addChild(p.layout);
        }

        ViewControlsWidget::ViewControlsWidget() :
            _p(new Private)
        {}

        ViewControlsWidget::~ViewControlsWidget()
        {}

        std::shared_ptr<ViewControlsWidget> ViewControlsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewControlsWidget>(new ViewControlsWidget);
            out->_init(context);
            return out;
        }

        std::map<std::string, bool> ViewControlsWidget::getBellowsState() const
        {
            DJV_PRIVATE_PTR();
            std::map<std::string, bool> out;
            for (const auto& i : p.bellows)
            {
                out[i.first] = i.second->isOpen();
            }
            return out;
        }

        void ViewControlsWidget::setBellowsState(const std::map<std::string, bool>& value)
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : value)
            {
                const auto j = p.bellows.find(i.first);
                if (j != p.bellows.end())
                {
                    j->second->setOpen(i.second, false);
                }
            }
        }

        void ViewControlsWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ViewControlsWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ViewControlsWidget::_initEvent(System::Event::Init & event)
        {
            Widget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.bellows["View"]->setText(_getText(DJV_TEXT("view")));
                p.bellows["Grid"]->setText(_getText(DJV_TEXT("view_grid")));
                p.bellows["HUD"]->setText(_getText(DJV_TEXT("view_hud")));
                p.bellows["Background"]->setText(_getText(DJV_TEXT("view_background")));
                p.bellows["Border"]->setText(_getText(DJV_TEXT("view_border")));
            }
        }

    } // namespace ViewApp
} // namespace djv

