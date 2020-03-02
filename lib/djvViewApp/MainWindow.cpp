//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvViewApp/MainWindow.h>

#include <djvViewApp/Application.h>
#include <djvViewApp/BackgroundImageWidget.h>
#include <djvViewApp/FileSettings.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/IToolSystem.h>
#include <djvViewApp/MDIWidget.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaCanvas.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/MemoryCacheWidget.h>
#include <djvViewApp/SettingsDrawer.h>
#include <djvViewApp/SettingsSystem.h>
#include <djvViewApp/ToolSystem.h>
#include <djvViewApp/ViewSystem.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/ThermometerWidget.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/MenuButton.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvCore/FileInfo.h>
#include <djvCore/OS.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//#define DJV_DEMO

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MainWindow::Private
        {
            std::vector<std::shared_ptr<Media> > media;
            
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> mediaActionGroup;
            std::shared_ptr<UI::Menu> mediaMenu;
            std::shared_ptr<UI::Button::Menu> mediaButton;
            std::shared_ptr<UI::PopupWidget> cachePopupWidget;
            std::shared_ptr<UI::ThermometerWidget> cacheThermometerWidget;
            std::shared_ptr<UI::ToolButton> settingsButton;
            std::shared_ptr<SettingsDrawer> settingsDrawer;
            std::shared_ptr<UI::MenuBar> menuBar;
            std::shared_ptr<MediaCanvas> mediaCanvas;
            std::shared_ptr<UI::MDI::Canvas> canvas;
#ifdef DJV_DEMO
            std::shared_ptr<UI::Label> titleLabel;
#endif // DJV_DEMO
            std::shared_ptr<UI::StackLayout> layout;

            std::shared_ptr<ValueObserver<bool> > escapeActionObserver;
            std::shared_ptr<ValueObserver<bool> > settingsActionObserver;
            std::shared_ptr<ListObserver<std::shared_ptr<Media> > > mediaObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<float> > cachePercentageObserver;
            std::shared_ptr<ValueObserver<bool> > maximizeObserver;
            std::shared_ptr<ValueObserver<float> > fadeObserver;
        };
        
        void MainWindow::_init(const std::shared_ptr<Core::Context>& context)
        {
            UI::Window::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MainWindow");

            auto viewSystems = context->getSystemsT<IViewSystem>();
            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            for (auto system : viewSystems)
            {
                for (auto action : system->getActions())
                {
                    addAction(action.second);
                }
                auto menu = system->getMenu();
                if (menu.menu)
                {
                    menus[menu.sortKey] = menu.menu;
                }
            }

            p.actions["Escape"] = UI::Action::create();
            p.actions["Escape"]->setShortcut(GLFW_KEY_ESCAPE);
            for (auto i : p.actions)
            {
                addAction(i.second);
            }

            p.mediaActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.mediaMenu = UI::Menu::create(context);
            addChild(p.mediaMenu);
            p.mediaButton = UI::Button::Menu::create(UI::Button::MenuStyle::Flat, context);
            p.mediaButton->setPopupIcon("djvIconPopupMenu");
            p.mediaButton->setEnabled(false);

            auto memoryCacheWidget = MemoryCacheWidget::create(context);
            p.cachePopupWidget = UI::PopupWidget::create(context);
            p.cachePopupWidget->setIcon("djvIconMemory");
            p.cachePopupWidget->addChild(memoryCacheWidget);
            p.cacheThermometerWidget = UI::ThermometerWidget::create(context);
            p.cacheThermometerWidget->setOrientation(UI::Orientation::Vertical);
            p.cacheThermometerWidget->setColorRole(UI::ColorRole::Cached);
            p.cacheThermometerWidget->setSizeRole(UI::MetricsRole::None);
            p.cacheThermometerWidget->setBackgroundRole(UI::ColorRole::None);

            auto maximizeButton = UI::ToolButton::create(context);
            auto autoHideButton = UI::ToolButton::create(context);
            auto windowSystem = context->getSystemT<WindowSystem>();
            if (windowSystem)
            {
                maximizeButton->addAction(windowSystem->getActions()["Maximize"]);
                autoHideButton->addAction(windowSystem->getActions()["AutoHide"]);
            }

            auto viewFillButton = UI::ToolButton::create(context);
            auto viewFrameButton = UI::ToolButton::create(context);
            auto viewCenterButton = UI::ToolButton::create(context);
            auto viewSystem = context->getSystemT<ViewSystem>();
            if (viewSystem)
            {
                viewFillButton->addAction(viewSystem->getActions()["FillLock"]);
                viewFrameButton->addAction(viewSystem->getActions()["FrameLock"]);
                viewCenterButton->addAction(viewSystem->getActions()["CenterLock"]);
            }

            std::map<std::string, std::shared_ptr<UI::ToolButton> > toolButtons;
            for (const auto& i : context->getSystemsT<IToolSystem>())
            {
                auto button = UI::ToolButton::create(context);
                auto data = i->getToolAction();
                button->addAction(data.action);
                toolButtons[data.sortKey] = button;
            }

            p.settingsButton = UI::ToolButton::create(context);
            auto toolSystem = context->getSystemT<ToolSystem>();
            if (toolSystem)
            {
                p.settingsButton->addAction(toolSystem->getActions()["Settings"]);
            }
            p.settingsDrawer = SettingsDrawer::create(context);

            p.menuBar = UI::MenuBar::create(context);
            p.menuBar->setBackgroundRole(UI::ColorRole::OverlayLight);
            for (auto i : menus)
            {
                p.menuBar->addChild(i.second);
            }
            p.menuBar->addSeparator(UI::Side::Right);
            p.menuBar->addChild(p.mediaButton);
            p.menuBar->setStretch(p.mediaButton, UI::RowStretch::Expand, UI::Side::Right);
            p.menuBar->addSeparator(UI::Side::Right);
            p.menuBar->addChild(p.cachePopupWidget);
            p.menuBar->addChild(p.cacheThermometerWidget);
            p.menuBar->addSeparator(UI::Side::Right);
            p.menuBar->addChild(maximizeButton);
            p.menuBar->addChild(autoHideButton);
            p.menuBar->addSeparator(UI::Side::Right);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            hLayout->addChild(viewFillButton);
            hLayout->addChild(viewFrameButton);
            hLayout->addChild(viewCenterButton);
            p.menuBar->addChild(hLayout);
            p.menuBar->addSeparator(UI::Side::Right);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            for (const auto& i : toolButtons)
            {
                hLayout->addChild(i.second);
            }
            p.menuBar->addChild(hLayout);
            p.menuBar->addSeparator(UI::Side::Right);
            p.menuBar->addChild(p.settingsButton);

            auto backgroundImageWidget = BackgroundImageWidget::create(context);

            p.mediaCanvas = MediaCanvas::create(context);

            p.canvas = UI::MDI::Canvas::create(context);
            for (auto system : viewSystems)
            {
                system->setCanvas(p.canvas);
            }

#ifdef DJV_DEMO
            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setFontSizeRole(UI::MetricsRole::TextColumn);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
#endif // DJV_DEMO

            p.layout = UI::StackLayout::create(context);
            auto stackLayout = UI::StackLayout::create(context);
            stackLayout->addChild(backgroundImageWidget);
            stackLayout->addChild(p.mediaCanvas);
            stackLayout->addChild(p.canvas);
            p.layout->addChild(stackLayout);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            vLayout->addChild(p.menuBar);
            vLayout->addChild(p.settingsDrawer);
            vLayout->setStretch(p.settingsDrawer, UI::Layout::RowStretch::Expand);
            p.layout->addChild(vLayout);
#ifdef DJV_DEMO
            vLayout = UI::VerticalLayout::create(context);
            vLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginDialog));
            vLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingLarge));
            vLayout->addChild(p.titleLabel);
            p.layout->addChild(vLayout);
#endif // DJV_DEMO
            addChild(p.layout);

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.mediaActionGroup->setRadioCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (value >= 0 && value < static_cast<int>(widget->_p->media.size()))
                            {
                                if (auto fileSystem = context->getSystemT<FileSystem>())
                                {
                                    fileSystem->setCurrentMedia(widget->_p->media[value]);
                                }
                            }
                            widget->_p->mediaMenu->close();
                        }
                    }
                });

            p.mediaButton->setOpenCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (value)
                        {
                            widget->_p->mediaMenu->popup(widget->_p->mediaButton);
                        }
                        else
                        {
                            widget->_p->mediaMenu->close();
                        }
                    }
                });

            p.mediaMenu->setCloseCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->mediaButton->setOpen(false);
                    }
                });

            p.escapeActionObserver = ValueObserver<bool>::create(
                p.actions["Escape"]->observeClicked(),
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto windowSystem = context->getSystemT<WindowSystem>())
                        {
                            if (windowSystem->observeFullScreen()->get())
                            {
                                windowSystem->setFullScreen(false);
                            }
                        }
                    }
                });

            if (toolSystem)
            {
                p.settingsActionObserver = ValueObserver<bool>::create(
                    toolSystem->getActions()["Settings"]->observeChecked(),
                    [weak, contextWeak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->settingsDrawer->setOpen(value);
                        }
                    });
            }

            if (auto context = getContext().lock())
            {
                if (auto fileSystem = context->getSystemT<FileSystem>())
                {
                    p.mediaObserver = ListObserver<std::shared_ptr<Media>>::create(
                        fileSystem->observeMedia(),
                        [weak](const std::vector<std::shared_ptr<Media> >& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->media = value;
                                widget->_p->mediaActionGroup->clearActions();
                                widget->_p->mediaMenu->clearActions();
                                for (const auto& i : widget->_p->media)
                                {
                                    auto action = UI::Action::create();
                                    action->setText(i->getFileInfo().getFileName());
                                    widget->_p->mediaActionGroup->addAction(action);
                                    widget->_p->mediaMenu->addAction(action);
                                }
                                widget->_p->mediaButton->setEnabled(widget->_p->media.size() > 0);
                            }
                        });

                    p.currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                        fileSystem->observeCurrentMedia(),
                        [weak](const std::shared_ptr<Media>& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                const auto i = std::find(widget->_p->media.begin(), widget->_p->media.end(), value);
                                if (i != widget->_p->media.end())
                                {
                                    widget->_p->mediaActionGroup->setChecked(i - widget->_p->media.begin());
                                }
                                widget->_p->mediaButton->setText(
                                    value ?
                                    value->getFileInfo().getFileName(Frame::invalid, false) :
                                    "-");
                                widget->_p->mediaButton->setTooltip(std::string(
                                    value ?
                                    value->getFileInfo() :
                                    Core::FileSystem::FileInfo()));
                            }
                        });

                    p.cachePercentageObserver = ValueObserver<float>::create(
                        fileSystem->observeCachePercentage(),
                        [weak](float value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_cacheUpdate();
                            }
                        });
                }
            }

            if (windowSystem)
            {
                p.maximizeObserver = ValueObserver<bool>::create(
                    windowSystem->observeMaximize(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->mediaCanvas->setMaximize(value);
                        }
                    });

                p.fadeObserver = ValueObserver<float>::create(
                    windowSystem->observeFade(),
                    [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->menuBar->setOpacity(value);
                    }
                });
            }
        }

        MainWindow::MainWindow() :
            _p(new Private)
        {}

        MainWindow::~MainWindow()
        {}

        std::shared_ptr<MainWindow> MainWindow::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<MainWindow>(new MainWindow);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<MediaCanvas>& MainWindow::getMediaCanvas() const
        {
            return _p->mediaCanvas;
        }

        void MainWindow::_dropEvent(Event::Drop & event)
        {
            if (auto context = getContext().lock())
            {
                const auto& style = _getStyle();
                const float s = style->getMetric(UI::MetricsRole::SpacingLarge);
                glm::vec2 pos = event.getPointerInfo().projectedPos;
                std::vector<std::string> fileNames;
                for (const auto& i : event.getDropPaths())
                {
                    fileNames.push_back(i);
                }
                auto fileSystem = context->getSystemT<FileSystem>();
                fileSystem->open(fileNames, pos, s);
            }
        }

        void MainWindow::_initEvent(Event::Init& event)
        {
            Window::_initEvent(event);
            DJV_PRIVATE_PTR();
            p.mediaButton->setTooltip(_getText(DJV_TEXT("menu_media_popup_tooltip")));
            p.cachePopupWidget->setTooltip(_getText(DJV_TEXT("menu_memory_cache_tooltip")));
#ifdef DJV_DEMO
            p.titleLabel->setText(_getText(DJV_TEXT("djv_2_0_4")));
#endif // DJV_DEMO
            _cacheUpdate();
        }

        void MainWindow::_cacheUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (auto fileSystem = context->getSystemT<FileSystem>())
                {
                    const float percentage = fileSystem->observeCachePercentage()->get();
                    p.cacheThermometerWidget->setPercentage(percentage);
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("menu_memory_cache_thermometer_tooltip")) << ": " <<
                        static_cast<int>(percentage) << "%";
                    p.cacheThermometerWidget->setTooltip(ss.str());
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

