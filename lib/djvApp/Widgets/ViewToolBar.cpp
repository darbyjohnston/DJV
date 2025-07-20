// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/ViewToolBar.h>

#include <djvApp/Actions/ViewActions.h>
#include <djvApp/Widgets/Viewport.h>
#include <djvApp/MainWindow.h>

#include <feather-tk/ui/DoubleEdit.h>
#include <feather-tk/ui/RowLayout.h>

namespace djv
{
    namespace app
    {
        struct ViewToolBar::Private
        {
            std::shared_ptr<feather_tk::DoubleEdit> zoomEdit;
            std::shared_ptr<feather_tk::DoubleResetButton> zoomReset;

            std::shared_ptr<feather_tk::ValueObserver<std::pair<feather_tk::V2I, double> > > posZoomObserver;
            bool updating = false;
        };

        void ViewToolBar::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<MainWindow>& mainWindow,
            const std::shared_ptr<ViewActions>& viewActions,
            const std::shared_ptr<IWidget>& parent)
        {
            ToolBar::_init(context, feather_tk::Orientation::Horizontal, parent);
            FEATHER_TK_P();

            auto actions = viewActions->getActions();
            addAction(actions["Frame"]);

            p.zoomEdit = feather_tk::DoubleEdit::create(context);
            p.zoomEdit->setRange(0.000001, 100000.0);
            p.zoomEdit->setStep(1.0);
            p.zoomEdit->setLargeStep(2.0);
            p.zoomEdit->setDefaultValue(1.0);
            p.zoomEdit->setBorderRole(feather_tk::ColorRole::None);
            p.zoomEdit->setTooltip("View zoom value");
            addWidget(p.zoomEdit);

            addAction(actions["ZoomReset"]);

            std::weak_ptr<MainWindow> mainWindowWeak(mainWindow);
            p.zoomEdit->setCallback(
                [this, mainWindowWeak](double value)
                {
                    if (!_p->updating)
                    {
                        if (auto mainWindow = mainWindowWeak.lock())
                        {
                            auto viewport = mainWindow->getViewport();
                            const feather_tk::Box2I& g = viewport->getGeometry();
                            const feather_tk::V2I focus(g.w() / 2, g.h() / 2);
                            mainWindow->getViewport()->setViewZoom(value, focus);
                        }
                    }
                });

            p.posZoomObserver = feather_tk::ValueObserver<std::pair<feather_tk::V2I, double> >::create(
                mainWindow->getViewport()->observeViewPosAndZoom(),
                [this](const std::pair<feather_tk::V2I, double>& value)
                {
                    FEATHER_TK_P();
                    p.updating = true;
                    p.zoomEdit->setValue(value.second);
                    p.updating = false;
                });
        }

        ViewToolBar::ViewToolBar() :
            _p(new Private)
        {}

        ViewToolBar::~ViewToolBar()
        {}

        std::shared_ptr<ViewToolBar> ViewToolBar::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<MainWindow>& mainWindow,
            const std::shared_ptr<ViewActions>& viewActions,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ViewToolBar>(new ViewToolBar);
            out->_init(context, mainWindow, viewActions, parent);
            return out;
        }
    }
}
