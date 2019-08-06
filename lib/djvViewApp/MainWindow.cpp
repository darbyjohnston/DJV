//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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
#include <djvViewApp/ImageViewSystem.h>
#include <djvViewApp/MDIWidget.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaCanvas.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/SettingsDialog.h>
#include <djvViewApp/SettingsSystem.h>
#include <djvViewApp/WindowSystem.h>
#include <djvViewApp/UISettings.h>

#include <djvUI/Action.h>
#include <djvUI/ActionButton.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/IntSlider.h>
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
#include <djvUI/ToggleButton.h>
#include <djvUI/ToolButton.h>

#include <djvCore/FileInfo.h>
#include <djvCore/OS.h>

#include <GLFW/glfw3.h>

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
            std::shared_ptr<UI::IntSlider> memoryCacheMaxSlider;
            std::shared_ptr<UI::ToggleButton> memoryCacheEnabledButton;
            std::shared_ptr<UI::PopupWidget> memoryCachePopupWidget;
            std::shared_ptr<UI::ToolButton> autoHideButton;
            std::shared_ptr<UI::ToolButton> settingsButton;
            std::shared_ptr<UI::MenuBar> menuBar;
            std::shared_ptr<MediaCanvas> mediaCanvas;
            std::shared_ptr<UI::MDI::Canvas> canvas;
            std::shared_ptr<UI::StackLayout> layout;
            std::shared_ptr<ValueObserver<bool> > escapeActionObserver;
            std::shared_ptr<ListObserver<std::shared_ptr<Media> > > mediaObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<bool> > memoryCacheEnabledObserver;
            std::shared_ptr<ValueObserver<int> > memoryCacheMaxObserver;
            std::shared_ptr<ValueObserver<bool> > maximizedObserver;
            std::shared_ptr<ValueObserver<float> > fadeObserver;
            std::shared_ptr<ValueObserver<bool> > autoHideObserver;
        };
        
        void MainWindow::_init(Context * context)
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

            p.memoryCacheMaxSlider = UI::IntSlider::create(context);
            p.memoryCacheMaxSlider->setRange(IntRange(1, OS::getRAMSize() / Memory::gigabyte));
            p.memoryCacheEnabledButton = UI::ToggleButton::create(context);
            p.memoryCacheEnabledButton->setHAlign(UI::HAlign::Center);
            p.memoryCacheEnabledButton->setMargin(UI::MetricsRole::None);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setMargin(UI::MetricsRole::MarginSmall);
            hLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            hLayout->addChild(p.memoryCacheMaxSlider);
            hLayout->addChild(p.memoryCacheEnabledButton);
            p.memoryCachePopupWidget = UI::PopupWidget::create(context);
            p.memoryCachePopupWidget->setIcon("djvIconMemory");
            p.memoryCachePopupWidget->setMargin(UI::MetricsRole::MarginSmall);
            p.memoryCachePopupWidget->addChild(hLayout);

            auto maximizedButton = UI::ActionButton::create(context);
            maximizedButton->setShowText(false);
            maximizedButton->setShowShortcuts(false);
            auto windowSystem = context->getSystemT<WindowSystem>();
            if (windowSystem)
            {
                maximizedButton->addAction(windowSystem->getActions()["Maximized"]);
            }

            auto viewLockFullButton = UI::ActionButton::create(context);
            viewLockFullButton->setShowText(false);
            viewLockFullButton->setShowShortcuts(false);
            auto viewLockFrameButton = UI::ActionButton::create(context);
            viewLockFrameButton->setShowText(false);
            viewLockFrameButton->setShowShortcuts(false);
            auto viewLockCenterButton = UI::ActionButton::create(context);
            viewLockCenterButton->setShowText(false);
            viewLockCenterButton->setShowShortcuts(false);
            auto imageViewSystem = context->getSystemT<ImageViewSystem>();
            if (imageViewSystem)
            {
                viewLockFullButton->addAction(imageViewSystem->getActions()["LockFull"]);
                viewLockFrameButton->addAction(imageViewSystem->getActions()["LockFrame"]);
                viewLockCenterButton->addAction(imageViewSystem->getActions()["LockCenter"]);
            }

            std::map<std::string, std::shared_ptr<UI::ActionButton> > toolButtons;
            for (const auto& i : context->getSystemsT<IToolSystem>())
            {
                auto button = UI::ActionButton::create(context);
                button->setShowText(false);
                button->setShowShortcuts(false);
                auto data = i->getToolAction();
                button->addAction(data.action);
                toolButtons[data.sortKey] = button;
            }

            p.autoHideButton = UI::ToolButton::create(context);
            p.autoHideButton->setButtonType(UI::ButtonType::Toggle);
            p.autoHideButton->setCheckedIcon("djvIconHidden");
            p.autoHideButton->setIcon("djvIconVisible");

            p.settingsButton = UI::ToolButton::create(context);
            p.settingsButton->setIcon("djvIconSettings");

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
            p.menuBar->addChild(p.memoryCachePopupWidget);
            p.menuBar->addSeparator(UI::Side::Right);
            p.menuBar->addChild(maximizedButton);
            p.menuBar->addSeparator(UI::Side::Right);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(viewLockFullButton);
            hLayout->addChild(viewLockFrameButton);
            hLayout->addChild(viewLockCenterButton);
            p.menuBar->addChild(hLayout);
            p.menuBar->addSeparator(UI::Side::Right);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            for (const auto& i : toolButtons)
            {
                hLayout->addChild(i.second);
            }
            p.menuBar->addChild(hLayout);
            p.menuBar->addSeparator(UI::Side::Right);
            p.menuBar->addChild(p.autoHideButton);
            p.menuBar->addSeparator(UI::Side::Right);
            p.menuBar->addChild(p.settingsButton);

            auto backgroundImageWidget = BackgroundImageWidget::create(context);

            p.mediaCanvas = MediaCanvas::create(context);

            p.canvas = UI::MDI::Canvas::create(context);
            for (auto system : viewSystems)
            {
                system->setCanvas(p.canvas);
            }

            p.layout = UI::StackLayout::create(context);
            auto stackLayout = UI::StackLayout::create(context);
            stackLayout->addChild(backgroundImageWidget);
            stackLayout->addChild(p.mediaCanvas);
            stackLayout->addChild(p.canvas);
            p.layout->addChild(stackLayout);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.menuBar);
            vLayout->addExpander();
            p.layout->addChild(vLayout);
            addChild(p.layout);

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            p.mediaActionGroup->setRadioCallback(
                [weak, context](int value)
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
                });

            p.mediaMenu->setCloseCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->mediaButton->setChecked(false);
                    }
                });

            p.mediaButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->mediaMenu->close();
                        if (value)
                        {
                            widget->_p->mediaMenu->popup(widget->_p->mediaButton);
                        }
                    }
                });

            p.memoryCacheMaxSlider->setValueCallback(
                [context](int value)
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                    if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                    {
                        fileSettings->setCacheMax(value);
                    }
                });

            p.memoryCacheEnabledButton->setCheckedCallback(
                [context](bool value)
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                    if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                    {
                        fileSettings->setCacheEnabled(value);
                    }
                });

            p.autoHideButton->setCheckedCallback(
                [weak, context](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto uiSettings = settingsSystem->getSettingsT<UISettings>();
                        uiSettings->setAutoHide(value);
                    }
                });

            p.settingsButton->setClickedCallback(
                [weak, context]
                {
                    if (auto widget = weak.lock())
                    {
                        auto settingsSystem = context->getSystemT<SettingsSystem>();
                        settingsSystem->showSettingsDialog();
                    }
                });

            p.escapeActionObserver = ValueObserver<bool>::create(
                p.actions["Escape"]->observeClicked(),
                [context](bool value)
                {
                    if (auto windowSystem = context->getSystemT<WindowSystem>())
                    {
                        if (windowSystem->observeFullScreen()->get())
                        {
                            windowSystem->setFullScreen(false);
                        }
                    }
                });

            if (auto fileSystem = getContext()->getSystemT<FileSystem>())
            {
                p.mediaObserver = ListObserver<std::shared_ptr<Media>>::create(
                    fileSystem->observeMedia(),
                    [weak](const std::vector<std::shared_ptr<Media> > & value)
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
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        const auto i = std::find(widget->_p->media.begin(), widget->_p->media.end(), value);
                        if (i != widget->_p->media.end())
                        {
                            widget->_p->mediaActionGroup->setChecked(i - widget->_p->media.begin());
                        }
                        widget->_p->mediaButton->setText(value ? value->getFileInfo().getFileName(Frame::invalid, false) : "-");
                        widget->_p->mediaButton->setTooltip(value ? value->getFileInfo() : Core::FileSystem::FileInfo());
                    }
                });
            }

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
            {
                p.memoryCacheEnabledObserver = ValueObserver<bool>::create(
                    fileSettings->observeCacheEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->memoryCacheEnabledButton->setChecked(value);
                        }
                    });

                p.memoryCacheMaxObserver = ValueObserver<int>::create(
                    fileSettings->observeCacheMax(),
                    [weak](int value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->memoryCacheMaxSlider->setValue(value);
                        }
                    });
            }

            if (windowSystem)
            {
                p.maximizedObserver = ValueObserver<bool>::create(
                    windowSystem->observeMaximized(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->mediaCanvas->setMaximized(value);
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

            auto uiSettings = settingsSystem->getSettingsT<UISettings>();
            p.autoHideObserver = ValueObserver<bool>::create(
                uiSettings->observeAutoHide(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->autoHideButton->setChecked(value);
                    }
                });
        }

        MainWindow::MainWindow() :
            _p(new Private)
        {}

        MainWindow::~MainWindow()
        {}

        std::shared_ptr<MainWindow> MainWindow::create(Context * context)
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
            const auto& style = _getStyle();
            const float s = style->getMetric(UI::MetricsRole::SpacingLarge);
            glm::vec2 pos = event.getPointerInfo().projectedPos;
            for (const auto & i : event.getDropPaths())
            {
                auto context = getContext();
                auto fileSystem = context->getSystemT<FileSystem>();
                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                auto fileSettings = settingsSystem->getSettingsT<FileSettings>();
                Core::FileSystem::FileInfo fileInfo;
                if (fileSettings->observeAutoDetectSequences()->get())
                {
                    fileInfo = Core::FileSystem::FileInfo::getFileSequence(i);
                }
                else
                {
                    fileInfo = i;
                }
                fileSystem->open(fileInfo, pos);
                pos += s;
            }
        }

        void MainWindow::_localeEvent(Event::Locale & event)
        {
            _textUpdate();
        }

        void MainWindow::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.mediaButton->setTooltip(_getText(DJV_TEXT("Media popup tooltip")));
            p.memoryCacheMaxSlider->setTooltip(_getText(DJV_TEXT("Memory cache maximum tooltip")));
            p.memoryCacheEnabledButton->setTooltip(_getText(DJV_TEXT("Memory cache enabled tooltip")));
            p.memoryCachePopupWidget->setTooltip(_getText(DJV_TEXT("Memory cache tooltip")));
            p.autoHideButton->setTooltip(_getText(DJV_TEXT("Auto-hide tooltip")));
            p.settingsButton->setTooltip(_getText(DJV_TEXT("Settings tooltip")));
        }

    } // namespace ViewApp
} // namespace djv

