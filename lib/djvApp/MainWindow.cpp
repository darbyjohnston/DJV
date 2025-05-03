// SPDX-License-Identifier: BSD-3-Clause
// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/MainWindow.h>

#include <djvApp/Actions/AudioActions.h>
#include <djvApp/Actions/CompareActions.h>
#include <djvApp/Actions/FileActions.h>
#include <djvApp/Actions/FrameActions.h>
#include <djvApp/Actions/PlaybackActions.h>
#include <djvApp/Actions/TimelineActions.h>
#include <djvApp/Actions/ToolsActions.h>
#include <djvApp/Actions/ViewActions.h>
#include <djvApp/Actions/WindowActions.h>
#include <djvApp/Menus/AudioMenu.h>
#include <djvApp/Menus/CompareMenu.h>
#include <djvApp/Menus/FileMenu.h>
#include <djvApp/Menus/FrameMenu.h>
#include <djvApp/Menus/PlaybackMenu.h>
#include <djvApp/Menus/TimelineMenu.h>
#include <djvApp/Menus/ToolsMenu.h>
#include <djvApp/Menus/ViewMenu.h>
#include <djvApp/Menus/WindowMenu.h>
#include <djvApp/Models/ColorModel.h>
#include <djvApp/Models/TimeUnitsModel.h>
#include <djvApp/Models/ViewportModel.h>
#include <djvApp/Tools/ToolsWidget.h>
#include <djvApp/Widgets/BottomToolBar.h>
#include <djvApp/Widgets/CompareToolBar.h>
#include <djvApp/Widgets/FileToolBar.h>
#include <djvApp/Widgets/StatusBar.h>
#include <djvApp/Widgets/TabBar.h>
#include <djvApp/Widgets/ToolsToolBar.h>
#include <djvApp/Widgets/ViewToolBar.h>
#include <djvApp/Widgets/Viewport.h>
#include <djvApp/Widgets/WindowToolBar.h>
#include <djvApp/App.h>

#include <tlTimelineUI/TimelineWidget.h>

#include <tlTimelineGL/Render.h>

#include <dtk/ui/ButtonGroup.h>
#include <dtk/ui/Divider.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/Menu.h>
#include <dtk/ui/MenuBar.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/Splitter.h>
#include <dtk/ui/ToolButton.h>

#if defined(TLRENDER_BMD)
#include <tlDevice/BMDOutputDevice.h>
#endif // TLRENDER_BMD

namespace djv
{
    namespace app
    {

        struct MainWindow::Private
        {
            std::weak_ptr<App> app;
            std::shared_ptr<SettingsModel> settingsModel;
            tl::timelineui::ItemOptions itemOptions;

            std::shared_ptr<Viewport> viewport;
            std::shared_ptr<tl::timelineui::TimelineWidget> timelineWidget;
            std::shared_ptr<FileActions> fileActions;
            std::shared_ptr<CompareActions> compareActions;
            std::shared_ptr<PlaybackActions> playbackActions;
            std::shared_ptr<FrameActions> frameActions;
            std::shared_ptr<TimelineActions> timelineActions;
            std::shared_ptr<AudioActions> audioActions;
            std::shared_ptr<ViewActions> viewActions;
            std::shared_ptr<WindowActions> windowActions;
            std::shared_ptr<ToolsActions> toolsActions;
            std::shared_ptr<FileMenu> fileMenu;
            std::shared_ptr<CompareMenu> compareMenu;
            std::shared_ptr<PlaybackMenu> playbackMenu;
            std::shared_ptr<FrameMenu> frameMenu;
            std::shared_ptr<TimelineMenu> timelineMenu;
            std::shared_ptr<AudioMenu> audioMenu;
            std::shared_ptr<ViewMenu> viewMenu;
            std::shared_ptr<WindowMenu> windowMenu;
            std::shared_ptr<ToolsMenu> toolsMenu;
            std::shared_ptr<dtk::MenuBar> menuBar;
            std::shared_ptr<FileToolBar> fileToolBar;
            std::shared_ptr<CompareToolBar> compareToolBar;
            std::shared_ptr<ViewToolBar> viewToolBar;
            std::shared_ptr<WindowToolBar> windowToolBar;
            std::shared_ptr<ToolsToolBar> toolsToolBar;
            std::shared_ptr<TabBar> tabBar;
            std::shared_ptr<BottomToolBar> bottomToolBar;
            std::shared_ptr<StatusBar> statusBar;
            std::shared_ptr<ToolsWidget> toolsWidget;
            std::map<std::string, std::shared_ptr<dtk::Divider> > dividers;
            std::shared_ptr<dtk::Splitter> splitter;
            std::shared_ptr<dtk::Splitter> splitter2;
            std::shared_ptr<dtk::VerticalLayout> layout;

            std::shared_ptr<dtk::ValueObserver<std::shared_ptr<tl::timeline::Player> > > playerObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timeline::OCIOOptions> > ocioOptionsObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timeline::LUTOptions> > lutOptionsObserver;
            std::shared_ptr<dtk::ValueObserver<dtk::ImageType> > colorBufferObserver;
            std::shared_ptr<dtk::ValueObserver<TimelineSettings> > timelineSettingsObserver;
            std::shared_ptr<dtk::ValueObserver<WindowSettings> > windowSettingsObserver;
        };

        void MainWindow::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            const WindowSettings& settings = app->getSettingsModel()->getWindow();
            Window::_init(context, "djv", settings.size);
            DTK_P();

            setBackgroundRole(dtk::ColorRole::Window);

            p.app = app;
            p.settingsModel = app->getSettingsModel();

            p.viewport = Viewport::create(context, app);

            auto timeUnitsModel = app->getTimeUnitsModel();
            p.timelineWidget = tl::timelineui::TimelineWidget::create(context, timeUnitsModel);
            p.timelineWidget->setScrollBarsVisible(false);

            p.fileActions = FileActions::create(context, app);
            p.compareActions = CompareActions::create(context, app);
            p.playbackActions = PlaybackActions::create(context, app);
            p.frameActions = FrameActions::create(
                context,
                app,
                std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            p.timelineActions = TimelineActions::create(
                context,
                app,
                std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            p.audioActions = AudioActions::create(context, app);
            p.viewActions = ViewActions::create(
                context,
                app,
                std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            p.windowActions = WindowActions::create(
                context,
                app,
                std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            p.toolsActions = ToolsActions::create(context, app);

            p.fileMenu = FileMenu::create(context, app, p.fileActions);
            p.compareMenu = CompareMenu::create(context, app, p.compareActions);
            p.playbackMenu = PlaybackMenu::create(context, p.playbackActions);
            p.frameMenu = FrameMenu::create(context, p.frameActions);
            p.timelineMenu = TimelineMenu::create(context, p.timelineActions);
            p.audioMenu = AudioMenu::create(context, p.audioActions);
            p.viewMenu = ViewMenu::create(context, p.viewActions);
            p.windowMenu = WindowMenu::create(
                context,
                std::dynamic_pointer_cast<MainWindow>(shared_from_this()),
                p.windowActions);
            p.toolsMenu = ToolsMenu::create(context, p.toolsActions);
            p.menuBar = dtk::MenuBar::create(context);
            p.menuBar->addMenu("File", p.fileMenu);
            p.menuBar->addMenu("Compare", p.compareMenu);
            p.menuBar->addMenu("Playback", p.playbackMenu);
            p.menuBar->addMenu("Frame", p.frameMenu);
            p.menuBar->addMenu("Timeline", p.timelineMenu);
            p.menuBar->addMenu("Audio", p.audioMenu);
            p.menuBar->addMenu("View", p.viewMenu);
            p.menuBar->addMenu("Window", p.windowMenu);
            p.menuBar->addMenu("Tools", p.toolsMenu);

            p.fileToolBar = FileToolBar::create(
                context,
                p.fileActions->getActions());
            p.compareToolBar = CompareToolBar::create(
                context,
                p.compareActions->getActions());
            p.viewToolBar = ViewToolBar::create(context, p.viewActions);
            p.windowToolBar = WindowToolBar::create(
                context,
                p.windowActions->getActions());
            p.toolsToolBar = ToolsToolBar::create(
                context,
                p.toolsActions->getActions());
            p.tabBar = TabBar::create(context, app);
            p.bottomToolBar = BottomToolBar::create(
                context,
                app,
                p.playbackActions,
                p.frameActions,
                p.audioActions);
            p.statusBar = StatusBar::create(context, app);

            p.toolsWidget = ToolsWidget::create(
                context,
                app,
                std::dynamic_pointer_cast<MainWindow>(shared_from_this()));

            p.layout = dtk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(dtk::SizeRole::None);
            p.menuBar->setParent(p.layout);
            p.dividers["MenuBar"] = dtk::Divider::create(context, dtk::Orientation::Vertical, p.layout);
            auto hLayout = dtk::HorizontalLayout::create(context, p.layout);
            hLayout->setSpacingRole(dtk::SizeRole::None);
            p.fileToolBar->setParent(hLayout);
            p.dividers["File"] = dtk::Divider::create(context, dtk::Orientation::Horizontal, hLayout);
            p.compareToolBar->setParent(hLayout);
            p.dividers["Compare"] = dtk::Divider::create(context, dtk::Orientation::Horizontal, hLayout);
            p.windowToolBar->setParent(hLayout);
            p.dividers["Window"] = dtk::Divider::create(context, dtk::Orientation::Horizontal, hLayout);
            p.viewToolBar->setParent(hLayout);
            p.dividers["View"] = dtk::Divider::create(context, dtk::Orientation::Horizontal, hLayout);
            p.toolsToolBar->setParent(hLayout);
            p.dividers["ToolBars"] = dtk::Divider::create(context, dtk::Orientation::Vertical, p.layout);
            p.splitter = dtk::Splitter::create(context, dtk::Orientation::Vertical, p.layout);
            p.splitter->setSplit(settings.splitter);
            p.splitter2 = dtk::Splitter::create(context, dtk::Orientation::Horizontal, p.splitter);
            p.splitter2->setSplit(settings.splitter2);
            auto vLayout = dtk::VerticalLayout::create(context, p.splitter2);
            vLayout->setSpacingRole(dtk::SizeRole::None);
            p.tabBar->setParent(vLayout);
            p.viewport->setParent(vLayout);
            p.toolsWidget->setParent(p.splitter2);
            p.timelineWidget->setParent(p.splitter);
            p.dividers["Bottom"] = dtk::Divider::create(context, dtk::Orientation::Vertical, p.layout);
            p.bottomToolBar->setParent(p.layout);
            p.dividers["Status"] = dtk::Divider::create(context, dtk::Orientation::Vertical, p.layout);
            p.statusBar->setParent(p.layout);

            auto appWeak = std::weak_ptr<App>(app);
            p.viewport->setCompareCallback(
                [appWeak](const tl::timeline::CompareOptions& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getFilesModel()->setCompareOptions(value);
                    }
                });
            p.playerObserver = dtk::ValueObserver<std::shared_ptr<tl::timeline::Player> >::create(
                app->observePlayer(),
                [this](const std::shared_ptr<tl::timeline::Player>& value)
                {
                    _playerUpdate(value);
                });

            p.ocioOptionsObserver = dtk::ValueObserver<tl::timeline::OCIOOptions>::create(
                app->getColorModel()->observeOCIOOptions(),
                [this](const tl::timeline::OCIOOptions& value)
                {
                    auto options = _p->timelineWidget->getDisplayOptions();
                    options.ocio = value;
                    _p->timelineWidget->setDisplayOptions(options);
                });

            p.lutOptionsObserver = dtk::ValueObserver<tl::timeline::LUTOptions>::create(
                app->getColorModel()->observeLUTOptions(),
                [this](const tl::timeline::LUTOptions& value)
                {
                    auto options = _p->timelineWidget->getDisplayOptions();
                    options.lut = value;
                    _p->timelineWidget->setDisplayOptions(options);
                });

            p.colorBufferObserver = dtk::ValueObserver<dtk::ImageType>::create(
                app->getViewportModel()->observeColorBuffer(),
                [this](dtk::ImageType value)
                {
                    setFrameBufferType(value);
                });

            p.windowSettingsObserver = dtk::ValueObserver<WindowSettings>::create(
                p.settingsModel->observeWindow(),
                [this](const WindowSettings& value)
                {
                    _settingsUpdate(value);
                });

            p.timelineSettingsObserver = dtk::ValueObserver<TimelineSettings>::create(
                p.settingsModel->observeTimeline(),
                [this](const TimelineSettings& value)
                {
                    _settingsUpdate(value);
                });
        }

        MainWindow::MainWindow() :
            _p(new Private)
        {}

        MainWindow::~MainWindow()
        {
            DTK_P();
            _makeCurrent();
            p.viewport->setParent(nullptr);
            p.timelineWidget->setParent(nullptr);

            WindowSettings settings = p.settingsModel->getWindow();
            settings.size = getGeometry().size();
#if defined(__APPLE__)
            //! \bug The window size needs to be scaled on macOS?
            const float displayScale = getDisplayScale();
            if (displayScale > 0.F)
            {
                settings.size = settings.size / displayScale;
            }
#endif // __APPLE__
            settings.splitter = p.splitter->getSplit();
            settings.splitter2 = p.splitter2->getSplit();
            p.settingsModel->setWindow(settings);
        }

        std::shared_ptr<MainWindow> MainWindow::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            auto out = std::shared_ptr<MainWindow>(new MainWindow);
            out->_init(context, app);
            return out;
        }

        const std::shared_ptr<Viewport>& MainWindow::getViewport() const
        {
            return _p->viewport;
        }

        const std::shared_ptr<tl::timelineui::TimelineWidget>& MainWindow::getTimelineWidget() const
        {
            return _p->timelineWidget;
        }

        void MainWindow::focusCurrentFrame()
        {
            _p->bottomToolBar->focusCurrentFrame();
        }

        void MainWindow::setGeometry(const dtk::Box2I& value)
        {
            Window::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void MainWindow::keyPressEvent(dtk::KeyEvent& event)
        {
            DTK_P();
            event.accept = p.menuBar->shortcut(event.key, event.modifiers);
        }

        void MainWindow::keyReleaseEvent(dtk::KeyEvent& event)
        {
            event.accept = true;
        }

        void MainWindow::_drop(const std::vector<std::string>& value)
        {
            DTK_P();
            if (auto app = p.app.lock())
            {
                for (const auto& i : value)
                {
                    app->open(tl::file::Path(i));
                }
            }
        }

        void MainWindow::_playerUpdate(const std::shared_ptr<tl::timeline::Player>& player)
        {
            DTK_P();
            p.viewport->setPlayer(player);
            p.timelineWidget->setPlayer(player);
        }

        void MainWindow::_settingsUpdate(const WindowSettings& settings)
        {
            DTK_P();

            p.fileToolBar->setVisible(settings.fileToolBar);
            p.dividers["File"]->setVisible(settings.fileToolBar);

            p.compareToolBar->setVisible(settings.compareToolBar);
            p.dividers["Compare"]->setVisible(settings.compareToolBar);

            p.windowToolBar->setVisible(settings.windowToolBar);
            p.dividers["Window"]->setVisible(settings.windowToolBar);

            p.viewToolBar->setVisible(settings.viewToolBar);
            p.dividers["View"]->setVisible(settings.viewToolBar);

            p.toolsToolBar->setVisible(settings.toolsToolBar);

            p.dividers["ToolBars"]->setVisible(
                settings.fileToolBar ||
                settings.compareToolBar ||
                settings.windowToolBar ||
                settings.viewToolBar ||
                settings.toolsToolBar);

            p.tabBar->setVisible(settings.tabBar);

            p.timelineWidget->setVisible(settings.timeline);

            p.bottomToolBar->setVisible(settings.bottomToolBar);
            p.dividers["Bottom"]->setVisible(settings.bottomToolBar);

            p.statusBar->setVisible(settings.statusToolBar);
            p.dividers["Status"]->setVisible(settings.statusToolBar);

            p.splitter->setSplit(settings.splitter);
            p.splitter2->setSplit(settings.splitter2);
        }

        void MainWindow::_settingsUpdate(const TimelineSettings& settings)
        {
            DTK_P();
            p.timelineWidget->setEditable(settings.editable);
            p.timelineWidget->setFrameView(settings.frameView);
            p.timelineWidget->setScrollToCurrentFrame(settings.scroll);
            p.timelineWidget->setStopOnScrub(settings.stopOnScrub);
            p.timelineWidget->setItemOptions(settings.item);
            tl::timelineui::DisplayOptions display = settings.display;
            if (settings.firstTrack)
            {
                display.tracks = { 0 };
            }
            display.waveformHeight = display.thumbnailHeight / 2;
            p.timelineWidget->setDisplayOptions(display);
        }
    }
}
