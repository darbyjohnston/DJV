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
#include <djvViewLib/IToolWidget.h>
#include <djvViewLib/Media.h>
#include <djvViewLib/MediaWidget.h>
#include <djvViewLib/PlaybackSystem.h>
#include <djvViewLib/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
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
            std::shared_ptr<UI::Label> currentMediaLabel;
            std::shared_ptr<UI::Toolbar> toolBar;
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
            auto fileSystem = context->getSystemT<FileSystem>().lock();
            auto playbackSystem = context->getSystemT<PlaybackSystem>().lock();
            std::map<std::string, std::shared_ptr<UI::Widget> > toolBarWidgets;
            for (auto i : viewSystems)
            {
                if (auto system = i.lock())
                {
                    for (auto action : system->getActions())
                    {
                        addAction(action.second);
                    }
                    if (system != fileSystem && system != playbackSystem)
                    {
                        auto toolBarWidget = system->getToolBarWidget();
                        if (toolBarWidget.widget)
                        {
                            toolBarWidgets[toolBarWidget.sortKey] = toolBarWidget.widget;
                        }
                    }
                }
            }

            p.actions["CloseTool"] = UI::Action::create();
            p.actions["CloseTool"]->setShortcut(GLFW_KEY_ESCAPE);
            for (auto i : p.actions)
            {
                addAction(i.second);
            }

            p.currentMediaLabel = UI::Label::create(context);
            p.currentMediaLabel->setMargin(UI::MetricsRole::MarginSmall);

            p.toolBar = UI::Toolbar::create(context);
            p.toolBar->setBackgroundRole(UI::ColorRole::Overlay);
            p.toolBar->addWidget(fileSystem->getToolBarWidget().widget);
            p.toolBar->addWidget(p.currentMediaLabel);
            p.toolBar->addExpander();
            for (auto i : toolBarWidgets)
            {
                p.toolBar->addWidget(i.second);
            }

            p.mediaWidget = MediaWidget::create(context);

            p.mdiCanvas = UI::MDI::Canvas::create(context);
            for (auto i : viewSystems)
            {
                if (auto system = i.lock())
                {
                    for (auto j : system->getToolWidgets())
                    {
                        j->setParent(p.mdiCanvas);
						j->setVisible(false);
                    }
                }
            }
            
            p.stackLayout = UI::StackLayout::create(context);
            p.stackLayout->addWidget(p.mediaWidget);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->addWidget(p.toolBar);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addExpander();
            p.stackLayout->addWidget(vLayout);
            p.stackLayout->addWidget(p.mdiCanvas);
            addWidget(p.stackLayout);

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            p.closeToolActionObserver = ValueObserver<bool>::create(
                p.actions["CloseTool"]->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto mainWindow = weak.lock())
                    {
                        const auto children = mainWindow->_p->mdiCanvas->getChildrenT<IToolWidget>();
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
            _textUpdate();
        }

        void MainWindow::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            std::string currentMediaText;
            if (auto media = p.mediaWidget->getMedia())
            {
                currentMediaText = Core::FileSystem::Path(media->getFileName()).getFileName();
            }
            p.currentMediaLabel->setText(currentMediaText);
        }

    } // namespace ViewLib
} // namespace djv

