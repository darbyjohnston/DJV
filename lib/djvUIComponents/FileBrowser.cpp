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

#include <djvUIComponents/FileBrowser.h>

#include <djvUIComponents/ActionButton.h>
#include <djvUIComponents/FileBrowserItemView.h>
#include <djvUIComponents/FileBrowserPrivate.h>
#include <djvUIComponents/FileBrowserSettings.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Bellows.h>
#include <djvUI/DialogSystem.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SearchBox.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Splitter.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Toolbar.h>
#include <djvUI/Window.h>

#include <djvAV/IO.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/DirectoryModel.h>
#include <djvCore/FileInfo.h>

#include <GLFW/glfw3.h>

#include <ctime>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct Widget::Private
            {
                std::shared_ptr<FileSystem::DirectoryModel> directoryModel;
                size_t itemCount = 0;

                std::map<std::string, std::shared_ptr<Action> > actions;
                std::shared_ptr<ActionGroup> viewTypeActionGroup;
                std::shared_ptr<ActionGroup> sortActionGroup;
                std::shared_ptr<Menu> directoryMenu;
                std::shared_ptr<Menu> shortcutsMenu;
                std::shared_ptr<Menu> viewMenu;
                std::shared_ptr<Menu> sortMenu;
                std::shared_ptr<ShortcutsWidget> shortcutsWidget;
                std::shared_ptr<DrivesWidget> drivesWidget;
                std::map<std::string, std::shared_ptr<Bellows> > shortcutsBellows;
                std::shared_ptr<ScrollWidget> shortcutsScrollWidget;
                std::shared_ptr<SearchBox> searchBox;
                std::shared_ptr<Label> itemCountLabel;
                std::shared_ptr<ItemView> itemView;
                std::shared_ptr<ScrollWidget> scrollWidget;
                std::shared_ptr<VerticalLayout> layout;

                std::function<void(const FileSystem::FileInfo &)> callback;

                std::shared_ptr<ValueObserver<FileSystem::Path> > pathObserver;
                std::shared_ptr<ListObserver<FileSystem::FileInfo> > fileInfoObserver;
                std::shared_ptr<ValueObserver<bool> > hasUpObserver;
                std::shared_ptr<ValueObserver<bool> > upObserver;
                std::shared_ptr<ValueObserver<bool> > currentObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > historyObserver;
                std::shared_ptr<ValueObserver<size_t> > historyIndexObserver;
                std::shared_ptr<ValueObserver<bool> > hasBackObserver;
                std::shared_ptr<ValueObserver<bool> > backObserver;
                std::shared_ptr<ValueObserver<bool> > hasForwardObserver;
                std::shared_ptr<ValueObserver<bool> > forwardObserver;
                std::shared_ptr<ValueObserver<bool> > editPathObserver;
                std::shared_ptr<ValueObserver<bool> > showShortcutsObserver;
                std::shared_ptr<ValueObserver<bool> > showShortcutsSettingsObserver;
                std::shared_ptr<MapObserver<std::string, bool> > shortcutsBellowsSettingsObserver;
                std::shared_ptr<ValueObserver<float> > shortcutsSplitterSettingsObserver;
                std::shared_ptr<ValueObserver<ViewType> > viewTypeSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > fileSequencesObserver;
                std::shared_ptr<ValueObserver<bool> > fileSequencesSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > showHiddenObserver;
                std::shared_ptr<ValueObserver<bool> > showHiddenSettingsObserver;
                std::shared_ptr<ValueObserver<FileSystem::DirectoryListSort> > sortSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > reverseSortObserver;
                std::shared_ptr<ValueObserver<bool> > reverseSortSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > sortDirectoriesFirstObserver;
                std::shared_ptr<ValueObserver<bool> > sortDirectoriesFirstSettingsObserver;

                std::string getItemCountLabel(size_t, Context *);
            };

            void Widget::_init(Context * context)
            {
                UI::Widget::_init(context);

                setClassName("djv::UI::FileBrowser::Widget");

                DJV_PRIVATE_PTR();
                p.directoryModel = FileSystem::DirectoryModel::create(context);

                p.actions["Back"] = Action::create();
                p.actions["Back"]->setIcon("djvIconArrowLeft");
                p.actions["Back"]->setShortcut(GLFW_KEY_LEFT);
                p.actions["Forward"] = Action::create();
                p.actions["Forward"]->setIcon("djvIconArrowRight");
                p.actions["Forward"]->setShortcut(GLFW_KEY_RIGHT);
                p.actions["Up"] = Action::create();
                p.actions["Up"]->setIcon("djvIconArrowUp");
                p.actions["Up"]->setShortcut(GLFW_KEY_UP);
                p.actions["Current"] = Action::create();
                p.actions["Current"]->setShortcut(GLFW_KEY_PERIOD);
                p.actions["EditPath"] = Action::create();
                p.actions["EditPath"]->setButtonType(ButtonType::Toggle);
                p.actions["EditPath"]->setIcon("djvIconEdit");

                p.actions["ShowShortcuts"] = Action::create();
                p.actions["ShowShortcuts"]->setButtonType(ButtonType::Toggle);
                p.actions["ShowShortcuts"]->setChecked(true);
                p.actions["ShowShortcuts"]->setShortcut(GLFW_KEY_GRAVE_ACCENT);
                p.actions["AddFavorite"] = Action::create();
                p.actions["AddFavorite"]->setIcon("djvIconFavorite");
                p.actions["AddFavorite"]->setShortcut(GLFW_KEY_F, GLFW_MOD_CONTROL);

                p.actions["LargeThumbnails"] = Action::create();
                p.actions["LargeThumbnails"]->setIcon("djvIconThumbnailsLarge");
                p.actions["LargeThumbnails"]->setShortcut(GLFW_KEY_1);
                p.actions["SmallThumbnails"] = Action::create();
                p.actions["SmallThumbnails"]->setIcon("djvIconThumbnailsSmall");
                p.actions["SmallThumbnails"]->setShortcut(GLFW_KEY_2);
                p.actions["ListView"] = Action::create();
                p.actions["ListView"]->setIcon("djvIconListView");
                p.actions["ListView"]->setShortcut(GLFW_KEY_3);
                p.viewTypeActionGroup = ActionGroup::create(ButtonType::Radio);
                p.viewTypeActionGroup->addAction(p.actions["LargeThumbnails"]);
                p.viewTypeActionGroup->addAction(p.actions["SmallThumbnails"]);
                p.viewTypeActionGroup->addAction(p.actions["ListView"]);

                p.actions["FileSequences"] = Action::create();
                p.actions["FileSequences"]->setButtonType(ButtonType::Toggle);
                p.actions["FileSequences"]->setIcon("djvIconFileSequence");
                p.actions["FileSequences"]->setShortcut(GLFW_KEY_S);
                p.actions["ShowHidden"] = Action::create();
                p.actions["ShowHidden"]->setButtonType(ButtonType::Toggle);
                p.actions["ShowHidden"]->setShortcut(GLFW_KEY_N);

                p.actions["SortByName"] = Action::create();
                p.actions["SortByName"]->setShortcut(GLFW_KEY_4);
                p.actions["SortBySize"] = Action::create();
                p.actions["SortBySize"]->setShortcut(GLFW_KEY_5);
                p.actions["SortByTime"] = Action::create();
                p.actions["SortByTime"]->setShortcut(GLFW_KEY_6);
                p.sortActionGroup = ActionGroup::create(ButtonType::Radio);
                p.sortActionGroup->addAction(p.actions["SortByName"]);
                p.sortActionGroup->addAction(p.actions["SortBySize"]);
                p.sortActionGroup->addAction(p.actions["SortByTime"]);
                p.actions["ReverseSort"] = Action::create();
                p.actions["ReverseSort"]->setButtonType(ButtonType::Toggle);
                p.actions["ReverseSort"]->setShortcut(GLFW_KEY_R);
                p.actions["SortDirectoriesFirst"] = Action::create();
                p.actions["SortDirectoriesFirst"]->setButtonType(ButtonType::Toggle);
                p.actions["SortDirectoriesFirst"]->setShortcut(GLFW_KEY_D);

                for (auto action : p.actions)
                {
                    addAction(action.second);
                }

                p.directoryMenu = Menu::create(context);
                p.directoryMenu->addAction(p.actions["Back"]);
                p.directoryMenu->addAction(p.actions["Forward"]);
                p.directoryMenu->addAction(p.actions["Up"]);
                p.directoryMenu->addAction(p.actions["Current"]);
                p.directoryMenu->addAction(p.actions["EditPath"]);

                p.shortcutsMenu = Menu::create(context);
                p.shortcutsMenu->addAction(p.actions["ShowShortcuts"]);
                p.shortcutsMenu->addAction(p.actions["AddFavorite"]);

                p.viewMenu = Menu::create(context);
                p.viewMenu->addAction(p.actions["LargeThumbnails"]);
                p.viewMenu->addAction(p.actions["SmallThumbnails"]);
                p.viewMenu->addAction(p.actions["ListView"]);
                p.viewMenu->addSeparator();
                p.viewMenu->addAction(p.actions["FileSequences"]);
                p.viewMenu->addAction(p.actions["ShowHidden"]);

                p.sortMenu = Menu::create(context);
                p.sortMenu->addAction(p.actions["SortByName"]);
                p.sortMenu->addAction(p.actions["SortBySize"]);
                p.sortMenu->addAction(p.actions["SortByTime"]);
                p.sortMenu->addSeparator();
                p.sortMenu->addAction(p.actions["ReverseSort"]);
                p.sortMenu->addSeparator();
                p.sortMenu->addAction(p.actions["SortDirectoriesFirst"]);

                auto menuBar = MenuBar::create(context);
                menuBar->addChild(p.directoryMenu);
                menuBar->addChild(p.shortcutsMenu);
                menuBar->addChild(p.viewMenu);
                menuBar->addChild(p.sortMenu);

                auto pathWidget = PathWidget::create(context);

                auto topToolBar = Toolbar::create(context);
                topToolBar->addAction(p.actions["Up"]);
                topToolBar->addAction(p.actions["Back"]);
                topToolBar->addAction(p.actions["Forward"]);
                topToolBar->addChild(pathWidget);
                topToolBar->setStretch(pathWidget, RowStretch::Expand);
                topToolBar->addAction(p.actions["EditPath"]);

                p.searchBox = SearchBox::create(context);
                p.itemCountLabel = Label::create(context);
                p.itemCountLabel->setTextHAlign(TextHAlign::Right);
                p.itemCountLabel->setMargin(MetricsRole::MarginSmall);

                auto bottomToolBar = Toolbar::create(context);
                bottomToolBar->addExpander();
                bottomToolBar->addChild(p.itemCountLabel);
                bottomToolBar->addChild(p.searchBox);

                p.shortcutsWidget = ShortcutsWidget::create(context);
                p.drivesWidget = DrivesWidget::create(context);
                p.shortcutsBellows["Shortcuts"] = Bellows::create(context);
                p.shortcutsBellows["Shortcuts"]->addChild(p.shortcutsWidget);
                p.shortcutsBellows["Drives"] = Bellows::create(context);
                p.shortcutsBellows["Drives"]->addChild(p.drivesWidget);
                p.shortcutsBellows["Favorites"] = Bellows::create(context);
                p.shortcutsBellows["Recent"] = Bellows::create(context);
                auto shortcutsLayout = VerticalLayout::create(context);
                shortcutsLayout->setSpacing(MetricsRole::None);
                shortcutsLayout->addChild(p.shortcutsBellows["Shortcuts"]);
                shortcutsLayout->addChild(p.shortcutsBellows["Drives"]);
                shortcutsLayout->addChild(p.shortcutsBellows["Favorites"]);
                shortcutsLayout->addChild(p.shortcutsBellows["Recent"]);
                p.shortcutsScrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                p.shortcutsScrollWidget->setBorder(false);
                p.shortcutsScrollWidget->addChild(shortcutsLayout);

                p.itemView = ItemView::create(context);
                p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                p.scrollWidget->setBorder(false);
                p.scrollWidget->addChild(p.itemView);

                p.layout = VerticalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                p.layout->addChild(menuBar);
                p.layout->addSeparator();
                p.layout->addChild(topToolBar);
                p.layout->addSeparator();
                auto splitter = Layout::Splitter::create(Orientation::Horizontal, context);
                splitter->addChild(p.shortcutsScrollWidget);
                splitter->addChild(p.scrollWidget);
                p.layout->addChild(splitter);
                p.layout->setStretch(splitter, RowStretch::Expand);
                p.layout->addSeparator();
                p.layout->addChild(bottomToolBar);
                addChild(p.layout);

                auto weak = std::weak_ptr<Widget>(std::dynamic_pointer_cast<Widget>(shared_from_this()));
                pathWidget->setPathCallback(
                    [weak](const FileSystem::Path & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->setPath(value);
                    }
                });
                pathWidget->setHistoryIndexCallback(
                    [weak](size_t value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->setHistoryIndex(value);
                    }
                });

                p.shortcutsWidget->setCallback(
                    [weak](const FileSystem::Path & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->setPath(value);
                    }
                });

                p.drivesWidget->setCallback(
                    [weak](const FileSystem::Path & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->setPath(value);
                    }
                });

                p.itemView->setCallback(
                    [weak](const FileSystem::FileInfo & value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (FileSystem::FileType::Directory == value.getType())
                        {
                            widget->_p->directoryModel->setPath(value.getPath());
                        }
                        else if (widget->_p->callback)
                        {
                            widget->_p->callback(value);
                        }
                    }
                });

                p.searchBox->setFilterCallback(
                    [weak](const std::string & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->setFilter(value);
                    }
                });

                p.pathObserver = ValueObserver<FileSystem::Path>::create(
                    p.directoryModel->observePath(),
                    [weak, pathWidget](const FileSystem::Path & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->scrollWidget->setScrollPos(glm::vec2(0.f, 0.f));
                    }
                    pathWidget->setPath(value);
                });

                p.fileInfoObserver = ListObserver<FileSystem::FileInfo>::create(
                    p.directoryModel->observeFileInfo(),
                    [weak, context](const std::vector<FileSystem::FileInfo> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->itemView->setItems(value);
                        widget->_p->itemCount = value.size();
                        widget->_p->itemCountLabel->setText(widget->_p->getItemCountLabel(value.size(), context));
                    }
                });

                p.hasUpObserver = ValueObserver<bool>::create(
                    p.directoryModel->observeHasUp(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->actions["Up"]->setEnabled(value);
                    }
                });

                p.upObserver = ValueObserver<bool>::create(
                    p.actions["Up"]->observeClicked(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->directoryModel->cdUp();
                        }
                    }
                });

                p.currentObserver = ValueObserver<bool>::create(
                    p.actions["Current"]->observeClicked(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->directoryModel->setPath(FileSystem::Path("."));
                        }
                    }
                });

                p.historyObserver = ListObserver<FileSystem::Path>::create(
                    p.directoryModel->observeHistory(),
                    [pathWidget](const std::vector<FileSystem::Path> & value)
                {
                    pathWidget->setHistory(value);
                });

                p.historyIndexObserver = ValueObserver<size_t>::create(
                    p.directoryModel->observeHistoryIndex(),
                    [pathWidget](size_t value)
                {
                    pathWidget->setHistoryIndex(value);
                });

                p.hasBackObserver = ValueObserver<bool>::create(
                    p.directoryModel->observeHasBack(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->actions["Back"]->setEnabled(value);
                    }
                });

                p.backObserver = ValueObserver<bool>::create(
                    p.actions["Back"]->observeClicked(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->directoryModel->goBack();
                        }
                    }
                });

                p.hasForwardObserver = ValueObserver<bool>::create(
                    p.directoryModel->observeHasForward(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->actions["Forward"]->setEnabled(value);
                    }
                });

                p.forwardObserver = ValueObserver<bool>::create(
                    p.actions["Forward"]->observeClicked(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->directoryModel->goForward();
                        }
                    }
                });

                p.editPathObserver = ValueObserver<bool>::create(
                    p.actions["EditPath"]->observeChecked(),
                    [pathWidget](bool value)
                {
                        pathWidget->setEdit(value);
                });

                if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
                {
                    if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                    {
                        p.showShortcutsSettingsObserver = ValueObserver<bool>::create(
                            fileBrowserSettings->observeShowShortcuts(),
                            [weak](bool value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->actions["ShowShortcuts"]->setChecked(value);
                                widget->_p->shortcutsScrollWidget->setVisible(value);
                            }
                        });

                        p.shortcutsBellowsSettingsObserver = MapObserver<std::string, bool>::create(
                            fileBrowserSettings->observeShortcutsBellows(),
                            [weak](const std::map<std::string, bool> & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                for (const auto & i : value)
                                {
                                    widget->_p->shortcutsBellows[i.first]->setOpen(i.second);
                                }
                            }
                        });

                        p.shortcutsSplitterSettingsObserver = ValueObserver<float>::create(
                            fileBrowserSettings->observeShortcutsSplitter(),
                            [splitter](float value)
                        {
                            splitter->setSplit(value);
                        });

                        p.viewTypeSettingsObserver = ValueObserver<ViewType>::create(
                            fileBrowserSettings->observeViewType(),
                            [weak](ViewType value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->setViewType(value);
                            }
                        });

                        p.fileSequencesSettingsObserver = ValueObserver<bool>::create(
                            fileBrowserSettings->observeFileSequences(),
                            [weak](bool value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->directoryModel->setFileSequences(value);
                                widget->_p->actions["FileSequences"]->setChecked(value);
                            }
                        });

                        p.showHiddenSettingsObserver = ValueObserver<bool>::create(
                            fileBrowserSettings->observeShowHidden(),
                            [weak](bool value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->directoryModel->setShowHidden(value);
                                widget->_p->actions["ShowHidden"]->setChecked(value);
                            }
                        });

                        p.sortSettingsObserver = ValueObserver<FileSystem::DirectoryListSort>::create(
                            fileBrowserSettings->observeSort(),
                            [weak](FileSystem::DirectoryListSort value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->directoryModel->setSort(value);
                                widget->_p->sortActionGroup->setChecked(static_cast<int>(value));
                            }
                        });

                        p.reverseSortSettingsObserver = ValueObserver<bool>::create(
                            fileBrowserSettings->observeReverseSort(),
                            [weak](bool value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->directoryModel->setReverseSort(value);
                                widget->_p->actions["ReverseSort"]->setChecked(value);
                            }
                        });

                        p.sortDirectoriesFirstSettingsObserver = ValueObserver<bool>::create(
                            fileBrowserSettings->observeSortDirectoriesFirst(),
                            [weak](bool value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->directoryModel->setSortDirectoriesFirst(value);
                                widget->_p->actions["SortDirectoriesFirst"]->setChecked(value);
                            }
                        });
                    }
                }

                p.viewTypeActionGroup->setRadioCallback(
                    [context](int value)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
                    {
                        if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                        {
                            fileBrowserSettings->setViewType(static_cast<ViewType>(value));
                        }
                    }
                });

                p.sortActionGroup->setRadioCallback(
                    [context](int value)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
                    {
                        if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                        {
                            fileBrowserSettings->setSort(static_cast<FileSystem::DirectoryListSort>(value));
                        }
                    }
                });

                for (auto & i : p.shortcutsBellows)
                {
                    i.second->setOpenCallback(
                        [weak, context](bool)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
                            {
                                if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                                {
                                    std::map<std::string, bool> bellows;
                                    for (const auto & i : widget->_p->shortcutsBellows)
                                    {
                                        bellows[i.first] = i.second->isOpen();
                                    }
                                    fileBrowserSettings->setShortcutsBellows(bellows);
                                }
                            }
                        }
                    });
                }

                splitter->setSplitCallback(
                    [context](float value)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
                    {
                        if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                        {
                            fileBrowserSettings->setShortcutsSplitter(value);
                        }
                    }
                });

                p.showShortcutsObserver = ValueObserver<bool>::create(
                    p.actions["ShowShortcuts"]->observeChecked(),
                    [context](bool value)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
                    {
                        if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                        {
                            fileBrowserSettings->setShowShortcuts(value);
                        }
                    }
                });

                p.fileSequencesObserver = ValueObserver<bool>::create(
                    p.actions["FileSequences"]->observeChecked(),
                    [context](bool value)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
                    {
                        if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                        {
                            fileBrowserSettings->setFileSequences(value);
                        }
                    }
                });

                p.showHiddenObserver = ValueObserver<bool>::create(
                    p.actions["ShowHidden"]->observeChecked(),
                    [context](bool value)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
                    {
                        if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                        {
                            fileBrowserSettings->setShowHidden(value);
                        }
                    }
                });

                p.reverseSortObserver = ValueObserver<bool>::create(
                    p.actions["ReverseSort"]->observeChecked(),
                    [context](bool value)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
                    {
                        if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                        {
                            fileBrowserSettings->setReverseSort(value);
                        }
                    }
                });

                p.sortDirectoriesFirstObserver = ValueObserver<bool>::create(
                    p.actions["SortDirectoriesFirst"]->observeChecked(),
                    [context](bool value)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
                    {
                        if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                        {
                            fileBrowserSettings->setSortDirectoriesFirst(value);
                        }
                    }
                });
            }

            Widget::Widget() :
                _p(new Private)
            {}

            Widget::~Widget()
            {}

            std::shared_ptr<Widget> Widget::create(Context * context)
            {
                auto out = std::shared_ptr<Widget>(new Widget);
                out->_init(context);
                return out;
            }

            void Widget::setPath(const FileSystem::Path & value)
            {
                _p->directoryModel->setPath(value);
            }

            void Widget::setViewType(ViewType value)
            {
                DJV_PRIVATE_PTR();
                p.viewTypeActionGroup->setChecked(static_cast<int>(value));
                p.itemView->setViewType(value);
            }

            void Widget::setCallback(const std::function<void(const FileSystem::FileInfo &)> & value)
            {
                _p->callback = value;
            }

            float Widget::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }
            
            void Widget::_preLayoutEvent(Event::PreLayout& event)
            {
                if (auto style = _getStyle().lock())
                {
                    _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
                }
            }

            void Widget::_layoutEvent(Event::Layout& event)
            {
                if (auto style = _getStyle().lock())
                {
                    _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
                }
            }

            void Widget::_localeEvent(Event::Locale &)
            {
                DJV_PRIVATE_PTR();
                p.actions["Back"]->setTitle(_getText(DJV_TEXT("Back")));
                p.actions["Back"]->setTooltip(_getText(DJV_TEXT("File browser back tooltip")));
                p.actions["Forward"]->setTitle(_getText(DJV_TEXT("Forward")));
                p.actions["Forward"]->setTooltip(_getText(DJV_TEXT("File browser forward tooltip")));
                p.actions["Up"]->setTitle(_getText(DJV_TEXT("Up")));
                p.actions["Up"]->setTooltip(_getText(DJV_TEXT("File browser up tooltip")));
                p.actions["Current"]->setTitle(_getText(DJV_TEXT("Current Directory")));
                p.actions["Current"]->setTooltip(_getText(DJV_TEXT("File browser current directory tooltip")));
                p.actions["EditPath"]->setTitle(_getText(DJV_TEXT("Edit Path")));
                p.actions["EditPath"]->setTooltip(_getText(DJV_TEXT("File browser edit path tooltip")));

                p.actions["ShowShortcuts"]->setTitle(_getText(DJV_TEXT("Show Shortcuts")));
                p.actions["ShowShortcuts"]->setTooltip(_getText(DJV_TEXT("File browser show shortcuts tooltip")));
                p.actions["AddFavorite"]->setTitle(_getText(DJV_TEXT("Add To Favorites")));
                p.actions["AddFavorite"]->setTooltip(_getText(DJV_TEXT("File browser add to favorites tooltip")));

                p.actions["LargeThumbnails"]->setTitle(_getText(DJV_TEXT("Large thumbnails")));
                p.actions["LargeThumbnails"]->setTooltip(_getText(DJV_TEXT("File browser large thumbnails tooltip")));
                p.actions["SmallThumbnails"]->setTitle(_getText(DJV_TEXT("Small thumbnails")));
                p.actions["SmallThumbnails"]->setTooltip(_getText(DJV_TEXT("File browser small thumbnails tooltip")));
                p.actions["ListView"]->setTitle(_getText(DJV_TEXT("List view")));
                p.actions["ListView"]->setTooltip(_getText(DJV_TEXT("File browser list view tooltip")));
                p.actions["FileSequences"]->setTitle(_getText(DJV_TEXT("Enable file sequences")));
                p.actions["FileSequences"]->setTooltip(_getText(DJV_TEXT("File browser file sequences tooltip")));
                p.actions["ShowHidden"]->setTitle(_getText(DJV_TEXT("Show hidden files")));
                p.actions["ShowHidden"]->setTooltip(_getText(DJV_TEXT("File browser show hidden tooltip")));

                p.actions["SortByName"]->setTitle(_getText(DJV_TEXT("Sort By Name")));
                p.actions["SortByName"]->setTooltip(_getText(DJV_TEXT("File browser sort by name tooltip")));
                p.actions["SortBySize"]->setTitle(_getText(DJV_TEXT("Sort By Size")));
                p.actions["SortBySize"]->setTooltip(_getText(DJV_TEXT("File browser sort by size tooltip")));
                p.actions["SortByTime"]->setTitle(_getText(DJV_TEXT("Sort By Time")));
                p.actions["SortByTime"]->setTooltip(_getText(DJV_TEXT("File browser sort by time tooltip")));
                p.actions["ReverseSort"]->setTitle(_getText(DJV_TEXT("Reverse Sort")));
                p.actions["ReverseSort"]->setTooltip(_getText(DJV_TEXT("File browser reverse sort tooltip")));
                p.actions["SortDirectoriesFirst"]->setTitle(_getText(DJV_TEXT("Sort Directories First")));
                p.actions["SortDirectoriesFirst"]->setTooltip(_getText(DJV_TEXT("File browser sort directories first tooltip")));

                p.directoryMenu->setText(_getText(DJV_TEXT("Directory")));
                p.shortcutsMenu->setText(_getText(DJV_TEXT("Shortcuts")));
                p.viewMenu->setText(_getText(DJV_TEXT("View")));
                p.sortMenu->setText(_getText(DJV_TEXT("Sort")));

                p.searchBox->setTooltip(_getText(DJV_TEXT("File browser search tooltip")));

                p.shortcutsBellows["Shortcuts"]->setText(_getText(DJV_TEXT("Shortcuts")));
                p.shortcutsBellows["Drives"]->setText(_getText(DJV_TEXT("Drives")));
                p.shortcutsBellows["Favorites"]->setText(_getText(DJV_TEXT("Favorites")));
                p.shortcutsBellows["Recent"]->setText(_getText(DJV_TEXT("Recent")));

                auto context = getContext();
                p.itemCountLabel->setText(p.getItemCountLabel(p.itemCount, context));
            }

            std::string Widget::Private::getItemCountLabel(size_t size, Context * context)
            {
                std::stringstream ss;
                ss << size << " " << context->getText(DJV_TEXT("items"));
                return ss.str();
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
