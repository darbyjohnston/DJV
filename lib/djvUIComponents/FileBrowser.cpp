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

#include <djvUIComponents/FileBrowser.h>

#include <djvUIComponents/FileBrowserItemView.h>
#include <djvUIComponents/FileBrowserPrivate.h>
#include <djvUIComponents/FileBrowserSettings.h>
#include <djvUIComponents/ListViewHeader.h>
#include <djvUIComponents/SearchBox.h>

#include <djvUI/Action.h>
#include <djvUI/ActionButton.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Bellows.h>
#include <djvUI/Border.h>
#include <djvUI/DialogSystem.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/Icon.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/Splitter.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvAV/IO.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>
#include <djvCore/DirectoryModel.h>
#include <djvCore/DrivesModel.h>
#include <djvCore/FileInfo.h>
#include <djvCore/RecentFilesModel.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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
                size_t itemCount = 0;
                std::shared_ptr<ShortcutsModel> shortcutsModel;
                std::shared_ptr<FileSystem::RecentFilesModel> recentPathsModel;
                std::shared_ptr<FileSystem::DrivesModel> drivesModel;

                std::map<std::string, std::shared_ptr<Action> > actions;
                std::shared_ptr<ActionGroup> viewTypeActionGroup;
                std::shared_ptr<ActionGroup> sortActionGroup;
                std::shared_ptr<PopupWidget> shortcutsPopupWidget;
                std::shared_ptr<PopupWidget> recentPathsPopupWidget;
                std::shared_ptr<PopupWidget> drivesPopupWidget;
                std::shared_ptr<Label> sortTitleLabel;
                std::shared_ptr<PopupWidget> sortPopupWidget;
                std::shared_ptr<Label> viewTypeLabel;
                std::shared_ptr<Label> thumbnailSizeLabel;
                std::shared_ptr<IntSlider> thumbnailSizeSlider;
                std::shared_ptr<Label> miscSettingsLabel;
                std::shared_ptr<PopupWidget> settingsPopupWidget;
                std::shared_ptr<ListViewHeader> listViewHeader;
                std::shared_ptr<VerticalLayout> listViewLayout;
                std::shared_ptr<ItemView> itemView;
                std::shared_ptr<ScrollWidget> scrollWidget;
                std::shared_ptr<Label> itemCountLabel;
                std::shared_ptr<SearchBox> searchBox;
                std::shared_ptr<VerticalLayout> layout;

                std::function<void(const FileSystem::FileInfo &)> callback;

                std::shared_ptr<ValueObserver<FileSystem::Path> > pathObserver;
                std::shared_ptr<ListObserver<FileSystem::FileInfo> > fileInfoObserver;
                std::shared_ptr<ValueObserver<bool> > hasUpObserver;
                std::shared_ptr<ValueObserver<bool> > upObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > historyObserver;
                std::shared_ptr<ValueObserver<size_t> > historyIndexObserver;
                std::shared_ptr<ValueObserver<bool> > hasBackObserver;
                std::shared_ptr<ValueObserver<bool> > backObserver;
                std::shared_ptr<ValueObserver<bool> > hasForwardObserver;
                std::shared_ptr<ValueObserver<bool> > forwardObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > shortcutsObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > shortcutsSettingsObserver;
                std::shared_ptr<ListObserver<FileSystem::FileInfo> > recentPathsObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > recentPathsSettingsObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > drivesObserver;
                std::shared_ptr<ValueObserver<ViewType> > viewTypeSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > increaseThumbnailSizeObserver;
                std::shared_ptr<ValueObserver<bool> > decreaseThumbnailSizeObserver;
                std::shared_ptr<ValueObserver<AV::Image::Size> > thumbnailSizeSettingsObserver;
                std::shared_ptr<ListObserver<float> > listViewHeaderSplitSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > fileSequencesObserver;
                std::shared_ptr<ValueObserver<bool> > fileSequencesSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > showHiddenObserver;
                std::shared_ptr<ValueObserver<bool> > showHiddenSettingsObserver;
                std::shared_ptr<ValueObserver<FileSystem::DirectoryListSort> > sortSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > reverseSortObserver;
                std::shared_ptr<ValueObserver<bool> > reverseSortSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > sortDirectoriesFirstObserver;
                std::shared_ptr<ValueObserver<bool> > sortDirectoriesFirstSettingsObserver;
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
                p.recentPathsModel = FileSystem::RecentFilesModel::create(context);
                p.recentPathsModel->setFilesMax(10);
                p.drivesModel = FileSystem::DrivesModel::create(context);

                p.actions["Back"] = Action::create();
                p.actions["Back"]->setIcon("djvIconArrowLeft");
                p.actions["Back"]->setShortcut(GLFW_KEY_LEFT);
                p.actions["Forward"] = Action::create();
                p.actions["Forward"]->setIcon("djvIconArrowRight");
                p.actions["Forward"]->setShortcut(GLFW_KEY_RIGHT);
                p.actions["Up"] = Action::create();
                p.actions["Up"]->setIcon("djvIconArrowUp");
                p.actions["Up"]->setShortcut(GLFW_KEY_UP);

                p.actions["Tiles"] = Action::create();
                p.actions["Tiles"]->setIcon("djvIconTileView");
                p.actions["Tiles"]->setShortcut(GLFW_KEY_T);
                p.actions["List"] = Action::create();
                p.actions["List"]->setIcon("djvIconListView");
                p.actions["List"]->setShortcut(GLFW_KEY_L);
                p.viewTypeActionGroup = ActionGroup::create(ButtonType::Radio);
                p.viewTypeActionGroup->addAction(p.actions["Tiles"]);
                p.viewTypeActionGroup->addAction(p.actions["List"]);
                p.actions["IncreaseThumbnailSize"] = Action::create();
                p.actions["IncreaseThumbnailSize"]->setIcon("djvIconAdd");
                p.actions["IncreaseThumbnailSize"]->setShortcut(GLFW_KEY_EQUAL);
                p.actions["DecreaseThumbnailSize"] = Action::create();
                p.actions["DecreaseThumbnailSize"]->setIcon("djvIconSubtract");
                p.actions["DecreaseThumbnailSize"]->setShortcut(GLFW_KEY_MINUS);

                p.actions["FileSequences"] = Action::create();
                p.actions["FileSequences"]->setButtonType(ButtonType::Toggle);
                p.actions["FileSequences"]->setIcon("djvIconFileSequence");
                p.actions["FileSequences"]->setShortcut(GLFW_KEY_S);
                p.actions["ShowHidden"] = Action::create();
                p.actions["ShowHidden"]->setButtonType(ButtonType::Toggle);
                p.actions["ShowHidden"]->setShortcut(GLFW_KEY_N);

                p.actions["SortByName"] = Action::create();
                p.actions["SortByName"]->setShortcut(GLFW_KEY_3);
                p.actions["SortBySize"] = Action::create();
                p.actions["SortBySize"]->setShortcut(GLFW_KEY_4);
                p.actions["SortByTime"] = Action::create();
                p.actions["SortByTime"]->setShortcut(GLFW_KEY_5);
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

                auto pathWidget = PathWidget::create(context);

                auto shortcutsWidget = ShortcutsWidget::create(p.shortcutsModel, context);
                p.shortcutsPopupWidget = PopupWidget::create(context);
                p.shortcutsPopupWidget->setIcon("djvIconBookmark");
                p.shortcutsPopupWidget->addChild(shortcutsWidget);

                auto recentPathsWidget = RecentPathsWidget::create(p.recentPathsModel, context);
                p.recentPathsPopupWidget = PopupWidget::create(context);
                p.recentPathsPopupWidget->setIcon("djvIconFileRecent");
                p.recentPathsPopupWidget->addChild(recentPathsWidget);

                auto drivesWidget = DrivesWidget::create(p.drivesModel, context);
                p.drivesPopupWidget = PopupWidget::create(context);
                p.drivesPopupWidget->setIcon("djvIconDrives");
                p.drivesPopupWidget->addChild(drivesWidget);

                p.sortTitleLabel = Label::create(context);
                p.sortTitleLabel->setTextHAlign(UI::TextHAlign::Left);
                p.sortTitleLabel->setMargin(Layout::Margin(MetricsRole::MarginSmall));
                p.sortTitleLabel->setBackgroundRole(UI::ColorRole::Trough);
                auto sortByNameButton = ActionButton::create(context);
                sortByNameButton->addAction(p.actions["SortByName"]);
                auto sortBySizeButton = ActionButton::create(context);
                sortBySizeButton->addAction(p.actions["SortBySize"]);
                auto sortByTimeButton = ActionButton::create(context);
                sortByTimeButton->addAction(p.actions["SortByTime"]);
                auto reverseSortButton = ActionButton::create(context);
                reverseSortButton->addAction(p.actions["ReverseSort"]);
                auto sortDirectoriesFirstButton = ActionButton::create(context);
                sortDirectoriesFirstButton->addAction(p.actions["SortDirectoriesFirst"]);

                p.viewTypeLabel = Label::create(context);
                p.viewTypeLabel->setTextHAlign(TextHAlign::Left);
                p.viewTypeLabel->setMargin(Layout::Margin(MetricsRole::MarginSmall));
                p.viewTypeLabel->setBackgroundRole(UI::ColorRole::Trough);
                auto tilesButton = ActionButton::create(context);
                tilesButton->addAction(p.actions["Tiles"]);
                auto listButton = ActionButton::create(context);
                listButton->addAction(p.actions["List"]);

                p.thumbnailSizeLabel = Label::create(context);
                p.thumbnailSizeLabel->setTextHAlign(TextHAlign::Left);
                p.thumbnailSizeLabel->setMargin(Layout::Margin(MetricsRole::MarginSmall));
                p.thumbnailSizeLabel->setBackgroundRole(UI::ColorRole::Trough);
                auto increaseThumbnailSizeButton = ActionButton::create(context);
                increaseThumbnailSizeButton->addAction(p.actions["IncreaseThumbnailSize"]);
                auto decreaseThumbnailSizeButton = ActionButton::create(context);
                decreaseThumbnailSizeButton->addAction(p.actions["DecreaseThumbnailSize"]);

                p.miscSettingsLabel = Label::create(context);
                p.miscSettingsLabel->setTextHAlign(TextHAlign::Left);
                p.miscSettingsLabel->setMargin(Layout::Margin(MetricsRole::MarginSmall));
                p.miscSettingsLabel->setBackgroundRole(UI::ColorRole::Trough);
                auto fileSequencesButton = ActionButton::create(context);
                fileSequencesButton->addAction(p.actions["FileSequences"]);
                auto showHiddenButton = ActionButton::create(context);
                showHiddenButton->addAction(p.actions["ShowHidden"]);

                auto vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(Layout::Spacing(MetricsRole::None));
                vLayout->addChild(p.sortTitleLabel);
                vLayout->addSeparator();
                auto vLayout2 = VerticalLayout::create(context);
                vLayout2->setSpacing(Layout::Spacing(MetricsRole::None));
                vLayout2->addChild(sortByNameButton);
                vLayout2->addChild(sortBySizeButton);
                vLayout2->addChild(sortByTimeButton);
                vLayout2->addSeparator();
                vLayout2->addChild(reverseSortButton);
                vLayout2->addChild(sortDirectoriesFirstButton);
                vLayout->addChild(vLayout2);
                p.sortPopupWidget = PopupWidget::create(context);
                p.sortPopupWidget->setIcon("djvIconSort");
                p.sortPopupWidget->addChild(vLayout);

                p.searchBox = SearchBox::create(context);

                p.thumbnailSizeSlider = IntSlider::create(context);
                p.thumbnailSizeSlider->setRange(thumbnailSizeRange);
                p.thumbnailSizeSlider->setDelay(Time::getTime(Time::TimerValue::Medium));
                p.thumbnailSizeSlider->setMargin(Layout::Margin(MetricsRole::MarginSmall));
                vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(Layout::Spacing(MetricsRole::None));
                vLayout->addChild(p.viewTypeLabel);
                vLayout->addSeparator();
                vLayout2 = VerticalLayout::create(context);
                vLayout2->setSpacing(Layout::Spacing(MetricsRole::None));
                vLayout2->addChild(tilesButton);
                vLayout2->addChild(listButton);
                vLayout->addChild(vLayout2);
                vLayout->addSeparator();
                vLayout->addChild(p.thumbnailSizeLabel);
                vLayout->addSeparator();
                vLayout2 = VerticalLayout::create(context);
                vLayout2->setSpacing(Layout::Spacing(MetricsRole::None));
                vLayout2->addChild(increaseThumbnailSizeButton);
                vLayout2->addChild(decreaseThumbnailSizeButton);
                vLayout2->addChild(p.thumbnailSizeSlider);
                vLayout->addChild(vLayout2);
                vLayout->addSeparator();
                vLayout->addChild(p.miscSettingsLabel);
                vLayout->addSeparator();
                vLayout2 = VerticalLayout::create(context);
                vLayout2->setSpacing(Layout::Spacing(MetricsRole::None));
                vLayout2->addChild(fileSequencesButton);
                vLayout2->addChild(showHiddenButton);
                vLayout->addChild(vLayout2);
                auto scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                scrollWidget->setBorder(false);
                scrollWidget->setMinimumSizeRole(MetricsRole::Menu);
                scrollWidget->addChild(vLayout);
                p.settingsPopupWidget = PopupWidget::create(context);
                p.settingsPopupWidget->setIcon("djvIconSettings");
                p.settingsPopupWidget->addChild(scrollWidget);

                auto toolBar = ToolBar::create(context);
                toolBar->addChild(p.shortcutsPopupWidget);
                toolBar->addChild(p.recentPathsPopupWidget);
                toolBar->addChild(p.drivesPopupWidget);
                toolBar->addAction(p.actions["Back"]);
                toolBar->addAction(p.actions["Forward"]);
                toolBar->addAction(p.actions["Up"]);
                toolBar->addChild(pathWidget);
                toolBar->setStretch(pathWidget, RowStretch::Expand);
                toolBar->addChild(p.sortPopupWidget);
                toolBar->addChild(p.searchBox);
                toolBar->addChild(p.settingsPopupWidget);

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
                p.itemCountLabel->setMargin(Layout::Margin(MetricsRole::Margin));

                p.layout = VerticalLayout::create(context);
                p.layout->setSpacing(Layout::Spacing(MetricsRole::None));
                p.layout->addChild(toolBar);
                p.layout->addSeparator();
                p.listViewLayout = VerticalLayout::create(context);
                p.listViewLayout->setSpacing(Layout::Spacing(MetricsRole::None));
                p.listViewLayout->addChild(p.listViewHeader);
                p.layout->addChild(p.listViewLayout);
                auto stackLayout = StackLayout::create(context);
                stackLayout->addChild(p.scrollWidget);
                stackLayout->addChild(p.itemCountLabel);
                p.layout->addChild(stackLayout);
                p.layout->setStretch(stackLayout, RowStretch::Expand);
                addChild(p.layout);

                auto weak = std::weak_ptr<FileBrowser>(std::dynamic_pointer_cast<FileBrowser>(shared_from_this()));
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
                            std::stringstream ss;
                            ss << widget->_getText(DJV_TEXT("error_file_browser_cannot_set_the_path")) << " '" << value << "'. " << e.what();
                            widget->_log(ss.str(), LogLevel::Error);
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

                shortcutsWidget->setCallback(
                    [weak](const FileSystem::Path & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->setPath(value);
                        widget->_p->shortcutsPopupWidget->close();
                    }
                });

                recentPathsWidget->setCallback(
                    [weak](const FileSystem::Path& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->directoryModel->setPath(value);
                            widget->_p->recentPathsPopupWidget->close();
                        }
                    });

                drivesWidget->setCallback(
                    [weak](const FileSystem::Path& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->directoryModel->setPath(value);
                            widget->_p->drivesPopupWidget->close();
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
                    [weak, shortcutsWidget, pathWidget](const FileSystem::Path & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->scrollWidget->setScrollPos(glm::vec2(0.F, 0.F));
                    }
                    shortcutsWidget->setPath(value);
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

                auto settingsSystem = context->getSystemT<Settings::System>();
                if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                {
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
                            widget->_p->listViewLayout->setVisible(ViewType::List == value);
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
                            widget->_p->thumbnailSizeSlider->setValue(value.w);
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
                }

                auto contextWeak = std::weak_ptr<Context>(context);
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
                                widget->_p->recentPathsPopupWidget->setEnabled(value.size());
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
                            widget->_p->drivesPopupWidget->setEnabled(value.size());
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

                p.thumbnailSizeSlider->setValueCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                fileBrowserSettings->setThumbnailSize(AV::Image::Size(value, ceilf(value / 2.F)));
                            }
                        }
                    });

                p.increaseThumbnailSizeObserver = ValueObserver<bool>::create(
                    p.actions["IncreaseThumbnailSize"]->observeClicked(),
                    [contextWeak](bool value)
                {
                    if (value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                auto size = fileBrowserSettings->observeThumbnailSize()->get();
                                size.w = Math::clamp(static_cast<int>(size.w * 1.25F), thumbnailSizeRange.min, thumbnailSizeRange.max);
                                size.h = static_cast<int>(ceilf(size.w / 2.F));
                                fileBrowserSettings->setThumbnailSize(size);
                            }
                        }
                    }
                });

                p.decreaseThumbnailSizeObserver = ValueObserver<bool>::create(
                    p.actions["DecreaseThumbnailSize"]->observeClicked(),
                    [contextWeak](bool value)
                {
                    if (value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                auto size = fileBrowserSettings->observeThumbnailSize()->get();
                                size.w = Math::clamp(static_cast<int>(size.w * .75F), thumbnailSizeRange.min, thumbnailSizeRange.max);
                                size.h = static_cast<int>(ceilf(size.w / 2.F));
                                fileBrowserSettings->setThumbnailSize(size);
                            }
                        }
                    }
                });

                p.fileSequencesObserver = ValueObserver<bool>::create(
                    p.actions["FileSequences"]->observeChecked(),
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

                p.showHiddenObserver = ValueObserver<bool>::create(
                    p.actions["ShowHidden"]->observeChecked(),
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

                p.reverseSortObserver = ValueObserver<bool>::create(
                    p.actions["ReverseSort"]->observeChecked(),
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

                p.sortDirectoriesFirstObserver = ValueObserver<bool>::create(
                    p.actions["SortDirectoriesFirst"]->observeChecked(),
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
            
            void FileBrowser::_preLayoutEvent(Event::PreLayout & event)
            {
                const auto& style = _getStyle();
                _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
            }

            void FileBrowser::_layoutEvent(Event::Layout & event)
            {
                const auto& style = _getStyle();
                _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
            }

            void FileBrowser::_initEvent(Event::Init & event)
            {
                Widget::_initEvent(event);
                DJV_PRIVATE_PTR();
                
                p.actions["Back"]->setText(_getText(DJV_TEXT("file_browser_back")));
                p.actions["Back"]->setTooltip(_getText(DJV_TEXT("file_browser_back_tooltip")));
                p.actions["Forward"]->setText(_getText(DJV_TEXT("file_browser_playback_forward")));
                p.actions["Forward"]->setTooltip(_getText(DJV_TEXT("file_browser_forward_tooltip")));
                p.actions["Up"]->setText(_getText(DJV_TEXT("file_browser_up")));
                p.actions["Up"]->setTooltip(_getText(DJV_TEXT("file_browser_up_tooltip")));

                p.actions["Tiles"]->setText(_getText(DJV_TEXT("file_browser_settings_tile")));
                p.actions["Tiles"]->setTooltip(_getText(DJV_TEXT("file_browser_settings_tile_view_tooltip")));
                p.actions["List"]->setText(_getText(DJV_TEXT("file_browser_settings_list")));
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

                p.sortTitleLabel->setText(_getText(DJV_TEXT("file_browser_sorting")));
                p.viewTypeLabel->setText(_getText(DJV_TEXT("file_browser_view")));
                p.thumbnailSizeLabel->setText(_getText(DJV_TEXT("file_browser_thumbnail_size")));
                p.miscSettingsLabel->setText(_getText(DJV_TEXT("file_browser_miscellaneous")));

                p.listViewHeader->setText(
                    {
                        _getText(DJV_TEXT("file_browser_column_name")),
                        _getText(DJV_TEXT("file_browser_column_size")),
                        _getText(DJV_TEXT("file_browser_column_time"))
                    }
                );

                p.itemCountLabel->setText(_getItemCountLabel(p.itemCount));

                p.shortcutsPopupWidget->setTooltip(_getText(DJV_TEXT("file_browser_shortcuts_tooltip")));
                p.recentPathsPopupWidget->setTooltip(_getText(DJV_TEXT("file_browser_recent_paths_tooltip")));
                p.drivesPopupWidget->setTooltip(_getText(DJV_TEXT("file_browser_drives_tooltip")));
                p.sortPopupWidget->setTooltip(_getText(DJV_TEXT("file_browser_sort_tooltip")));
                p.settingsPopupWidget->setTooltip(_getText(DJV_TEXT("file_browser_settings_tooltip")));

                p.searchBox->setTooltip(_getText(DJV_TEXT("file_browser_search_tooltip")));
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
