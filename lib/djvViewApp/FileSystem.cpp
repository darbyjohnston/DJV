// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
#include <djvUI/Menu.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/LogSystem.h>
#include <djvCore/RecentFilesModel.h>
#include <djvCore/StringFormat.h>
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
            Private(FileSystem& p) :
                p(p)
            {}

            FileSystem& p;

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

            typedef std::pair<Core::FileSystem::FileInfo, std::string> FileInfoAndNumber;

            std::vector<FileInfoAndNumber> processFileNames(
                const std::vector<std::string>&,
                const OpenOptions&,
                const std::weak_ptr<Context>&);
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
            p.actions["Recent"] = UI::Action::create();
            p.actions["Recent"]->setIcon("djvIconFileRecent");
            p.actions["Reload"] = UI::Action::create();
            p.actions["Close"] = UI::Action::create();
            p.actions["Close"]->setIcon("djvIconFileClose");
            p.actions["CloseAll"] = UI::Action::create();
            p.actions["Next"] = UI::Action::create();
            p.actions["Prev"] = UI::Action::create();
            p.actions["Layers"] = UI::Action::create();
            p.actions["Layers"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["NextLayer"] = UI::Action::create();
            p.actions["PrevLayer"] = UI::Action::create();
            //! \todo Implement me!
            //p.actions["8BitConversion"] = UI::Action::create();
            //p.actions["8BitConversion"]->setButtonType(UI::ButtonType::Toggle);
            //p.actions["8BitConversion"]->setEnabled(false);
            p.actions["Exit"] = UI::Action::create();

            _addShortcut("ViewApp/File/Open", GLFW_KEY_O, UI::Shortcut::getSystemModifier());
            _addShortcut("ViewApp/File/Recent", GLFW_KEY_T, UI::Shortcut::getSystemModifier());
            _addShortcut("ViewApp/File/Reload", GLFW_KEY_R, UI::Shortcut::getSystemModifier());
            _addShortcut("ViewApp/File/Close", GLFW_KEY_E, UI::Shortcut::getSystemModifier());
            _addShortcut("ViewApp/File/CloseAll", GLFW_KEY_E, GLFW_MOD_SHIFT | UI::Shortcut::getSystemModifier());
            _addShortcut("ViewApp/File/Next", GLFW_KEY_PAGE_DOWN);
            _addShortcut("ViewApp/File/Prev", GLFW_KEY_PAGE_UP);
            _addShortcut("ViewApp/File/Layers", GLFW_KEY_L, UI::Shortcut::getSystemModifier());
            _addShortcut("ViewApp/File/NextLayer", GLFW_KEY_EQUAL, UI::Shortcut::getSystemModifier());
            _addShortcut("ViewApp/File/PrevLayer", GLFW_KEY_MINUS, UI::Shortcut::getSystemModifier());
            _addShortcut("ViewApp/File/Exit", GLFW_KEY_Q, UI::Shortcut::getSystemModifier());

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Open"]);
            p.menu->addAction(p.actions["Recent"]);
            p.menu->addAction(p.actions["Reload"]);
            p.menu->addAction(p.actions["Close"]);
            p.menu->addAction(p.actions["CloseAll"]);
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

            p.recentFilesModel = Core::FileSystem::RecentFilesModel::create();

            _actionsUpdate();
            _textUpdate();
            _shortcutsUpdate();

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
                            std::dynamic_pointer_cast<Application>(context)->exit(0);
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
                Time::getTime(Time::TimerValue::Medium),
                [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
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
            _p(new Private(*this))
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

        void FileSystem::open(const Core::FileSystem::FileInfo& fileInfo, const OpenOptions& options)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                {
                    std::stringstream ss;
                    ss << "Open: " << fileInfo;
                    _log(ss.str());
                }

                auto media = Media::create(fileInfo, context);
                media->setThreadCount(p.threadCount);
                if (options.speed)
                {
                    media->setPlaybackSpeed(PlaybackSpeed::Custom);
                    media->setCustomSpeed(*options.speed);
                }
                else
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                    if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                    {
                        media->setPlaybackSpeed(playbackSettings->observePlaybackSpeed()->get());
                        media->setCustomSpeed(playbackSettings->observeCustomSpeed()->get());
                    }
                }
                if (options.inPoint || options.outPoint || options.frame)
                {
                    auto avSystem = context->getSystemT<AV::AVSystem>();
                    const Time::Units timeUnits = avSystem->observeTimeUnits()->get();
                    const auto& speed = media->observeDefaultSpeed()->get();
                    const auto& sequence = media->observeSequence()->get();
                    const Frame::Index start = 0;
                    const Frame::Index end = sequence.getLastIndex();
                    bool inOutEnabled = false;
                    Frame::Index inPoint = start;
                    Frame::Index outPoint = end;
                    Frame::Index frame = start;
                    if (options.inPoint)
                    {
                        inOutEnabled = true;
                        const Frame::Index i = sequence.getIndex(Time::fromString(*options.inPoint, speed, timeUnits));
                        inPoint = i != Frame::invalidIndex ? i : start;
                        frame = inPoint;
                    }
                    if (options.outPoint)
                    {
                        inOutEnabled = true;
                        const Frame::Index i = sequence.getIndex(Time::fromString(*options.outPoint, speed, timeUnits));
                        outPoint = i != Frame::invalidIndex ? i : end;
                    }
                    if (options.frame &&
                        !options.frame->empty() &&
                        !Core::FileSystem::FileInfo::isSequenceWildcard(*options.frame))
                    {
                        const Frame::Index i = sequence.getIndex(Time::fromString(*options.frame, speed, timeUnits));
                        frame = i != Frame::invalid ? Math::clamp(i, inPoint, outPoint) : inPoint;
                    }
                    media->setInOutPoints(AV::IO::InOutPoints(inOutEnabled, inPoint, outPoint));
                    media->setCurrentFrame(frame);
                }
                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                {
                    media->setPlayEveryFrame(playbackSettings->observePlayEveryFrame()->get());
                    media->setPlaybackMode(playbackSettings->observePlaybackMode()->get());
                    if (playbackSettings->observeStartPlayback()->get())
                    {
                        media->setPlayback(Playback::Forward);
                    }
                }
                p.media->pushBack(media);
                if (options.pos)
                {
                    p.opened2->setIfChanged(std::make_pair(media, *options.pos));
                    // Reset the observer so we don't have an extra shared_ptr holding
                    // onto the media object.
                    p.opened2->setIfChanged(std::make_pair(nullptr, glm::ivec2(0, 0)));
                }
                else
                {
                    p.opened->setIfChanged(media);
                    // Reset the observer so we don't have an extra shared_ptr holding
                    // onto the media object.
                    p.opened->setIfChanged(nullptr);
                }
                setCurrentMedia(media);
                p.recentFilesModel->addFile(fileInfo);
                _cacheUpdate();
            }
        }

        void FileSystem::open(const std::vector<std::string>& fileNames, OpenOptions options)
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : p.processFileNames(fileNames, options, getContext()))
            {
                if (!options.frame)
                {
                    options.frame.reset(new std::string(i.second));
                }
                open(i.first, options);
                if (options.pos && options.spacing)
                {
                    options.pos->x += *options.spacing;
                    options.pos->y += *options.spacing;
                }
            }
        }

        void FileSystem::close(const std::shared_ptr<Media>& media)
        {
            DJV_PRIVATE_PTR();
            size_t index = p.media->indexOf(media);
            if (index != invalidListIndex)
            {
                {
                    std::stringstream ss;
                    ss << "Close: " << media->getFileInfo();
                    _log(ss.str());
                }

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
            p.actions["Reload"]->setEnabled(size > 0);
            p.actions["Close"]->setEnabled(size > 0);
            p.actions["CloseAll"]->setEnabled(size > 0);
            p.actions["Next"]->setEnabled(size > 1);
            p.actions["Prev"]->setEnabled(size > 1);
            p.actions["NextLayer"]->setEnabled(size > 0);
            p.actions["PrevLayer"]->setEnabled(size > 0);
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
                Core::FileSystem::Path path;
                if (auto media = p.currentMedia->get())
                {
                    path = Core::FileSystem::Path(media->getFileInfo().getPath().getDirectoryName());
                }
                else
                {
                    path = p.fileBrowserPath;
                }
                p.fileBrowserDialog->setPath(path);
                auto weak = std::weak_ptr<FileSystem>(std::dynamic_pointer_cast<FileSystem>(shared_from_this()));
                p.fileBrowserDialog->setCallback(
                    [weak](const Core::FileSystem::FileInfo& value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (system->_p->fileBrowserDialog)
                            {
                                system->_p->fileBrowserPath = system->_p->fileBrowserDialog->getPath();
                                system->_p->fileBrowserDialog->close();
                                system->_p->fileBrowserDialog.reset();
                                system->open(value);
                            }
                        }
                    });
                p.fileBrowserDialog->setCloseCallback(
                    [weak]
                    {
                        if (auto system = weak.lock())
                        {
                            if (system->_p->fileBrowserDialog)
                            {
                                system->_p->fileBrowserPath = system->_p->fileBrowserDialog->getPath();
                                system->_p->fileBrowserDialog->close();
                                system->_p->fileBrowserDialog.reset();
                            }
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
                if (p.recentFilesDialog)
                {
                    p.recentFilesDialog->close();
                }
                p.recentFilesDialog = RecentFilesDialog::create(context);
                auto weak = std::weak_ptr<FileSystem>(std::dynamic_pointer_cast<FileSystem>(shared_from_this()));
                p.recentFilesDialog->setCallback(
                    [weak](const Core::FileSystem::FileInfo& value)
                    {
                        if (auto system = weak.lock())
                        {
                            if (system->_p->recentFilesDialog)
                            {
                                system->_p->recentFilesDialog->close();
                                system->_p->recentFilesDialog.reset();
                                system->open(value);
                            }
                        }
                    });
                p.recentFilesDialog->setCloseCallback(
                    [weak]
                    {
                        if (auto system = weak.lock())
                        {
                            if (system->_p->recentFilesDialog)
                            {
                                system->_p->recentFilesDialog->close();
                                system->_p->recentFilesDialog.reset();
                            }
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
                p.actions["Open"]->setText(_getText(DJV_TEXT("menu_file_open")));
                p.actions["Open"]->setTooltip(_getText(DJV_TEXT("menu_file_open_tooltip")));
                p.actions["Recent"]->setText(_getText(DJV_TEXT("menu_file_recent")));
                p.actions["Recent"]->setTooltip(_getText(DJV_TEXT("menu_file_recent_tooltip")));
                p.actions["Reload"]->setText(_getText(DJV_TEXT("menu_file_reload")));
                p.actions["Reload"]->setTooltip(_getText(DJV_TEXT("menu_file_reload_tooltip")));
                p.actions["Close"]->setText(_getText(DJV_TEXT("menu_file_close")));
                p.actions["Close"]->setTooltip(_getText(DJV_TEXT("menu_file_close_tooltip")));
                p.actions["CloseAll"]->setText(_getText(DJV_TEXT("menu_file_close_all")));
                p.actions["CloseAll"]->setTooltip(_getText(DJV_TEXT("menu_file_close_all_tooltip")));
                p.actions["Next"]->setText(_getText(DJV_TEXT("menu_file_next")));
                p.actions["Next"]->setTooltip(_getText(DJV_TEXT("menu_file_next_tooltip")));
                p.actions["Prev"]->setText(_getText(DJV_TEXT("menu_file_previous")));
                p.actions["Prev"]->setTooltip(_getText(DJV_TEXT("menu_file_prev_tooltip")));
                p.actions["Layers"]->setText(_getText(DJV_TEXT("menu_file_layers")));
                p.actions["Layers"]->setTooltip(_getText(DJV_TEXT("menu_file_layers_tooltip")));
                p.actions["NextLayer"]->setText(_getText(DJV_TEXT("menu_file_next_layer")));
                p.actions["NextLayer"]->setTooltip(_getText(DJV_TEXT("menu_file_next_layer_tooltip")));
                p.actions["PrevLayer"]->setText(_getText(DJV_TEXT("menu_file_previous_layer")));
                p.actions["PrevLayer"]->setTooltip(_getText(DJV_TEXT("menu_file_previous_layer_tooltip")));
                //p.actions["8BitConversion"]->setText(_getText(DJV_TEXT("8-bit_conversion")));
                //p.actions["8BitConversion"]->setTooltip(_getText(DJV_TEXT("8-bit_conversion_tooltip")));
                p.actions["Exit"]->setText(_getText(DJV_TEXT("menu_file_exit")));
                p.actions["Exit"]->setTooltip(_getText(DJV_TEXT("menu_file_exit_tooltip")));

                p.menu->setText(_getText(DJV_TEXT("menu_file")));
            }
        }

        void FileSystem::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["Open"]->setShortcuts(_getShortcuts("ViewApp/File/Open"));
                p.actions["Recent"]->setShortcuts(_getShortcuts("ViewApp/File/Recent"));
                p.actions["Reload"]->setShortcuts(_getShortcuts("ViewApp/File/Reload"));
                p.actions["Close"]->setShortcuts(_getShortcuts("ViewApp/File/Close"));
                p.actions["CloseAll"]->setShortcuts(_getShortcuts("ViewApp/File/CloseAll"));
                p.actions["Next"]->setShortcuts(_getShortcuts("ViewApp/File/Next"));
                p.actions["Prev"]->setShortcuts(_getShortcuts("ViewApp/File/Prev"));
                p.actions["Layers"]->setShortcuts(_getShortcuts("ViewApp/File/Layers"));
                p.actions["NextLayer"]->setShortcuts(_getShortcuts("ViewApp/File/NextLayer"));
                p.actions["PrevLayer"]->setShortcuts(_getShortcuts("ViewApp/File/PrevLayer"));
                p.actions["Exit"]->setShortcuts(_getShortcuts("ViewApp/File/Exit"));
            }
        }

        std::vector<FileSystem::Private::FileInfoAndNumber> FileSystem::Private::processFileNames(
            const std::vector<std::string>& fileNames,
            const OpenOptions& options,
            const std::weak_ptr<Context>& contextWeak)
        {
            {
                std::stringstream ss;
                ss << "Process file names: " << String::join(fileNames, ", ");
                p._log(ss.str());
            }

            std::vector<FileInfoAndNumber> out;
            if (auto context = contextWeak.lock())
            {
                // Get absolute paths.
                for (const auto& i : fileNames)
                {
                    auto path = Core::FileSystem::Path(i);
                    try
                    {
                        out.push_back(std::make_pair(
                            Core::FileSystem::FileInfo(Core::FileSystem::Path::getAbsolute(path)),
                            std::string()));
                    }
                    catch (const std::exception& e)
                    {
                        p._log(String::Format("{0}: {1}").arg(path.get()).arg(e.what()), LogLevel::Error);
                    }
                }

                // Find arguments that belong to the same sequence (for
                // example when a shell wildcard is used).
                auto io = context->getSystemT<AV::IO::System>();
                auto i = out.begin();
                while (i != out.end())
                {
                    if (io->canSequence(i->first))
                    {
                        auto j = i + 1;
                        while (j != out.end())
                        {
                            if (i->first.isCompatible(j->first) &&
                                i->first.addToSequence(j->first))
                            {
                                j = out.erase(j);
                            }
                            else
                            {
                                ++j;
                            }
                        }
                    }
                    ++i;
                }
                
                // Find sequences.
                if (settings->observeAutoDetectSequences()->get())
                {
                    const bool firstFrame = settings->observeSequencesFirstFrame()->get();
                    for (auto& i : out)
                    {
                        if (Core::FileSystem::FileType::File == i.first.getType())
                        {
                            const auto fileInfo = Core::FileSystem::FileInfo::getFileSequence(
                                i.first.getPath(),
                                io->getSequenceExtensions());
                            if (fileInfo.getSequence().getFrameCount() > 1)
                            {
                                if (!firstFrame)
                                {
                                    i.second = i.first.getPath().getNumber();
                                }
                                i.first = fileInfo;
                            }
                        }
                    }
                }
            
                // Check the directory for wildcards.
                for (auto& i : out)
                {
                    if (Core::FileSystem::FileType::File == i.first.getType())
                    {
                        const std::string& number = i.first.getPath().getNumber();
                        const bool wildcard = Core::FileSystem::FileInfo::isSequenceWildcard(number);
                        Core::FileSystem::FileInfo fileSequence;
                        if (wildcard)
                        {
                            fileSequence = Core::FileSystem::FileInfo::getFileSequence(i.first.getPath(), io->getSequenceExtensions());
                        }
                        if (wildcard &&
                            (number.size() == 1 || number.size() == fileSequence.getSequence().getPad()))
                        {
                            if (options.startEnd)
                            {
                                fileSequence.setSequence(Core::Frame::Sequence(*options.startEnd, fileSequence.getSequence().getPad()));
                            }
                            i.first = fileSequence;
                        }
                    }
                }
                
                // Check for missing file extensions (e.g., Maya movie playblasts).
                for (auto& i : out)
                {
                    const std::set<std::string> extensions = io->getNonSequenceExtensions();
                    if (!i.first.doesExist())
                    {
                        for (const auto& j : extensions)
                        {
                            Core::FileSystem::FileInfo fileInfo(i.first.getFileName() + j);
                            if (fileInfo.doesExist())
                            {
                                i.first = fileInfo;
                                break;
                            }
                        }
                    }
                }
                
                // Limit the number of file names.
                const size_t openMax = settings->observeOpenMax()->get();
                if (out.size() > openMax)
                {
                    while (out.size() > openMax)
                    {
                        out.pop_back();
                    }
                    auto logSystem = context->getSystemT<LogSystem>();
                    auto textSystem = context->getSystemT<TextSystem>();
                    logSystem->log(
                        "djv::ViewApp::FileSystem",
                        String::Format(textSystem->getText(DJV_TEXT("error_max_files"))).arg(openMax),
                        LogLevel::Error);
                }
            }

            for (const auto& i : out)
            {
                std::stringstream ss;
                ss << "Process file names result: " << i.first << ":" << i.second;
                p._log(ss.str());
            }

            return out;
        }

    } // namespace ViewApp
} // namespace djv

