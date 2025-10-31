// SPDX-License-Identifier: BSD-3-Clause
// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the DJV project.

#include <djvApp/MainWindow.h>

#include <djvApp/Actions/AudioActions.h>
#include <djvApp/Actions/ColorActions.h>
#include <djvApp/Actions/CompareActions.h>
#include <djvApp/Actions/FileActions.h>
#include <djvApp/Actions/FrameActions.h>
#include <djvApp/Actions/HelpActions.h>
#include <djvApp/Actions/PlaybackActions.h>
#include <djvApp/Actions/TimelineActions.h>
#include <djvApp/Actions/ToolsActions.h>
#include <djvApp/Actions/ViewActions.h>
#include <djvApp/Actions/WindowActions.h>
#include <djvApp/Menus/AudioMenu.h>
#include <djvApp/Menus/ColorMenu.h>
#include <djvApp/Menus/CompareMenu.h>
#include <djvApp/Menus/FileMenu.h>
#include <djvApp/Menus/FrameMenu.h>
#include <djvApp/Menus/HelpMenu.h>
#include <djvApp/Menus/PlaybackMenu.h>
#include <djvApp/Menus/TimelineMenu.h>
#include <djvApp/Menus/ToolsMenu.h>
#include <djvApp/Menus/ViewMenu.h>
#include <djvApp/Menus/WindowMenu.h>
#include <djvApp/Models/ColorModel.h>
#include <djvApp/Models/TimeUnitsModel.h>
#include <djvApp/Models/ViewportModel.h>
#include <djvApp/Tools/ToolsWidget.h>
#include <djvApp/Widgets/AboutDialog.h>
#include <djvApp/Widgets/BottomToolBar.h>
#include <djvApp/Widgets/CompareToolBar.h>
#include <djvApp/Widgets/FileToolBar.h>
#include <djvApp/Widgets/SetupDialog.h>
#include <djvApp/Widgets/StatusBar.h>
#include <djvApp/Widgets/TabBar.h>
#include <djvApp/Widgets/ToolsToolBar.h>
#include <djvApp/Widgets/ViewToolBar.h>
#include <djvApp/Widgets/Viewport.h>
#include <djvApp/Widgets/WindowToolBar.h>
#include <djvApp/App.h>

#include <tlTimelineUI/TimelineWidget.h>

#include <tlTimelineGL/Render.h>

#include <ftk/UI/ButtonGroup.h>
#include <ftk/UI/Divider.h>
#include <ftk/UI/IconSystem.h>
#include <ftk/UI/Label.h>
#include <ftk/UI/Menu.h>
#include <ftk/UI/MenuBar.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/Splitter.h>
#include <ftk/UI/ToolButton.h>

#if defined(TLRENDER_BMD)
#include <tlDevice/BMDOutputDevice.h>
#endif // TLRENDER_BMD

namespace djv_resource
{
    extern std::vector<uint8_t> DJV_Icon_512;
    extern std::vector<uint8_t> DJV_Icon_256;
    extern std::vector<uint8_t> DJV_Icon_128;
    extern std::vector<uint8_t> DJV_Icon_64;
    extern std::vector<uint8_t> DJV_Icon_32;
    extern std::vector<uint8_t> DJV_Icon_16;
}

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
            std::shared_ptr<ColorActions> colorActions;
            std::shared_ptr<ToolsActions> toolsActions;
            std::shared_ptr<HelpActions> helpActions;
            std::shared_ptr<FileMenu> fileMenu;
            std::shared_ptr<CompareMenu> compareMenu;
            std::shared_ptr<PlaybackMenu> playbackMenu;
            std::shared_ptr<FrameMenu> frameMenu;
            std::shared_ptr<TimelineMenu> timelineMenu;
            std::shared_ptr<AudioMenu> audioMenu;
            std::shared_ptr<ViewMenu> viewMenu;
            std::shared_ptr<WindowMenu> windowMenu;
            std::shared_ptr<ColorMenu> colorMenu;
            std::shared_ptr<ToolsMenu> toolsMenu;
            std::shared_ptr<HelpMenu> helpMenu;
            std::shared_ptr<ftk::MenuBar> menuBar;
            std::shared_ptr<FileToolBar> fileToolBar;
            std::shared_ptr<CompareToolBar> compareToolBar;
            std::shared_ptr<ViewToolBar> viewToolBar;
            std::shared_ptr<WindowToolBar> windowToolBar;
            std::shared_ptr<ToolsToolBar> toolsToolBar;
            std::shared_ptr<TabBar> tabBar;
            std::shared_ptr<BottomToolBar> bottomToolBar;
            std::shared_ptr<StatusBar> statusBar;
            std::shared_ptr<ToolsWidget> toolsWidget;
            std::shared_ptr<SetupDialog> setupDialog;
            std::shared_ptr<AboutDialog> aboutDialog;
            std::map<std::string, std::shared_ptr<ftk::Divider> > dividers;
            std::shared_ptr<ftk::Splitter> splitter;
            std::shared_ptr<ftk::Splitter> splitter2;
            std::shared_ptr<ftk::VerticalLayout> splitterLayout;
            std::shared_ptr<ftk::VerticalLayout> layout;

            std::shared_ptr<ftk::ValueObserver<std::shared_ptr<tl::timeline::Player> > > playerObserver;
            std::shared_ptr<ftk::ValueObserver<tl::timeline::CompareOptions> > compareOptionsObserver;
            std::shared_ptr<ftk::ValueObserver<tl::timeline::OCIOOptions> > ocioOptionsObserver;
            std::shared_ptr<ftk::ValueObserver<tl::timeline::LUTOptions> > lutOptionsObserver;
            std::shared_ptr<ftk::ValueObserver<ftk::ImageType> > colorBufferObserver;
            std::shared_ptr<ftk::ValueObserver<MouseSettings> > mouseSettingsObserver;
            std::shared_ptr<ftk::ValueObserver<TimelineSettings> > timelineSettingsObserver;
            std::shared_ptr<ftk::ValueObserver<bool> > timelineFrameViewObserver;
            std::shared_ptr<ftk::ValueObserver<WindowSettings> > windowSettingsObserver;
        };

        void MainWindow::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            const WindowSettings& settings = app->getSettingsModel()->getWindow();
            Window::_init(context, "djv", settings.size);
            FTK_P();

            auto iconSystem = context->getSystem<ftk::IconSystem>();
            iconSystem->add("DJV_Icon_128", djv_resource::DJV_Icon_128);
            setIcon(iconSystem->get("DJV_Icon_128", 1.0));

            p.app = app;
            p.settingsModel = app->getSettingsModel();

            p.viewport = Viewport::create(context, app);

            auto timeUnitsModel = app->getTimeUnitsModel();
            p.timelineWidget = tl::timelineui::TimelineWidget::create(context, timeUnitsModel);

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
            p.colorActions = ColorActions::create(context, app);
            p.toolsActions = ToolsActions::create(context, app);
            p.helpActions = HelpActions::create(
                context,
                app,
                std::dynamic_pointer_cast<MainWindow>(shared_from_this()));

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
            p.colorMenu = ColorMenu::create(context, p.colorActions);
            p.toolsMenu = ToolsMenu::create(context, p.toolsActions);
            p.helpMenu = HelpMenu::create(context, p.helpActions);
            p.menuBar = ftk::MenuBar::create(context);
            p.menuBar->addMenu("File", p.fileMenu);
            p.menuBar->addMenu("Compare", p.compareMenu);
            p.menuBar->addMenu("Playback", p.playbackMenu);
            p.menuBar->addMenu("Frame", p.frameMenu);
            p.menuBar->addMenu("Timeline", p.timelineMenu);
            p.menuBar->addMenu("Audio", p.audioMenu);
            p.menuBar->addMenu("View", p.viewMenu);
            p.menuBar->addMenu("Window", p.windowMenu);
            p.menuBar->addMenu("Color", p.colorMenu);
            p.menuBar->addMenu("Tools", p.toolsMenu);
            p.menuBar->addMenu("Help", p.helpMenu);

            p.fileToolBar = FileToolBar::create(
                context,
                p.fileActions->getActions());
            p.compareToolBar = CompareToolBar::create(
                context,
                p.compareActions->getActions());
            p.viewToolBar = ViewToolBar::create(
                context,
                std::dynamic_pointer_cast<MainWindow>(shared_from_this()),
                p.viewActions);
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

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
            p.menuBar->setParent(p.layout);
            p.dividers["MenuBar"] = ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            auto hLayout = ftk::HorizontalLayout::create(context, p.layout);
            hLayout->setSpacingRole(ftk::SizeRole::None);
            p.fileToolBar->setParent(hLayout);
            p.dividers["File"] = ftk::Divider::create(context, ftk::Orientation::Horizontal, hLayout);
            p.compareToolBar->setParent(hLayout);
            p.dividers["Compare"] = ftk::Divider::create(context, ftk::Orientation::Horizontal, hLayout);
            p.windowToolBar->setParent(hLayout);
            p.dividers["Window"] = ftk::Divider::create(context, ftk::Orientation::Horizontal, hLayout);
            p.viewToolBar->setParent(hLayout);
            p.dividers["View"] = ftk::Divider::create(context, ftk::Orientation::Horizontal, hLayout);
            p.toolsToolBar->setParent(hLayout);
            p.dividers["ToolBars"] = ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            p.splitterLayout = ftk::VerticalLayout::create(context, p.layout);
            p.splitterLayout->setSpacingRole(ftk::SizeRole::None);
            p.splitterLayout->setVStretch(ftk::Stretch::Expanding);
            p.splitter = ftk::Splitter::create(context, ftk::Orientation::Vertical, p.splitterLayout);
            p.splitter->setSplit(settings.splitter);
            p.splitter2 = ftk::Splitter::create(context, ftk::Orientation::Horizontal, p.splitter);
            p.splitter2->setSplit(settings.splitter2);
            auto vLayout = ftk::VerticalLayout::create(context, p.splitter2);
            vLayout->setSpacingRole(ftk::SizeRole::None);
            p.tabBar->setParent(vLayout);
            p.viewport->setParent(vLayout);
            p.toolsWidget->setParent(p.splitter2);
            p.timelineWidget->setParent(p.splitter);
            p.dividers["Bottom"] = ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            p.bottomToolBar->setParent(p.layout);
            p.dividers["Status"] = ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            p.statusBar->setParent(p.layout);

            auto miscSettings = app->getSettingsModel()->getMisc();
            if (miscSettings.showSetup)
            {
                miscSettings.showSetup = false;
                app->getSettingsModel()->setMisc(miscSettings);
                p.setupDialog = SetupDialog::create(context, app);
                p.setupDialog->open(std::dynamic_pointer_cast<IWindow>(shared_from_this()));
                p.setupDialog->setCloseCallback(
                    [this]
                    {
                        _p->setupDialog.reset();
                    });
            }

            p.playerObserver = ftk::ValueObserver<std::shared_ptr<tl::timeline::Player> >::create(
                app->observePlayer(),
                [this](const std::shared_ptr<tl::timeline::Player>& player)
                {
                    FTK_P();
                    p.viewport->setPlayer(player);
                    p.timelineWidget->setPlayer(player);
                });

            auto appWeak = std::weak_ptr<App>(app);
            p.compareOptionsObserver = ftk::ValueObserver<tl::timeline::CompareOptions>::create(
                p.viewport->observeCompareOptions(),
                [appWeak](const tl::timeline::CompareOptions& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getFilesModel()->setCompareOptions(value);
                    }
                });

            p.ocioOptionsObserver = ftk::ValueObserver<tl::timeline::OCIOOptions>::create(
                app->getColorModel()->observeOCIOOptions(),
                [this](const tl::timeline::OCIOOptions& value)
                {
                    auto options = _p->timelineWidget->getDisplayOptions();
                    options.ocio = value;
                    _p->timelineWidget->setDisplayOptions(options);
                });

            p.lutOptionsObserver = ftk::ValueObserver<tl::timeline::LUTOptions>::create(
                app->getColorModel()->observeLUTOptions(),
                [this](const tl::timeline::LUTOptions& value)
                {
                    auto options = _p->timelineWidget->getDisplayOptions();
                    options.lut = value;
                    _p->timelineWidget->setDisplayOptions(options);
                });

            p.colorBufferObserver = ftk::ValueObserver<ftk::ImageType>::create(
                app->getViewportModel()->observeColorBuffer(),
                [this](ftk::ImageType value)
                {
                    setFrameBufferType(value);
                });

            p.mouseSettingsObserver = ftk::ValueObserver<MouseSettings>::create(
                p.settingsModel->observeMouse(),
                [this](const MouseSettings& value)
                {
                    _settingsUpdate(value);
                });

            p.timelineSettingsObserver = ftk::ValueObserver<TimelineSettings>::create(
                p.settingsModel->observeTimeline(),
                [this](const TimelineSettings& value)
                {
                    _settingsUpdate(value);
                });

            p.timelineFrameViewObserver = ftk::ValueObserver<bool>::create(
                p.timelineWidget->observeFrameView(),
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto settings = app->getSettingsModel()->getTimeline();
                        settings.frameView = value;
                        app->getSettingsModel()->setTimeline(settings);
                    }
                });

            p.windowSettingsObserver = ftk::ValueObserver<WindowSettings>::create(
                p.settingsModel->observeWindow(),
                [this](const WindowSettings& value)
                {
                    _settingsUpdate(value);
                });
        }

        MainWindow::MainWindow() :
            _p(new Private)
        {}

        MainWindow::~MainWindow()
        {
            FTK_P();
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
            const std::shared_ptr<ftk::Context>& context,
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

        void MainWindow::showAboutDialog()
        {
            FTK_P();
            if (auto context = getContext())
            {
                if (auto app = p.app.lock())
                {
                    p.aboutDialog = AboutDialog::create(context, app);
                    p.aboutDialog->open(std::dynamic_pointer_cast<IWindow>(shared_from_this()));
                    p.aboutDialog->setCloseCallback(
                        [this]
                        {
                            _p->aboutDialog.reset();
                        });
                }
            }
        }

        void MainWindow::setGeometry(const ftk::Box2I& value)
        {
            Window::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void MainWindow::keyPressEvent(ftk::KeyEvent& event)
        {
            FTK_P();
            event.accept = p.menuBar->shortcut(event.key, event.modifiers);
        }

        void MainWindow::keyReleaseEvent(ftk::KeyEvent& event)
        {
            event.accept = true;
        }

        void MainWindow::_drop(const std::vector<std::string>& value)
        {
            FTK_P();
            if (auto app = p.app.lock())
            {
                for (const auto& i : value)
                {
                    app->open(tl::file::Path(i));
                }
            }
        }

        void MainWindow::_settingsUpdate(const MouseSettings& settings)
        {
            FTK_P();
            p.timelineWidget->setMouseWheelScale(settings.wheelScale);
            p.viewport->setMouseWheelScale(settings.wheelScale);
        }

        void MainWindow::_settingsUpdate(const TimelineSettings& settings)
        {
            FTK_P();

            p.timelineWidget->setFrameView(settings.frameView);
            p.timelineWidget->setScrollBarsVisible(settings.scrollBars);
            p.timelineWidget->setAutoScroll(settings.autoScroll);
            p.timelineWidget->setStopOnScrub(settings.stopOnScrub);

            auto display = p.timelineWidget->getDisplayOptions();

            display.minimize = settings.minimize;
            display.thumbnails = settings.thumbnails != TimelineThumbnails::None;
            display.thumbnailHeight = getTimelineThumbnailsSize(settings.thumbnails);
            display.waveformHeight = getTimelineWaveformSize(settings.thumbnails);
            p.timelineWidget->setDisplayOptions(display);

            if (settings.minimize)
            {
                if (p.splitter->getParent().lock())
                {
                    p.splitter->setParent(nullptr);
                    p.splitter2->setParent(p.splitterLayout);
                    p.timelineWidget->setParent(p.splitterLayout);
                }
            }
            else
            {
                if (!p.splitter->getParent().lock())
                {
                    p.splitter->setParent(p.splitterLayout);
                    p.splitter2->setParent(p.splitter);
                    p.timelineWidget->setParent(p.splitter);
                }
            }
        }

        void MainWindow::_settingsUpdate(const WindowSettings& settings)
        {
            FTK_P();

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
    }
}
