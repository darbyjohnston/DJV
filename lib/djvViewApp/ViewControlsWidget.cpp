// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewControlsWidget.h>

#include <djvViewApp/ViewControlsWidgetPrivate.h>

#include <djvUI/Bellows.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

#include <djvCore/Context.h>

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
            std::shared_ptr<UI::LabelSizeGroup> sizeGroup;
            std::map<std::string, std::shared_ptr<UI::Bellows> > bellows;
        };

        void ViewControlsWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ViewControlsWidget");

            p.sizeGroup = UI::LabelSizeGroup::create();

            p.viewWidget = ViewControlsViewWidget::create(context);
            p.viewWidget->setLabelSizeGroup(p.sizeGroup);
            p.gridWidget = ViewControlsGridWidget::create(context);
            p.gridWidget->setLabelSizeGroup(p.sizeGroup);
            p.hudWidget = ViewControlsHUDWidget::create(context);
            p.hudWidget->setLabelSizeGroup(p.sizeGroup);
            p.backgroundWidget = ViewControlsBackgroundWidget::create(context);
            p.backgroundWidget->setLabelSizeGroup(p.sizeGroup);
            p.borderWidget = ViewControlsBorderWidget::create(context);
            p.borderWidget->setLabelSizeGroup(p.sizeGroup);

            p.bellows["View"] = UI::Bellows::create(context);
            p.bellows["View"]->addChild(p.viewWidget);
            p.bellows["Grid"] = UI::Bellows::create(context);
            p.bellows["Grid"]->addWidget(p.gridWidget->getEnabledButton());
            p.bellows["Grid"]->addChild(p.gridWidget);
            p.bellows["HUD"] = UI::Bellows::create(context);
            p.bellows["HUD"]->addWidget(p.hudWidget->getEnabledButton());
            p.bellows["HUD"]->addChild(p.hudWidget);
            p.bellows["Background"] = UI::Bellows::create(context);
            p.bellows["Background"]->addChild(p.backgroundWidget);
            p.bellows["Border"] = UI::Bellows::create(context);
            p.bellows["Border"]->addWidget(p.borderWidget->getEnabledButton());
            p.bellows["Border"]->addChild(p.borderWidget);
            
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.bellows["View"]);
            vLayout->addChild(p.bellows["Grid"]);
            vLayout->addChild(p.bellows["HUD"]);
            vLayout->addChild(p.bellows["Background"]);
            vLayout->addChild(p.bellows["Border"]);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setBackgroundRole(UI::ColorRole::Background);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(vLayout);
            addChild(scrollWidget);
        }

        ViewControlsWidget::ViewControlsWidget() :
            _p(new Private)
        {}

        ViewControlsWidget::~ViewControlsWidget()
        {}

        std::shared_ptr<ViewControlsWidget> ViewControlsWidget::create(const std::shared_ptr<Core::Context>& context)
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

        void ViewControlsWidget::_initLayoutEvent(Event::InitLayout&)
        {
            _p->sizeGroup->calcMinimumSize();
        }

        void ViewControlsWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("view_controls")));
                p.bellows["View"]->setText(_getText(DJV_TEXT("view")));
                p.bellows["Grid"]->setText(_getText(DJV_TEXT("view_grid")));
                p.bellows["HUD"]->setText(_getText(DJV_TEXT("view_hud")));
                p.bellows["Background"]->setText(_getText(DJV_TEXT("view_background")));
                p.bellows["Border"]->setText(_getText(DJV_TEXT("view_border")));
            }
        }

    } // namespace ViewApp
} // namespace djv

