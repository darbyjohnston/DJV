// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowser.h>

#include <djvUIComponents/FileBrowserItemView.h>
#include <djvUIComponents/FileBrowserPrivate.h>
#include <djvUIComponents/FileBrowserSettings.h>
#include <djvUIComponents/ListViewHeader.h>
#include <djvUIComponents/SearchBox.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Label.h>
#include <djvUI/PopupButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvAV/IOSystem.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>
#include <djvCore/DirectoryModel.h>
#include <djvCore/DrivesModel.h>
#include <djvCore/FileInfo.h>
#include <djvCore/RecentFilesModel.h>
#include <djvCore/StringFormat.h>

#include <ctime>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct FileBrowser::Private
            {
                std::shared_ptr<FileSystem::DirectoryModel> directoryModel;
                FileSystem::Path path;
                size_t itemCount = 0;
                std::shared_ptr<ShortcutsModel> shortcutsModel;
                std::shared_ptr<FileSystem::RecentFilesModel> recentPathsModel;
                std::shared_ptr<FileSystem::DrivesModel> drivesModel;

                std::map<std::string, std::shared_ptr<Action> > actions;
                std::shared_ptr<ActionGroup> viewTypeActionGroup;
                std::shared_ptr<ActionGroup> sortActionGroup;
                std::shared_ptr<PopupButton> shortcutsPopupButton;
                std::shared_ptr<PopupButton> recentPathsPopupButton;
                std::shared_ptr<PopupButton> drivesPopupButton;
                std::shared_ptr<PopupButton> sortPopupButton;
                std::shared_ptr<PopupButton> menuPopupButton;
                std::shared_ptr<ListViewHeader> listViewHeader;
                std::shared_ptr<VerticalLayout> itemViewLayout;
                std::shared_ptr<ItemView> itemView;
                std::shared_ptr<ScrollWidget> scrollWidget;
                std::shared_ptr<Label> itemCountLabel;
                std::shared_ptr<SearchBox> searchBox;
                std::shared_ptr<VerticalLayout> layout;

                std::function<void(const FileSystem::FileInfo &)> callback;

                std::shared_ptr<ValueObserver<FileSystem::Path> > pathObserver;
                std::shared_ptr<ListObserver<FileSystem::FileInfo> > fileInfoObserver;
                std::shared_ptr<ValueObserver<bool> > hasUpObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > historyObserver;
                std::shared_ptr<ValueObserver<size_t> > historyIndexObserver;
                std::shared_ptr<ValueObserver<bool> > hasBackObserver;
                std::shared_ptr<ValueObserver<bool> > hasForwardObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > shortcutsObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > shortcutsSettingsObserver;
                std::shared_ptr<ListObserver<FileSystem::FileInfo> > recentPathsObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > recentPathsSettingsObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > drivesObserver;
                std::shared_ptr<ValueObserver<ViewType> > viewTypeSettingsObserver;
                std::shared_ptr<ValueObserver<AV::Image::Size> > thumbnailSizeSettingsObserver;
                std::shared_ptr<ListObserver<float> > listViewHeaderSplitSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > fileSequencesSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > showHiddenSettingsObserver;
                std::shared_ptr<ValueObserver<FileSystem::DirectoryListSort> > sortSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > reverseSortSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > sortDirectoriesFirstSettingsObserver;
                std::shared_ptr<MapObserver<std::string, ShortcutDataPair> > keyShortcutsObserver;
            };

            void FileBrowser::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::FileBrowser::FileBrowser");

                p.directoryModel = FileSystem::DirectoryModel::create(context);
                auto io = context->getSystemT<AV::IO::System>();
                p.directoryModel->setFileSequenceExtensions(io->getSequenceExtensions());
                p.shortcutsModel = ShortcutsModel::create(context);
                p.recentPathsModel = FileSystem::RecentFilesModel::create();
                p.recentPathsModel->setFilesMax(10);
                p.drivesModel = FileSystem::DrivesModel::create(context);

                p.actions["Back"] = Action::create();
                p.actions["Back"]->setIcon("djvIconArrowLeft");
                p.actions["Forward"] = Action::create();
                p.actions["Forward"]->setIcon("djvIconArrowRight");
                p.actions["Up"] = Action::create();
                p.actions["Up"]->setIcon("djvIconArrowUp");

                p.actions["Tiles"] = Action::create();
                p.actions["Tiles"]->setIcon("djvIconTileView");
                p.actions["List"] = Action::create();
                p.actions["List"]->setIcon("djvIconListView");
                p.viewTypeActionGroup = ActionGroup::create(ButtonType::Radio);
                p.viewTypeActionGroup->setActions({
                    p.actions["Tiles"],
                    p.actions["List"] });
                p.actions["IncreaseThumbnailSize"] = Action::create();
                p.actions["IncreaseThumbnailSize"]->setIcon("djvIconAdd");
                p.actions["DecreaseThumbnailSize"] = Action::create();
                p.actions["DecreaseThumbnailSize"]->setIcon("djvIconSubtract");

                p.actions["FileSequences"] = Action::create();
                p.actions["FileSequences"]->setButtonType(ButtonType::Toggle);
                p.actions["FileSequences"]->setIcon("djvIconFileSequence");
                p.actions["ShowHidden"] = Action::create();
                p.actions["ShowHidden"]->setButtonType(ButtonType::Toggle);

                p.actions["SortByName"] = Action::create();
                p.actions["SortBySize"] = Action::create();
                p.actions["SortByTime"] = Action::create();
                p.sortActionGroup = ActionGroup::create(ButtonType::Radio);
                p.sortActionGroup->setActions({
                    p.actions["SortByName"],
                    p.actions["SortBySize"],
                    p.actions["SortByTime"] });
                p.actions["ReverseSort"] = Action::create();
                p.actions["ReverseSort"]->setButtonType(ButtonType::Toggle);
                p.actions["SortDirectoriesFirst"] = Action::create();
                p.actions["SortDirectoriesFirst"]->setButtonType(ButtonType::Toggle);

                for (auto action : p.actions)
                {
                    addAction(action.second);
                }

                auto pathWidget = PathWidget::create(context);

                p.shortcutsPopupButton = PopupButton::create(MenuButtonStyle::Tool, context);
                p.shortcutsPopupButton->setIcon("djvIconBookmark");

                p.recentPathsPopupButton = PopupButton::create(MenuButtonStyle::Tool, context);
                p.recentPathsPopupButton->setIcon("djvIconFileRecent");

                p.drivesPopupButton = PopupButton::create(MenuButtonStyle::Tool, context);
                p.drivesPopupButton->setIcon("djvIconDrives");

                p.sortPopupButton = PopupButton::create(MenuButtonStyle::Tool, context);
                p.sortPopupButton->setIcon("djvIconSort");

                p.menuPopupButton = PopupButton::create(MenuButtonStyle::Tool, context);
                p.menuPopupButton->setIcon("djvIconMenu");

                p.searchBox = SearchBox::create(context);

                auto toolBar = ToolBar::create(context);
                toolBar->addChild(p.shortcutsPopupButton);
                toolBar->addChild(p.recentPathsPopupButton);
                toolBar->addChild(p.drivesPopupButton);
                toolBar->addAction(p.actions["Back"]);
                toolBar->addAction(p.actions["Forward"]);
                toolBar->addAction(p.actions["Up"]);
                toolBar->addChild(pathWidget);
                toolBar->setStretch(pathWidget, RowStretch::Expand);
                toolBar->addChild(p.sortPopupButton);
                toolBar->addChild(p.searchBox);
                toolBar->addChild(p.menuPopupButton);

                p.listViewHeader = ListViewHeader::create(context);
                p.listViewHeader->setText({ std::string(), std::string(), std::string() });
                p.itemView = ItemView::create(context);
                p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                p.scrollWidget->setBorder(false);
                p.scrollWidget->addChild(p.itemView);
                p.scrollWidget->setShadowOverlay({ Side::Top });

                p.itemCountLabel = Label::create(context);
                p.itemCountLabel->setHAlign(HAlign::Right);
                p.itemCountLabel->setVAlign(VAlign::Bottom);
                p.itemCountLabel->setMargin(MetricsRole::Margin);

                p.layout = VerticalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                p.layout->addChild(toolBar);
                p.layout->addSeparator();
                p.itemViewLayout = VerticalLayout::create(context);
                p.itemViewLayout->setSpacing(MetricsRole::None);
                p.itemViewLayout->addChild(p.listViewHeader);
                auto stackLayout2 = StackLayout::create(context);
                stackLayout2->addChild(p.scrollWidget);
                stackLayout2->addChild(p.itemCountLabel);
                p.itemViewLayout->addChild(stackLayout2);
                p.itemViewLayout->setStretch(stackLayout2, RowStretch::Expand);
                p.layout->addChild(p.itemViewLayout);
                p.layout->setStretch(p.itemViewLayout, RowStretch::Expand);
                addChild(p.layout);

                auto weak = std::weak_ptr<FileBrowser>(std::dynamic_pointer_cast<FileBrowser>(shared_from_this()));
                p.actions["Up"]->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->directoryModel->cdUp();
                        }
                    });

                p.actions["Back"]->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->directoryModel->goBack();
                        }
                    });

                p.actions["Forward"]->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->directoryModel->goForward();
                        }
                    });

                auto contextWeak = std::weak_ptr<Context>(context);
                p.actions["IncreaseThumbnailSize"]->setClickedCallback(
                    [contextWeak]
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                auto size = fileBrowserSettings->observeThumbnailSize()->get();
                                size.w = Math::clamp(static_cast<int>(size.w * 1.25F), thumbnailSizeRange.getMin(), thumbnailSizeRange.getMax());
                                size.h = static_cast<int>(ceilf(size.w / 2.F));
                                fileBrowserSettings->setThumbnailSize(size);
                            }
                        }
                    });

                p.actions["DecreaseThumbnailSize"]->setClickedCallback(
                    [contextWeak]
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                auto size = fileBrowserSettings->observeThumbnailSize()->get();
                                size.w = Math::clamp(static_cast<int>(size.w * .75F), thumbnailSizeRange.getMin(), thumbnailSizeRange.getMax());
                                size.h = static_cast<int>(ceilf(size.w / 2.F));
                                fileBrowserSettings->setThumbnailSize(size);
                            }
                        }
                    });

                p.actions["FileSequences"]->setCheckedCallback(
                    [contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setFileSequences(value);
                            }
                        }
                    });

                p.actions["ShowHidden"]->setCheckedCallback(
                    [contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setShowHidden(value);
                            }
                        }
                    });

                p.actions["ReverseSort"]->setCheckedCallback(
                    [contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setReverseSort(value);
                            }
                        }
                    });

                p.actions["SortDirectoriesFirst"]->setCheckedCallback(
                    [contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setSortDirectoriesFirst(value);
                            }
                        }
                    });

                pathWidget->setPathCallback(
                    [weak](const FileSystem::Path & value)
                {
                    if (auto widget = weak.lock())
                    {
                        try
                        {
                            widget->_p->directoryModel->setPath(value);
                        }
                        catch (const std::exception& e)
                        {
                            std::vector<std::string> messages;
                            messages.push_back(String::Format("{0}: {1}").
                                arg(value.get()).
                                arg(widget->_getText(DJV_TEXT("error_file_browser_cannot_set_the_path"))));
                            messages.push_back(e.what());
                            widget->_log(String::join(messages, ' '), LogLevel::Error);
                        }
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

                p.shortcutsPopupButton->setOpenCallback(
                    [weak, contextWeak]() -> std::shared_ptr<Widget>
                    {
                        std::shared_ptr<Widget> out;
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                auto shortcutsWidget = ShortcutsWidget::create(widget->_p->shortcutsModel, context);
                                shortcutsWidget->setPath(widget->_p->path);
                                shortcutsWidget->setCallback(
                                    [weak](const FileSystem::Path& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->directoryModel->setPath(value);
                                            widget->_p->shortcutsPopupButton->close();
                                        }
                                    });
                                out = shortcutsWidget;
                            }
                        }
                        return out;
                    });

                p.recentPathsPopupButton->setOpenCallback(
                    [weak, contextWeak]() -> std::shared_ptr<Widget>
                    {
                        std::shared_ptr<Widget> out;
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                auto recentPathsWidget = RecentPathsWidget::create(widget->_p->recentPathsModel, context);
                                recentPathsWidget->setCallback(
                                    [weak](const FileSystem::Path& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->directoryModel->setPath(value);
                                            widget->_p->recentPathsPopupButton->close();
                                        }
                                    });
                                out = recentPathsWidget;
                            }
                        }
                        return out;
                    });

                p.drivesPopupButton->setOpenCallback(
                    [weak, contextWeak]() -> std::shared_ptr<Widget>
                    {
                        std::shared_ptr<Widget> out;
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                auto drivesWidget = DrivesWidget::create(widget->_p->drivesModel, context);
                                drivesWidget->setCallback(
                                    [weak](const FileSystem::Path& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->directoryModel->setPath(value);
                                            widget->_p->drivesPopupButton->close();
                                        }
                                    });
                                out = drivesWidget;
                            }
                        }
                        return out;
                    });

                p.sortPopupButton->setOpenCallback(
                    [weak, contextWeak]() -> std::shared_ptr<Widget>
                    {
                        std::shared_ptr<Widget> out;
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                out = SortWidget::create(widget->_p->actions, context);
                            }
                        }
                        return out;
                    });

                p.menuPopupButton->setOpenCallback(
                    [weak, contextWeak]() -> std::shared_ptr<Widget>
                    {
                        std::shared_ptr<Widget> out;
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                out = Menu::create(widget->_p->actions, context);
                            }
                        }
                        return out;
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
                            std::string s = value.getPath().getDirectoryName();
                            FileSystem::Path::removeTrailingSeparator(s);
                            widget->_p->recentPathsModel->addFile(s);
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
                    [weak, pathWidget](const FileSystem::Path& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->path = value;
                        widget->_p->scrollWidget->setScrollPos(glm::vec2(0.F, 0.F));
                    }
                    pathWidget->setPath(value);
                });

                p.fileInfoObserver = ListObserver<FileSystem::FileInfo>::create(
                    p.directoryModel->observeFileInfo(),
                    [weak](const std::vector<FileSystem::FileInfo> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->itemView->setItems(value);
                        widget->_p->itemCount = value.size();
                        widget->_p->itemCountLabel->setText(widget->_getItemCountLabel(value.size()));
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

                p.hasForwardObserver = ValueObserver<bool>::create(
                    p.directoryModel->observeHasForward(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->actions["Forward"]->setEnabled(value);
                    }
                });

                auto settingsSystem = context->getSystemT<Settings::System>();
                auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                p.shortcutsSettingsObserver = ListObserver<FileSystem::Path>::create(
                    fileBrowserSettings->observeShortcuts(),
                    [weak](const std::vector<FileSystem::Path> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->shortcutsModel->setShortcuts(value);
                    }
                });

                p.recentPathsSettingsObserver = ListObserver<FileSystem::Path>::create(
                    fileBrowserSettings->observeRecentPaths(),
                    [weak](const std::vector<FileSystem::Path>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            std::vector<FileSystem::FileInfo> tmp;
                            for (const auto& i : value)
                            {
                                tmp.push_back(i);
                            }
                            widget->_p->recentPathsModel->setFiles(tmp);
                        }
                    });

                p.viewTypeSettingsObserver = ValueObserver<ViewType>::create(
                    fileBrowserSettings->observeViewType(),
                    [weak](ViewType value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->viewTypeActionGroup->setChecked(static_cast<int>(value));
                        widget->_p->listViewHeader->setVisible(ViewType::List == value);
                        widget->_p->itemView->setViewType(value);
                    }
                });

                p.thumbnailSizeSettingsObserver = ValueObserver<AV::Image::Size>::create(
                    fileBrowserSettings->observeThumbnailSize(),
                    [weak](const AV::Image::Size & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->itemView->setThumbnailSize(value);
                    }
                });

                p.listViewHeaderSplitSettingsObserver = ListObserver<float>::create(
                    fileBrowserSettings->observeListViewHeaderSplit(),
                    [weak](const std::vector<float> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->listViewHeader->setSplit(value);
                        widget->_p->itemView->setSplit(value);
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
                        widget->_p->listViewHeader->setSort(static_cast<size_t>(value));
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
                        widget->_p->listViewHeader->setReverseSort(value);
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

                p.keyShortcutsObserver = MapObserver<std::string, ShortcutDataPair>::create(
                    fileBrowserSettings->observeKeyShortcuts(),
                    [weak](const std::map<std::string, ShortcutDataPair>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            auto i = value.find("file_browser_shortcut_back");
                            if (i != value.end())
                            {
                                widget->_p->actions["Back"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_forward");
                            if (i != value.end())
                            {
                                widget->_p->actions["Forward"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_up");
                            if (i != value.end())
                            {
                                widget->_p->actions["Up"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_tiles");
                            if (i != value.end())
                            {
                                widget->_p->actions["Tiles"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_list");
                            if (i != value.end())
                            {
                                widget->_p->actions["List"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_increase_thumbnail_size");
                            if (i != value.end())
                            {
                                widget->_p->actions["IncreaseThumbnailSize"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_decrease_thumbnail_size");
                            if (i != value.end())
                            {
                                widget->_p->actions["DecreaseThumbnailSize"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_file_sequences");
                            if (i != value.end())
                            {
                                widget->_p->actions["FileSequences"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_show_hidden");
                            if (i != value.end())
                            {
                                widget->_p->actions["ShowHidden"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_sort_by_name");
                            if (i != value.end())
                            {
                                widget->_p->actions["SortByName"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_sort_by_size");
                            if (i != value.end())
                            {
                                widget->_p->actions["SortBySize"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_sort_by_time");
                            if (i != value.end())
                            {
                                widget->_p->actions["SortByTime"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_reverse_sort");
                            if (i != value.end())
                            {
                                widget->_p->actions["ReverseSort"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_sort_directories_first");
                            if (i != value.end())
                            {
                                widget->_p->actions["SortDirectoriesFirst"]->setShortcuts(i->second);
                            }
                        }
                    });

                p.viewTypeActionGroup->setRadioCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setViewType(static_cast<ViewType>(value));
                            }
                        }
                    });

                p.sortActionGroup->setRadioCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setSort(static_cast<FileSystem::DirectoryListSort>(value));
                            }
                        }
                    });

                p.shortcutsObserver = ListObserver<FileSystem::Path>::create(
                    p.shortcutsModel->observeShortcuts(),
                    [contextWeak](const std::vector<FileSystem::Path>& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setShortcuts(value);
                            }
                        }
                    });

                p.recentPathsObserver = ListObserver<FileSystem::FileInfo>::create(
                    p.recentPathsModel->observeFiles(),
                    [weak, contextWeak](const std::vector<FileSystem::FileInfo>& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->recentPathsPopupButton->setEnabled(value.size());
                            }
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                std::vector<FileSystem::Path> tmp;
                                for (const auto& i : value)
                                {
                                    tmp.push_back(i.getPath());
                                }
                                fileBrowserSettings->setRecentPaths(tmp);
                            }
                        }
                    });

                p.drivesObserver = ListObserver<FileSystem::Path>::create(
                    p.drivesModel->observeDrives(),
                    [weak](const std::vector<FileSystem::Path>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->drivesPopupButton->setEnabled(value.size());
                        }
                    });

                p.listViewHeader->setSortCallback(
                    [contextWeak](size_t sort, bool reverse)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setSort(static_cast<FileSystem::DirectoryListSort>(sort));
                                fileBrowserSettings->setReverseSort(reverse);
                            }
                        }
                    });

                p.listViewHeader->setSplitCallback(
                    [contextWeak](const std::vector<float> & value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setListViewHeaderSplit(value);
                            }
                        }
                    });
            }

            FileBrowser::FileBrowser() :
                _p(new Private)
            {}

            FileBrowser::~FileBrowser()
            {}

            std::shared_ptr<FileBrowser> FileBrowser::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<FileBrowser>(new FileBrowser);
                out->_init(context);
                return out;
            }

            void FileBrowser::setFileExtensions(const std::set<std::string>& value)
            {
                _p->directoryModel->setFileExtensions(value);
            }

            const FileSystem::Path& FileBrowser::getPath() const
            {
                return _p->directoryModel->observePath()->get();
            }

            void FileBrowser::setPath(const FileSystem::Path & value)
            {
                _p->directoryModel->setPath(value);
            }

            void FileBrowser::setCallback(const std::function<void(const FileSystem::FileInfo &)> & value)
            {
                _p->callback = value;
            }

            float FileBrowser::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }
            
            void FileBrowser::_preLayoutEvent(Event::PreLayout& event)
            {
                const auto& style = _getStyle();
                _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
            }

            void FileBrowser::_layoutEvent(Event::Layout& event)
            {
                const auto& style = _getStyle();
                _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
            }

            void FileBrowser::_initEvent(Event::Init & event)
            {
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.actions["Back"]->setText(_getText(DJV_TEXT("file_browser_back")));
                    p.actions["Back"]->setTooltip(_getText(DJV_TEXT("file_browser_back_tooltip")));
                    p.actions["Forward"]->setText(_getText(DJV_TEXT("file_browser_forward")));
                    p.actions["Forward"]->setTooltip(_getText(DJV_TEXT("file_browser_forward_tooltip")));
                    p.actions["Up"]->setText(_getText(DJV_TEXT("file_browser_up")));
                    p.actions["Up"]->setTooltip(_getText(DJV_TEXT("file_browser_up_tooltip")));

                    p.actions["Tiles"]->setText(_getText(DJV_TEXT("file_browser_settings_tile_view")));
                    p.actions["Tiles"]->setTooltip(_getText(DJV_TEXT("file_browser_settings_tile_view_tooltip")));
                    p.actions["List"]->setText(_getText(DJV_TEXT("file_browser_settings_list_view")));
                    p.actions["List"]->setTooltip(_getText(DJV_TEXT("file_browser_settings_list_view_tooltip")));
                    p.actions["IncreaseThumbnailSize"]->setText(_getText(DJV_TEXT("file_browser_settings_increase_thumbnail")));
                    p.actions["IncreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("file_browser_settings_increase_thumbnail_tooltip")));
                    p.actions["DecreaseThumbnailSize"]->setText(_getText(DJV_TEXT("file_browser_settings_decrease_thumbnail")));
                    p.actions["DecreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("file_browser_settings_decrease_thumbnail_tooltip")));

                    p.actions["FileSequences"]->setText(_getText(DJV_TEXT("file_browser_settings_enable_file_sequences")));
                    p.actions["FileSequences"]->setTooltip(_getText(DJV_TEXT("file_browser_settings_file_sequences_tooltip")));
                    p.actions["ShowHidden"]->setText(_getText(DJV_TEXT("file_browser_settings_show_hidden_files")));
                    p.actions["ShowHidden"]->setTooltip(_getText(DJV_TEXT("file_browser_settings_show_hidden_tooltip")));

                    p.actions["SortByName"]->setText(_getText(DJV_TEXT("file_browser_sort_name")));
                    p.actions["SortByName"]->setTooltip(_getText(DJV_TEXT("file_browser_sort_name_tooltip")));
                    p.actions["SortBySize"]->setText(_getText(DJV_TEXT("file_browser_sort_size")));
                    p.actions["SortBySize"]->setTooltip(_getText(DJV_TEXT("file_browser_sort_size_tooltip")));
                    p.actions["SortByTime"]->setText(_getText(DJV_TEXT("file_browser_sort_time")));
                    p.actions["SortByTime"]->setTooltip(_getText(DJV_TEXT("file_browser_sort_time_tooltip")));
                    p.actions["ReverseSort"]->setText(_getText(DJV_TEXT("file_browser_reverse_sort")));
                    p.actions["ReverseSort"]->setTooltip(_getText(DJV_TEXT("file_browser_reverse_sort_tooltip")));
                    p.actions["SortDirectoriesFirst"]->setText(_getText(DJV_TEXT("file_browser_sort_directories_first")));
                    p.actions["SortDirectoriesFirst"]->setTooltip(_getText(DJV_TEXT("file_browser_sort_directories_first_tooltip")));

                    p.listViewHeader->setText(
                        {
                            _getText(DJV_TEXT("file_browser_column_name")),
                            _getText(DJV_TEXT("file_browser_column_size")),
                            _getText(DJV_TEXT("file_browser_column_time"))
                        }
                    );

                    p.itemCountLabel->setText(_getItemCountLabel(p.itemCount));

                    p.shortcutsPopupButton->setTooltip(_getText(DJV_TEXT("file_browser_shortcuts_tooltip")));
                    p.recentPathsPopupButton->setTooltip(_getText(DJV_TEXT("file_browser_recent_paths_tooltip")));
                    p.drivesPopupButton->setTooltip(_getText(DJV_TEXT("file_browser_drives_tooltip")));
                    p.sortPopupButton->setTooltip(_getText(DJV_TEXT("file_browser_sort_tooltip")));

                    p.searchBox->setTooltip(_getText(DJV_TEXT("file_browser_search_tooltip")));
                }
            }

            std::string FileBrowser::_getItemCountLabel(size_t size) const
            {
                std::stringstream ss;
                ss << size << " " << _getText(DJV_TEXT("file_browser_items"));
                return ss.str();
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
