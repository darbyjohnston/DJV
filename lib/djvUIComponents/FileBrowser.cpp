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
#include <djvUI/DialogSystem.h>
#include <djvUI/Drawer.h>
#include <djvUI/FlatButton.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/PopupMenu.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SearchBox.h>
#include <djvUI/SettingsSystem.h>
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
                std::shared_ptr<ShortcutsModel> shortcutsModel;
                size_t itemCount = 0;

                std::map<std::string, std::shared_ptr<Action> > actions;
                std::shared_ptr<ActionGroup> viewTypeActionGroup;
                std::shared_ptr<Menu> shortcutsMenu;
                std::shared_ptr<PopupMenu> shortcutsPopupMenu;
                std::shared_ptr<ShortcutsWidget> shortcutsWidget;
                std::shared_ptr<Menu> navigationMenu;
                std::shared_ptr<PopupMenu> navigationPopupMenu;
                std::shared_ptr<Label> itemCountLabel;
                std::shared_ptr<FlatButton> settingsButton;
                std::shared_ptr<PopupWidget> searchPopupWidget;
                std::shared_ptr<ItemView> itemView;
                std::shared_ptr<ScrollWidget> scrollWidget;
                std::shared_ptr<VerticalLayout> layout;

                std::function<void(const FileSystem::FileInfo &)> callback;

                std::shared_ptr<ValueObserver<FileSystem::Path> > pathObserver;
                std::shared_ptr<ListObserver<FileSystem::FileInfo> > fileInfoObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > shortcutsObserver;
                std::vector<std::shared_ptr<ValueObserver<bool> > > shortcutClickedObservers;
                std::shared_ptr<ValueObserver<bool> > hasUpObserver;
                std::shared_ptr<ValueObserver<bool> > upObserver;
                std::shared_ptr<ValueObserver<bool> > currentObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > historyObserver;
                std::shared_ptr<ValueObserver<size_t> > historyIndexObserver;
                std::shared_ptr<ValueObserver<bool> > hasBackObserver;
                std::shared_ptr<ValueObserver<bool> > backObserver;
                std::shared_ptr<ValueObserver<bool> > hasForwardObserver;
                std::shared_ptr<ValueObserver<bool> > forwardObserver;
                std::shared_ptr<ValueObserver<ViewType> > viewTypeObserver;
                std::shared_ptr<ValueObserver<bool> > fileSequencesObserver;
                std::shared_ptr<ValueObserver<bool> > showHiddenObserver;

                std::string getItemCountLabel(size_t, Context *);
            };

            void Widget::_init(Context * context)
            {
                UI::Widget::_init(context);

                setClassName("djv::UI::FileBrowser::Widget");

                DJV_PRIVATE_PTR();
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
                p.actions["EditPath"] = Action::create();
                p.actions["EditPath"]->setIcon("djvIconEdit");

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

                for (auto action : p.actions)
                {
                    addAction(action.second);
                }

                auto pathWidget = PathWidget::create(context);

                p.shortcutsPopupMenu = PopupMenu::create(context);
                p.shortcutsPopupMenu->setIcon("djvIconFavorite");
                p.shortcutsMenu = Menu::create(context);
                p.shortcutsPopupMenu->setMenu(p.shortcutsMenu);
                p.shortcutsWidget = ShortcutsWidget::create(context);

                auto navigationMenu = Menu::create(context);
                navigationMenu->addAction(p.actions["Back"]);
                navigationMenu->addAction(p.actions["Forward"]);
                navigationMenu->addAction(p.actions["Up"]);
                navigationMenu->addAction(p.actions["Current"]);
                navigationMenu->addAction(p.actions["EditPath"]);
                p.navigationPopupMenu = PopupMenu::create(context);
                p.navigationPopupMenu->setIcon("djvIconArrowLeft");
                p.navigationPopupMenu->setMenu(navigationMenu);

                auto vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(MetricsRole::None);
                vLayout->setBackgroundRole(ColorRole::Background);
                vLayout->setPointerEnabled(true);
                vLayout->addWidget(ActionButton::create(p.actions["LargeThumbnails"], context));
                vLayout->addWidget(ActionButton::create(p.actions["SmallThumbnails"], context));
                vLayout->addWidget(ActionButton::create(p.actions["ListView"], context));
                vLayout->addSeparator();
                vLayout->addWidget(ActionButton::create(p.actions["FileSequences"], context));
                vLayout->addWidget(ActionButton::create(p.actions["ShowHidden"], context));
                auto settingsDrawer = Drawer::create(context);
                settingsDrawer->setSide(Side::Right);
                settingsDrawer->addWidget(vLayout);
                p.settingsButton = FlatButton::create(context);
                p.settingsButton->setButtonType(ButtonType::Toggle);
                p.settingsButton->setIcon("djvIconSettings");

                auto topToolBar = Toolbar::create(context);
                topToolBar->addWidget(p.shortcutsPopupMenu);
                topToolBar->addWidget(p.navigationPopupMenu);
                topToolBar->addWidget(pathWidget, RowStretch::Expand);
                topToolBar->addWidget(p.settingsButton);

                auto searchBox = SearchBox::create(context);
                p.itemCountLabel = Label::create(context);
                p.itemCountLabel->setTextHAlign(TextHAlign::Right);
                p.itemCountLabel->setMargin(MetricsRole::MarginSmall);
                vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(MetricsRole::None);
                vLayout->addWidget(searchBox);
                vLayout->addWidget(p.itemCountLabel);
                p.searchPopupWidget = PopupWidget::create(context);
                p.searchPopupWidget->setIcon("djvIconSearch");
                p.searchPopupWidget->setWidget(vLayout);
                p.searchPopupWidget->setCapturePointer(false);

                auto bottomToolBar = Toolbar::create(context);
                bottomToolBar->addExpander();
                bottomToolBar->addWidget(p.searchPopupWidget);

                p.itemView = ItemView::create(context);
                p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                p.scrollWidget->setBorder(false);
                p.scrollWidget->addWidget(p.itemView);

                p.layout = VerticalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(MetricsRole::None);
                vLayout->addWidget(topToolBar);
                vLayout->addSeparator();
                auto stackLayout = StackLayout::create(context);
                stackLayout->addWidget(p.scrollWidget);
                stackLayout->addWidget(settingsDrawer);
                vLayout->addWidget(stackLayout, RowStretch::Expand);
                vLayout->addSeparator();
                vLayout->addWidget(bottomToolBar);
                p.layout->addWidget(vLayout, RowStretch::Expand);
                p.layout->setParent(shared_from_this());

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

                p.shortcutsWidget->setShortcutCallback(
                    [weak](const FileSystem::Path & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->setPath(value);
                    }
                });

                p.viewTypeActionGroup->setRadioCallback(
                    [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        const auto viewType = static_cast<ViewType>(value);
                        widget->setViewType(viewType);
                        if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
                        {
							if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
							{
								fileBrowserSettings->setViewType(viewType);
							}
                        }
                    }
                });

                p.settingsButton->setCheckedCallback(
                    [settingsDrawer](bool value)
                {
                    settingsDrawer->setOpen(value);
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

                p.shortcutsObserver = ListObserver<FileSystem::Path>::create(
                    p.shortcutsModel->observeShortcuts(),
                    [weak, context](const std::vector<FileSystem::Path> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->shortcutsMenu->clearActions();
                        widget->_p->shortcutClickedObservers.clear();
                        for (const auto & i : value)
                        {
                            auto action = Action::create();
                            action->setTitle(i.getFileName());
                            widget->_p->shortcutsMenu->addAction(action);
                            widget->_p->shortcutClickedObservers.push_back(
                                ValueObserver<bool>::create(
                                    action->observeClicked(),
                                    [weak, i](bool value)
                            {
                                if (value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_p->directoryModel->setPath(i);
                                    }
                                }
                            }));
                        }
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

				if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
				{
					if (auto fileBrowserSettings = settingsSystem->getSettingsT<Settings::FileBrowser>())
					{
						p.viewTypeObserver = ValueObserver<ViewType>::create(
							fileBrowserSettings->observeViewType(),
							[weak](ViewType value)
						{
							if (auto widget = weak.lock())
							{
								widget->setViewType(value);
							}
						});
					}
				}

                p.fileSequencesObserver = ValueObserver<bool>::create(
                    p.actions["FileSequences"]->observeChecked(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->setFileSequences(value);
                    }
                });

                p.showHiddenObserver = ValueObserver<bool>::create(
                    p.actions["ShowHidden"]->observeChecked(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->setShowHidden(value);
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
                p.actions["Current"]->setTitle(_getText(DJV_TEXT("Current")));
                p.actions["Current"]->setTooltip(_getText(DJV_TEXT("File browser current directory tooltip")));
                p.actions["EditPath"]->setTitle(_getText(DJV_TEXT("Edit")));
                p.actions["EditPath"]->setTooltip(_getText(DJV_TEXT("File browser edit path tooltip")));
                p.actions["LargeThumbnails"]->setText(_getText(DJV_TEXT("Large thumbnails")));
                p.actions["LargeThumbnails"]->setTooltip(_getText(DJV_TEXT("File browser large thumbnails tooltip")));
                p.actions["SmallThumbnails"]->setText(_getText(DJV_TEXT("Small thumbnails")));
                p.actions["SmallThumbnails"]->setTooltip(_getText(DJV_TEXT("File browser small thumbnails tooltip")));
                p.actions["ListView"]->setText(_getText(DJV_TEXT("List view")));
                p.actions["ListView"]->setTooltip(_getText(DJV_TEXT("File browser list view tooltip")));
                p.actions["FileSequences"]->setText(_getText(DJV_TEXT("Enable file sequences")));
                p.actions["FileSequences"]->setTooltip(_getText(DJV_TEXT("File browser file sequences tooltip")));
                p.actions["ShowHidden"]->setText(_getText(DJV_TEXT("Show hidden files")));
                p.actions["ShowHidden"]->setTooltip(_getText(DJV_TEXT("File browser show hidden tooltip")));

                p.shortcutsPopupMenu->setTooltip(_getText(DJV_TEXT("File browser shortcuts popup tooltip")));
                p.navigationPopupMenu->setTooltip(_getText(DJV_TEXT("File browser navigation popup tooltip")));
                p.settingsButton->setTooltip(_getText(DJV_TEXT("File browser settings popup tooltip")));
                p.searchPopupWidget->setTooltip(_getText(DJV_TEXT("File browser search popup tooltip")));

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
