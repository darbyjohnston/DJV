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

#include <djvViewLib/FileSystem.h>

#include <djvViewLib/Application.h>
#include <djvViewLib/FileBrowserWidget.h>
#include <djvViewLib/FileSystemSettings.h>
#include <djvViewLib/Media.h>
#include <djvViewLib/RecentFilesWidget.h>
#include <djvViewLib/SettingsSystem.h>
#include <djvViewLib/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/DialogSystem.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/RecentFilesModel.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct FileSystem::Private
        {
            std::shared_ptr<FileSystemSettings> settings;
            std::shared_ptr<ValueSubject<std::pair<std::shared_ptr<Media>, glm::vec2> > > opened;
            std::shared_ptr<ValueSubject<std::shared_ptr<Media> > > closed;
            std::shared_ptr<ListSubject<std::shared_ptr<Media> > > media;
            std::shared_ptr<ValueSubject<std::shared_ptr<Media> > > currentMedia;
			std::shared_ptr<FileBrowserWidget> fileBrowserWidget;
			std::shared_ptr<Core::FileSystem::RecentFilesModel> recentFilesModel;
            std::shared_ptr<RecentFilesWidget> recentFilesWidget;
			std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            std::shared_ptr<ListObserver<Core::FileSystem::FileInfo> > recentFilesObserver;
            std::shared_ptr<ListObserver<Core::FileSystem::FileInfo> > recentFilesObserver2;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
        };

        void FileSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::FileSystem", context);

            DJV_PRIVATE_PTR();
            p.settings = FileSystemSettings::create(context);
            p.opened = ValueSubject<std::pair<std::shared_ptr<Media>, glm::vec2> >::create();
            p.closed = ValueSubject<std::shared_ptr<Media> >::create();
            p.media = ListSubject<std::shared_ptr<Media> >::create();
            p.currentMedia = ValueSubject<std::shared_ptr<Media> >::create();

			p.fileBrowserWidget = FileBrowserWidget::create(context);

            p.recentFilesModel = Core::FileSystem::RecentFilesModel::create(context);
            p.recentFilesWidget = RecentFilesWidget::create(context);

            p.actions["Open"] = UI::Action::create();
			p.actions["Open"]->setIcon("djvIconFileOpen");
            p.actions["Open"]->setShortcut(GLFW_KEY_O, GLFW_MOD_CONTROL);

            p.actions["Recent"] = UI::Action::create();
			p.actions["Recent"]->setIcon("djvIconFileRecent");
            p.actions["Recent"]->setShortcut(GLFW_KEY_T, GLFW_MOD_CONTROL);

            //! \todo Implement me!
            p.actions["Reload"] = UI::Action::create();
            p.actions["Reload"]->setShortcut(GLFW_KEY_R, GLFW_MOD_CONTROL);
            p.actions["Reload"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["ReloadFrame"] = UI::Action::create();
            p.actions["ReloadFrame"]->setShortcut(GLFW_KEY_R, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT);
            p.actions["ReloadFrame"]->setEnabled(false);

            p.actions["Close"] = UI::Action::create();
			p.actions["Close"]->setIcon("djvIconFileClose");
            p.actions["Close"]->setShortcut(GLFW_KEY_E, GLFW_MOD_CONTROL);

            //! \todo Implement me!
            p.actions["Export"] = UI::Action::create();
            p.actions["Export"]->setShortcut(GLFW_KEY_X, GLFW_MOD_CONTROL);
            p.actions["Export"]->setEnabled(false);

            p.actions["Next"] = UI::Action::create();
            p.actions["Next"]->setShortcut(GLFW_KEY_PAGE_DOWN);

            p.actions["Prev"] = UI::Action::create();
            p.actions["Prev"]->setShortcut(GLFW_KEY_PAGE_UP);

            //! \todo Implement me!
            p.actions["Layers"] = UI::Action::create();
            p.actions["Layers"]->setShortcut(GLFW_KEY_L, GLFW_MOD_CONTROL);
            p.actions["Layers"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["NextLayer"] = UI::Action::create();
            p.actions["NextLayer"]->setShortcut(GLFW_KEY_EQUAL, GLFW_MOD_CONTROL);
            p.actions["NextLayer"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["PrevLayer"] = UI::Action::create();
            p.actions["PrevLayer"]->setShortcut(GLFW_KEY_MINUS, GLFW_MOD_CONTROL);
            p.actions["PrevLayer"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["8BitConversion"] = UI::Action::create();
            p.actions["8BitConversion"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["8BitConversion"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["MemoryCache"] = UI::Action::create();
            p.actions["MemoryCache"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["MemoryCache"]->setEnabled(false);

            //! \todo Implement me!
            p.actions["ClearCache"] = UI::Action::create();
            p.actions["ClearCache"]->setEnabled(false);

            p.actions["Settings"] = UI::Action::create();

            p.actions["Exit"] = UI::Action::create();
            p.actions["Exit"]->setShortcut(GLFW_KEY_Q, GLFW_MOD_CONTROL);

            p.menus["File"] = UI::Menu::create(context);
            p.menus["File"]->addAction(p.actions["Open"]);
            p.menus["File"]->addAction(p.actions["Recent"]);
            p.menus["File"]->addAction(p.actions["Reload"]);
            p.menus["File"]->addAction(p.actions["ReloadFrame"]);
            p.menus["File"]->addAction(p.actions["Close"]);
            p.menus["File"]->addAction(p.actions["Export"]);
            p.menus["File"]->addAction(p.actions["Next"]);
            p.menus["File"]->addAction(p.actions["Prev"]);
            p.menus["File"]->addSeparator();
            p.menus["File"]->addAction(p.actions["Layers"]);
            p.menus["File"]->addAction(p.actions["NextLayer"]);
            p.menus["File"]->addAction(p.actions["PrevLayer"]);
            //! \todo Implement me!
            p.menus["ProxyScale"] = UI::Menu::create(context);
            p.menus["File"]->addMenu(p.menus["ProxyScale"]);
            p.menus["File"]->addSeparator();
            p.menus["File"]->addAction(p.actions["Settings"]);
            p.menus["File"]->addSeparator();
            p.menus["File"]->addAction(p.actions["Exit"]);

            auto weak = std::weak_ptr<FileSystem>(std::dynamic_pointer_cast<FileSystem>(shared_from_this()));
			p.fileBrowserWidget->setCallback(
				[weak](const Core::FileSystem::FileInfo & value)
			{
				if (auto system = weak.lock())
				{
					system->open(value);
				}
			});

			p.recentFilesWidget->setCallback(
				[weak](const Core::FileSystem::FileInfo & value)
			{
				if (auto system = weak.lock())
				{
					system->open(value);
				}
			});

            p.recentFilesObserver = ListObserver<Core::FileSystem::FileInfo>::create(
                p.settings->observeRecentFiles(),
                [weak](const std::vector<Core::FileSystem::FileInfo> & value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->recentFilesModel->setFiles(value);
                }
            });

            p.recentFilesObserver2 = ListObserver<Core::FileSystem::FileInfo>::create(
                p.recentFilesModel->getFiles(),
                [weak](const std::vector<Core::FileSystem::FileInfo> & value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->recentFilesWidget->setRecentFiles(value);
                    system->_p->settings->setRecentFiles(value);
                }
            });

            p.clickedObservers["Open"] = ValueObserver<bool>::create(
                p.actions["Open"]->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
						system->_p->fileBrowserWidget->moveToFront();
						system->_p->fileBrowserWidget->show();
                    }
                }
            });

            p.clickedObservers["Recent"] = ValueObserver<bool>::create(
                p.actions["Recent"]->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
						system->_p->recentFilesWidget->moveToFront();
						system->_p->recentFilesWidget->show();
					}
                }
            });

            p.clickedObservers["Close"] = ValueObserver<bool>::create(
                p.actions["Close"]->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia->get())
                        {
                            system->close(media);
                        }
                    }
                }
            });

            p.clickedObservers["Next"] = ValueObserver<bool>::create(
                p.actions["Next"]->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        const size_t size = system->_p->media->getSize();
                        if (size > 1)
                        {
                            size_t index = system->_p->media->indexOf(system->_p->currentMedia->get());
                            if (index < size - 1)
                            {
                                ++index;
                            }
                            else
                            {
                                index = 0;
                            }
                            system->_p->currentMedia->setIfChanged(system->_p->media->getItem(index));
                        }
                    }
                }
            });

            p.clickedObservers["Prev"] = ValueObserver<bool>::create(
                p.actions["Prev"]->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        const size_t size = system->_p->media->getSize();
                        if (size > 1)
                        {
                            size_t index = system->_p->media->indexOf(system->_p->currentMedia->get());
                            if (index > 0)
                            {
                                --index;
                            }
                            else
                            {
                                index = size - 1;
                            }
                            system->_p->currentMedia->setIfChanged(system->_p->media->getItem(index));
                        }
                    }
                }
            });

            p.clickedObservers["Settings"] = ValueObserver<bool>::create(
                p.actions["Settings"]->observeClicked(),
                [context](bool value)
            {
                if (auto settingsSystem = context->getSystemT<SettingsSystem>().lock())
                {
                    settingsSystem->showSettings();
                }
            });

            p.clickedObservers["Exit"] = ValueObserver<bool>::create(
                p.actions["Exit"]->observeClicked(),
                [weak, context](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto dialogSystem = context->getSystemT<UI::DialogSystem>().lock())
                        {
                            dialogSystem->confirmation(
                                context->getText(DJV_TEXT("djv::ViewLib::FileSystem", "Exit")),
                                context->getText(DJV_TEXT("djv::ViewLib::FileSystem", "Are you sure you want to exit?")),
                                context->getText(DJV_TEXT("djv::ViewLib::FileSystem", "Yes")),
                                context->getText(DJV_TEXT("djv::ViewLib::FileSystem", "No")),
                                [context](bool value)
                            {
                                if (value)
                                {
                                    dynamic_cast<Application *>(context)->exit();
                                }
                            });
                        }
                    }
                }
            });
        }

        FileSystem::FileSystem() :
            _p(new Private)
        {}

        FileSystem::~FileSystem()
        {}

        std::shared_ptr<FileSystem> FileSystem::create(Context * context)
        {
            auto out = std::shared_ptr<FileSystem>(new FileSystem);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<std::pair<std::shared_ptr<Media>, glm::vec2> > > FileSystem::observeOpened() const
        {
            return _p->opened;
        }

        std::shared_ptr<IValueSubject<std::shared_ptr<Media>> > FileSystem::observeClosed() const
        {
            return _p->closed;
        }

        std::shared_ptr<IListSubject<std::shared_ptr<Media> > > FileSystem::observeMedia() const
        {
            return _p->media;
        }

        std::shared_ptr<IValueSubject<std::shared_ptr<Media> > > FileSystem::observeCurrentMedia() const
        {
            return _p->currentMedia;
        }

        void FileSystem::open(const std::string & fileName, const glm::vec2 & pos)
        {
            auto context = getContext();
            if (auto windowSystem = context->getSystemT<WindowSystem>().lock())
            {
                if (WindowMode::SDI == windowSystem->observeWindowMode()->get())
                {
                    if (auto media = _p->currentMedia->get())
                    {
                        const size_t index = _p->media->indexOf(media);
                        if (index != invalidListIndex)
                        {
                            _p->media->removeItem(index);
                            _p->closed->setAlways(media);
                        }
                    }
                }
            }
            auto media = Media::create(fileName, context);
            _p->media->pushBack(media);
            _p->opened->setAlways(std::make_pair(media, pos));
            _p->currentMedia->setAlways(media);
            _p->recentFilesModel->addFile(fileName);
        }

        void FileSystem::close(const std::shared_ptr<Media> & media)
        {
            size_t index = _p->media->indexOf(media);
            if (index != invalidListIndex)
            {
                _p->media->removeItem(index);
                _p->closed->setAlways(media);
                const size_t size = _p->media->getSize();
                std::shared_ptr<Media> current;
                if (size > 0)
                {
                    if (index == size)
                    {
                        --index;
                    }
                    current = _p->media->getItem(index);
                }
                _p->currentMedia->setIfChanged(current);
            }
        }

        void FileSystem::setCurrentMedia(const std::shared_ptr<Media> & media)
        {
            DJV_PRIVATE_PTR();
            if (_p->currentMedia->setIfChanged(media))
            {
                p.actions["Close"]->setEnabled(media ? true : false);
                p.actions["Export"]->setEnabled(media ? true : false);
            }
        }

        const std::shared_ptr<FileSystemSettings> & FileSystem::getSettings() const
        {
            return _p->settings;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > FileSystem::getActions()
        {
            return _p->actions;
        }
        
        NewMenu FileSystem::getMenu()
        {
            DJV_PRIVATE_PTR();
            return { p.menus["File"], "A" };
        }

		std::vector<NewMDIWidget> FileSystem::getMDIWidgets()
		{
			DJV_PRIVATE_PTR();
			return
			{
				NewMDIWidget(p.fileBrowserWidget, "A1"),
				NewMDIWidget(p.recentFilesWidget, "A2")
			};
		}

        void FileSystem::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            p.actions["Open"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Open")));
            p.actions["Open"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Open Tooltip")));
            p.actions["Recent"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Recent")));
            p.actions["Recent"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Recent Tooltip")));
            p.actions["Reload"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Reload")));
            p.actions["Reload"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Reload Tooltip")));
            p.actions["ReloadFrame"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Reload Frame")));
            p.actions["ReloadFrame"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Reload Frame Tooltip")));
            p.actions["Close"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Close")));
            p.actions["Close"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Close Tooltip")));
            p.actions["Export"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Export")));
            p.actions["Export"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Export Tooltip")));
            p.actions["Next"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Next")));
            p.actions["Next"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Next Tooltip")));
            p.actions["Prev"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Prev")));
            p.actions["Prev"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Prev Tooltip")));
            p.actions["Layers"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Layers")));
            p.actions["Layers"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Layers Tooltip")));
            p.actions["NextLayer"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Next Layer")));
            p.actions["NextLayer"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Next Layer Tooltip")));
            p.actions["PrevLayer"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Previous Layer")));
            p.actions["PrevLayer"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Previous Layer Tooltip")));
            p.actions["8BitConversion"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "8-bit Conversion")));
            p.actions["8BitConversion"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "8-bit Conversion Tooltip")));
            p.actions["MemoryCache"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Memory Cache")));
            p.actions["MemoryCache"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Memory Cache Tooltip")));
            p.actions["ClearCache"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Clear Cache")));
            p.actions["ClearCache"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Clear Cache Tooltip")));
            p.actions["Settings"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Settings")));
            p.actions["Settings"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Settings Tooltip")));
            p.actions["Exit"]->setText(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Exit")));
            p.actions["Exit"]->setTooltip(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Exit Tooltip")));

            p.menus["File"]->setMenuName(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "File")));
            p.menus["ProxyScale"]->setMenuName(_getText(DJV_TEXT("djv::ViewLib::FileSystem", "Proxy Scale")));
        }

    } // namespace ViewLib
} // namespace djv

