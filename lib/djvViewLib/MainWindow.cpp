//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvViewLib/MainWindow.h>

#include <djvViewLib/Application.h>
#include <djvViewLib/FileSystem.h>
#include <djvViewLib/IMDIWidget.h>
#include <djvViewLib/IViewSystem.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/Media.h>
#include <djvViewLib/MediaMDIWidget.h>
#include <djvViewLib/MediaPlaylistWidget.h>
#include <djvViewLib/MediaSDIWidget.h>
#include <djvViewLib/SettingsSystem.h>
#include <djvViewLib/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Shortcut.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolButton.h>

#include <djvCore/FileInfo.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct MainWindow::Private
        {
            std::shared_ptr<UI::MenuBar> menuBar;
            std::shared_ptr<MediaSDIWidget> mediaSDIWidget;
            std::shared_ptr<UI::MDI::Canvas> mediaMDICanvas;
            std::shared_ptr<UI::ScrollWidget> mediaMDIScrollWidget;
            std::map<std::shared_ptr<Media>, std::shared_ptr<MediaMDIWidget> > mediaToMDIWidget;
            std::shared_ptr<MediaPlaylistWidget> mediaPlaylistWidget;
            std::shared_ptr<UI::SoloLayout> soloLayout;
            std::shared_ptr<UI::ToolButton> mediaSDIButton;
            std::shared_ptr<UI::ToolButton> mediaMDIButton;
            std::shared_ptr<UI::ToolButton> mediaPlaylistButton;
            std::shared_ptr<UI::ButtonGroup> windowButtonGroup;
			std::shared_ptr<UI::ToolButton> settingsButton;
            std::shared_ptr<UI::MDI::Canvas> mdiCanvas;
			std::shared_ptr<UI::Action> mdiCloseAction;
            std::shared_ptr<UI::StackLayout> stackLayout;
            std::shared_ptr<ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> > > fileOpenedObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > fileClosedObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<WindowMode> > windowModeObserver;
			std::shared_ptr<ValueObserver<bool> > mdiCloseObserver;
        };
        
        void MainWindow::_init(Core::Context * context)
        {
            UI::Window::_init(context);

            setClassName("djv::ViewLib::MainWindow");
            setBackgroundRole(UI::ColorRole::Trough);

            auto viewSystems = context->getSystemsT<IViewSystem>();
            for (auto i : viewSystems)
            {
                if (auto system = i.lock())
                {
                    for (auto action : system->getActions())
                    {
                        addAction(action.second);
                    }
                }
            }

            DJV_PRIVATE_PTR();
            p.mediaSDIWidget = MediaSDIWidget::create(context);

            p.mediaMDICanvas = UI::MDI::Canvas::create(context);
            p.mediaMDIScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            p.mediaMDIScrollWidget->setBorder(false);
            p.mediaMDIScrollWidget->addWidget(p.mediaMDICanvas);

            p.mediaPlaylistWidget = MediaPlaylistWidget::create(context);

            p.soloLayout = UI::SoloLayout::create(context);
            p.soloLayout->addWidget(p.mediaSDIWidget);
            p.soloLayout->addWidget(p.mediaMDIScrollWidget);
            p.soloLayout->addWidget(p.mediaPlaylistWidget);

            p.mediaSDIButton = UI::ToolButton::create(context);
            p.mediaSDIButton->setIcon("djvIconViewLibSDISmall");
            p.mediaSDIButton->setInsideMargin(UI::MetricsRole::None);
            p.mediaMDIButton = UI::ToolButton::create(context);
            p.mediaMDIButton->setIcon("djvIconViewLibMDISmall");
            p.mediaMDIButton->setInsideMargin(UI::MetricsRole::None);
            p.mediaPlaylistButton = UI::ToolButton::create(context);
            p.mediaPlaylistButton->setIcon("djvIconViewLibPlaylistSmall");
            p.mediaPlaylistButton->setInsideMargin(UI::MetricsRole::None);
            p.windowButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);
            p.windowButtonGroup->addButton(p.mediaSDIButton);
            p.windowButtonGroup->addButton(p.mediaMDIButton);
            p.windowButtonGroup->addButton(p.mediaPlaylistButton);

			p.settingsButton = UI::ToolButton::create(context);
			p.settingsButton->setIcon("djvIconSettingsSmall");
			p.settingsButton->setInsideMargin(UI::MetricsRole::None);

            p.mdiCanvas = UI::MDI::Canvas::create(context);
            for (auto i : viewSystems)
            {
                if (auto system = i.lock())
                {
                    for (auto j : system->getMDIWidgets())
                    {
                        j.widget->setParent(p.mdiCanvas);
						j.widget->setVisible(false);
                    }
                }
            }

			p.mdiCloseAction = UI::Action::create();
			p.mdiCloseAction->setShortcut(GLFW_KEY_ESCAPE);
			addAction(p.mdiCloseAction);

            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            for (auto i : viewSystems)
            {
                if (auto system = i.lock())
                {
                    auto newMenu = system->getMenu();
                    if (newMenu.menu)
                    {
                        menus[newMenu.sortKey] = newMenu.menu;
                    }
                }
            }
            p.menuBar = UI::MenuBar::create(context);
            p.menuBar->setBackgroundRole(UI::ColorRole::Overlay);
            for (auto i : menus)
            {
                p.menuBar->addMenu(i.second);
            }
            p.menuBar->addWidget(p.mediaSDIButton);
            p.menuBar->addWidget(p.mediaMDIButton);
			p.menuBar->addWidget(p.mediaPlaylistButton);
			p.menuBar->addWidget(p.settingsButton);
            
            p.stackLayout = UI::StackLayout::create(context);
            p.stackLayout->addWidget(p.soloLayout);
            p.stackLayout->addWidget(p.mdiCanvas);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addWidget(p.menuBar);
            vLayout->addExpander();
            p.stackLayout->addWidget(vLayout);
            addWidget(p.stackLayout);

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            p.mediaMDICanvas->setActiveCallback(
                [weak, context](const std::shared_ptr<UI::Widget> & value)
            {
                if (auto mainWindow = weak.lock())
                {
                    if (auto mediaMDIWidget = std::dynamic_pointer_cast<MediaMDIWidget>(value))
                    {
                        if (auto fileSystem = context->getSystemT<FileSystem>().lock())
                        {
                            fileSystem->setCurrentMedia(mediaMDIWidget->getMedia());
                        }
                    }
                }
            });

            p.windowButtonGroup->setPushCallback(
                [context](int radio)
            {
                if (auto windowSystem = context->getSystemT<WindowSystem>().lock())
                {
                    windowSystem->setWindowMode(static_cast<WindowMode>(radio));
                }
            });

			p.settingsButton->setClickedCallback(
				[context]
			{
				if (auto settingsSystem = context->getSystemT<SettingsSystem>().lock())
				{
					settingsSystem->showSettings();
				}
			});

			p.mdiCloseObserver = ValueObserver<bool>::create(
				p.mdiCloseAction->observeClicked(),
				[weak](bool value)
			{
				if (value)
				{
					if (auto mainWindow = weak.lock())
					{
						const auto children = mainWindow->_p->mdiCanvas->getChildrenT<UI::MDI::IWidget>();
						for (auto i = children.rbegin(); i != children.rend(); ++i)
						{
							if ((*i)->isVisible())
							{
								(*i)->hide();
								break;
							}
						}
					}
				}
			});

            if (auto fileSystem = context->getSystemT<FileSystem>().lock())
            {
                p.fileOpenedObserver = ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> >::create(
                    fileSystem->observeOpened(),
                    [weak, context](const std::pair<std::shared_ptr<Media>, glm::vec2> & value)
                {
                    if (value.first)
                    {
                        if (auto mainWindow = weak.lock())
                        {
                            auto mediaMDIWidget = MediaMDIWidget::create(context);
							mediaMDIWidget->setTitle(Core::FileSystem::FileInfo(value.first->getFileName()).getFileName(Frame::Invalid, false));
							mediaMDIWidget->setMedia(value.first);
                            glm::vec2 size(mainWindow->getSize() / 2.f);
							mediaMDIWidget->resize(size);
							mediaMDIWidget->setParent(mainWindow->_p->mediaMDICanvas);
                            glm::vec2 pos(0.f, 0.f);
                            if (value.second.x >= 0.f && value.second.y >= 0.f)
                            {
                                pos = value.second - mainWindow->_p->mediaMDICanvas->getGeometry().min;
                            }
                            else
                            {
                                pos = mainWindow->getGeometry().getSize() / 2.f - mainWindow->_p->mediaMDICanvas->getGeometry().min;
                            }
                            mainWindow->_p->mediaMDICanvas->setWidgetPos(mediaMDIWidget, pos - size / 2.f);
                            mainWindow->_p->mediaToMDIWidget[value.first] = mediaMDIWidget;
							auto weak = std::weak_ptr<MediaMDIWidget>(std::dynamic_pointer_cast<MediaMDIWidget>(mediaMDIWidget));
							mediaMDIWidget->setMaximizeCallback(
                                [weak, context]
                            {
								if (auto widget = weak.lock())
								{
									if (auto fileSystem = context->getSystemT<FileSystem>().lock())
									{
										fileSystem->setCurrentMedia(widget->getMedia());
									}
									if (auto windowSystem = context->getSystemT<WindowSystem>().lock())
									{
										windowSystem->setWindowMode(WindowMode::SDI);
									}
								}
                            });
							mediaMDIWidget->setCloseCallback(
                                [weak, context]
                            {
								if (auto widget = weak.lock())
								{
									if (auto fileSystem = context->getSystemT<FileSystem>().lock())
									{
										fileSystem->close(widget->getMedia());
									}
								}
                            });
                        }
                    }
                });

                p.fileClosedObserver = ValueObserver<std::shared_ptr<Media>>::create(
                    fileSystem->observeClosed(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto mainWindow = weak.lock())
                    {
                        const auto i = mainWindow->_p->mediaToMDIWidget.find(value);
                        if (i != mainWindow->_p->mediaToMDIWidget.end())
                        {
                            i->second->setParent(nullptr);
                            mainWindow->_p->mediaToMDIWidget.erase(i);
                        }
                    }
                });

                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto mainWindow = weak.lock())
                    {
                        const auto i = mainWindow->_p->mediaToMDIWidget.find(value);
                        if (i != mainWindow->_p->mediaToMDIWidget.end())
                        {
                            i->second->moveToFront();
                        }
                        mainWindow->_p->mediaSDIWidget->setMedia(value);
                    }
                });
            }

            if (auto windowSystem = context->getSystemT<WindowSystem>().lock())
            {
                _p->windowModeObserver = ValueObserver<WindowMode>::create(
                    windowSystem->observeWindowMode(),
                    [weak](WindowMode value)
                {
                    if (auto mainWindow = weak.lock())
                    {
                        std::shared_ptr<UI::Widget> currentWidget;
                        switch (value)
                        {
                        case WindowMode::SDI: currentWidget = mainWindow->_p->mediaSDIWidget; break;
                        case WindowMode::MDI: currentWidget = mainWindow->_p->mediaMDIScrollWidget; break;
                        case WindowMode::Playlist: currentWidget = mainWindow->_p->mediaPlaylistWidget; break;
                        default: break;
                        }
                        mainWindow->_p->soloLayout->setCurrentWidget(currentWidget);
                        mainWindow->_p->windowButtonGroup->setChecked(static_cast<int>(value));
                    }
                });
            }
        }

        MainWindow::MainWindow() :
            _p(new Private)
        {}

        MainWindow::~MainWindow()
        {}

        std::shared_ptr<MainWindow> MainWindow::create(Core::Context * context)
        {
            auto out = std::shared_ptr<MainWindow>(new MainWindow);
            out->_init(context);
            return out;
        }

        void MainWindow::_dropEvent(Core::Event::Drop& event)
        {
            if (auto style = _getStyle().lock())
            {
                if (auto fileSystem = getContext()->getSystemT<FileSystem>().lock())
                {
                    const float s = style->getMetric(UI::MetricsRole::SpacingLarge);
                    glm::vec2 pos = event.getPointerInfo().projectedPos;
                    for (const auto & i : event.getDropPaths())
                    {
                        fileSystem->open(i, pos);
                        pos += s;
                    }
                }
            }
        }

        void MainWindow::_localeEvent(Core::Event::Locale & event)
        {
            DJV_PRIVATE_PTR();
            p.mediaSDIButton->setTooltip(_getText(DJV_TEXT("djv::ViewLib::MainWindow", "SDI Window Mode Tooltip")));
            p.mediaMDIButton->setTooltip(_getText(DJV_TEXT("djv::ViewLib::MainWindow", "MDI Window Mode Tooltip")));
            p.mediaPlaylistButton->setTooltip(_getText(DJV_TEXT("djv::ViewLib::MainWindow", "Playlist Window Mode Tooltip")));
        }

    } // namespace ViewLib
} // namespace djv

