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
#include <djvUI/Drawer.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/PopupButton.h>
#include <djvUI/PopupMenu.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvAV/IOSystem.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/DirectoryModel.h>
#include <djvSystem/DrivesModel.h>
#include <djvSystem/FileInfo.h>
#include <djvSystem/PathFunc.h>
#include <djvSystem/RecentFilesModel.h>

#include <djvMath/MathFunc.h>

#include <djvCore/StringFormat.h>
#include <djvCore/StringFunc.h>

#include <ctime>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace FileBrowser
        {
            struct FileBrowser::Private
            {
                System::File::DirectoryListOptions options;
                std::shared_ptr<System::File::DirectoryModel> directoryModel;
                System::File::Path path;
                size_t itemCount = 0;
                std::shared_ptr<ShortcutsModel> shortcutsModel;
                std::shared_ptr<System::File::RecentFilesModel> recentPathsModel;
                std::shared_ptr<System::File::DrivesModel> drivesModel;
                std::vector<System::File::Info> selected;

                std::map<std::string, std::shared_ptr<UI::Action> > actions;
                std::shared_ptr<UI::ActionGroup> viewTypeActionGroup;
                std::shared_ptr<UI::ActionGroup> sortActionGroup;
                std::shared_ptr<UI::Drawer> pathsDrawer;
                std::shared_ptr<UI::PopupButton> thumbnailPopupButton;
                std::shared_ptr<SearchBox> searchBox;
                std::shared_ptr<UI::Menu> menu;
                std::shared_ptr<UI::PopupMenu> popupMenu;
                std::shared_ptr<ListViewHeader> listViewHeader;
                std::shared_ptr<UI::VerticalLayout> itemViewLayout;
                std::shared_ptr<ItemView> itemView;
                std::shared_ptr<UI::ScrollWidget> scrollWidget;
                std::shared_ptr<UI::Text::Label> itemCountLabel;
                std::shared_ptr<UI::PushButton> acceptButton;
                std::shared_ptr<UI::PushButton> cancelButton;
                std::shared_ptr<UI::VerticalLayout> layout;

                std::function<void(const std::vector<System::File::Info>&)> callback;
                std::function<void(void)> cancelCallback;

                std::shared_ptr<Observer::Value<System::File::Path> > pathObserver;
                std::shared_ptr<Observer::List<System::File::Info> > fileInfoObserver;
                std::shared_ptr<Observer::Value<bool> > hasUpObserver;
                std::shared_ptr<Observer::List<System::File::Path> > historyObserver;
                std::shared_ptr<Observer::Value<size_t> > historyIndexObserver;
                std::shared_ptr<Observer::Value<bool> > hasBackObserver;
                std::shared_ptr<Observer::Value<bool> > hasForwardObserver;
                std::shared_ptr<Observer::Value<bool> > pathsOpenSettingsObserver;
                std::shared_ptr<Observer::List<System::File::Path> > shortcutsObserver;
                std::shared_ptr<Observer::List<System::File::Path> > shortcutsSettingsObserver;
                std::shared_ptr<Observer::List<System::File::Info> > recentPathsObserver;
                std::shared_ptr<Observer::List<System::File::Path> > recentPathsSettingsObserver;
                std::shared_ptr<Observer::Value<UI::ViewType> > viewTypeSettingsObserver;
                std::shared_ptr<Observer::Value<Image::Size> > thumbnailSizeSettingsObserver;
                std::shared_ptr<Observer::List<float> > listViewHeaderSplitSettingsObserver;
                std::shared_ptr<Observer::Value<bool> > fileSequencesSettingsObserver;
                std::shared_ptr<Observer::Value<bool> > showHiddenSettingsObserver;
                std::shared_ptr<Observer::Value<System::File::DirectoryListSort> > sortSettingsObserver;
                std::shared_ptr<Observer::Value<bool> > reverseSortSettingsObserver;
                std::shared_ptr<Observer::Value<bool> > sortDirectoriesFirstSettingsObserver;
                std::shared_ptr<Observer::Map<std::string, UI::ShortcutDataPair> > keyShortcutsObserver;
            };

            void FileBrowser::_init(UI::SelectionType selectionType, const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UIComponents::FileBrowser::FileBrowser");

                auto io = context->getSystemT<AV::IO::IOSystem>();
                p.options.sequenceExtensions = io->getSequenceExtensions();
                p.directoryModel = System::File::DirectoryModel::create(context);
                p.shortcutsModel = ShortcutsModel::create(context);
                p.recentPathsModel = System::File::RecentFilesModel::create();
                p.recentPathsModel->setMax(10);
                p.drivesModel = System::File::DrivesModel::create(context);

                p.actions["Paths"] = UI::Action::create();
                p.actions["Paths"]->setButtonType(UI::ButtonType::Toggle);
                p.actions["Paths"]->setIcon("djvIconDrawerLeft");

                p.actions["Back"] = UI::Action::create();
                p.actions["Back"]->setIcon("djvIconArrowLeft");
                p.actions["Forward"] = UI::Action::create();
                p.actions["Forward"]->setIcon("djvIconArrowRight");
                p.actions["Up"] = UI::Action::create();
                p.actions["Up"]->setIcon("djvIconArrowUp");

                p.actions["SelectAll"] = UI::Action::create();
                p.actions["SelectNone"] = UI::Action::create();
                p.actions["InvertSelection"] = UI::Action::create();

                p.actions["Tiles"] = UI::Action::create();
                p.actions["Tiles"]->setIcon("djvIconTileView");
                p.actions["List"] = UI::Action::create();
                p.actions["List"]->setIcon("djvIconListView");
                p.viewTypeActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
                p.viewTypeActionGroup->setActions({
                    p.actions["Tiles"],
                    p.actions["List"] });
                p.actions["IncreaseThumbnailSize"] = UI::Action::create();
                p.actions["IncreaseThumbnailSize"]->setIcon("djvIconAdd");
                p.actions["DecreaseThumbnailSize"] = UI::Action::create();
                p.actions["DecreaseThumbnailSize"]->setIcon("djvIconSubtract");

                p.actions["FileSequences"] = UI::Action::create();
                p.actions["FileSequences"]->setButtonType(UI::ButtonType::Toggle);
                p.actions["FileSequences"]->setIcon("djvIconFileSequence");
                p.actions["ShowHidden"] = UI::Action::create();
                p.actions["ShowHidden"]->setButtonType(UI::ButtonType::Toggle);

                p.actions["SortByName"] = UI::Action::create();
                p.actions["SortBySize"] = UI::Action::create();
                p.actions["SortByTime"] = UI::Action::create();
                p.sortActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
                p.sortActionGroup->setActions({
                    p.actions["SortByName"],
                    p.actions["SortBySize"],
                    p.actions["SortByTime"] });
                p.actions["ReverseSort"] = UI::Action::create();
                p.actions["ReverseSort"]->setButtonType(UI::ButtonType::Toggle);
                p.actions["SortDirectoriesFirst"] = UI::Action::create();
                p.actions["SortDirectoriesFirst"]->setButtonType(UI::ButtonType::Toggle);

                for (auto action : p.actions)
                {
                    addAction(action.second);
                }

                p.pathsDrawer = UI::Drawer::create(UI::Side::Left, context);

                auto pathWidget = PathWidget::create(context);

                p.thumbnailPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::Tool, context);
                p.thumbnailPopupButton->setIcon("djvIconThumbnailSize");

                p.searchBox = SearchBox::create(context);

                p.menu = UI::Menu::create(context);
                p.menu->setIcon("djvIconMenu");
                p.menu->setMinimumSizeRole(UI::MetricsRole::None);
                p.menu->addAction(p.actions["Paths"]);
                p.menu->addSeparator();
                p.menu->addAction(p.actions["Forward"]);
                p.menu->addAction(p.actions["Back"]);
                p.menu->addAction(p.actions["Up"]);
                p.menu->addSeparator();
                p.menu->addAction(p.actions["SortByName"]);
                p.menu->addAction(p.actions["SortBySize"]);
                p.menu->addAction(p.actions["SortByTime"]);
                p.menu->addAction(p.actions["ReverseSort"]);
                p.menu->addAction(p.actions["SortDirectoriesFirst"]);
                p.menu->addSeparator();
                p.menu->addAction(p.actions["SelectAll"]);
                p.menu->addAction(p.actions["SelectNone"]);
                p.menu->addAction(p.actions["InvertSelection"]);
                p.menu->addSeparator();
                p.menu->addAction(p.actions["Tiles"]);
                p.menu->addAction(p.actions["List"]);
                p.menu->addSeparator();
                p.menu->addAction(p.actions["IncreaseThumbnailSize"]);
                p.menu->addAction(p.actions["DecreaseThumbnailSize"]);
                p.menu->addSeparator();
                p.menu->addAction(p.actions["FileSequences"]);
                p.menu->addAction(p.actions["ShowHidden"]);

                p.popupMenu = UI::PopupMenu::create(context);
                p.popupMenu->setMenu(p.menu);

                auto toolBar = UI::ToolBar::create(context);
                toolBar->addAction(p.actions["Paths"]);
                toolBar->addAction(p.actions["Back"]);
                toolBar->addAction(p.actions["Forward"]);
                toolBar->addAction(p.actions["Up"]);
                toolBar->addChild(pathWidget);
                toolBar->setStretch(pathWidget, UI::RowStretch::Expand);
                toolBar->addChild(p.thumbnailPopupButton);
                toolBar->addChild(p.searchBox);
                toolBar->addChild(p.popupMenu);

                p.listViewHeader = ListViewHeader::create(context);
                p.listViewHeader->setText({ std::string(), std::string(), std::string() });
                p.itemView = ItemView::create(selectionType, context);
                p.scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                p.scrollWidget->setBorder(false);
                p.scrollWidget->addChild(p.itemView);
                p.scrollWidget->setShadowOverlay({ UI::Side::Top });

                p.itemCountLabel = UI::Text::Label::create(context);
                p.itemCountLabel->setHAlign(UI::HAlign::Right);
                p.itemCountLabel->setVAlign(UI::VAlign::Bottom);
                p.itemCountLabel->setMargin(UI::MetricsRole::Margin);

                p.acceptButton = UI::PushButton::create(context);
                p.cancelButton = UI::PushButton::create(context);

                p.layout = UI::VerticalLayout::create(context);
                p.layout->setSpacing(UI::MetricsRole::None);
                p.layout->addChild(toolBar);
                p.layout->addSeparator();
                auto hLayout = UI::HorizontalLayout::create(context);
                hLayout->setSpacing(UI::MetricsRole::None);
                hLayout->addChild(p.pathsDrawer);
                p.itemViewLayout = UI::VerticalLayout::create(context);
                p.itemViewLayout->setSpacing(UI::MetricsRole::None);
                p.itemViewLayout->addChild(p.listViewHeader);
                auto stackLayout = UI::StackLayout::create(context);
                stackLayout->addChild(p.scrollWidget);
                stackLayout->addChild(p.itemCountLabel);
                p.itemViewLayout->addChild(stackLayout);
                p.itemViewLayout->setStretch(stackLayout, UI::RowStretch::Expand);
                hLayout->addChild(p.itemViewLayout);
                hLayout->setStretch(p.itemViewLayout, UI::RowStretch::Expand);
                p.layout->addChild(hLayout);
                p.layout->setStretch(hLayout, UI::RowStretch::Expand);
                p.layout->addSeparator();
                hLayout = UI::HorizontalLayout::create(context);
                hLayout->setMargin(UI::MetricsRole::MarginSmall);
                hLayout->setSpacing(UI::MetricsRole::SpacingSmall);
                hLayout->addExpander();
                hLayout->addChild(p.acceptButton);
                hLayout->addChild(p.cancelButton);
                p.layout->addChild(hLayout);
                addChild(p.layout);

                _optionsUpdate();
                _selectedUpdate();

                auto weak = std::weak_ptr<FileBrowser>(std::dynamic_pointer_cast<FileBrowser>(shared_from_this()));
                p.actions["Paths"]->setCheckedCallback(
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->pathsDrawer->setOpen(value);
                        }
                    });

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

                p.actions["SelectAll"]->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->itemView->selectAll();
                        }
                    });
                p.actions["SelectNone"]->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->itemView->selectNone();
                        }
                    });
                p.actions["InvertSelection"]->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->itemView->invertSelection();
                        }
                    });

                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.actions["IncreaseThumbnailSize"]->setClickedCallback(
                    [contextWeak]
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                            auto size = fileBrowserSettings->observeThumbnailSize()->get();
                            size.w = Math::clamp(static_cast<int>(size.w * 1.25F), thumbnailSizeRange.getMin(), thumbnailSizeRange.getMax());
                            size.h = static_cast<int>(ceilf(size.w / 2.F));
                            fileBrowserSettings->setThumbnailSize(size);
                        }
                    });
                p.actions["DecreaseThumbnailSize"]->setClickedCallback(
                    [contextWeak]
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                            auto size = fileBrowserSettings->observeThumbnailSize()->get();
                            size.w = Math::clamp(static_cast<int>(size.w * .75F), thumbnailSizeRange.getMin(), thumbnailSizeRange.getMax());
                            size.h = static_cast<int>(ceilf(size.w / 2.F));
                            fileBrowserSettings->setThumbnailSize(size);
                        }
                    });

                p.actions["FileSequences"]->setCheckedCallback(
                    [contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                            fileBrowserSettings->setFileSequences(value);
                        }
                    });

                p.actions["ShowHidden"]->setCheckedCallback(
                    [contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                            fileBrowserSettings->setShowHidden(value);
                        }
                    });

                p.actions["ReverseSort"]->setCheckedCallback(
                    [contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                            fileBrowserSettings->setReverseSort(value);
                        }
                    });
                p.actions["SortDirectoriesFirst"]->setCheckedCallback(
                    [contextWeak](bool value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                            fileBrowserSettings->setSortDirectoriesFirst(value);
                        }
                    });

                p.pathsDrawer->setOpenCallback(
                    [weak, contextWeak]() -> std::shared_ptr<Widget>
                    {
                        std::shared_ptr<Widget> out;
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                auto drawerWidget = DrawerWidget::create(
                                    widget->_p->directoryModel,
                                    widget->_p->shortcutsModel,
                                    widget->_p->recentPathsModel,
                                    widget->_p->drivesModel,
                                    context);
                                out = drawerWidget;

                                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                                auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                                fileBrowserSettings->setPathsOpen(true);
                            }
                        }
                        return out;
                    });

                p.pathsDrawer->setCloseCallback(
                    [contextWeak](const std::shared_ptr<Widget>& widget)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                            fileBrowserSettings->setPathsOpen(false);
                        }
                    });

                pathWidget->setPathCallback(
                    [weak](const System::File::Path & value)
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
                            widget->_log(String::join(messages, ' '), System::LogLevel::Error);
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

                p.thumbnailPopupButton->setOpenCallback(
                    [weak, contextWeak]() -> std::shared_ptr<Widget>
                    {
                        std::shared_ptr<Widget> out;
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                out = ThumbnailWidget::create(context);
                            }
                        }
                        return out;
                    });

                p.searchBox->setFilterCallback(
                    [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->options.filter = value;
                            widget->_optionsUpdate();
                        }
                    });

                p.itemView->setSelectedCallback(
                    [weak](const std::vector<System::File::Info>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->selected = value;
                            widget->_selectedUpdate();
                        }
                    });

                p.itemView->setActivatedCallback(
                    [weak](const std::vector<System::File::Info>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (!value.empty())
                            {
                                const auto& first = value[0];
                                const auto& path = first.getPath();
                                if (System::File::Type::Directory == first.getType())
                                {
                                    widget->_p->directoryModel->setPath(path);
                                }
                                else
                                {
                                    std::string s = path.getDirectoryName();
                                    System::File::removeTrailingSeparator(s);
                                    widget->_p->recentPathsModel->addFile(s);
                                    widget->_p->callback(value);
                                }
                            }
                        }
                    });

                p.acceptButton->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->callback)
                            {
                                widget->_p->callback(widget->_p->selected);
                            }
                        }
                    });
                p.cancelButton->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->cancelCallback)
                            {
                                widget->_p->cancelCallback();
                            }
                        }
                    });

                p.pathObserver = Observer::Value<System::File::Path>::create(
                    p.directoryModel->observePath(),
                    [weak, pathWidget](const System::File::Path& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->path = value;
                        pathWidget->setPath(value);
                        widget->_p->scrollWidget->setScrollPos(glm::vec2(0.F, 0.F));
                    }
                });

                p.fileInfoObserver = Observer::List<System::File::Info>::create(
                    p.directoryModel->observeInfo(),
                    [weak](const std::vector<System::File::Info>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->itemView->setItems(value);
                        widget->_p->itemCount = value.size();
                        widget->_p->itemCountLabel->setText(widget->_getItemCountLabel(value.size()));
                    }
                });

                p.hasUpObserver = Observer::Value<bool>::create(
                    p.directoryModel->observeHasUp(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->actions["Up"]->setEnabled(value);
                    }
                });

                p.historyObserver = Observer::List<System::File::Path>::create(
                    p.directoryModel->observeHistory(),
                    [pathWidget](const std::vector<System::File::Path> & value)
                {
                    pathWidget->setHistory(value);
                });

                p.historyIndexObserver = Observer::Value<size_t>::create(
                    p.directoryModel->observeHistoryIndex(),
                    [pathWidget](size_t value)
                {
                    pathWidget->setHistoryIndex(value);
                });

                p.hasBackObserver = Observer::Value<bool>::create(
                    p.directoryModel->observeHasBack(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->actions["Back"]->setEnabled(value);
                    }
                });

                p.hasForwardObserver = Observer::Value<bool>::create(
                    p.directoryModel->observeHasForward(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->actions["Forward"]->setEnabled(value);
                    }
                });

                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>();
                p.pathsOpenSettingsObserver = Observer::Value<bool>::create(
                    fileBrowserSettings->observePathsOpen(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->pathsDrawer->setOpen(value, false);
                            widget->_p->actions["Paths"]->setChecked(value);
                        }
                    });

                p.shortcutsSettingsObserver = Observer::List<System::File::Path>::create(
                    fileBrowserSettings->observeShortcuts(),
                    [weak](const std::vector<System::File::Path>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->shortcutsModel->setShortcuts(value);
                        }
                    });

                p.recentPathsSettingsObserver = Observer::List<System::File::Path>::create(
                    fileBrowserSettings->observeRecentPaths(),
                    [weak](const std::vector<System::File::Path>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            std::vector<System::File::Info> tmp;
                            for (const auto& i : value)
                            {
                                tmp.push_back(i);
                            }
                            widget->_p->recentPathsModel->setFiles(tmp);
                        }
                    });

                p.viewTypeSettingsObserver = Observer::Value<UI::ViewType>::create(
                    fileBrowserSettings->observeViewType(),
                    [weak](UI::ViewType value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->viewTypeActionGroup->setChecked(static_cast<int>(value));
                        widget->_p->listViewHeader->setVisible(UI::ViewType::List == value);
                        widget->_p->itemView->setViewType(value);
                    }
                });

                p.thumbnailSizeSettingsObserver = Observer::Value<Image::Size>::create(
                    fileBrowserSettings->observeThumbnailSize(),
                    [weak](const Image::Size & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->itemView->setThumbnailSize(value);
                    }
                });

                p.listViewHeaderSplitSettingsObserver = Observer::List<float>::create(
                    fileBrowserSettings->observeListViewHeaderSplit(),
                    [weak](const std::vector<float> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->listViewHeader->setSplit(value);
                        widget->_p->itemView->setSplit(value);
                    }
                });

                p.fileSequencesSettingsObserver = Observer::Value<bool>::create(
                    fileBrowserSettings->observeFileSequences(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->options.sequences = value;
                        widget->_p->actions["FileSequences"]->setChecked(value);
                        widget->_optionsUpdate();
                    }
                });

                p.showHiddenSettingsObserver = Observer::Value<bool>::create(
                    fileBrowserSettings->observeShowHidden(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->options.showHidden = value;
                        widget->_p->actions["ShowHidden"]->setChecked(value);
                        widget->_optionsUpdate();
                    }
                });

                p.sortSettingsObserver = Observer::Value<System::File::DirectoryListSort>::create(
                    fileBrowserSettings->observeSort(),
                    [weak](System::File::DirectoryListSort value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->options.sort = value;
                        widget->_p->sortActionGroup->setChecked(static_cast<int>(value));
                        widget->_p->listViewHeader->setSort(static_cast<size_t>(value));
                        widget->_optionsUpdate();
                    }
                });

                p.reverseSortSettingsObserver = Observer::Value<bool>::create(
                    fileBrowserSettings->observeReverseSort(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->options.reverseSort = value;
                        widget->_p->actions["ReverseSort"]->setChecked(value);
                        widget->_p->listViewHeader->setReverseSort(value);
                        widget->_optionsUpdate();
                    }
                });

                p.sortDirectoriesFirstSettingsObserver = Observer::Value<bool>::create(
                    fileBrowserSettings->observeSortDirectoriesFirst(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->options.sortDirectoriesFirst = value;
                        widget->_p->actions["SortDirectoriesFirst"]->setChecked(value);
                        widget->_optionsUpdate();
                    }
                });

                p.keyShortcutsObserver = Observer::Map<std::string, UI::ShortcutDataPair>::create(
                    fileBrowserSettings->observeKeyShortcuts(),
                    [weak](const std::map<std::string, UI::ShortcutDataPair>& value)
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
                            i = value.find("file_browser_shortcut_select_all");
                            if (i != value.end())
                            {
                                widget->_p->actions["SelectAll"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_select_none");
                            if (i != value.end())
                            {
                                widget->_p->actions["SelectNone"]->setShortcuts(i->second);
                            }
                            i = value.find("file_browser_shortcut_invert_selection");
                            if (i != value.end())
                            {
                                widget->_p->actions["InvertSelection"]->setShortcuts(i->second);
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
                            i = value.find("file_browser_shortcut_paths");
                            if (i != value.end())
                            {
                                widget->_p->actions["Paths"]->setShortcuts(i->second);
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
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setViewType(static_cast<UI::ViewType>(value));
                            }
                        }
                    });

                p.sortActionGroup->setRadioCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setSort(static_cast<System::File::DirectoryListSort>(value));
                            }
                        }
                    });

                p.shortcutsObserver = Observer::List<System::File::Path>::create(
                    p.shortcutsModel->observeShortcuts(),
                    [contextWeak](const std::vector<System::File::Path>& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setShortcuts(value);
                            }
                        }
                    });

                p.recentPathsObserver = Observer::List<System::File::Info>::create(
                    p.recentPathsModel->observeFiles(),
                    [weak, contextWeak](const std::vector<System::File::Info>& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                std::vector<System::File::Path> tmp;
                                for (const auto& i : value)
                                {
                                    tmp.push_back(i.getPath());
                                }
                                fileBrowserSettings->setRecentPaths(tmp);
                            }
                        }
                    });

                p.listViewHeader->setSortCallback(
                    [contextWeak](size_t sort, bool reverse)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setSort(static_cast<System::File::DirectoryListSort>(sort));
                                fileBrowserSettings->setReverseSort(reverse);
                            }
                        }
                    });

                p.listViewHeader->setSplitCallback(
                    [contextWeak](const std::vector<float> & value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
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

            std::shared_ptr<FileBrowser> FileBrowser::create(UI::SelectionType selectionType, const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<FileBrowser>(new FileBrowser);
                out->_init(selectionType, context);
                return out;
            }

            void FileBrowser::setFileExtensions(const std::set<std::string>& value)
            {
                if (value == _p->options.extensions)
                    return;
                _p->options.extensions = value;
                _optionsUpdate();
            }

            const System::File::Path& FileBrowser::getPath() const
            {
                return _p->directoryModel->observePath()->get();
            }

            void FileBrowser::setPath(const System::File::Path & value)
            {
                _p->directoryModel->setPath(value);
            }

            void FileBrowser::setCallback(const std::function<void(const std::vector<System::File::Info>&)> & value)
            {
                _p->callback = value;
            }

            void FileBrowser::setCancelCallback(const std::function<void(void)>& value)
            {
                _p->cancelCallback = value;
            }

            float FileBrowser::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }
            
            void FileBrowser::_preLayoutEvent(System::Event::PreLayout& event)
            {
                const auto& style = _getStyle();
                _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
            }

            void FileBrowser::_layoutEvent(System::Event::Layout& event)
            {
                const auto& style = _getStyle();
                _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
            }

            void FileBrowser::_initEvent(System::Event::Init & event)
            {
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.actions["Paths"]->setTooltip(_getText(DJV_TEXT("file_browser_paths_tooltip")));

                    p.actions["Back"]->setText(_getText(DJV_TEXT("file_browser_back")));
                    p.actions["Back"]->setTooltip(_getText(DJV_TEXT("file_browser_back_tooltip")));
                    p.actions["Forward"]->setText(_getText(DJV_TEXT("file_browser_forward")));
                    p.actions["Forward"]->setTooltip(_getText(DJV_TEXT("file_browser_forward_tooltip")));
                    p.actions["Up"]->setText(_getText(DJV_TEXT("file_browser_up")));
                    p.actions["Up"]->setTooltip(_getText(DJV_TEXT("file_browser_up_tooltip")));

                    p.actions["SelectAll"]->setText(_getText(DJV_TEXT("file_browser_select_all")));
                    p.actions["SelectAll"]->setTooltip(_getText(DJV_TEXT("file_browser_select_all_tooltip")));
                    p.actions["SelectNone"]->setText(_getText(DJV_TEXT("file_browser_select_none")));
                    p.actions["SelectNone"]->setTooltip(_getText(DJV_TEXT("file_browser_select_none_tooltip")));
                    p.actions["InvertSelection"]->setText(_getText(DJV_TEXT("file_browser_invert_selection")));
                    p.actions["InvertSelection"]->setTooltip(_getText(DJV_TEXT("file_browser_invert_selection_tooltip")));

                    p.actions["Tiles"]->setText(_getText(DJV_TEXT("file_browser_tile_view")));
                    p.actions["Tiles"]->setTooltip(_getText(DJV_TEXT("file_browser_tile_view_tooltip")));
                    p.actions["List"]->setText(_getText(DJV_TEXT("file_browser_list_view")));
                    p.actions["List"]->setTooltip(_getText(DJV_TEXT("file_browser_list_view_tooltip")));
                    p.actions["IncreaseThumbnailSize"]->setText(_getText(DJV_TEXT("file_browser_increase_thumbnail_size")));
                    p.actions["IncreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("file_browser_increase_thumbnail_size_tooltip")));
                    p.actions["DecreaseThumbnailSize"]->setText(_getText(DJV_TEXT("file_browser_decrease_thumbnail_size")));
                    p.actions["DecreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("file_browser_decrease_thumbnail_size_tooltip")));

                    p.actions["FileSequences"]->setText(_getText(DJV_TEXT("file_browser_enable_file_sequences")));
                    p.actions["FileSequences"]->setTooltip(_getText(DJV_TEXT("file_browser_file_sequences_tooltip")));
                    p.actions["ShowHidden"]->setText(_getText(DJV_TEXT("file_browser_show_hidden_files")));
                    p.actions["ShowHidden"]->setTooltip(_getText(DJV_TEXT("file_browser_show_hidden_tooltip")));

                    p.actions["Paths"]->setText(_getText(DJV_TEXT("file_browser_paths")));
                    p.actions["Paths"]->setTooltip(_getText(DJV_TEXT("file_browser_paths_tooltip")));
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

                    p.thumbnailPopupButton->setTooltip(_getText(DJV_TEXT("file_browser_thumbnail_size_tooltip")));

                    p.searchBox->setTooltip(_getText(DJV_TEXT("file_browser_search_tooltip")));

                    p.popupMenu->setTooltip(_getText(DJV_TEXT("file_browser_menu_tooltip")));

                    p.itemCountLabel->setText(_getItemCountLabel(p.itemCount));

                    p.acceptButton->setText(_getText(DJV_TEXT("file_browser_accept")));
                    p.cancelButton->setText(_getText(DJV_TEXT("file_browser_cancel")));
                }
            }

            std::string FileBrowser::_getItemCountLabel(size_t size) const
            {
                std::stringstream ss;
                ss << size << " " << _getText(DJV_TEXT("file_browser_items"));
                return ss.str();
            }

            void FileBrowser::_optionsUpdate()
            {
                _p->directoryModel->setOptions(_p->options);
            }

            void FileBrowser::_selectedUpdate()
            {
                DJV_PRIVATE_PTR();
                p.acceptButton->setEnabled(!p.selected.empty());
            }

        } // namespace FileBrowser
    } // namespace UIComponents
} // namespace djv
