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

#include <djvViewLib/MainWindow.h>

#include <djvViewLib/Application.h>
#include <djvViewLib/FileSystem.h>
#include <djvViewLib/ITool.h>
#include <djvViewLib/Media.h>
#include <djvViewLib/MediaWidget.h>
#include <djvViewLib/SettingsSystem.h>
#include <djvViewLib/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/FlatButton.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/MenuBar.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>

#include <djvCore/FileInfo.h>
#include <djvCore/Path.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct MainWindow::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::FlatButton> settingsButton;
            std::shared_ptr<UI::MenuBar> menuBar;
            std::shared_ptr<MediaWidget> mediaWidget;
            std::shared_ptr<UI::MDI::Canvas> mdiCanvas;
            std::shared_ptr<UI::StackLayout> stackLayout;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<bool> > closeToolActionObserver;
        };
        
        void MainWindow::_init(Core::Context * context)
        {
            UI::Window::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::ViewLib::MainWindow");
            setBackgroundRole(UI::ColorRole::Trough);

            auto viewSystems = context->getSystemsT<IViewSystem>();
            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            for (auto i : viewSystems)
            {
                if (auto system = i.lock())
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
            }

            p.actions["CloseTool"] = UI::Action::create();
            p.actions["CloseTool"]->setShortcut(GLFW_KEY_ESCAPE);
            for (auto i : p.actions)
            {
                addAction(i.second);
            }

            p.settingsButton = UI::FlatButton::create(context);
            p.settingsButton->setIcon("djvIconSettings");

            p.menuBar = UI::MenuBar::create(context);
            p.menuBar->setBackgroundRole(UI::ColorRole::Overlay);
            for (auto i : menus)
            {
                p.menuBar->addChild(i.second);
            }
            p.menuBar->addExpander();
            p.menuBar->addChild(p.settingsButton);

            p.mediaWidget = MediaWidget::create(context);

            p.mdiCanvas = UI::MDI::Canvas::create(context);
            for (auto i : viewSystems)
            {
                if (auto system = i.lock())
                {
                    for (auto j : system->getTools())
                    {
                        p.mdiCanvas->addChild(j);
                        j->setVisible(false);
                    }
                }
            }
            
            p.stackLayout = UI::StackLayout::create(context);
            p.stackLayout->addChild(p.mediaWidget);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.menuBar);
            vLayout->addExpander();
            p.stackLayout->addChild(vLayout);
            p.stackLayout->addChild(p.mdiCanvas);
            addChild(p.stackLayout);

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            p.settingsButton->setClickedCallback(
                [context]
            {
                if (auto system = context->getSystemT<SettingsSystem>().lock())
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
                    if (auto mainWindow = weak.lock())
                    {
                        const auto children = mainWindow->_p->mdiCanvas->getChildrenT<ITool>();
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

            if (auto fileSystem = context->getSystemT<FileSystem>().lock())
            {
                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto mainWindow = weak.lock())
                    {
                        mainWindow->_p->mediaWidget->setMedia(value);
                        mainWindow->_textUpdate();
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

        void MainWindow::_dropEvent(Core::Event::Drop & event)
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
            _textUpdate();
        }

        void MainWindow::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.settingsButton->setTooltip(_getText(DJV_TEXT("ViewLib settings tooltip")));
        }

    } // namespace ViewLib
} // namespace djv

