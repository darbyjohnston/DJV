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
#include <djvUI/ButtonGroup.h>
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
            struct FileBrowser::Private
            {
                std::shared_ptr<FileSystem::DirectoryModel> directoryModel;
                size_t itemCount = 0;
                std::shared_ptr<ShortcutsModel> shortcutsModel;

                std::map<std::string, std::shared_ptr<Action> > actions;
                std::shared_ptr<ActionGroup> viewTypeActionGroup;
                std::shared_ptr<ActionGroup> sortActionGroup;
                std::shared_ptr<ShortcutsWidget> shortcutsWidget;
                std::shared_ptr<DrivesWidget> drivesWidget;
                std::shared_ptr<PopupWidget> shortcutsPopupWidget;
                std::shared_ptr<ButtonGroup> modelButtonGroup;
                std::shared_ptr<PopupWidget> sortPopupWidget;
                std::shared_ptr<IntSlider> thumbnailSizeSlider;
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
                std::shared_ptr<ValueObserver<bool> > currentObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > historyObserver;
                std::shared_ptr<ValueObserver<size_t> > historyIndexObserver;
                std::shared_ptr<ValueObserver<bool> > hasBackObserver;
                std::shared_ptr<ValueObserver<bool> > backObserver;
                std::shared_ptr<ValueObserver<bool> > hasForwardObserver;
                std::shared_ptr<ValueObserver<bool> > forwardObserver;
                std::shared_ptr<ValueObserver<bool> > addShortcutObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > shortcutsObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > shortcutsSettingsObserver;
                std::shared_ptr<MapObserver<std::string, bool> > shortcutsBellowsSettingsObserver;
                std::shared_ptr<ListObserver<float> > shortcutsSplitSettingsObserver;
                std::shared_ptr<ValueObserver<ViewType> > viewTypeSettingsObserver;
                std::shared_ptr<ValueObserver<bool> > increaseThumbnailSizeObserver;
                std::shared_ptr<ValueObserver<bool> > decreaseThumbnailSizeObserver;
                std::shared_ptr<ValueObserver<glm::ivec2> > thumbnailSizeSettingsObserver;
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

            void FileBrowser::_init(Context * context)
            {
                UI::Widget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::FileBrowser::FileBrowser");

                p.directoryModel = FileSystem::DirectoryModel::create(context);
                p.shortcutsModel = ShortcutsModel::create(context);

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

                p.actions["AddShortcut"] = Action::create();
                p.actions["AddShortcut"]->setIcon("djvIconBookmark");
                p.actions["AddShortcut"]->setShortcut(GLFW_KEY_S, Shortcut::getSystemModifier());

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
                p.actions["IncreaseThumbnailSize"]->setShortcut(GLFW_KEY_EQUAL);
                p.actions["DecreaseThumbnailSize"] = Action::create();
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

                p.shortcutsWidget = ShortcutsWidget::create(p.shortcutsModel, context);
                p.drivesWidget = DrivesWidget::create(context);
                auto vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(MetricsRole::None);
                vLayout->setBackgroundRole(ColorRole::BackgroundToolBar);
                vLayout->addChild(p.shortcutsWidget);
                vLayout->addSeparator();
                vLayout->addChild(UI::ActionButton::create(p.actions["AddShortcut"], context));
                vLayout->addSeparator();
                vLayout->addChild(p.drivesWidget);
                auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                scrollWidget->setBorder(false);
                scrollWidget->addChild(vLayout);
                p.shortcutsPopupWidget = PopupWidget::create(context);
                p.shortcutsPopupWidget->setIcon("djvIconBookmark");
                p.shortcutsPopupWidget->addChild(scrollWidget);

                vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(MetricsRole::None);
                vLayout->setBackgroundRole(ColorRole::BackgroundToolBar);
                vLayout->addChild(ActionButton::create(p.actions["SortByName"], context));
                vLayout->addChild(ActionButton::create(p.actions["SortBySize"], context));
                vLayout->addChild(ActionButton::create(p.actions["SortByTime"], context));
                vLayout->addSeparator();
                vLayout->addChild(ActionButton::create(p.actions["ReverseSort"], context));
                vLayout->addChild(ActionButton::create(p.actions["SortDirectoriesFirst"], context));
                p.sortPopupWidget = PopupWidget::create(context);
                p.sortPopupWidget->setIcon("djvIconSort");
                p.sortPopupWidget->addChild(vLayout);

                p.searchBox = SearchBox::create(context);

                p.thumbnailSizeSlider = IntSlider::create(context);
                p.thumbnailSizeSlider->setRange(thumbnailSizeRange);
                p.thumbnailSizeSlider->setDelay(Time::getMilliseconds(Time::TimerValue::Medium));
                p.thumbnailSizeSlider->setMargin(MetricsRole::MarginSmall);
                vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(MetricsRole::None);
                vLayout->setBackgroundRole(ColorRole::BackgroundToolBar);
                vLayout->addChild(ActionButton::create(p.actions["Tiles"], context));
                vLayout->addChild(ActionButton::create(p.actions["List"], context));
                vLayout->addSeparator();
                vLayout->addChild(ActionButton::create(p.actions["IncreaseThumbnailSize"], context));
                vLayout->addChild(ActionButton::create(p.actions["DecreaseThumbnailSize"], context));
                vLayout->addChild(p.thumbnailSizeSlider);
                vLayout->addSeparator();
                vLayout->addChild(ActionButton::create(p.actions["FileSequences"], context));
                vLayout->addChild(ActionButton::create(p.actions["ShowHidden"], context));
                p.settingsPopupWidget = PopupWidget::create(context);
                p.settingsPopupWidget->setIcon("djvIconSettings");
                p.settingsPopupWidget->addChild(vLayout);

                auto toolBar = ToolBar::create(context);
                toolBar->setBackgroundRole(ColorRole::BackgroundToolBar);
                toolBar->setShadowOverlay({ Side::Top });
                toolBar->addChild(p.shortcutsPopupWidget);
                toolBar->addAction(p.actions["Up"]);
                toolBar->addAction(p.actions["Back"]);
                toolBar->addAction(p.actions["Forward"]);
                toolBar->addChild(pathWidget);
                toolBar->setStretch(pathWidget, RowStretch::Expand);
                toolBar->addChild(p.sortPopupWidget);
                toolBar->addChild(p.searchBox);
                toolBar->addChild(p.settingsPopupWidget);

                p.listViewHeader = ListViewHeader::create(context);
                p.listViewHeader->setText({ std::string(), std::string(), std::string() });
                p.listViewHeader->setShadowOverlay({ Side::Top });
                p.itemView = ItemView::create(context);
                p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                p.scrollWidget->setBorder(false);
                p.scrollWidget->addChild(p.itemView);
                p.scrollWidget->setShadowOverlay({ Side::Top });

                p.itemCountLabel = Label::create(context);
                p.itemCountLabel->setTextHAlign(TextHAlign::Right);
                p.itemCountLabel->setTextVAlign(TextVAlign::Bottom);
                p.itemCountLabel->setMargin(MetricsRole::Margin);

                p.layout = VerticalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                p.layout->addChild(toolBar);
                p.listViewLayout = VerticalLayout::create(context);
                p.listViewLayout->setSpacing(MetricsRole::None);
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
                        widget->_p->shortcutsPopupWidget->close();
                    }
                });

                p.drivesWidget->setCallback(
                    [weak](const FileSystem::Path & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->setPath(value);
                        widget->_p->shortcutsPopupWidget->close();
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

                p.addShortcutObserver = ValueObserver<bool>::create(
                    p.actions["AddShortcut"]->observeClicked(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->shortcutsModel->addShortcut(widget->_p->directoryModel->observePath()->get());
                            }
                        }
                    }
                });

                if (auto settingsSystem = context->getSystemT<Settings::System>())
                {
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

                        p.thumbnailSizeSettingsObserver = ValueObserver<glm::ivec2>::create(
                            fileBrowserSettings->observeThumbnailSize(),
                            [weak](const glm::ivec2 & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->itemView->setThumbnailSize(value);
                                widget->_p->thumbnailSizeSlider->setValue(value.x);
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
                }

                p.viewTypeActionGroup->setRadioCallback(
                    [context](int value)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>())
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
                    if (auto settingsSystem = context->getSystemT<Settings::System>())
                    {
                        if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                        {
                            fileBrowserSettings->setSort(static_cast<FileSystem::DirectoryListSort>(value));
                        }
                    }
                });

                p.shortcutsObserver = ListObserver<FileSystem::Path>::create(
                    p.shortcutsModel->observeShortcuts(),
                    [weak, context](const std::vector<FileSystem::Path> & value)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>())
                    {
                        if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                        {
                            fileBrowserSettings->setShortcuts(value);
                        }
                    }
                });

                p.listViewHeader->setSortCallback(
                    [context](size_t sort, bool reverse)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>())
                    {
                        if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                        {
                            fileBrowserSettings->setSort(static_cast<FileSystem::DirectoryListSort>(sort));
                            fileBrowserSettings->setReverseSort(reverse);
                        }
                    }
                });

                p.listViewHeader->setSplitCallback(
                    [context](const std::vector<float> & value)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>())
                    {
                        if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                        {
                            fileBrowserSettings->setListViewHeaderSplit(value);
                        }
                    }
                });

                p.thumbnailSizeSlider->setValueCallback(
                    [context](int value)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>())
                    {
                        if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                        {
                            fileBrowserSettings->setThumbnailSize(glm::ivec2(value, ceilf(value / 2.f)));
                        }
                    }
                });

                p.increaseThumbnailSizeObserver = ValueObserver<bool>::create(
                    p.actions["IncreaseThumbnailSize"]->observeClicked(),
                    [context](bool value)
                {
                    if (value)
                    {
                        if (auto settingsSystem = context->getSystemT<Settings::System>())
                        {
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                auto size = fileBrowserSettings->observeThumbnailSize()->get();
                                size.x = Math::clamp(static_cast<int>(size.x * 1.25f), thumbnailSizeRange.min, thumbnailSizeRange.max);
                                size.y = static_cast<int>(ceilf(size.x / 2.f));
                                fileBrowserSettings->setThumbnailSize(size);
                            }
                        }
                    }
                });

                p.decreaseThumbnailSizeObserver = ValueObserver<bool>::create(
                    p.actions["DecreaseThumbnailSize"]->observeClicked(),
                    [context](bool value)
                {
                    if (value)
                    {
                        if (auto settingsSystem = context->getSystemT<Settings::System>())
                        {
                            if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
                            {
                                auto size = fileBrowserSettings->observeThumbnailSize()->get();
                                size.x = Math::clamp(static_cast<int>(size.x * .75f), thumbnailSizeRange.min, thumbnailSizeRange.max);
                                size.y = static_cast<int>(ceilf(size.x / 2.f));
                                fileBrowserSettings->setThumbnailSize(size);
                            }
                        }
                    }
                });

                p.fileSequencesObserver = ValueObserver<bool>::create(
                    p.actions["FileSequences"]->observeChecked(),
                    [context](bool value)
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>())
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
                    if (auto settingsSystem = context->getSystemT<Settings::System>())
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
                    if (auto settingsSystem = context->getSystemT<Settings::System>())
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
                    if (auto settingsSystem = context->getSystemT<Settings::System>())
                    {
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

            std::shared_ptr<FileBrowser> FileBrowser::create(Context * context)
            {
                auto out = std::shared_ptr<FileBrowser>(new FileBrowser);
                out->_init(context);
                return out;
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
                auto style = _getStyle();
                _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
            }

            void FileBrowser::_layoutEvent(Event::Layout & event)
            {
                auto style = _getStyle();
                _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
            }

            void FileBrowser::_localeEvent(Event::Locale &)
            {
                DJV_PRIVATE_PTR();
                p.actions["Back"]->setText(_getText(DJV_TEXT("Back")));
                p.actions["Back"]->setTooltip(_getText(DJV_TEXT("File browser back tooltip")));
                p.actions["Forward"]->setText(_getText(DJV_TEXT("Forward")));
                p.actions["Forward"]->setTooltip(_getText(DJV_TEXT("File browser forward tooltip")));
                p.actions["Up"]->setText(_getText(DJV_TEXT("Up")));
                p.actions["Up"]->setTooltip(_getText(DJV_TEXT("File browser up tooltip")));
                p.actions["Current"]->setText(_getText(DJV_TEXT("Current Directory")));
                p.actions["Current"]->setTooltip(_getText(DJV_TEXT("File browser current directory tooltip")));

                p.actions["AddShortcut"]->setText(_getText(DJV_TEXT("Add To Shortcuts")));
                p.actions["AddShortcut"]->setTooltip(_getText(DJV_TEXT("File browser add to shortcuts tooltip")));

                p.actions["Tiles"]->setText(_getText(DJV_TEXT("Tile View")));
                p.actions["Tiles"]->setTooltip(_getText(DJV_TEXT("File browser tile view tooltip")));
                p.actions["List"]->setText(_getText(DJV_TEXT("List View")));
                p.actions["List"]->setTooltip(_getText(DJV_TEXT("File browser list view tooltip")));
                p.actions["IncreaseThumbnailSize"]->setText(_getText(DJV_TEXT("Increase Thumbnail Size")));
                p.actions["IncreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("File browser increase thumbnail size tooltip")));
                p.actions["DecreaseThumbnailSize"]->setText(_getText(DJV_TEXT("Decrease Thumbnail Size")));
                p.actions["DecreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("File browser decrease thumbnail size tooltip")));

                p.actions["FileSequences"]->setText(_getText(DJV_TEXT("Enable File Sequences")));
                p.actions["FileSequences"]->setTooltip(_getText(DJV_TEXT("File browser file sequences tooltip")));
                p.actions["ShowHidden"]->setText(_getText(DJV_TEXT("Show Hidden Files")));
                p.actions["ShowHidden"]->setTooltip(_getText(DJV_TEXT("File browser show hidden tooltip")));

                p.actions["SortByName"]->setText(_getText(DJV_TEXT("Sort By Name")));
                p.actions["SortByName"]->setTooltip(_getText(DJV_TEXT("File browser sort by name tooltip")));
                p.actions["SortBySize"]->setText(_getText(DJV_TEXT("Sort By Size")));
                p.actions["SortBySize"]->setTooltip(_getText(DJV_TEXT("File browser sort by size tooltip")));
                p.actions["SortByTime"]->setText(_getText(DJV_TEXT("Sort By Time")));
                p.actions["SortByTime"]->setTooltip(_getText(DJV_TEXT("File browser sort by time tooltip")));
                p.actions["ReverseSort"]->setText(_getText(DJV_TEXT("Reverse Sort")));
                p.actions["ReverseSort"]->setTooltip(_getText(DJV_TEXT("File browser reverse sort tooltip")));
                p.actions["SortDirectoriesFirst"]->setText(_getText(DJV_TEXT("Sort Directories First")));
                p.actions["SortDirectoriesFirst"]->setTooltip(_getText(DJV_TEXT("File browser sort directories first tooltip")));

                p.listViewHeader->setText(
                    {
                        _getText(DJV_TEXT("File browser name column")),
                        _getText(DJV_TEXT("File browse size column")),
                        _getText(DJV_TEXT("File browser time column"))
                    }
                );

                p.itemCountLabel->setText(_getItemCountLabel(p.itemCount));

                p.shortcutsPopupWidget->setTooltip(_getText(DJV_TEXT("Shortcuts tooltip")));
                p.sortPopupWidget->setTooltip(_getText(DJV_TEXT("Sort tooltip")));
                p.settingsPopupWidget->setTooltip(_getText(DJV_TEXT("File browser settings tooltip")));

                p.searchBox->setTooltip(_getText(DJV_TEXT("File browser search tooltip")));
            }

            std::string FileBrowser::_getItemCountLabel(size_t size) const
            {
                std::stringstream ss;
                ss << size << " " << _getText(DJV_TEXT("Items"));
                return ss.str();
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
