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
#include <djvUI/Border.h>
#include <djvUI/DialogSystem.h>
#include <djvUI/FileBrowserPrivate.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Splitter.h>
#include <djvUI/ToolBar.h>
#include <djvUI/Window.h>

#include <djvAV/IO.h>
#include <djvAV/IconSystem.h>

#include <djvCore/DirectoryModel.h>
#include <djvCore/FileInfo.h>
#include <djvCore/IEventSystem.h>

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
                std::shared_ptr<ActionGroup> viewTypeActionGroup;
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

                std::string _getTooltip(const FileSystem::FileInfo &);
                std::string _getTooltip(const FileSystem::FileInfo &, const AV::IO::Info &);
            };

            void Widget::_init(Context * context)
            {
                UI::Widget::_init(context);

                setClassName("djv::UI::FileBrowser::Widget");

                _p->directoryModel = FileSystem::DirectoryModel::create(context);

                auto upAction = Action::create();
                upAction->setText(DJV_TEXT("Up"));
                upAction->setIcon(context->getPath(FileSystem::ResourcePath::IconsDirectory, "djvIconArrowUp90DPI.png"));
                upAction->setTooltip(DJV_TEXT("Go up a directory."));
                upAction->setShortcut(GLFW_KEY_UP);
                addAction(upAction);

                auto currentAction = Action::create();
                currentAction->setText(DJV_TEXT("Current Directory"));
                currentAction->setTooltip(DJV_TEXT("Go to the current directory."));
                currentAction->setShortcut(GLFW_KEY_PERIOD);
                addAction(currentAction);

                auto backAction = Action::create();
                backAction->setText(DJV_TEXT("Back"));
                backAction->setIcon(context->getPath(FileSystem::ResourcePath::IconsDirectory, "djvIconArrowLeft90DPI.png"));
                backAction->setTooltip(DJV_TEXT("Go back a directory."));
                backAction->setShortcut(GLFW_KEY_LEFT);
                addAction(backAction);

                auto forwardAction = Action::create();
                forwardAction->setText(DJV_TEXT("Forward"));
                forwardAction->setIcon(context->getPath(FileSystem::ResourcePath::IconsDirectory, "djvIconArrowRight90DPI.png"));
                forwardAction->setTooltip(DJV_TEXT("Go forward a directory."));
                forwardAction->setShortcut(GLFW_KEY_RIGHT);
                addAction(forwardAction);

                auto reloadAction = Action::create();
                reloadAction->setText(DJV_TEXT("Reload"));
                reloadAction->setTooltip(DJV_TEXT("Reload the directory."));
                reloadAction->setShortcut(GLFW_KEY_R);
                addAction(reloadAction);

                auto largeThumbnailsAction = Action::create();
                largeThumbnailsAction->setText(DJV_TEXT("Large Thumbnails"));
                largeThumbnailsAction->setIcon(context->getPath(FileSystem::ResourcePath::IconsDirectory, "djvIconThumbnailsLarge90DPI.png"));
                largeThumbnailsAction->setTooltip(DJV_TEXT("Show items with large thumbnail previews."));
                largeThumbnailsAction->setShortcut(GLFW_KEY_1);
                addAction(largeThumbnailsAction);
                auto smallThumbnailsAction = Action::create();
                smallThumbnailsAction->setText(DJV_TEXT("Small Thumbnails"));
                smallThumbnailsAction->setIcon(context->getPath(FileSystem::ResourcePath::IconsDirectory, "djvIconThumbnailsSmall90DPI.png"));
                smallThumbnailsAction->setTooltip(DJV_TEXT("Show items with small thumbnail previews."));
                smallThumbnailsAction->setShortcut(GLFW_KEY_2);
                addAction(smallThumbnailsAction);
                auto listViewAction = Action::create();
                listViewAction->setText(DJV_TEXT("List View"));
                listViewAction->setIcon(context->getPath(FileSystem::ResourcePath::IconsDirectory, "djvIconListView90DPI.png"));
                listViewAction->setTooltip(DJV_TEXT("Show items in a list with detailed information."));
                listViewAction->setShortcut(GLFW_KEY_3);
                addAction(listViewAction);
                _p->viewTypeActionGroup = ActionGroup::create(ButtonType::Radio);
                _p->viewTypeActionGroup->addAction(largeThumbnailsAction);
                _p->viewTypeActionGroup->addAction(smallThumbnailsAction);
                _p->viewTypeActionGroup->addAction(listViewAction);
                _p->viewTypeActionGroup->setChecked(static_cast<int>(_p->viewType));

                auto fileSequencesAction = Action::create();
                fileSequencesAction->setButtonType(ButtonType::Toggle);
                fileSequencesAction->setText(DJV_TEXT("File Sequences"));
                fileSequencesAction->setTooltip(DJV_TEXT("Toggle whether file sequences are enabled."));
                fileSequencesAction->setShortcut(GLFW_KEY_S);
                addAction(fileSequencesAction);

                auto showHiddenAction = Action::create();
                showHiddenAction->setButtonType(ButtonType::Toggle);
                showHiddenAction->setText(DJV_TEXT("Show Hidden"));
                showHiddenAction->setTooltip(DJV_TEXT("Show hidden items."));
                showHiddenAction->setShortcut(GLFW_KEY_N);
                addAction(showHiddenAction);

                auto directoryMenu = Menu::create(DJV_TEXT("Directory"), context);
                directoryMenu->addAction(upAction);
                directoryMenu->addAction(currentAction);
                directoryMenu->addSeparator();
                directoryMenu->addAction(backAction);
                directoryMenu->addAction(forwardAction);
                directoryMenu->addSeparator();
                directoryMenu->addAction(reloadAction);

                auto viewMenu = Menu::create(DJV_TEXT("View"), context);
                //! \todo Make these a sub-menu.
                viewMenu->addAction(largeThumbnailsAction);
                viewMenu->addAction(smallThumbnailsAction);
                viewMenu->addAction(listViewAction);
                viewMenu->addSeparator();
                viewMenu->addAction(fileSequencesAction);
                viewMenu->addAction(showHiddenAction);

                auto sortMenu = Menu::create(DJV_TEXT("Sort"), context);

                auto bookmarksMenu = Menu::create(DJV_TEXT("Bookmarks"), context);

                auto menuBar = MenuBar::create(context);
                menuBar->addMenu(directoryMenu);
                menuBar->addMenu(viewMenu);
                menuBar->addMenu(sortMenu);
                menuBar->addMenu(bookmarksMenu);

                auto pathWidget = PathWidget::create(context);

                auto topToolBar = Toolbar::create(context);
                topToolBar->addAction(upAction);
                topToolBar->addAction(backAction);
                topToolBar->addAction(forwardAction);
                topToolBar->addWidget(pathWidget, Layout::RowStretch::Expand);

                auto itemCountLabel = Label::create(context);
                itemCountLabel->setMargin(Style::MetricsRole::Margin);

                auto bottomToolBar = Toolbar::create(context);
                bottomToolBar->addAction(largeThumbnailsAction);
                bottomToolBar->addAction(smallThumbnailsAction);
                bottomToolBar->addAction(listViewAction);
                bottomToolBar->addSeparator();
                bottomToolBar->addWidget(itemCountLabel);

                auto shortcutsWidget = ShorcutsWidget::create(context);

                _p->iconsLayout = Layout::Flow::create(context);
                _p->iconsLayout->setSpacing(Style::MetricsRole::None);
                _p->listLayout = Layout::Vertical::create(context);
                _p->listLayout->setSpacing(Style::MetricsRole::None);
                _p->scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                _p->scrollWidget->addWidget(_p->iconsLayout);
                _p->scrollWidget->addWidget(_p->listLayout);

                _p->layout = Layout::Vertical::create(context);
                _p->layout->setSpacing(Style::MetricsRole::None);
                _p->layout->addWidget(menuBar);
                _p->layout->addWidget(topToolBar);
                auto splitter = Layout::Splitter::create(Orientation::Horizontal, context);
                splitter->addWidget(shortcutsWidget);
                splitter->addWidget(_p->scrollWidget);
                splitter->setSplit(.15f);
                _p->layout->addWidget(splitter, Layout::RowStretch::Expand);
                _p->layout->addWidget(bottomToolBar);
                _p->layout->setParent(shared_from_this());

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

                _p->viewTypeActionGroup->setRadioCallback(
                    [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->setViewType(static_cast<ViewType>(value));
                    }
                });
                
                _p->pathObserver = ValueObserver<FileSystem::Path>::create(
                    _p->directoryModel->observePath(),
                    [weak, pathWidget](const FileSystem::Path & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->scrollWidget->setScrollPos(glm::vec2(0.f, 0.f));
                    }
                    pathWidget->setPath(value);
                });

                _p->fileInfoObserver = ListObserver<FileSystem::FileInfo>::create(
                    _p->directoryModel->observeFileInfoList(),
                    [weak, itemCountLabel](const std::vector<FileSystem::FileInfo> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->fileInfoList = value;
                        widget->_updateItems();
                    }
                    std::stringstream ss;
                    ss << DJV_TEXT("Number of items") << ": " << value.size();
                    itemCountLabel->setText(ss.str());
                });

                _p->hasUpObserver = ValueObserver<bool>::create(
                    _p->directoryModel->observeHasUp(),
                    [upAction](bool value)
                {
                    upAction->setEnabled(value);
                });

                _p->upObserver = ValueObserver<bool>::create(
                    upAction->observeClicked(),
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

                _p->currentObserver = ValueObserver<bool>::create(
                    currentAction->observeClicked(),
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

                _p->historyObserver = ListObserver<FileSystem::Path>::create(
                    _p->directoryModel->observeHistory(),
                    [pathWidget](const std::vector<FileSystem::Path> & value)
                {
                    pathWidget->setHistory(value);
                });

                _p->historyIndexObserver = ValueObserver<size_t>::create(
                    _p->directoryModel->observeHistoryIndex(),
                    [pathWidget](size_t value)
                {
                    pathWidget->setHistoryIndex(value);
                });

                _p->hasBackObserver = ValueObserver<bool>::create(
                    _p->directoryModel->observeHasBack(),
                    [backAction](bool value)
                {
                    backAction->setEnabled(value);
                });

                _p->backObserver = ValueObserver<bool>::create(
                    backAction->observeClicked(),
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

                _p->hasForwardObserver = ValueObserver<bool>::create(
                    _p->directoryModel->observeHasForward(),
                    [forwardAction](bool value)
                {
                    forwardAction->setEnabled(value);
                });

                _p->forwardObserver = ValueObserver<bool>::create(
                    forwardAction->observeClicked(),
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

                _p->reloadObserver = ValueObserver<bool>::create(
                    reloadAction->observeClicked(),
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

                _p->fileSequencesObserver = ValueObserver<bool>::create(
                    fileSequencesAction->isChecked(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->setFileSequences(value);
                    }
                });

                _p->showHiddenObserver = ValueObserver<bool>::create(
                    showHiddenAction->isChecked(),
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
                if (value == _p->viewType)
                    return;
                _p->viewType = value;
                _updateItems();
            }

            void Widget::setCallback(const std::function<void(const FileSystem::FileInfo &)> & value)
            {
                _p->callback = value;
            }

            void Widget::_updateEvent(Event::Update& event)
            {
                {
                    auto i = _p->infoFutures.begin();
                    while (i != _p->infoFutures.end())
                    {
                        if (i->future.valid() &&
                            i->future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                const auto info = i->future.get();
                                auto context = getContext();
                                auto iconSystem = context->getSystemT<AV::Image::IconSystem>().lock();
                                auto ioSystem = context->getSystemT<AV::IO::System>().lock();
                                auto style = _getStyle().lock();
                                auto widget = i->widget.lock();
                                if (iconSystem && ioSystem && style && widget)
                                {
                                    widget->setTooltip(_p->_getTooltip(i->fileInfo, info));
                                    int t = 0;
                                    switch (_p->viewType)
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
                                    future.future = iconSystem->getImage(i->fileInfo.getPath(), thumbnailSize);
                                    future.widget = i->widget;
                                    _p->imageFutures.push_back(std::move(future));
                                }
                            }
                            catch (const std::exception & e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = _p->infoFutures.erase(i);
                            continue;
                        }
                        ++i;
                    }
                }
                {
                    auto i = _p->imageFutures.begin();
                    while (i != _p->imageFutures.end())
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
                            i = _p->imageFutures.erase(i);
                            continue;
                        }
                        ++i;
                    }
                }
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
                            const auto i = _p->buttonToFileInfo.find(button);
                            if (i != _p->buttonToFileInfo.end())
                            {
                                auto context = getContext();
                                if (auto iconSystem = context->getSystemT<AV::Image::IconSystem>().lock())
                                {
                                    if (auto ioSystem = context->getSystemT<AV::IO::System>().lock())
                                    {
                                        if (ioSystem->canRead(i->second))
                                        {
                                            if (auto style = _getStyle().lock())
                                            {
                                                Private::InfoFuture future;
                                                future.fileInfo = i->second;
                                                future.future = iconSystem->getInfo(i->second.getPath());
                                                future.widget = button;
                                                _p->infoFutures.push_back(std::move(future));
                                            }
                                        }
                                    }
                                }
                                button->removeEventFilter(shared_from_this());
                                _p->buttonToFileInfo.erase(i);
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
                _p->buttonToFileInfo.clear();
                _p->iconsLayout->clearWidgets();
                _p->listLayout->clearWidgets();
                _p->infoFutures.clear();
                _p->imageFutures.clear();
                auto weak = std::weak_ptr<Widget>(std::dynamic_pointer_cast<Widget>(shared_from_this()));
                for (const auto & fileInfo : _p->fileInfoList)
                {
                    auto button = ItemButton::create(_p->viewType, context);
                    button->setText(fileInfo.getFileName(Frame::Invalid, false));
                    button->setTooltip(_p->_getTooltip(fileInfo));
                    std::string name;
                    switch (fileInfo.getType())
                    {
                    case FileSystem::FileType::Directory:
                        switch (_p->viewType)
                        {
                        case ViewType::ThumbnailsLarge: name = "djvIconFileBrowserDirectoryLarge90DPI.png"; break;
                        case ViewType::ThumbnailsSmall: name = "djvIconFileBrowserDirectorySmall90DPI.png"; break;
                        case ViewType::ListView:        name = "djvIconFileBrowserDirectoryList96DPI.png";  break;
                        default: break;
                        }
                        break;
                    default:
                        switch (_p->viewType)
                        {
                        case ViewType::ThumbnailsLarge: name = "djvIconFileBrowserFileLarge90DPI.png"; break;
                        case ViewType::ThumbnailsSmall: name = "djvIconFileBrowserFileSmall90DPI.png"; break;
                        case ViewType::ListView:        name = "djvIconFileBrowserFileList96DPI.png";  break;
                        default: break;
                        }
                        break;
                    }
                    if (!name.empty())
                    {
                        button->setIcon(context->getPath(FileSystem::ResourcePath::IconsDirectory, name));
                    }
                    Style::MetricsRole iconSizeRole = Style::MetricsRole::None;
                    Style::MetricsRole textSizeRole = Style::MetricsRole::None;
                    switch (_p->viewType)
                    {
                    case ViewType::ThumbnailsLarge: textSizeRole = Style::MetricsRole::ThumbnailLarge; break;
                    case ViewType::ThumbnailsSmall: textSizeRole = Style::MetricsRole::ThumbnailSmall; break;
                    case ViewType::ListView:        iconSizeRole = Style::MetricsRole::Icon;           break;
                    default: break;
                    }
                    button->setIconSizeRole(iconSizeRole);
                    button->setTextSizeRole(textSizeRole);
                    switch (_p->viewType)
                    {
                    case ViewType::ThumbnailsLarge:
                    case ViewType::ThumbnailsSmall: _p->iconsLayout->addWidget(button); break;
                    case ViewType::ListView:        _p->listLayout->addWidget(button);  break;
                    default: break;
                    }
                    _p->buttonToFileInfo[button] = fileInfo;
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

            std::string Widget::Private::_getTooltip(const FileSystem::FileInfo & fileInfo)
            {
                std::stringstream ss;
                ss << fileInfo.getFileName(Frame::Invalid, false) << '\n';
                ss << '\n';
                ss << DJV_TEXT("Type") << ": " << fileInfo.getType() << '\n';
                ss << DJV_TEXT("Size") << ": " << Memory::getSizeLabel(fileInfo.getSize()) << '\n';
                ss << DJV_TEXT("Modification time") << ": " << Time::getLabel(fileInfo.getTime());
                return ss.str();
            }

            std::string Widget::Private::_getTooltip(const FileSystem::FileInfo & fileInfo, const AV::IO::Info & avInfo)
            {
                std::stringstream ss;
                ss << _getTooltip(fileInfo);
                size_t track = 0;
                for (const auto & videoInfo : avInfo.video)
                {
                    ss << '\n' << '\n';
                    ss << DJV_TEXT("Video track") << " #" << track << '\n';
                    ss << DJV_TEXT("Size") << ": " << videoInfo.info.size << '\n';
                    ss << DJV_TEXT("Type") << ": " << videoInfo.info.type << '\n';
                    ss.precision(2);
                    ss << DJV_TEXT("Speed") << ": " << Time::Speed::speedToFloat(videoInfo.speed) << DJV_TEXT("FPS") << '\n';
                    ss << DJV_TEXT("Duration") << ": " << Time::getLabel(Time::durationToSeconds(videoInfo.duration));
                    ++track;
                }
                track = 0;
                for (const auto & audioInfo : avInfo.audio)
                {
                    ss << '\n' << '\n';
                    ss << DJV_TEXT("Audio track") << " #" << track << '\n';
                    ss << DJV_TEXT("Channels") << ": " << audioInfo.info.channelCount << '\n';
                    ss << DJV_TEXT("Type") << ": " << audioInfo.info.type << '\n';
                    ss << DJV_TEXT("Sample rate") << ": " << audioInfo.info.sampleRate / 1000.f << DJV_TEXT("kHz") << '\n';
                    ss << DJV_TEXT("Duration") << ": " << Time::getLabel(Time::durationToSeconds(audioInfo.duration));
                    ++track;
                }
                return ss.str();
            }

            struct DialogSystem::Private
            {
                std::shared_ptr<Widget> widget;
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

            void DialogSystem::show(const std::function<void(const Core::FileSystem::FileInfo &)> & callback)
            {
                auto context = getContext();
                if (!_p->widget)
                {
                    _p->widget = Widget::create(context);
                    _p->widget->setPath(FileSystem::Path("."));
                    _p->widget->setBackgroundRole(Style::ColorRole::Background);

                    auto border = Layout::Border::create(context);
                    border->addWidget(_p->widget);

                    _p->overlay = Layout::Overlay::create(context);
                    _p->overlay->setBackgroundRole(Style::ColorRole::Overlay);
                    _p->overlay->setMargin(Style::MetricsRole::MarginLarge);
                    _p->overlay->addWidget(border);
                }
                if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
                {
                    if (auto window = windowSystem->observeCurrentWindow()->get())
                    {
                        window->addWidget(_p->overlay);
                        _p->overlay->show();

                        auto weak = std::weak_ptr<DialogSystem>(std::dynamic_pointer_cast<DialogSystem>(shared_from_this()));
                        _p->widget->setCallback(
                            [weak, window, callback](const FileSystem::FileInfo & value)
                        {
                            if (auto system = weak.lock())
                            {
                                window->removeWidget(system->_p->overlay);
                                callback(value);
                            }
                        });
                        _p->overlay->setCloseCallback(
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
        DJV_TEXT("ThumbnailsLarge"),
        DJV_TEXT("ThumbnailsSmall"),
        DJV_TEXT("ListView"));

} // namespace djv
