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

#include <djvViewApp/FileSystem.h>

#include <djvViewApp/Application.h>
#include <djvViewApp/FileSettings.h>
#include <djvViewApp/LayersWidget.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/PlaybackSettings.h>
#include <djvViewApp/RecentFilesDialog.h>

#include <djvUIComponents/FileBrowserDialog.h>
#include <djvUIComponents/IOSettings.h>

#include <djvUI/Action.h>
#include <djvUI/GroupBox.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/RecentFilesModel.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct FileSystem::Private
        {
            std::shared_ptr<FileSettings> settings;
            std::shared_ptr<ValueSubject<std::shared_ptr<Media> > > opened;
            std::shared_ptr<ValueSubject<std::pair<std::shared_ptr<Media>, glm::vec2> > > opened2;
            std::shared_ptr<ValueSubject<std::shared_ptr<Media> > > closed;
            std::shared_ptr<ListSubject<std::shared_ptr<Media> > > media;
            std::shared_ptr<ValueSubject<std::shared_ptr<Media> > > currentMedia;
            std::shared_ptr<ValueSubject<float> > cachePercentage;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::shared_ptr<UI::FileBrowser::Dialog> fileBrowserDialog;
            Core::FileSystem::Path fileBrowserPath = Core::FileSystem::Path(".");
            std::shared_ptr<RecentFilesDialog> recentFilesDialog;
            size_t threadCount = 4;
            std::shared_ptr<Core::FileSystem::RecentFilesModel> recentFilesModel;
            std::shared_ptr<ListObserver<Core::FileSystem::FileInfo> > recentFilesObserver;
            std::shared_ptr<ListObserver<Core::FileSystem::FileInfo> > recentFilesObserver2;
            std::shared_ptr<ValueObserver<size_t> > threadCountObserver;
            std::shared_ptr<ValueObserver<bool> > cacheEnabledObserver;
            std::shared_ptr<ValueObserver<int> > cacheMaxGBObserver;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<Time::Timer> cacheTimer;
        };

        void FileSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::FileSystem", context);

            DJV_PRIVATE_PTR();

            p.settings = FileSettings::create(context);
            _setWidgetGeom(p.settings->getWidgetGeom());

            p.opened = ValueSubject<std::shared_ptr<Media> >::create();
            p.opened2 = ValueSubject<std::pair<std::shared_ptr<Media>, glm::vec2> >::create();
            p.closed = ValueSubject<std::shared_ptr<Media> >::create();
            p.media = ListSubject<std::shared_ptr<Media> >::create();
            p.currentMedia = ValueSubject<std::shared_ptr<Media> >::create();
            p.cachePercentage = ValueSubject<float>::create();

            p.actions["Open"] = UI::Action::create();
            p.actions["Open"]->setIcon("djvIconFileOpen");
            p.actions["Open"]->setShortcut(GLFW_KEY_O, UI::Shortcut::getSystemModifier());
            p.actions["Recent"] = UI::Action::create();
            p.actions["Recent"]->setIcon("djvIconFileRecent");
            p.actions["Recent"]->setShortcut(GLFW_KEY_T, UI::Shortcut::getSystemModifier());
            p.actions["Reload"] = UI::Action::create();
            p.actions["Reload"]->setShortcut(GLFW_KEY_R, UI::Shortcut::getSystemModifier());
            p.actions["Close"] = UI::Action::create();
            p.actions["Close"]->setIcon("djvIconFileClose");
            p.actions["Close"]->setShortcut(GLFW_KEY_E, UI::Shortcut::getSystemModifier());
            p.actions["CloseAll"] = UI::Action::create();
            p.actions["CloseAll"]->setShortcut(GLFW_KEY_E, GLFW_MOD_SHIFT | UI::Shortcut::getSystemModifier());
            //! \todo Implement me!
            //p.actions["Export"] = UI::Action::create();
            //p.actions["Export"]->setShortcut(GLFW_KEY_X, UI::Shortcut::getSystemModifier());
            //p.actions["Export"]->setEnabled(false);
            p.actions["Next"] = UI::Action::create();
            p.actions["Next"]->setShortcut(GLFW_KEY_PAGE_DOWN);
            p.actions["Prev"] = UI::Action::create();
            p.actions["Prev"]->setShortcut(GLFW_KEY_PAGE_UP);
            p.actions["Layers"] = UI::Action::create();
            p.actions["Layers"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Layers"]->setShortcut(GLFW_KEY_L, UI::Shortcut::getSystemModifier());
            p.actions["NextLayer"] = UI::Action::create();
            p.actions["NextLayer"]->setShortcut(GLFW_KEY_EQUAL, UI::Shortcut::getSystemModifier());
            p.actions["PrevLayer"] = UI::Action::create();
            p.actions["PrevLayer"]->setShortcut(GLFW_KEY_MINUS, UI::Shortcut::getSystemModifier());
            //! \todo Implement me!
            //p.actions["8BitConversion"] = UI::Action::create();
            //p.actions["8BitConversion"]->setButtonType(UI::ButtonType::Toggle);
            //p.actions["8BitConversion"]->setEnabled(false);
            p.actions["Exit"] = UI::Action::create();
            p.actions["Exit"]->setShortcut(GLFW_KEY_Q, UI::Shortcut::getSystemModifier());

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Open"]);
            p.menu->addAction(p.actions["Recent"]);
            p.menu->addAction(p.actions["Reload"]);
            p.menu->addAction(p.actions["Close"]);
            p.menu->addAction(p.actions["CloseAll"]);
            //p.menu->addAction(p.actions["Export"]);
            p.menu->addAction(p.actions["Next"]);
            p.menu->addAction(p.actions["Prev"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["NextLayer"]);
            p.menu->addAction(p.actions["PrevLayer"]);
            p.menu->addAction(p.actions["Layers"]);
            p.menu->addSeparator();
            //p.menu->addAction(p.actions["8BitConversion"]);
            //p.menu->addSeparator();
            p.menu->addAction(p.actions["Exit"]);

            p.recentFilesModel = Core::FileSystem::RecentFilesModel::create(context);

            _actionsUpdate();

            auto weak = std::weak_ptr<FileSystem>(std::dynamic_pointer_cast<FileSystem>(shared_from_this()));
            p.recentFilesObserver = ListObserver<Core::FileSystem::FileInfo>::create(
                p.settings->observeRecentFiles(),
                [weak](const std::vector<Core::FileSystem::FileInfo>& value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->recentFilesModel->setFiles(value);
                    }
                });

            p.recentFilesObserver2 = ListObserver<Core::FileSystem::FileInfo>::create(
                p.recentFilesModel->observeFiles(),
                [weak](const std::vector<Core::FileSystem::FileInfo>& value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->settings->setRecentFiles(value);
                    }
                });

            p.actionObservers["Open"] = ValueObserver<bool>::create(
                p.actions["Open"]->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_showFileBrowserDialog();
                    }
                }
            });

            p.actionObservers["Recent"] = ValueObserver<bool>::create(
                p.actions["Recent"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_showRecentFilesDialog();
                        }
                    }
                });

            p.actionObservers["Reload"] = ValueObserver<bool>::create(
                p.actions["Reload"]->observeClicked(),
                [weak](bool value)
                {
                    if (value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (auto media = system->_p->currentMedia->get())
                            {
                                media->reload();
                                system->_cacheUpdate();
                            }
                        }
                    }
                });

            p.actionObservers["Close"] = ValueObserver<bool>::create(
                p.actions["Close"]->observeClicked(),
                [weak](bool value)
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

            p.actionObservers["CloseAll"] = ValueObserver<bool>::create(
                p.actions["CloseAll"]->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        system->closeAll();
                    }
                }
            });

            p.actionObservers["Next"] = ValueObserver<bool>::create(
                p.actions["Next"]->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia->get())
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
                                system->setCurrentMedia(system->_p->media->getItem(index));
                            }
                        }
                    }
                }
            });

            p.actionObservers["Prev"] = ValueObserver<bool>::create(
                p.actions["Prev"]->observeClicked(),
                [weak](bool value)
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
                            system->setCurrentMedia(system->_p->media->getItem(index));
                        }
                    }
                }
            });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.actionObservers["NextLayer"] = ValueObserver<bool>::create(
                p.actions["NextLayer"]->observeClicked(),
                [weak, contextWeak](bool value)
                {
                    if (value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto system = weak.lock())
                            {
                                if (auto media = system->_p->currentMedia->get())
                                {
                                    media->nextLayer();
                                }
                            }
                        }
                    }
                });

            p.actionObservers["PrevLayer"] = ValueObserver<bool>::create(
                p.actions["PrevLayer"]->observeClicked(),
                [weak, contextWeak](bool value)
                {
                    if (value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto system = weak.lock())
                            {
                                if (auto media = system->_p->currentMedia->get())
                                {
                                    media->prevLayer();
                                }
                            }
                        }
                    }
                });

            p.actionObservers["Layers"] = ValueObserver<bool>::create(
                p.actions["Layers"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = weak.lock())
                        {
                            if (value)
                            {
                                system->_openWidget("Layers", LayersWidget::create(context));
                            }
                            else
                            {
                                system->_closeWidget("Layers");
                            }
                        }
                    }
                });

            p.cacheEnabledObserver = ValueObserver<bool>::create(
                p.settings->observeCacheEnabled(),
                [weak](bool value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_cacheUpdate();
                    }
                });

            p.cacheMaxGBObserver = ValueObserver<int>::create(
                p.settings->observeCacheMaxGB(),
                [weak](int value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_cacheUpdate();
                    }
                });

            p.actionObservers["Exit"] = ValueObserver<bool>::create(
                p.actions["Exit"]->observeClicked(),
                [weak, contextWeak](bool value)
                {
                    if (value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            std::dynamic_pointer_cast<Application>(context)->exit();
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto ioSettings = settingsSystem->getSettingsT<UI::Settings::IO>();
            p.threadCountObserver = ValueObserver<size_t>::create(
                ioSettings->observeThreadCount(),
                [weak](size_t value)
                {
                    if (auto system = weak.lock())
                    {
                        system->_p->threadCount = value;
                        const auto& media = system->_p->media->get();
                        for (const auto& i : media)
                        {
                            i->setThreadCount(value);
                        }
                    }
                });

            p.cacheTimer = Time::Timer::create(context);
            p.cacheTimer->setRepeating(true);
            p.cacheTimer->start(
                Time::getMilliseconds(Time::TimerValue::Medium),
                [weak](float)
                {
                    if (auto system = weak.lock())
                    {
                        size_t cacheMaxByteCount  = 0;
                        size_t cacheByteCount = 0;
                        for (const auto& i : system->_p->media->get())
                        {
                            if (i->hasCache())
                            {
                                cacheMaxByteCount  += i->getCacheMaxByteCount();
                                cacheByteCount += i->getCacheByteCount();
                            }
                        }
                        const float percentage = cacheMaxByteCount ?
                            (cacheByteCount / static_cast<float>(cacheMaxByteCount) * 100.F) :
                            0.F;
                        system->_p->cachePercentage->setIfChanged(percentage);
                    }
                });
        }

        FileSystem::FileSystem() :
            _p(new Private)
        {}

        FileSystem::~FileSystem()
        {
            DJV_PRIVATE_PTR();
            _closeWidget("Layers");
            p.settings->setWidgetGeom(_getWidgetGeom());
            if (p.fileBrowserDialog)
            {
                p.fileBrowserDialog->close();
            }
            if (p.recentFilesDialog)
            {
                p.recentFilesDialog->close();
            }
        }

        std::shared_ptr<FileSystem> FileSystem::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<FileSystem>(new FileSystem);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<std::shared_ptr<Media> > > FileSystem::observeOpened() const
        {
            return _p->opened;
        }

        std::shared_ptr<IValueSubject<std::pair<std::shared_ptr<Media>, glm::vec2> > > FileSystem::observeOpened2() const
        {
            return _p->opened2;
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

        std::shared_ptr<IValueSubject<float> > FileSystem::observeCachePercentage() const
        {
            return _p->cachePercentage;
        }

        void FileSystem::open()
        {
            _showFileBrowserDialog();
        }

        void FileSystem::open(const Core::FileSystem::FileInfo& fileInfo)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto media = Media::create(fileInfo, context);
                _mediaInit(media);
                p.media->pushBack(media);
                p.opened->setIfChanged(media);
                // Reset the observer so we don't have an extra shared_ptr holding
                // onto the media object.
                p.opened->setIfChanged(nullptr);
                setCurrentMedia(media);
                p.recentFilesModel->addFile(fileInfo);
                _cacheUpdate();
            }
        }

        void FileSystem::open(const Core::FileSystem::FileInfo& fileInfo, const glm::vec2& pos)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto media = Media::create(fileInfo, context);
                _mediaInit(media);
                p.media->pushBack(media);
                p.opened2->setIfChanged(std::make_pair(media, pos));
                // Reset the observer so we don't have an extra shared_ptr holding
                // onto the media object.
                p.opened2->setIfChanged(std::make_pair(nullptr, glm::ivec2(0, 0)));
                setCurrentMedia(media);
                p.recentFilesModel->addFile(fileInfo);
                _cacheUpdate();
            }
        }

        void FileSystem::open(const std::vector<std::string>& fileNames)
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : _processFileNames(fileNames))
            {
                open(i);
            }
        }
        
        void FileSystem::open(const std::vector<std::string>& fileNames, const glm::vec2& pos, float spacing)
        {
            DJV_PRIVATE_PTR();
            glm::vec2 tmp = pos;
            for (const auto& i : _processFileNames(fileNames))
            {
                open(i, tmp);
                tmp.x += spacing;
                tmp.y += spacing;
            }
        }

        void FileSystem::close(const std::shared_ptr<Media>& media)
        {
            DJV_PRIVATE_PTR();
            size_t index = p.media->indexOf(media);
            if (index != invalidListIndex)
            {
                p.media->removeItem(index);
                p.closed->setIfChanged(media);
                p.closed->setIfChanged(nullptr);
                const size_t size = p.media->getSize();
                std::shared_ptr<Media> current;
                if (size > 0)
                {
                    if (index == size)
                    {
                        --index;
                    }
                    current = p.media->getItem(index);
                }
                setCurrentMedia(current);
                _cacheUpdate();
            }
        }

        void FileSystem::closeAll()
        {
            DJV_PRIVATE_PTR();
            while (p.media->getSize())
            {
                const size_t i = p.media->getSize() - 1;
                auto media = p.media->getItem(i);
                p.media->removeItem(i);
                p.closed->setIfChanged(media);
                p.closed->setIfChanged(nullptr);
            }
            setCurrentMedia(nullptr);
            _cacheUpdate();
        }

        void FileSystem::setCurrentMedia(const std::shared_ptr<Media> & media)
        {
            DJV_PRIVATE_PTR();
            if (p.currentMedia->setIfChanged(media))
            {
                _actionsUpdate();
            }
        }

        std::map<std::string, std::shared_ptr<UI::Action> > FileSystem::getActions() const
        {
            return _p->actions;
        }

        MenuData FileSystem::getMenu() const
        {
            return
            {
                _p->menu,
                "A"
            };
        }

        void FileSystem::_actionsUpdate()
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.media->getSize();
            p.actions["NextLayer"]->setEnabled(size);
            p.actions["PrevLayer"]->setEnabled(size);
            p.actions["Close"]->setEnabled(size);
            p.actions["CloseAll"]->setEnabled(size);
            //p.actions["Export"]->setEnabled(size);
            p.actions["Next"]->setEnabled(size > 1);
            p.actions["Prev"]->setEnabled(size > 1);
        }

        void FileSystem::_cacheUpdate()
        {
            DJV_PRIVATE_PTR();
            size_t cacheCount = 0;
            const auto& media = p.media->get();
            for (const auto& i : media)
            {
                if (i->hasCache())
                {
                    ++cacheCount;
                }
            }
            const bool cacheEnabled = p.settings->observeCacheEnabled()->get();
            const size_t cacheMaxByteCount = p.settings->observeCacheMaxGB()->get() * Memory::gigabyte;
            const size_t mediaCacheSizeByteCount = cacheCount > 0 ? (cacheMaxByteCount / cacheCount) : 0;
            for (const auto& i : media)
            {
                i->setCacheEnabled(cacheEnabled);
                i->setCacheMaxByteCount(mediaCacheSizeByteCount);
            }
        }

        void FileSystem::_mediaInit(const std::shared_ptr<Media>& value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                value->setThreadCount(p.threadCount);
                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                {
                    value->setPlayEveryFrame(playbackSettings->observePlayEveryFrame()->get());
                    value->setPlaybackMode(playbackSettings->observePlaybackMode()->get());
                    if (playbackSettings->observeStartPlayback()->get())
                    {
                        value->setPlayback(Playback::Forward);
                    }
                }
            }
        }

        void FileSystem::_showFileBrowserDialog()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (p.fileBrowserDialog)
                {
                    p.fileBrowserDialog->close();
                }
                p.fileBrowserDialog = UI::FileBrowser::Dialog::create(context);
                auto io = context->getSystemT<AV::IO::System>();
                p.fileBrowserDialog->setFileExtensions(io->getFileExtensions());
                p.fileBrowserDialog->setPath(p.fileBrowserPath);
                auto weak = std::weak_ptr<FileSystem>(std::dynamic_pointer_cast<FileSystem>(shared_from_this()));
                p.fileBrowserDialog->setCallback(
                    [weak](const Core::FileSystem::FileInfo& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->fileBrowserPath = system->_p->fileBrowserDialog->getPath();
                            system->_p->fileBrowserDialog->close();
                            system->_p->fileBrowserDialog.reset();
                            system->open(value);
                        }
                    });
                p.fileBrowserDialog->setCloseCallback(
                    [weak]
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->fileBrowserPath = system->_p->fileBrowserDialog->getPath();
                            system->_p->fileBrowserDialog->close();
                            system->_p->fileBrowserDialog.reset();
                        }
                    });
                p.fileBrowserDialog->show();
            }
        }

        void FileSystem::_showRecentFilesDialog()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (p.fileBrowserDialog)
                {
                    p.fileBrowserDialog->close();
                }
                p.recentFilesDialog = RecentFilesDialog::create(context);
                auto weak = std::weak_ptr<FileSystem>(std::dynamic_pointer_cast<FileSystem>(shared_from_this()));
                p.recentFilesDialog->setCallback(
                    [weak](const Core::FileSystem::FileInfo& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->recentFilesDialog->close();
                            system->_p->recentFilesDialog.reset();
                            system->open(value);
                        }
                    });
                p.recentFilesDialog->setCloseCallback(
                    [weak]
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->recentFilesDialog->close();
                            system->_p->recentFilesDialog.reset();
                        }
                    });
                p.recentFilesDialog->show();
            }
        }

        void FileSystem::_closeWidget(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            const auto i = p.actions.find(value);
            if (i != p.actions.end())
            {
                i->second->setChecked(false);
            }
            IViewSystem::_closeWidget(value);
        }

        void FileSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Open"]->setText(_getText(DJV_TEXT("Open")));
                p.actions["Open"]->setTooltip(_getText(DJV_TEXT("Open tooltip")));
                p.actions["Recent"]->setText(_getText(DJV_TEXT("Recent")));
                p.actions["Recent"]->setTooltip(_getText(DJV_TEXT("Recent tooltip")));
                p.actions["Reload"]->setText(_getText(DJV_TEXT("Reload")));
                p.actions["Reload"]->setTooltip(_getText(DJV_TEXT("Reload tooltip")));
                p.actions["Close"]->setText(_getText(DJV_TEXT("Close")));
                p.actions["Close"]->setTooltip(_getText(DJV_TEXT("Close tooltip")));
                p.actions["CloseAll"]->setText(_getText(DJV_TEXT("Close All")));
                p.actions["CloseAll"]->setTooltip(_getText(DJV_TEXT("Close all tooltip")));
                //p.actions["Export"]->setText(_getText(DJV_TEXT("Export")));
                //p.actions["Export"]->setTooltip(_getText(DJV_TEXT("Export tooltip")));
                p.actions["Next"]->setText(_getText(DJV_TEXT("Next")));
                p.actions["Next"]->setTooltip(_getText(DJV_TEXT("Next tooltip")));
                p.actions["Prev"]->setText(_getText(DJV_TEXT("Previous")));
                p.actions["Prev"]->setTooltip(_getText(DJV_TEXT("Prev tooltip")));
                p.actions["Layers"]->setText(_getText(DJV_TEXT("Layers")));
                p.actions["Layers"]->setTooltip(_getText(DJV_TEXT("Layers tooltip")));
                p.actions["NextLayer"]->setText(_getText(DJV_TEXT("Next layer")));
                p.actions["NextLayer"]->setTooltip(_getText(DJV_TEXT("Next layer tooltip")));
                p.actions["PrevLayer"]->setText(_getText(DJV_TEXT("Previous Layer")));
                p.actions["PrevLayer"]->setTooltip(_getText(DJV_TEXT("Previous layer tooltip")));
                //p.actions["8BitConversion"]->setText(_getText(DJV_TEXT("8-Bit Conversion")));
                //p.actions["8BitConversion"]->setTooltip(_getText(DJV_TEXT("8-bit conversion tooltip")));
                p.actions["Exit"]->setText(_getText(DJV_TEXT("Exit")));
                p.actions["Exit"]->setTooltip(_getText(DJV_TEXT("Exit tooltip")));

                p.menu->setText(_getText(DJV_TEXT("File")));
            }
        }

        std::vector<Core::FileSystem::FileInfo> FileSystem::_processFileNames(std::vector<std::string> fileNames)
        {
            DJV_PRIVATE_PTR();
            std::vector<Core::FileSystem::FileInfo> fileInfos;
            if (auto context = getContext().lock())
            {
                const size_t openMax = p.settings->observeOpenMax()->get();
                bool exceededMax = false;
                if (p.settings->observeAutoDetectSequences()->get())
                {
                    auto io = context->getSystemT<AV::IO::System>();
                    auto i = fileNames.begin();
                    while (i != fileNames.end())
                    {
                        if (fileInfos.size() < openMax)
                        {
                            Core::FileSystem::FileInfo fileInfo(*i);
                            fileInfo.evalSequence();
                            if (fileInfo.isSequenceValid() &&
                                1 == fileInfo.getSequence().getSize() &&
                                io->canSequence(fileInfo))
                            {
                                auto j = i + 1;
                                while (j != fileNames.end())
                                {
                                    if (fileInfo.addToSequence(*j))
                                    {
                                        j = fileNames.erase(j);
                                    }
                                    else
                                    {
                                        ++j;
                                    }
                                }
                                fileInfo = Core::FileSystem::FileInfo::getFileSequence(
                                    Core::FileSystem::Path(*i),
                                    io->getSequenceExtensions());
                            }
                            fileInfos.push_back(fileInfo);
                            ++i;
                        }
                        else
                        {
                            exceededMax = true;
                            break;
                        }
                    }
                }
                else
                {
                    for (const auto& i : fileNames)
                    {
                        if (fileInfos.size() < openMax)
                        {
                            Core::FileSystem::FileInfo fileInfo(i);
                            fileInfo.evalSequence();
                            fileInfos.push_back(fileInfo);
                        }
                        else
                        {
                            exceededMax = true;
                            break;
                        }
                    }
                }
                if (exceededMax)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("Cannot open more than") << " " << openMax << " " << DJV_TEXT("files at once.");
                    _log(ss.str(), LogLevel::Error);
                }
            }
            return fileInfos;
        }

    } // namespace ViewApp
} // namespace djv

