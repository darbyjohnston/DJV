// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the DJV project.

#include <djv/App/ViewToolBar.h>
#include <djv/Models/SettingsModel.h>
#include <ftk/UI/Divider.h>
#include <ftk/Core/Format.h>

#include <djv/App/App.h>
#include <djv/App/MainWindow.h>
#include <djv/App/ViewActions.h>
#include <djv/UI/Viewport.h>

#include <ftk/UI/Action.h>
#include <ftk/UI/ChoiceToolButton.h>
#include <ftk/UI/DoubleEdit.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/ScreenshotTag.h>
#include <ftk/UI/ToolButton.h>

namespace djv
{
    namespace app
    {
        struct ViewToolBar::Private
        {
            std::shared_ptr<ftk::DoubleEdit> zoomEdit;
            std::shared_ptr<ftk::DoubleResetButton> zoomReset;

            std::shared_ptr<ftk::ChoiceToolButton> mouseToolButton;

            std::shared_ptr<ftk::Observer<std::pair<ftk::V2I, double> > > posZoomObserver;
            std::shared_ptr<ftk::Observer<std::shared_ptr<tl::Player> > > playerObserver;
            bool updating = false;
        };

        void ViewToolBar::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow,
            const std::shared_ptr<ViewActions>& viewActions,
            const std::shared_ptr<IWidget>& parent)
        {
            ToolBar::_init(context, ftk::Orientation::Horizontal, parent);
            FTK_P();

            setMarginRole(ftk::SizeRole::MarginInside);

            auto actions = viewActions->getActions();
            addAction(actions["Frame"]);

            p.zoomEdit = ftk::DoubleEdit::create(context);
            auto viewport = mainWindow->getViewport();
            p.zoomEdit->setRange(viewport->getZoomRange());
            p.zoomEdit->setStep(0.1);
            p.zoomEdit->setLargeStep(1.0);
            p.zoomEdit->setDefault(1.0);
            p.zoomEdit->setPrecision(2);
            p.zoomEdit->setBorderRole(ftk::ColorRole::None);
            p.zoomEdit->setTooltip("View zoom");
            addWidget(p.zoomEdit);

            addAction(actions["ZoomReset"]);

            // The mouse tools, after a divider: what a finger or a pen
            // does on the picture, chosen here in place of a modifier key.
            // They are one choice, so they take one place: a button for the
            // last tool chosen, on or off, and an arrow for the list. Four
            // buttons crowded a row that is short of room on a tablet.
            addWidget(ftk::Divider::create(context, ftk::Orientation::Horizontal));
            std::vector<std::shared_ptr<ftk::Action> > mouseToolActions;
            for (size_t i = 1; i < static_cast<size_t>(models::MouseTool::Count); ++i)
            {
                mouseToolActions.push_back(actions[ftk::Format("MouseTool_{0}").arg(i)]);
            }
            p.mouseToolButton = ftk::ChoiceToolButton::create(context, mouseToolActions);
            p.mouseToolButton->getMenuButton()->setTooltip("Choose the mouse tool");
            ftk::setScreenshotTag(p.mouseToolButton->getButton(), "View.MouseTool");
            ftk::setScreenshotTag(p.mouseToolButton->getMenuButton(), "View.MouseToolMenu");
            addWidget(p.mouseToolButton);
            // Listed when the tool bar does not fit, which a widget cannot be.
            for (const auto& action : mouseToolActions)
            {
                addOverflowAction(action);
            }

            std::weak_ptr<MainWindow> mainWindowWeak(mainWindow);
            p.zoomEdit->setCallback(
                [this, mainWindowWeak](double value)
                {
                    if (!_p->updating)
                    {
                        if (auto mainWindow = mainWindowWeak.lock())
                        {
                            auto viewport = mainWindow->getViewport();
                            const ftk::Box2I& g = viewport->getGeometry();
                            const ftk::V2I focus(g.w() / 2, g.h() / 2);
                            mainWindow->getViewport()->setZoom(value, focus);
                        }
                    }
                });

            p.posZoomObserver = ftk::Observer<std::pair<ftk::V2I, double> >::create(
                viewport->observeViewPosAndZoom(),
                [this](const std::pair<ftk::V2I, double>& value)
                {
                    FTK_P();
                    p.updating = true;
                    p.zoomEdit->setValue(value.second);
                    p.updating = false;
                });

            p.playerObserver = ftk::Observer<std::shared_ptr<tl::Player> >::create(
                app->observePlayer(),
                [this](const std::shared_ptr<tl::Player>& value)
                {
                    // There is no zoom to show without an image; the actions
                    // beside this are gated the same way in ViewActions.
                    _p->zoomEdit->setEnabled(
                        value && !value->getIOInfo().video.empty());
                });
        }

        ViewToolBar::ViewToolBar() :
            _p(new Private)
        {}

        ViewToolBar::~ViewToolBar()
        {}

        std::shared_ptr<ViewToolBar> ViewToolBar::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow,
            const std::shared_ptr<ViewActions>& viewActions,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ViewToolBar>(new ViewToolBar);
            out->_init(context, app, mainWindow, viewActions, parent);
            return out;
        }
    }
}
