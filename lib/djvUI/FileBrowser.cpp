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

#include <djvUI/FileBrowser.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/DialogSystem.h>
#include <djvUI/FileBrowserItemView.h>
#include <djvUI/FileBrowserPrivate.h>
#include <djvUI/FileBrowserSettings.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SearchBox.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Splitter.h>
#include <djvUI/ToolButton.h>
#include <djvUI/Toolbar.h>
#include <djvUI/UISystem.h>
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
                std::map<std::string, std::shared_ptr<Action> > actions;
                std::map<std::string, std::shared_ptr<Menu> > menus;
                std::shared_ptr<ActionGroup> viewTypeActionGroup;
                std::shared_ptr<ItemView> itemView;
                size_t itemCount = 0;
                std::shared_ptr<Label> itemCountLabel;
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

                p.actions["Back"] = Action::create();
                p.actions["Back"]->setIcon("djvIconArrowLeft");
                p.actions["Back"]->setShortcut(GLFW_KEY_LEFT);
                addAction(p.actions["Back"]);

                p.actions["Forward"] = Action::create();
                p.actions["Forward"]->setIcon("djvIconArrowRight");
                p.actions["Forward"]->setShortcut(GLFW_KEY_RIGHT);
                addAction(p.actions["Forward"]);

                p.actions["Up"] = Action::create();
                p.actions["Up"]->setIcon("djvIconArrowUp");
                p.actions["Up"]->setShortcut(GLFW_KEY_UP);
                addAction(p.actions["Up"]);

                p.actions["Current"] = Action::create();
                p.actions["Current"]->setShortcut(GLFW_KEY_PERIOD);
                addAction(p.actions["Current"]);

                p.actions["EditPath"] = Action::create();
                addAction(p.actions["EditPath"]);
                p.actions["EditPath"]->setIcon("djvIconEdit");

                p.actions["LargeThumbnails"] = Action::create();
                p.actions["LargeThumbnails"]->setIcon("djvIconThumbnailsLarge");
                p.actions["LargeThumbnails"]->setShortcut(GLFW_KEY_1);
                addAction(p.actions["LargeThumbnails"]);
                p.actions["SmallThumbnails"] = Action::create();
                p.actions["SmallThumbnails"]->setIcon("djvIconThumbnailsSmall");
                p.actions["SmallThumbnails"]->setShortcut(GLFW_KEY_2);
                addAction(p.actions["SmallThumbnails"]);
                p.actions["ListView"] = Action::create();
                p.actions["ListView"]->setIcon("djvIconListView");
                p.actions["ListView"]->setShortcut(GLFW_KEY_3);
                addAction(p.actions["ListView"]);
                p.viewTypeActionGroup = ActionGroup::create(ButtonType::Radio);
                p.viewTypeActionGroup->addAction(p.actions["LargeThumbnails"]);
                p.viewTypeActionGroup->addAction(p.actions["SmallThumbnails"]);
                p.viewTypeActionGroup->addAction(p.actions["ListView"]);

                p.actions["FileSequences"] = Action::create();
                p.actions["FileSequences"]->setButtonType(ButtonType::Toggle);
                p.actions["FileSequences"]->setIcon("djvIconFileSequence");
                p.actions["FileSequences"]->setShortcut(GLFW_KEY_S);
                addAction(p.actions["FileSequences"]);

                p.actions["ShowHidden"] = Action::create();
                p.actions["ShowHidden"]->setButtonType(ButtonType::Toggle);
                p.actions["ShowHidden"]->setShortcut(GLFW_KEY_N);
                addAction(p.actions["ShowHidden"]);

                p.menus["Directory"] = Menu::create(context);
                p.menus["Directory"]->addAction(p.actions["Back"]);
                p.menus["Directory"]->addAction(p.actions["Forward"]);
                p.menus["Directory"]->addAction(p.actions["Up"]);
                p.menus["Directory"]->addAction(p.actions["Current"]);

                p.menus["View"] = Menu::create(context);
                //! \todo Make these a sub-menu.
                p.menus["View"]->addAction(p.actions["LargeThumbnails"]);
                p.menus["View"]->addAction(p.actions["SmallThumbnails"]);
                p.menus["View"]->addAction(p.actions["ListView"]);
                p.menus["View"]->addSeparator();
                p.menus["View"]->addAction(p.actions["FileSequences"]);
                p.menus["View"]->addAction(p.actions["ShowHidden"]);

                p.menus["Sort"] = Menu::create(context);

                p.menus["Bookmarks"] = Menu::create(context);

                auto menuBar = MenuBar::create(context);
                menuBar->addMenu(p.menus["Directory"]);
                menuBar->addMenu(p.menus["View"]);
                menuBar->addMenu(p.menus["Sort"]);
                menuBar->addMenu(p.menus["Bookmarks"]);

                auto pathWidget = PathWidget::create(context);

                auto topToolBar = Toolbar::create(context);
				topToolBar->addAction(p.actions["Back"]);
				topToolBar->addAction(p.actions["Forward"]);
				topToolBar->addAction(p.actions["Up"]);
				topToolBar->addWidget(pathWidget, RowStretch::Expand);
                topToolBar->addAction(p.actions["EditPath"]);

                p.itemCountLabel = Label::create(context);
                p.itemCountLabel->setMargin(MetricsRole::MarginSmall);

                auto searchBox = SearchBox::create(context);
                
                auto bottomToolBar = Toolbar::create(context);
                bottomToolBar->addExpander();
                bottomToolBar->addWidget(p.itemCountLabel);
                bottomToolBar->addAction(p.actions["LargeThumbnails"]);
                bottomToolBar->addAction(p.actions["SmallThumbnails"]);
                bottomToolBar->addAction(p.actions["ListView"]);
                bottomToolBar->addAction(p.actions["FileSequences"]);
                bottomToolBar->addWidget(searchBox);

                auto shortcutsWidget = ShortcutsWidget::create(context);

                p.itemView = ItemView::create(context);
                p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                p.scrollWidget->setBorder(false);
                p.scrollWidget->addWidget(p.itemView);

                p.layout = VerticalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                p.layout->addWidget(menuBar);
                p.layout->addSeparator();

                auto vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(MetricsRole::None);
                vLayout->addWidget(topToolBar);
                vLayout->addSeparator();
                auto hLayout = HorizontalLayout::create(context);
                hLayout->setSpacing(MetricsRole::None);
                hLayout->addSeparator();
                hLayout->addWidget(p.scrollWidget, RowStretch::Expand);
                vLayout->addWidget(hLayout, RowStretch::Expand);
                vLayout->addSeparator();
                vLayout->addWidget(bottomToolBar);

                auto splitter = Layout::Splitter::create(Orientation::Horizontal, context);
				splitter->setSplit(.15f);
				splitter->addWidget(shortcutsWidget);
                splitter->addWidget(vLayout);
                p.layout->addWidget(splitter, RowStretch::Expand);
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

                shortcutsWidget->setShortcutCallback(
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
                        if (auto uiSystem = context->getSystemT<UISystem>().lock())
                        {
                            uiSystem->getFileBrowserSettings()->setViewType(viewType);
                        }
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
                    p.directoryModel->observeFileInfoList(),
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

                if (auto uiSystem = context->getSystemT<UISystem>().lock())
                {
                    p.viewTypeObserver = ValueObserver<ViewType>::create(
                        uiSystem->getFileBrowserSettings()->observeViewType(),
                        [weak](ViewType value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->setViewType(value);
                        }
                    });
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
                auto context = getContext();
                p.actions["Up"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Up")));
                p.actions["Up"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Up Tooltip")));
                p.actions["Current"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Current Directory")));
                p.actions["Current"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Current Directory Tooltip")));
                p.actions["Back"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Back")));
                p.actions["Back"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Back Tooltip")));
                p.actions["Forward"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Forward")));
                p.actions["Forward"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Forward Tooltip")));
                p.actions["LargeThumbnails"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Large Thumbnails")));
                p.actions["LargeThumbnails"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Large Thumbnails Tooltip")));
                p.actions["SmallThumbnails"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Small Thumbnails")));
                p.actions["SmallThumbnails"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Small Thumbnails Tooltip")));
                p.actions["ListView"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "List View")));
                p.actions["ListView"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "List View Tooltip")));
                p.actions["FileSequences"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "File Sequences")));
                p.actions["FileSequences"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "File Sequences Tooltip")));
                p.actions["ShowHidden"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Show Hidden")));
                p.actions["ShowHidden"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Show Hidden Tooltip")));

                p.menus["Directory"]->setMenuName(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Directory")));
                p.menus["View"]->setMenuName(context->getText(DJV_TEXT("djv::UI::FileBrowser", "View")));
                p.menus["Sort"]->setMenuName(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Sort")));
                p.menus["Bookmarks"]->setMenuName(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Bookmarks")));

                p.itemCountLabel->setText(p.getItemCountLabel(p.itemCount, context));
            }

            std::string Widget::Private::getItemCountLabel(size_t size, Context * context)
            {
                std::stringstream ss;
                ss << size << " " << context->getText(DJV_TEXT("djv::UI::FileBrowser", "items"));
                return ss.str();
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
