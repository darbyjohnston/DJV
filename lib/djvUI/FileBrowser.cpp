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
#include <djvUI/Border.h>
#include <djvUI/DialogSystem.h>
#include <djvUI/FileBrowserPrivate.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SearchBox.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Splitter.h>
#include <djvUI/ToolButton.h>
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
                std::vector<FileSystem::FileInfo> fileInfoList;
                std::map<std::shared_ptr<ItemButton>, FileSystem::FileInfo> buttonToFileInfo;
                ViewType viewType = ViewType::First;
                std::map<std::string, std::shared_ptr<Action> > actions;
                std::map<std::string, std::shared_ptr<Menu> > menus;
                std::shared_ptr<ActionGroup> viewTypeActionGroup;
                size_t itemCount = 0;
                std::shared_ptr<Label> itemCountLabel;
                std::shared_ptr<Layout::Flow> iconsLayout;
                std::shared_ptr<Layout::Vertical> listLayout;
                std::shared_ptr<ScrollWidget> scrollWidget;
                std::shared_ptr<Layout::Vertical> layout;
                std::function<void(const FileSystem::FileInfo &)> callback;

                struct InfoFuture
                {
                    FileSystem::FileInfo fileInfo;
                    std::future<AV::IO::Info> future;
                    std::weak_ptr<ItemButton> widget;
                };
                std::vector<InfoFuture> infoFutures;
                struct ImageFuture
                {
                    std::future<std::shared_ptr<AV::Image::Image> > future;
                    std::weak_ptr<ItemButton> widget;
                };
                std::vector<ImageFuture> imageFutures;

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
                std::shared_ptr<ValueObserver<bool> > reloadObserver;
                std::shared_ptr<ValueObserver<bool> > fileSequencesObserver;
                std::shared_ptr<ValueObserver<bool> > showHiddenObserver;

                std::string getTooltip(const FileSystem::FileInfo &, Context * context);
                std::string getTooltip(const FileSystem::FileInfo &, const AV::IO::Info &, Context *);
                std::string getItemCountLabel(size_t, Context *);
            };

            void Widget::_init(Context * context)
            {
                UI::Widget::_init(context);

                setClassName("djv::UI::FileBrowser::Widget");

                DJV_PRIVATE_PTR();
                p.directoryModel = FileSystem::DirectoryModel::create(context);

                p.actions["Up"] = Action::create();
                p.actions["Up"]->setIcon("djvIconArrowUp");
                p.actions["Up"]->setShortcut(GLFW_KEY_UP);
                addAction(p.actions["Up"]);

                p.actions["Current"] = Action::create();
                p.actions["Current"]->setShortcut(GLFW_KEY_PERIOD);
                addAction(p.actions["Current"]);

                p.actions["Back"] = Action::create();
                p.actions["Back"]->setIcon("djvIconArrowLeft");
                p.actions["Back"]->setShortcut(GLFW_KEY_LEFT);
                addAction(p.actions["Back"]);

                p.actions["Forward"] = Action::create();
                p.actions["Forward"]->setIcon("djvIconArrowRight");
                p.actions["Forward"]->setShortcut(GLFW_KEY_RIGHT);
                addAction(p.actions["Forward"]);

                p.actions["Reload"] = Action::create();
                p.actions["Reload"]->setShortcut(GLFW_KEY_R);
                addAction(p.actions["Reload"]);

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
                p.viewTypeActionGroup->setChecked(static_cast<int>(p.viewType));

                p.actions["FileSequences"] = Action::create();
                p.actions["FileSequences"]->setButtonType(ButtonType::Toggle);
                p.actions["FileSequences"]->setShortcut(GLFW_KEY_S);
                addAction(p.actions["FileSequences"]);

                p.actions["ShowHidden"] = Action::create();
                p.actions["ShowHidden"]->setButtonType(ButtonType::Toggle);
                p.actions["ShowHidden"]->setShortcut(GLFW_KEY_N);
                addAction(p.actions["ShowHidden"]);

                p.menus["Directory"] = Menu::create(context);
                p.menus["Directory"]->addAction(p.actions["Up"]);
                p.menus["Directory"]->addAction(p.actions["Current"]);
                p.menus["Directory"]->addSeparator();
                p.menus["Directory"]->addAction(p.actions["Back"]);
                p.menus["Directory"]->addAction(p.actions["Forward"]);
                p.menus["Directory"]->addSeparator();
                p.menus["Directory"]->addAction(p.actions["Reload"]);

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
                topToolBar->addAction(p.actions["Up"]);
                topToolBar->addAction(p.actions["Back"]);
                topToolBar->addAction(p.actions["Forward"]);
                topToolBar->addWidget(pathWidget, Layout::RowStretch::Expand);

                p.itemCountLabel = Label::create(context);
                p.itemCountLabel->setMargin(Style::MetricsRole::MarginSmall);

                auto searchBox = SearchBox::create(context);
                
                auto bottomToolBar = Toolbar::create(context);
                bottomToolBar->addExpander();
                bottomToolBar->addWidget(p.itemCountLabel);
                bottomToolBar->addWidget(searchBox);

                auto shortcutsWidget = ShorcutsWidget::create(context);

                p.iconsLayout = Layout::Flow::create(context);
                p.iconsLayout->setSpacing(Style::MetricsRole::None);
                p.listLayout = Layout::Vertical::create(context);
                p.listLayout->setSpacing(Style::MetricsRole::None);
                p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                p.scrollWidget->addWidget(p.iconsLayout);
                p.scrollWidget->addWidget(p.listLayout);

                p.layout = Layout::Vertical::create(context);
                p.layout->setSpacing(Style::MetricsRole::None);
                p.layout->addWidget(menuBar);
                p.layout->addSeparator();
                p.layout->addWidget(topToolBar);
                auto splitter = Layout::Splitter::create(Orientation::Horizontal, context);
                splitter->setMargin(Layout::Margin(Style::MetricsRole::MarginSmall, Style::MetricsRole::MarginSmall, Style::MetricsRole::None, Style::MetricsRole::None));
                splitter->addWidget(shortcutsWidget);
                splitter->addWidget(p.scrollWidget);
                splitter->setSplit(.15f);
                p.layout->addWidget(splitter, Layout::RowStretch::Expand);
                p.layout->addWidget(bottomToolBar);
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
                    [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->setViewType(static_cast<ViewType>(value));
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
                        widget->_p->fileInfoList = value;
                        widget->_updateItems();
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

                p.reloadObserver = ValueObserver<bool>::create(
                    p.actions["Reload"]->observeClicked(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->directoryModel->reload();
                        }
                    }
                });

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
                if (value == p.viewType)
                    return;
                p.viewType = value;
                _updateItems();
            }

            void Widget::setCallback(const std::function<void(const FileSystem::FileInfo &)> & value)
            {
                _p->callback = value;
            }

            float Widget::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }

            void Widget::_styleEvent(Event::Style &)
            {
                _updateItems();
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
                p.actions["Up"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "UpTooltip")));
                p.actions["Current"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Current Directory")));
                p.actions["Current"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "CurrentDirectoryTooltip")));
                p.actions["Back"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Back")));
                p.actions["Back"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "BackTooltip")));
                p.actions["Forward"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Forward")));
                p.actions["Forward"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "ForwardTooltip")));
                p.actions["Reload"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Reload")));
                p.actions["Reload"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "ReloadTooltip")));
                p.actions["LargeThumbnails"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Large Thumbnails")));
                p.actions["LargeThumbnails"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "LargethumbnailsTooltip")));
                p.actions["SmallThumbnails"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Small Thumbnails")));
                p.actions["SmallThumbnails"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "SmallThumbnailsTooltip")));
                p.actions["ListView"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "List View")));
                p.actions["ListView"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "ListViewTooltip")));
                p.actions["FileSequences"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "File Sequences")));
                p.actions["FileSequences"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "FileSequencesTooltip")));
                p.actions["ShowHidden"]->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Show Hidden")));
                p.actions["ShowHidden"]->setTooltip(context->getText(DJV_TEXT("djv::UI::FileBrowser", "ShowHiddenTooltip")));

                p.menus["Directory"]->setMenuName(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Directory")));
                p.menus["View"]->setMenuName(context->getText(DJV_TEXT("djv::UI::FileBrowser", "View")));
                p.menus["Sort"]->setMenuName(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Sort")));
                p.menus["Bookmarks"]->setMenuName(context->getText(DJV_TEXT("djv::UI::FileBrowser", "Bookmarks")));

                p.itemCountLabel->setText(p.getItemCountLabel(p.itemCount, context));
            }

            void Widget::_updateEvent(Event::Update& event)
            {
                DJV_PRIVATE_PTR();
                {
                    auto i = p.infoFutures.begin();
                    while (i != p.infoFutures.end())
                    {
                        if (i->future.valid() &&
                            i->future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                const auto info = i->future.get();
                                auto context = getContext();
                                auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>().lock();
                                auto ioSystem = context->getSystemT<AV::IO::System>().lock();
                                auto style = _getStyle().lock();
                                auto widget = i->widget.lock();
                                if (thumbnailSystem && ioSystem && style && widget)
                                {
                                    widget->setTooltip(p.getTooltip(i->fileInfo, info, context));
                                    int t = 0;
                                    switch (p.viewType)
                                    {
                                    case ViewType::ThumbnailsLarge: t = static_cast<int>(style->getMetric(UI::Style::MetricsRole::ThumbnailLarge)); break;
                                    case ViewType::ThumbnailsSmall: t = static_cast<int>(style->getMetric(UI::Style::MetricsRole::ThumbnailSmall)); break;
                                    case ViewType::ListView:        t = static_cast<int>(style->getMetric(UI::Style::MetricsRole::Icon));           break;
                                    default: break;
                                    }
                                    glm::ivec2 thumbnailSize(t, t);
                                    if (info.video.size())
                                    {
                                        const glm::ivec2 & videoSize = info.video[0].info.size;
                                        if (videoSize.x >= videoSize.y)
                                        {
                                            thumbnailSize.y = 0;
                                        }
                                        else
                                        {
                                            thumbnailSize.x = 0;
                                        }
                                    }
                                    Private::ImageFuture future;
                                    future.future = thumbnailSystem->getImage(i->fileInfo.getPath(), thumbnailSize);
                                    future.widget = i->widget;
                                    p.imageFutures.push_back(std::move(future));
                                }
                            }
                            catch (const std::exception & e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.infoFutures.erase(i);
                            continue;
                        }
                        ++i;
                    }
                }
                {
                    auto i = p.imageFutures.begin();
                    while (i != p.imageFutures.end())
                    {
                        if (i->future.valid() &&
                            i->future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                const auto icon = i->future.get();
                                if (auto widget = i->widget.lock())
                                {
                                    widget->setThumbnail(icon, createUID());
                                }
                            }
                            catch (const std::exception & e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.imageFutures.erase(i);
                            continue;
                        }
                        ++i;
                    }
                }
            }

            bool Widget::_eventFilter(const std::shared_ptr<IObject>& object, Event::IEvent& event)
            {
                switch (event.getEventType())
                {
                case Event::Type::Clip:
                {
                    if (auto button = std::dynamic_pointer_cast<ItemButton>(object))
                    {
                        if (!button->isClipped())
                        {
                            DJV_PRIVATE_PTR();
                            const auto i = p.buttonToFileInfo.find(button);
                            if (i != p.buttonToFileInfo.end())
                            {
                                auto context = getContext();
                                if (auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>().lock())
                                {
                                    if (auto ioSystem = context->getSystemT<AV::IO::System>().lock())
                                    {
                                        if (ioSystem->canRead(i->second))
                                        {
                                            if (auto style = _getStyle().lock())
                                            {
                                                Private::InfoFuture future;
                                                future.fileInfo = i->second;
                                                future.future = thumbnailSystem->getInfo(i->second.getPath());
                                                future.widget = button;
                                                p.infoFutures.push_back(std::move(future));
                                            }
                                        }
                                    }
                                }
                                button->removeEventFilter(shared_from_this());
                                p.buttonToFileInfo.erase(i);
                            }
                        }
                    }
                    break;
                }
                default: break;
                }
                return false;
            }

            void Widget::_updateItems()
            {
                auto context = getContext();
                DJV_PRIVATE_PTR();
                p.buttonToFileInfo.clear();
                p.iconsLayout->clearWidgets();
                p.listLayout->clearWidgets();
                p.infoFutures.clear();
                p.imageFutures.clear();
                auto weak = std::weak_ptr<Widget>(std::dynamic_pointer_cast<Widget>(shared_from_this()));
                for (const auto & fileInfo : p.fileInfoList)
                {
                    auto button = ItemButton::create(p.viewType, context);
                    button->setText(fileInfo.getFileName(Frame::Invalid, false));
                    button->setTooltip(p.getTooltip(fileInfo, context));
                    std::string name;
                    switch (fileInfo.getType())
                    {
                    case FileSystem::FileType::Directory:
                        switch (p.viewType)
                        {
                        case ViewType::ThumbnailsLarge: name = "djvIconFileBrowserDirectoryLarge"; break;
                        case ViewType::ThumbnailsSmall: name = "djvIconFileBrowserDirectorySmall"; break;
                        case ViewType::ListView:        name = "djvIconFileBrowserDirectoryList";  break;
                        default: break;
                        }
                        break;
                    default:
                        switch (p.viewType)
                        {
                        case ViewType::ThumbnailsLarge: name = "djvIconFileBrowserFileLarge"; break;
                        case ViewType::ThumbnailsSmall: name = "djvIconFileBrowserFileSmall"; break;
                        case ViewType::ListView:        name = "djvIconFileBrowserFileList";  break;
                        default: break;
                        }
                        break;
                    }
                    if (!name.empty())
                    {
                        button->setIcon(name);
                    }
                    Style::MetricsRole iconSizeRole = Style::MetricsRole::None;
                    Style::MetricsRole textSizeRole = Style::MetricsRole::None;
                    switch (p.viewType)
                    {
                    case ViewType::ThumbnailsLarge: textSizeRole = Style::MetricsRole::ThumbnailLarge; break;
                    case ViewType::ThumbnailsSmall: textSizeRole = Style::MetricsRole::ThumbnailSmall; break;
                    case ViewType::ListView:        iconSizeRole = Style::MetricsRole::Icon;           break;
                    default: break;
                    }
                    button->setIconSizeRole(iconSizeRole);
                    button->setTextSizeRole(textSizeRole);
                    switch (p.viewType)
                    {
                    case ViewType::ThumbnailsLarge:
                    case ViewType::ThumbnailsSmall: p.iconsLayout->addWidget(button); break;
                    case ViewType::ListView:        p.listLayout->addWidget(button);  break;
                    default: break;
                    }
                    p.buttonToFileInfo[button] = fileInfo;
                    button->installEventFilter(shared_from_this());
                    button->setClickedCallback(
                        [weak, fileInfo]
                    {
                        if (auto widget = weak.lock())
                        {
                            if (FileSystem::FileType::Directory == fileInfo.getType())
                            {
                                widget->_p->directoryModel->setPath(fileInfo.getPath());
                            }
                            else if (widget->_p->callback)
                            {
                                widget->_p->callback(fileInfo);
                            }
                        }
                    });
                }
                _resize();
            }

            std::string Widget::Private::getTooltip(const FileSystem::FileInfo & fileInfo, Context * context)
            {
                std::stringstream ss;
                ss << fileInfo.getFileName(Frame::Invalid, false) << '\n';
                ss << '\n';
                ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Type")) << ": " << fileInfo.getType() << '\n';
                ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Size")) << ": " << Memory::getSizeLabel(fileInfo.getSize()) << '\n';
                ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Modification time")) << ": " << Time::getLabel(fileInfo.getTime());
                return ss.str();
            }

            std::string Widget::Private::getTooltip(const FileSystem::FileInfo & fileInfo, const AV::IO::Info & avInfo, Context * context)
            {
                std::stringstream ss;
                ss << getTooltip(fileInfo, context);
                size_t track = 0;
                for (const auto & videoInfo : avInfo.video)
                {
                    ss << '\n' << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Video track")) << " #" << track << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Size")) << ": " << videoInfo.info.size << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Type")) << ": " << videoInfo.info.type << '\n';
                    ss.precision(2);
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Speed")) << ": " <<
                        Time::Speed::speedToFloat(videoInfo.speed) <<
                        context->getText(DJV_TEXT("djv::UI::FileBrowser", "FPS")) << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Duration")) << ": " <<
                        Time::getLabel(Time::durationToSeconds(videoInfo.duration));
                    ++track;
                }
                track = 0;
                for (const auto & audioInfo : avInfo.audio)
                {
                    ss << '\n' << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Audio track")) << " #" << track << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Channels")) << ": " <<
                        audioInfo.info.channelCount << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Type")) << ": " <<
                        audioInfo.info.type << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Sample rate")) << ": " <<
                        audioInfo.info.sampleRate / 1000.f << DJV_TEXT("djv::UI::FileBrowser", "kHz") << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Duration")) << ": " <<
                        Time::getLabel(Time::durationToSeconds(audioInfo.duration));
                    ++track;
                }
                return ss.str();
            }

            std::string Widget::Private::getItemCountLabel(size_t size, Context * context)
            {
                std::stringstream ss;
                ss << size << " " << context->getText(DJV_TEXT("djv::UI::FileBrowser", "items"));
                return ss.str();
            }

            namespace
            {
                class Dialog : public Widget
                {
                    DJV_NON_COPYABLE(Dialog);

                protected:
                    void _init(Context * context)
                    {
                        Widget::_init(context);

                        setBackgroundRole(Style::ColorRole::Background);
                        setPointerEnabled(true);

                        auto titleIcon = Icon::create(context);
                        titleIcon->setIcon("djvIconDirectory");
                        titleIcon->setMargin(Layout::Margin(
                            Style::MetricsRole::MarginSmall,
                            Style::MetricsRole::None,
                            Style::MetricsRole::MarginSmall,
                            Style::MetricsRole::MarginSmall));

                        _titleLabel = Label::create(context);
                        _titleLabel->setFontSizeRole(Style::MetricsRole::FontLarge);
                        _titleLabel->setTextHAlign(TextHAlign::Left);
                        _titleLabel->setMargin(Layout::Margin(
                            Style::MetricsRole::Margin,
                            Style::MetricsRole::None,
                            Style::MetricsRole::MarginSmall,
                            Style::MetricsRole::MarginSmall));

                        _closeButton = Button::Tool::create(context);
                        _closeButton->setIcon("djvIconClose");
                        _closeButton->setInsideMargin(Style::MetricsRole::MarginSmall);

                        _widget = Widget::create(context);
                        _widget->setPath(FileSystem::Path("."));

                        _layout = Layout::Vertical::create(context);
                        _layout->setSpacing(Style::MetricsRole::None);
                        auto hLayout = Layout::Horizontal::create(context);
                        hLayout->setSpacing(Style::MetricsRole::None);
                        hLayout->setBackgroundRole(Style::ColorRole::BackgroundHeader);
                        hLayout->addWidget(titleIcon);
                        hLayout->addWidget(_titleLabel, Layout::RowStretch::Expand);
                        hLayout->addWidget(_closeButton);
                        _layout->addWidget(hLayout);
                        _layout->addSeparator();
                        _layout->addWidget(_widget, Layout::RowStretch::Expand);
                        _layout->setParent(shared_from_this());
                    }

                    Dialog()
                    {}

                public:
                    static std::shared_ptr<Dialog> create(Context * context)
                    {
                        auto out = std::shared_ptr<Dialog>(new Dialog);
                        out->_init(context);
                        return out;
                    }

                    void setCallback(const std::function<void(const FileSystem::FileInfo &)> & value)
                    {
                        _widget->setCallback(value);
                    }

                    void setCloseCallback(const std::function<void(void)> & value)
                    {
                        _closeButton->setClickedCallback(value);
                    }

                    float getHeightForWidth(float value) const override
                    {
                        return _layout->getHeightForWidth(value);
                    }

                protected:
                    void _preLayoutEvent(Event::PreLayout&) override
                    {
                        _setMinimumSize(_layout->getMinimumSize());
                    }

                    void _layoutEvent(Event::Layout&) override
                    {
                        _layout->setGeometry(getGeometry());
                    }

                    void _localeEvent(Event::Locale &) override
                    {
                        auto context = getContext();
                        _titleLabel->setText(context->getText(DJV_TEXT("djv::UI::FileBrowser", "File Browser")));
                    }

                private:
                    std::shared_ptr<Label> _titleLabel;
                    std::shared_ptr<Widget> _widget;
                    std::shared_ptr<Button::Tool> _closeButton;
                    std::shared_ptr<Layout::Vertical> _layout;
                };

            } // namespace

            struct DialogSystem::Private
            {
                std::shared_ptr<Dialog> dialog;
                std::shared_ptr<Layout::Overlay> overlay;
            };

            void DialogSystem::_init(Context * context)
            {
                ISystem::_init("djv::UI::FileBrowser::DialogSystem", context);
            }

            DialogSystem::DialogSystem() :
                _p(new Private)
            {}

            DialogSystem::~DialogSystem()
            {}

            std::shared_ptr<DialogSystem> DialogSystem::create(Context * context)
            {
                auto out = std::shared_ptr<DialogSystem>(new DialogSystem);
                out->_init(context);
                return out;
            }

            void DialogSystem::show(const std::function<void(const FileSystem::FileInfo &)> & callback)
            {
                auto context = getContext();
                DJV_PRIVATE_PTR();
                if (!p.dialog)
                {
                    p.dialog = Dialog::create(context);
                    auto border = Layout::Border::create(context);
                    border->addWidget(p.dialog);
                    p.overlay = Layout::Overlay::create(context);
                    p.overlay->setBackgroundRole(Style::ColorRole::Overlay);
                    p.overlay->setMargin(Style::MetricsRole::MarginLarge);
                    p.overlay->addWidget(border);
                }
                if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
                {
                    if (auto window = windowSystem->observeCurrentWindow()->get())
                    {
                        window->addWidget(p.overlay);
                        p.overlay->show();

                        auto weak = std::weak_ptr<DialogSystem>(std::dynamic_pointer_cast<DialogSystem>(shared_from_this()));
                        p.dialog->setCallback(
                            [weak, window, callback](const FileSystem::FileInfo & value)
                        {
                            if (auto system = weak.lock())
                            {
                                window->removeWidget(system->_p->overlay);
                                callback(value);
                            }
                        });
                        p.dialog->setCloseCallback(
                            [weak, window]
                        {
                            if (auto system = weak.lock())
                            {
                                window->removeWidget(system->_p->overlay);
                            }
                        });
                        p.overlay->setCloseCallback(
                            [weak, window]
                        {
                            if (auto system = weak.lock())
                            {
                                window->removeWidget(system->_p->overlay);
                            }
                        });
                    }
                }
            }

        } // namespace FileBrowser
    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI::FileBrowser,
        ViewType,
        DJV_TEXT("djv::UI::FileBrowser", "ThumbnailsLarge"),
        DJV_TEXT("djv::UI::FileBrowser", "ThumbnailsSmall"),
        DJV_TEXT("djv::UI::FileBrowser", "ListView"));

} // namespace djv
