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
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/ITool.h>
#include <djvViewApp/MDIWidget.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/PlaylistWidget.h>
#include <djvViewApp/SDIWidget.h>
#include <djvViewApp/SettingsSystem.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ActionButton.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/MenuBar.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvCore/FileInfo.h>
#include <djvCore/Path.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MainWindow::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ToolButton> settingsButton;
            std::shared_ptr<UI::MenuBar> menuBar;
            std::shared_ptr<SDIWidget> sdiWidget;
            std::shared_ptr<MDIWidget> mdiWidget;
            std::shared_ptr<PlaylistWidget> playlistWidget;
            std::shared_ptr<UI::MDI::Canvas> toolCanvas;
            std::shared_ptr<UI::StackLayout> stackLayout;
            std::shared_ptr<ValueObserver<bool> > closeToolActionObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<WindowMode> > windowModeObserver;
        };
        
        void MainWindow::_init(Core::Context * context)
        {
            UI::Window::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::MainWindow");
            setBackgroundRole(UI::ColorRole::Trough);

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

            p.actions["CloseTool"] = UI::Action::create();
            p.actions["CloseTool"]->setShortcut(GLFW_KEY_ESCAPE);
            for (auto i : p.actions)
            {
                addAction(i.second);
            }

            auto fileNameLabel = UI::Label::create(context);
            fileNameLabel->setTextHAlign(UI::TextHAlign::Left);
            fileNameLabel->setMargin(UI::MetricsRole::Margin);

            auto windowSystem = context->getSystemT<WindowSystem>();
            auto windowActions = windowSystem->getActions();
            auto sdiButton = UI::ActionButton::create(context);
            sdiButton->setShowText(false);
            sdiButton->setShowShortcuts(false);
            sdiButton->addAction(windowActions["SDI"]);
            auto mdiButton = UI::ActionButton::create(context);
            mdiButton->setShowText(false);
            mdiButton->setShowShortcuts(false);
            mdiButton->addAction(windowActions["MDI"]);
            auto playlistButton = UI::ActionButton::create(context);
            playlistButton->setShowText(false);
            playlistButton->setShowShortcuts(false);
            playlistButton->addAction(windowActions["Playlist"]);

            p.settingsButton = UI::ToolButton::create(context);
            p.settingsButton->setIcon("djvIconSettings");

            p.menuBar = UI::MenuBar::create(context);
            p.menuBar->setBackgroundRole(UI::ColorRole::Overlay);
            for (auto i : menus)
            {
                p.menuBar->addChild(i.second);
            }
            p.menuBar->addSeparator();
            p.menuBar->addChild(fileNameLabel);
            p.menuBar->setStretch(fileNameLabel, UI::RowStretch::Expand);
            p.menuBar->addSeparator();
            p.menuBar->addChild(sdiButton);
            p.menuBar->addChild(mdiButton);
            p.menuBar->addChild(playlistButton);
            p.menuBar->addChild(p.settingsButton);

            p.sdiWidget = SDIWidget::create(context);
            p.mdiWidget = MDIWidget::create(context);
            p.playlistWidget = PlaylistWidget::create(context);

            p.toolCanvas = UI::MDI::Canvas::create(context);
            for (auto system : viewSystems)
            {
                for (auto j : system->getTools())
                {
                    p.toolCanvas->addChild(j);
                    j->setVisible(false);
                }
            }
            
            p.stackLayout = UI::StackLayout::create(context);
            auto soloLayout = UI::SoloLayout::create(context);
            soloLayout->addChild(p.sdiWidget);
            soloLayout->addChild(p.mdiWidget);
            soloLayout->addChild(p.playlistWidget);
            p.stackLayout->addChild(soloLayout);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.menuBar);
            vLayout->addExpander();
            p.stackLayout->addChild(vLayout);
            p.stackLayout->addChild(p.toolCanvas);
            addChild(p.stackLayout);

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            p.settingsButton->setClickedCallback(
                [context]
            {
                if (auto system = context->getSystemT<SettingsSystem>())
                {
                    system->showSettings();
                }
            });

            p.closeToolActionObserver = ValueObserver<bool>::create(
                p.actions["CloseTool"]->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        const auto children = widget->_p->toolCanvas->getChildrenT<ITool>();
                        for (auto i = children.rbegin(); i != children.rend(); ++i)
                        {
                            if ((*i)->isVisible())
                            {
                                (*i)->close();
                                break;
                            }
                        }
                    }
                }
            });

            auto fileSystem = getContext()->getSystemT<FileSystem>();
            p.currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                fileSystem->observeCurrentMedia(),
                [fileNameLabel](const std::shared_ptr<Media> & value)
            {
                fileNameLabel->setText(value ? Core::FileSystem::Path(value->getFileName()).getFileName() : std::string());
            });
            
            p.windowModeObserver = ValueObserver<WindowMode>::create(
                windowSystem->observeWindowMode(),
                [soloLayout](WindowMode value)
            {
                soloLayout->setCurrentIndex(static_cast<int>(value));
            });
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

        void MainWindow::_dropEvent(Core::Event::Drop & event)
        {
            auto style = _getStyle();
            const float s = style->getMetric(UI::MetricsRole::SpacingLarge);
            glm::vec2 pos = event.getPointerInfo().projectedPos;
            for (const auto & i : event.getDropPaths())
            {
                auto fileSystem = getContext()->getSystemT<FileSystem>();
                fileSystem->open(i, pos);
                pos += s;
            }
        }

        void MainWindow::_localeEvent(Core::Event::Locale & event)
        {
            DJV_PRIVATE_PTR();
            _textUpdate();
        }

        void MainWindow::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.settingsButton->setTooltip(_getText(DJV_TEXT("Settings tooltip")));
        }

    } // namespace ViewApp
} // namespace djv

