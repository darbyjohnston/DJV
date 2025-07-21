// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/App.h>

#include <djvApp/Models/AudioModel.h>
#include <djvApp/Models/ColorModel.h>
#include <djvApp/Models/FilesModel.h>
#include <djvApp/Models/RecentFilesModel.h>
#include <djvApp/Models/TimeUnitsModel.h>
#include <djvApp/Models/ToolsModel.h>
#include <djvApp/Models/ViewportModel.h>
#if defined(TLRENDER_BMD)
#include <djvApp/Models/BMDDevicesModel.h>
#endif // TLRENDER_BMD
#include <djvApp/Widgets/SeparateAudioDialog.h>
#include <djvApp/Widgets/Viewport.h>
#include <djvApp/MainWindow.h>
#include <djvApp/SecondaryWindow.h>

#include <tlTimelineUI/ThumbnailSystem.h>

#include <tlTimeline/ColorOptions.h>
#include <tlTimeline/CompareOptions.h>
#include <tlTimeline/Util.h>

#if defined(TLRENDER_BMD)
#include <tlDevice/BMDDevicesModel.h>
#include <tlDevice/BMDOutputDevice.h>
#endif // TLRENDER_BMD

#include <tlIO/Cache.h>
#include <tlIO/System.h>
#if defined(TLRENDER_FFMPEG)
#include <tlIO/FFmpeg.h>
#endif // TLRENDER_FFMPEG
#if defined(TLRENDER_USD)
#include <tlIO/USD.h>
#endif // TLRENDER_USD

#include <tlCore/FileLogSystem.h>

#include <feather-tk/ui/FileBrowser.h>
#include <feather-tk/ui/Settings.h>
#include <feather-tk/core/CmdLine.h>
#include <feather-tk/core/File.h>
#include <feather-tk/core/Format.h>

#include <filesystem>

namespace djv
{
    namespace app
    {
        struct Options
        {
            std::vector<std::string> fileNames;
            std::string audioFileName;
            std::string compareFileName;
            tl::timeline::CompareOptions compareOptions;
            double speed = 0.0;
            tl::timeline::Playback playback = tl::timeline::Playback::Stop;
            tl::timeline::Loop loop = tl::timeline::Loop::Loop;
            OTIO_NS::RationalTime seek = tl::time::invalidTime;
            OTIO_NS::TimeRange inOutRange = tl::time::invalidTimeRange;
            tl::timeline::OCIOOptions ocioOptions;
            tl::timeline::LUTOptions lutOptions;

#if defined(TLRENDER_USD)
            int usdRenderWidth = 1920;
            float usdComplexity = 1.F;
            tl::usd::DrawMode usdDrawMode = tl::usd::DrawMode::ShadedSmooth;
            bool usdEnableLighting = true;
            bool usdSRGB = true;
            size_t usdStageCache = 10;
            size_t usdDiskCache = 0;
#endif // TLRENDER_USD

            std::string logFile;
            bool resetSettings = false;
            std::string settingsFile;
        };

        struct App::Private
        {
            Options options;
            std::shared_ptr<tl::file::FileLogSystem> fileLogSystem;
            std::shared_ptr<feather_tk::Settings> settings;
            std::shared_ptr<SettingsModel> settingsModel;
            std::shared_ptr<TimeUnitsModel> timeUnitsModel;
            std::shared_ptr<FilesModel> filesModel;
            std::vector<std::shared_ptr<FilesModelItem> > files;
            std::vector<std::shared_ptr<FilesModelItem> > activeFiles;
            std::shared_ptr<RecentFilesModel> recentFilesModel;
            std::vector<std::shared_ptr<tl::timeline::Timeline> > timelines;
            std::shared_ptr<feather_tk::ObservableValue<std::shared_ptr<tl::timeline::Player> > > player;
            std::shared_ptr<ColorModel> colorModel;
            std::shared_ptr<ViewportModel> viewportModel;
            std::shared_ptr<AudioModel> audioModel;
            std::shared_ptr<ToolsModel> toolsModel;

            std::shared_ptr<feather_tk::ObservableValue<bool> > secondaryWindowActive;
            std::shared_ptr<MainWindow> mainWindow;
            std::shared_ptr<SecondaryWindow> secondaryWindow;
            std::shared_ptr<SeparateAudioDialog> separateAudioDialog;

            bool bmdDeviceActive = false;
#if defined(TLRENDER_BMD)
            std::shared_ptr<BMDDevicesModel> bmdDevicesModel;
            std::shared_ptr<tl::bmd::OutputDevice> bmdOutputDevice;
            feather_tk::VideoLevels bmdOutputVideoLevels = feather_tk::VideoLevels::LegalRange;
#endif // TLRENDER_BMD

            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::PlayerCacheOptions> > cacheObserver;
            std::shared_ptr<feather_tk::ListObserver<std::shared_ptr<FilesModelItem> > > filesObserver;
            std::shared_ptr<feather_tk::ListObserver<std::shared_ptr<FilesModelItem> > > activeObserver;
            std::shared_ptr<feather_tk::ListObserver<int> > layersObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::CompareTime> > compareTimeObserver;
            std::shared_ptr<feather_tk::ValueObserver<std::pair<feather_tk::V2I, double> > > viewPosZoomObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > viewFramedObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::audio::DeviceID> > audioDeviceObserver;
            std::shared_ptr<feather_tk::ValueObserver<float> > volumeObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > muteObserver;
            std::shared_ptr<feather_tk::ListObserver<bool> > channelMuteObserver;
            std::shared_ptr<feather_tk::ValueObserver<double> > syncOffsetObserver;
            std::shared_ptr<feather_tk::ValueObserver<StyleSettings> > styleSettingsObserver;
            std::shared_ptr<feather_tk::ValueObserver<MiscSettings> > miscSettingsObserver;
#if defined(TLRENDER_BMD)
            std::shared_ptr<feather_tk::ValueObserver<tl::bmd::DevicesModelData> > bmdDevicesObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > bmdActiveObserver;
            std::shared_ptr<feather_tk::ValueObserver<feather_tk::Size2I> > bmdSizeObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::bmd::FrameRate> > bmdFrameRateObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::OCIOOptions> > ocioOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::LUTOptions> > lutOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<feather_tk::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::DisplayOptions> > displayOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::CompareOptions> > compareOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::BackgroundOptions> > bgOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::ForegroundOptions> > fgOptionsObserver;
#endif // TLRENDER_BMD
        };

        void App::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            std::vector<std::string>& argv)
        {
            FEATHER_TK_P();
            const std::string appName = "djv";
            const std::filesystem::path appDocsPath = _appDocsPath();
            p.options.logFile = _getLogFilePath(appName, appDocsPath).u8string();
            p.options.settingsFile = _getSettingsPath(appName, appDocsPath).u8string();
            feather_tk::App::_init(
                context,
                argv,
                appName,
                "Playback application.",
                _getCmdLineArgs(),
                _getCmdLineOptions());
        }

        App::App() :
            _p(new Private)
        {}

        App::~App()
        {}

        std::shared_ptr<App> App::create(
            const std::shared_ptr<feather_tk::Context>& context,
            std::vector<std::string>& argv)
        {
            auto out = std::shared_ptr<App>(new App);
            out->_init(context, argv);
            return out;
        }

        void App::openDialog()
        {
            FEATHER_TK_P();
            auto fileBrowserSystem = _context->getSystem<feather_tk::FileBrowserSystem>();
            fileBrowserSystem->open(
                p.mainWindow,
                [this](const std::filesystem::path& value)
                {
                    open(tl::file::Path(value.u8string()));
                },
                std::filesystem::path(),
                feather_tk::FileBrowserMode::File);
        }

        void App::openSeparateAudioDialog()
        {
            FEATHER_TK_P();
            p.separateAudioDialog = SeparateAudioDialog::create(_context);
            p.separateAudioDialog->open(p.mainWindow);
            p.separateAudioDialog->setCallback(
                [this](const tl::file::Path& value, const tl::file::Path& audio)
                {
                    open(value, audio);
                    _p->separateAudioDialog->close();
                });
            p.separateAudioDialog->setCloseCallback(
                [this]
                {
                    _p->separateAudioDialog.reset();
                });
        }

        void App::open(const tl::file::Path& path, const tl::file::Path& audioPath)
        {
            FEATHER_TK_P();
            tl::file::PathOptions pathOptions;
            pathOptions.maxNumberDigits = p.settingsModel->getImageSequence().maxDigits;
            for (const auto& i : tl::timeline::getPaths(_context, path, pathOptions))
            {
                auto item = std::make_shared<FilesModelItem>();
                item->path = i;
                item->audioPath = audioPath;
                p.filesModel->add(item);
                p.recentFilesModel->addRecent(path.get());
            }
        }

        const std::shared_ptr<feather_tk::Settings>& App::getSettings() const
        {
            return _p->settings;
        }

        const std::shared_ptr<SettingsModel>& App::getSettingsModel() const
        {
            return _p->settingsModel;
        }

        const std::shared_ptr<TimeUnitsModel>& App::getTimeUnitsModel() const
        {
            return _p->timeUnitsModel;
        }

        const std::shared_ptr<FilesModel>& App::getFilesModel() const
        {
            return _p->filesModel;
        }

        const std::shared_ptr<RecentFilesModel>& App::getRecentFilesModel() const
        {
            return _p->recentFilesModel;
        }

        void App::reload()
        {
            FEATHER_TK_P();
            const auto activeFiles = p.activeFiles;
            const auto files = p.files;
            for (const auto& i : activeFiles)
            {
                const auto j = std::find(p.files.begin(), p.files.end(), i);
                if (j != p.files.end())
                {
                    const size_t index = j - p.files.begin();
                    p.files.erase(j);
                    p.timelines.erase(p.timelines.begin() + index);
                }
            }
            p.activeFiles.clear();
            if (!activeFiles.empty())
            {
                if (auto player = p.player->get())
                {
                    activeFiles.front()->currentTime = player->getCurrentTime();
                    activeFiles.front()->inOutRange = player->getInOutRange();
                }
            }

            auto thumbnailSytem = _context->getSystem<tl::timelineui::ThumbnailSystem>();
            thumbnailSytem->getCache()->clear();

            _filesUpdate(files);
            _activeUpdate(activeFiles);
        }

        std::shared_ptr<feather_tk::IObservableValue<std::shared_ptr<tl::timeline::Player> > > App::observePlayer() const
        {
            return _p->player;
        }

        const std::shared_ptr<ColorModel>& App::getColorModel() const
        {
            return _p->colorModel;
        }

        const std::shared_ptr<ViewportModel>& App::getViewportModel() const
        {
            return _p->viewportModel;
        }

        const std::shared_ptr<AudioModel>& App::getAudioModel() const
        {
            return _p->audioModel;
        }

        const std::shared_ptr<ToolsModel>& App::getToolsModel() const
        {
            return _p->toolsModel;
        }

        const std::shared_ptr<MainWindow>& App::getMainWindow() const
        {
            return _p->mainWindow;
        }

        std::shared_ptr<feather_tk::IObservableValue<bool> > App::observeSecondaryWindow() const
        {
            return _p->secondaryWindowActive;
        }

        void App::setSecondaryWindow(bool value)
        {
            FEATHER_TK_P();
            if (p.secondaryWindowActive->setIfChanged(value))
            {
                if (p.secondaryWindow)
                {
                    removeWindow(p.secondaryWindow);
                    p.secondaryWindow.reset();
                }

                if (value)
                {
                    //! \bug macOS and Windows do not seem to like having an
                    //! application with normal and fullscreen windows?
                    int secondaryScreen = -1;
#if defined(__APPLE__)
#elif defined(_WINDOWS)
#else
                    std::vector<int> screens;
                    for (int i = 0; i < getScreenCount(); ++i)
                    {
                        screens.push_back(i);
                    }
                    auto i = std::find(
                        screens.begin(),
                        screens.end(),
                        p.mainWindow->getScreen());
                    if (i != screens.end())
                    {
                        screens.erase(i);
                    }
                    if (!screens.empty())
                    {
                        secondaryScreen = screens.front();
                    }
#endif // __APPLE__

                    p.secondaryWindow = SecondaryWindow::create(
                        _context,
                        std::dynamic_pointer_cast<App>(shared_from_this()));
                    p.secondaryWindow->setCloseCallback(
                        [this]
                        {
                            FEATHER_TK_P();
                            p.secondaryWindowActive->setIfChanged(false);
                            removeWindow(p.secondaryWindow);
                            p.secondaryWindow.reset();
                        });
                    addWindow(p.secondaryWindow);
                    if (secondaryScreen != -1)
                    {
                        p.secondaryWindow->setFullScreen(true, secondaryScreen);
                    }
                    p.secondaryWindow->show();
                }
            }
        }

#if defined(TLRENDER_BMD)
        const std::shared_ptr<BMDDevicesModel>& App::getBMDDevicesModel() const
        {
            return _p->bmdDevicesModel;
        }

        const std::shared_ptr<tl::bmd::OutputDevice>& App::getBMDOutputDevice() const
        {
            return _p->bmdOutputDevice;
        }
#endif // TLRENDER_BMD

        void App::run()
        {
            FEATHER_TK_P();

            p.fileLogSystem = tl::file::FileLogSystem::create(
                _context,
                std::filesystem::u8path(p.options.logFile));

            p.settings = feather_tk::Settings::create(
                _context,
                std::filesystem::u8path(p.options.settingsFile),
                p.options.resetSettings);

            _modelsInit();
            _devicesInit();
            _observersInit();
            _inputFilesInit();
            _windowsInit();

            feather_tk::App::run();
        }

        void App::_tick()
        {
            FEATHER_TK_P();
            if (auto player = p.player->get())
            {
                player->tick();
            }
#if defined(TLRENDER_BMD)
            if (p.bmdOutputDevice)
            {
                p.bmdOutputDevice->tick();
            }
#endif // TLRENDER_BMD
        }

        void App::_modelsInit()
        {
            FEATHER_TK_P();

            p.settingsModel = SettingsModel::create(_context, p.settings);

            p.timeUnitsModel = TimeUnitsModel::create(_context, p.settings);
            
            p.filesModel = FilesModel::create(p.settings);

            p.recentFilesModel = RecentFilesModel::create(_context, p.settings);
            auto fileBrowserSystem = _context->getSystem<feather_tk::FileBrowserSystem>();
            fileBrowserSystem->getModel()->setExtensions(tl::timeline::getExtensions(_context));
            fileBrowserSystem->setRecentFilesModel(p.recentFilesModel);

            p.colorModel = ColorModel::create(_context);
            p.colorModel->setOCIOOptions(p.options.ocioOptions);
            p.colorModel->setLUTOptions(p.options.lutOptions);

            p.viewportModel = ViewportModel::create(_context, p.settings);

            p.audioModel = AudioModel::create(_context, p.settings);

            p.toolsModel = ToolsModel::create(p.settings);
        }

        void App::_devicesInit()
        {
            FEATHER_TK_P();
#if defined(TLRENDER_BMD)
            p.bmdOutputDevice = tl::bmd::OutputDevice::create(_context);
            p.bmdDevicesModel = BMDDevicesModel::create(_context, p.settings);
#endif // TLRENDER_BMD
        }

        void App::_observersInit()
        {
            FEATHER_TK_P();

            p.player = feather_tk::ObservableValue<std::shared_ptr<tl::timeline::Player> >::create();

            p.cacheObserver = feather_tk::ValueObserver<tl::timeline::PlayerCacheOptions>::create(
                p.settingsModel->observeCache(),
                [this](const tl::timeline::PlayerCacheOptions& value)
                {
                    if (auto player = _p->player->get())
                    {
                        player->setCacheOptions(value);
                    }
                });

            p.filesObserver = feather_tk::ListObserver<std::shared_ptr<FilesModelItem> >::create(
                p.filesModel->observeFiles(),
                [this](const std::vector<std::shared_ptr<FilesModelItem> >& value)
                {
                    _filesUpdate(value);
                });
            p.activeObserver = feather_tk::ListObserver<std::shared_ptr<FilesModelItem> >::create(
                p.filesModel->observeActive(),
                [this](const std::vector<std::shared_ptr<FilesModelItem> >& value)
                {
                    _activeUpdate(value);
                });
            p.layersObserver = feather_tk::ListObserver<int>::create(
                p.filesModel->observeLayers(),
                [this](const std::vector<int>& value)
                {
                    _layersUpdate(value);
                });
            p.compareTimeObserver = feather_tk::ValueObserver<tl::timeline::CompareTime>::create(
                p.filesModel->observeCompareTime(),
                [this](tl::timeline::CompareTime value)
                {
                    if (auto player = _p->player->get())
                    {
                        player->setCompareTime(value);
                    }
                });

            p.audioDeviceObserver = feather_tk::ValueObserver<tl::audio::DeviceID>::create(
                p.audioModel->observeDevice(),
                [this](const tl::audio::DeviceID& value)
                {
                    if (auto player = _p->player->get())
                    {
                        player->setAudioDevice(value);
                    }
                });
            p.volumeObserver = feather_tk::ValueObserver<float>::create(
                p.audioModel->observeVolume(),
                [this](float)
                {
                    _audioUpdate();
                });
            p.muteObserver = feather_tk::ValueObserver<bool>::create(
                p.audioModel->observeMute(),
                [this](bool)
                {
                    _audioUpdate();
                });
            p.channelMuteObserver = feather_tk::ListObserver<bool>::create(
                p.audioModel->observeChannelMute(),
                [this](const std::vector<bool>&)
                {
                    _audioUpdate();
                });
            p.syncOffsetObserver = feather_tk::ValueObserver<double>::create(
                p.audioModel->observeSyncOffset(),
                [this](double)
                {
                    _audioUpdate();
                });

            p.styleSettingsObserver = feather_tk::ValueObserver<StyleSettings>::create(
                p.settingsModel->observeStyle(),
                [this](const StyleSettings& value)
                {
                    getStyle()->setColorControls(value.colorControls);
                    setColorStyle(value.colorStyle);
                    setCustomColorRoles(value.customColorRoles);
                    setDisplayScale(value.displayScale);
                });

            p.miscSettingsObserver = feather_tk::ValueObserver<MiscSettings>::create(
                p.settingsModel->observeMisc(),
                [this](const MiscSettings& value)
                {
                    setTooltipsEnabled(value.tooltipsEnabled);
                });

#if defined(TLRENDER_BMD)
            p.bmdDevicesObserver = feather_tk::ValueObserver<tl::bmd::DevicesModelData>::create(
                p.bmdDevicesModel->observeData(),
                [this](const tl::bmd::DevicesModelData& value)
                {
                    FEATHER_TK_P();
                    tl::bmd::DeviceConfig config;
                    config.deviceIndex = value.deviceIndex - 1;
                    config.displayModeIndex = value.displayModeIndex - 1;
                    config.pixelType = value.pixelTypeIndex >= 0 &&
                        value.pixelTypeIndex < value.pixelTypes.size() ?
                        value.pixelTypes[value.pixelTypeIndex] :
                        tl::bmd::PixelType::None;
                    config.boolOptions = value.boolOptions;
                    p.bmdOutputDevice->setConfig(config);
                    p.bmdOutputDevice->setEnabled(value.deviceEnabled);
                    p.bmdOutputVideoLevels = value.videoLevels;
                    tl::timeline::DisplayOptions displayOptions = p.viewportModel->getDisplayOptions();
                    displayOptions.videoLevels = p.bmdOutputVideoLevels;
                    p.bmdOutputDevice->setDisplayOptions({ displayOptions });
                    p.bmdOutputDevice->setHDR(value.hdrMode, value.hdrData);
                });
            p.bmdActiveObserver = feather_tk::ValueObserver<bool>::create(
                p.bmdOutputDevice->observeActive(),
                [this](bool value)
                {
                    _p->bmdDeviceActive = value;
                    _audioUpdate();
                });
            p.bmdSizeObserver = feather_tk::ValueObserver<feather_tk::Size2I>::create(
                p.bmdOutputDevice->observeSize(),
                [this](const feather_tk::Size2I& value)
                {
                    //std::cout << "output device size: " << value << std::endl;
                });
            p.bmdFrameRateObserver = feather_tk::ValueObserver<tl::bmd::FrameRate>::create(
                p.bmdOutputDevice->observeFrameRate(),
                [this](const tl::bmd::FrameRate& value)
                {
                    //std::cout << "output device frame rate: " <<
                    //    value.num << "/" <<
                    //    value.den <<
                    //    std::endl;
                });

            p.ocioOptionsObserver = feather_tk::ValueObserver<tl::timeline::OCIOOptions>::create(
                p.colorModel->observeOCIOOptions(),
                [this](const tl::timeline::OCIOOptions& value)
                {
                    _p->bmdOutputDevice->setOCIOOptions(value);
                });
            p.lutOptionsObserver = feather_tk::ValueObserver<tl::timeline::LUTOptions>::create(
                p.colorModel->observeLUTOptions(),
                [this](const tl::timeline::LUTOptions& value)
                {
                    _p->bmdOutputDevice->setLUTOptions(value);
                });
            p.imageOptionsObserver = feather_tk::ValueObserver<feather_tk::ImageOptions>::create(
                p.viewportModel->observeImageOptions(),
                [this](const feather_tk::ImageOptions& value)
                {
                    _p->bmdOutputDevice->setImageOptions({ value });
                });
            p.displayOptionsObserver = feather_tk::ValueObserver<tl::timeline::DisplayOptions>::create(
                p.viewportModel->observeDisplayOptions(),
                [this](const tl::timeline::DisplayOptions& value)
                {
                    tl::timeline::DisplayOptions tmp = value;
                    tmp.videoLevels = _p->bmdOutputVideoLevels;
                    _p->bmdOutputDevice->setDisplayOptions({ tmp });
                });

            p.compareOptionsObserver = feather_tk::ValueObserver<tl::timeline::CompareOptions>::create(
                p.filesModel->observeCompareOptions(),
                [this](const tl::timeline::CompareOptions& value)
                {
                    _p->bmdOutputDevice->setCompareOptions(value);
                });

            p.bgOptionsObserver = feather_tk::ValueObserver<tl::timeline::BackgroundOptions>::create(
                p.viewportModel->observeBackgroundOptions(),
                [this](const tl::timeline::BackgroundOptions& value)
                {
                    _p->bmdOutputDevice->setBackgroundOptions(value);
                });

            p.fgOptionsObserver = feather_tk::ValueObserver<tl::timeline::ForegroundOptions>::create(
                p.viewportModel->observeForegroundOptions(),
                [this](const tl::timeline::ForegroundOptions& value)
                {
                    _p->bmdOutputDevice->setForegroundOptions(value);
                });
#endif // TLRENDER_BMD
        }

        void App::_inputFilesInit()
        {
            FEATHER_TK_P();
            if (!p.options.fileNames.empty())
            {
                if (!p.options.compareFileName.empty())
                {
                    open(tl::file::Path(p.options.compareFileName));
                    p.filesModel->setCompareOptions(p.options.compareOptions);
                    p.filesModel->setB(0, true);
                }

                for (const auto& fileName : p.options.fileNames)
                {
                    open(
                        tl::file::Path(fileName),
                        tl::file::Path(p.options.audioFileName));

                    if (auto player = p.player->get())
                    {
                        if (p.options.speed > 0.0)
                        {
                            player->setSpeed(p.options.speed);
                        }
                        if (tl::time::isValid(p.options.inOutRange))
                        {
                            player->setInOutRange(p.options.inOutRange);
                            player->seek(p.options.inOutRange.start_time());
                        }
                        if (tl::time::isValid(p.options.seek))
                        {
                            player->seek(p.options.seek);
                        }
                        player->setLoop(p.options.loop);
                        player->setPlayback(p.options.playback);
                    }
                }
            }
        }

        void App::_windowsInit()
        {
            FEATHER_TK_P();

            p.secondaryWindowActive = feather_tk::ObservableValue<bool>::create(false);

            p.mainWindow = MainWindow::create(
                _context,
                std::dynamic_pointer_cast<App>(shared_from_this()));
            addWindow(p.mainWindow);
            p.mainWindow->show();

            p.viewPosZoomObserver = feather_tk::ValueObserver<std::pair<feather_tk::V2I, double> >::create(
                p.mainWindow->getViewport()->observeViewPosAndZoom(),
                [this](const std::pair<feather_tk::V2I, double>& value)
                {
                    _viewUpdate(
                        value.first,
                        value.second,
                        _p->mainWindow->getViewport()->hasFrameView());
                });
            p.viewFramedObserver = feather_tk::ValueObserver<bool>::create(
                p.mainWindow->getViewport()->observeFramed(),
                [this](bool value)
                {
                    _viewUpdate(
                        _p->mainWindow->getViewport()->getViewPos(),
                        _p->mainWindow->getViewport()->getViewZoom(),
                        value);
                });
            p.mainWindow->setCloseCallback(
                [this]
                {
                    FEATHER_TK_P();
                    if (p.secondaryWindow)
                    {
                        removeWindow(p.secondaryWindow);
                        p.secondaryWindow.reset();
                    }
                });
        }


        std::filesystem::path App::_appDocsPath()
        {
            const std::filesystem::path documentsPath = feather_tk::getUserPath(feather_tk::UserPath::Documents);
            if (!std::filesystem::exists(documentsPath))
            {
                std::filesystem::create_directory(documentsPath);
            }
            const std::filesystem::path out = documentsPath / "DJV";
            if (!std::filesystem::exists(out))
            {
                std::filesystem::create_directory(out);
            }
            return out;
        }

        std::filesystem::path App::_getLogFilePath(
            const std::string& appName,
            const std::filesystem::path& appDocsPath)
        {
            return appDocsPath / feather_tk::Format("{0}.{1}.log").
                arg(appName).
                arg(DJV_VERSION).
                str();
        }

        std::filesystem::path App::_getSettingsPath(
            const std::string& appName,
            const std::filesystem::path& appDocsPath)
        {
            return appDocsPath / feather_tk::Format("{0}.{1}.json").
                arg(appName).
                arg(DJV_VERSION).
                str();
        }

        std::vector<std::shared_ptr<feather_tk::ICmdLineArg> > App::_getCmdLineArgs()
        {
            FEATHER_TK_P();
            return
            {
                feather_tk::CmdLineListArg<std::string>::create(
                    p.options.fileNames,
                    "inputs",
                    "Timelines, movies, image sequences, or folders.",
                    true)
            };
        }

        std::vector<std::shared_ptr<feather_tk::ICmdLineOption> > App::_getCmdLineOptions()
        {
            FEATHER_TK_P();
            return
            {
                feather_tk::CmdLineValueOption<std::string>::create(
                    p.options.audioFileName,
                    { "-audio", "-a" },
                    "Audio file name."),
                feather_tk::CmdLineValueOption<std::string>::create(
                    p.options.compareFileName,
                    { "-b" },
                    "A/B comparison \"B\" file name."),
                feather_tk::CmdLineValueOption<tl::timeline::Compare>::create(
                    p.options.compareOptions.compare,
                    { "-compare", "-c" },
                    "A/B comparison mode.",
                    feather_tk::Format("{0}").arg(p.options.compareOptions.compare),
                    feather_tk::join(tl::timeline::getCompareLabels(), ", ")),
                feather_tk::CmdLineValueOption<feather_tk::V2F>::create(
                    p.options.compareOptions.wipeCenter,
                    { "-wipeCenter", "-wc" },
                    "A/B comparison wipe center.",
                    feather_tk::Format("{0}").arg(p.options.compareOptions.wipeCenter)),
                feather_tk::CmdLineValueOption<float>::create(
                    p.options.compareOptions.wipeRotation,
                    { "-wipeRotation", "-wr" },
                    "A/B comparison wipe rotation.",
                    feather_tk::Format("{0}").arg(p.options.compareOptions.wipeRotation)),
                feather_tk::CmdLineValueOption<double>::create(
                    p.options.speed,
                    { "-speed" },
                    "Playback speed."),
                feather_tk::CmdLineValueOption<tl::timeline::Playback>::create(
                    p.options.playback,
                    { "-playback", "-p" },
                    "Playback mode.",
                    feather_tk::Format("{0}").arg(p.options.playback),
                    feather_tk::join(tl::timeline::getPlaybackLabels(), ", ")),
                feather_tk::CmdLineValueOption<tl::timeline::Loop>::create(
                    p.options.loop,
                    { "-loop", "-lp" },
                    "Playback loop mode.",
                    feather_tk::Format("{0}").arg(p.options.loop),
                    feather_tk::join(tl::timeline::getLoopLabels(), ", ")),
                feather_tk::CmdLineValueOption<OTIO_NS::RationalTime>::create(
                    p.options.seek,
                    { "-seek" },
                    "Seek to the given time."),
                feather_tk::CmdLineValueOption<OTIO_NS::TimeRange>::create(
                    p.options.inOutRange,
                    { "-inOutRange" },
                    "Set the in/out points range."),
                feather_tk::CmdLineValueOption<std::string>::create(
                    p.options.ocioOptions.fileName,
                    { "-ocio" },
                    "OpenColorIO configuration file name (e.g., config.ocio)."),
                feather_tk::CmdLineValueOption<std::string>::create(
                    p.options.ocioOptions.input,
                    { "-ocioInput" },
                    "OpenColorIO input name."),
                feather_tk::CmdLineValueOption<std::string>::create(
                    p.options.ocioOptions.display,
                    { "-ocioDisplay" },
                    "OpenColorIO display name."),
                feather_tk::CmdLineValueOption<std::string>::create(
                    p.options.ocioOptions.view,
                    { "-ocioView" },
                    "OpenColorIO view name."),
                feather_tk::CmdLineValueOption<std::string>::create(
                    p.options.ocioOptions.look,
                    { "-ocioLook" },
                    "OpenColorIO look name."),
                feather_tk::CmdLineValueOption<std::string>::create(
                    p.options.lutOptions.fileName,
                    { "-lut" },
                    "LUT file name."),
                feather_tk::CmdLineValueOption<tl::timeline::LUTOrder>::create(
                    p.options.lutOptions.order,
                    { "-lutOrder" },
                    "LUT operation order.",
                    feather_tk::Format("{0}").arg(p.options.lutOptions.order),
                    feather_tk::join(tl::timeline::getLUTOrderLabels(), ", ")),
#if defined(TLRENDER_USD)
                feather_tk::CmdLineValueOption<int>::create(
                    p.options.usdRenderWidth,
                    { "-usdRenderWidth" },
                    "USD render width.",
                    feather_tk::Format("{0}").arg(p.options.usdRenderWidth)),
                feather_tk::CmdLineValueOption<float>::create(
                    p.options.usdComplexity,
                    { "-usdComplexity" },
                    "USD render complexity setting.",
                    feather_tk::Format("{0}").arg(p.options.usdComplexity)),
                feather_tk::CmdLineValueOption<tl::usd::DrawMode>::create(
                    p.options.usdDrawMode,
                    { "-usdDrawMode" },
                    "USD draw mode.",
                    feather_tk::Format("{0}").arg(p.options.usdDrawMode),
                    feather_tk::join(tl::usd::getDrawModeLabels(), ", ")),
                feather_tk::CmdLineValueOption<bool>::create(
                    p.options.usdEnableLighting,
                    { "-usdEnableLighting" },
                    "USD enable lighting.",
                    feather_tk::Format("{0}").arg(p.options.usdEnableLighting)),
                feather_tk::CmdLineValueOption<bool>::create(
                    p.options.usdSRGB,
                    { "-usdSRGB" },
                    "USD enable sRGB color space.",
                    feather_tk::Format("{0}").arg(p.options.usdSRGB)),
                feather_tk::CmdLineValueOption<size_t>::create(
                    p.options.usdStageCache,
                    { "-usdStageCache" },
                    "USD stage cache size.",
                    feather_tk::Format("{0}").arg(p.options.usdStageCache)),
                feather_tk::CmdLineValueOption<size_t>::create(
                    p.options.usdDiskCache,
                    { "-usdDiskCache" },
                    "USD disk cache size in gigabytes. A size of zero disables the disk cache.",
                    feather_tk::Format("{0}").arg(p.options.usdDiskCache)),
#endif // TLRENDER_USD
                feather_tk::CmdLineValueOption<std::string>::create(
                    p.options.logFile,
                    { "-logFile" },
                    "Log file name.",
                    feather_tk::Format("{0}").arg(p.options.logFile)),
                feather_tk::CmdLineFlagOption::create(
                    p.options.resetSettings,
                    { "-resetSettings" },
                    "Reset settings to defaults."),
                feather_tk::CmdLineValueOption<std::string>::create(
                    p.options.settingsFile,
                    { "-settingsFile" },
                    "Settings file name.",
                    feather_tk::Format("{0}").arg(p.options.settingsFile)),
            };
        }

        tl::io::Options App::_getIOOptions() const
        {
            FEATHER_TK_P();
            tl::io::Options out;
            out = tl::io::merge(out, tl::io::getOptions(p.settingsModel->getImageSequence().io));
#if defined(TLRENDER_FFMPEG)
            out = tl::io::merge(out, tl::ffmpeg::getOptions(p.settingsModel->getFFmpeg()));
#endif // TLRENDER_FFMPEG
#if defined(TLRENDER_USD)
            out = tl::io::merge(out, tl::usd::getOptions(p.settingsModel->getUSD()));
#endif // TLRENDER_USD
            return out;
        }

        void App::_filesUpdate(const std::vector<std::shared_ptr<FilesModelItem> >& files)
        {
            FEATHER_TK_P();

            std::vector<std::shared_ptr<tl::timeline::Timeline> > timelines(files.size());
            for (size_t i = 0; i < files.size(); ++i)
            {
                const auto j = std::find(p.files.begin(), p.files.end(), files[i]);
                if (j != p.files.end())
                {
                    timelines[i] = p.timelines[j - p.files.begin()];
                }
            }

            for (size_t i = 0; i < files.size(); ++i)
            {
                if (!timelines[i])
                {
                    try
                    {
                        tl::timeline::Options options;
                        const ImageSequenceSettings imageSequence = p.settingsModel->getImageSequence();
                        options.imageSequenceAudio = imageSequence.audio;
                        options.imageSequenceAudioExtensions = imageSequence.audioExtensions;
                        options.imageSequenceAudioFileName = imageSequence.audioFileName;
                        const AdvancedSettings advanced = p.settingsModel->getAdvanced();
                        options.compat = advanced.compat;
                        options.videoRequestMax = advanced.videoRequestMax;
                        options.audioRequestMax = advanced.audioRequestMax;
                        options.ioOptions = _getIOOptions();
                        options.pathOptions.maxNumberDigits = imageSequence.maxDigits;
                        auto otioTimeline = files[i]->audioPath.isEmpty() ?
                            tl::timeline::create(_context, files[i]->path, options) :
                            tl::timeline::create(_context, files[i]->path, files[i]->audioPath, options);
                        timelines[i] = tl::timeline::Timeline::create(_context, otioTimeline, options);
                        for (const auto& video : timelines[i]->getIOInfo().video)
                        {
                            files[i]->videoLayers.push_back(video.name);
                        }
                    }
                    catch (const std::exception& e)
                    {
                        _context->log("djv::app::App", e.what(), feather_tk::LogType::Error);
                    }
                }
            }

            p.files = files;
            p.timelines = timelines;
        }

        void App::_activeUpdate(const std::vector<std::shared_ptr<FilesModelItem> >& activeFiles)
        {
            FEATHER_TK_P();

            if (!p.activeFiles.empty())
            {
                if (auto player = p.player->get())
                {
                    p.activeFiles.front()->currentTime = player->getCurrentTime();
                    p.activeFiles.front()->inOutRange = player->getInOutRange();
                }
            }

            std::shared_ptr<tl::timeline::Player> player;
            if (!activeFiles.empty())
            {
                if (!p.activeFiles.empty() && activeFiles[0] == p.activeFiles[0])
                {
                    player = p.player->get();
                }
                else
                {
                    if (auto player = p.player->get())
                    {
                        player->setAudioDevice(tl::audio::DeviceID());
                    }
                    auto i = std::find(p.files.begin(), p.files.end(), activeFiles[0]);
                    if (i != p.files.end())
                    {
                        if (auto timeline = p.timelines[i - p.files.begin()])
                        {
                            try
                            {
                                tl::timeline::PlayerOptions playerOptions;
                                playerOptions.audioDevice = p.audioModel->getDevice();
                                playerOptions.cache = p.settingsModel->getCache();
                                const AdvancedSettings advanced = p.settingsModel->getAdvanced();
                                playerOptions.videoRequestMax = advanced.videoRequestMax;
                                playerOptions.audioRequestMax = advanced.audioRequestMax;
                                playerOptions.audioBufferFrameCount = advanced.audioBufferFrameCount;
                                player = tl::timeline::Player::create(_context, timeline, playerOptions);
                            }
                            catch (const std::exception& e)
                            {
                                _context->log("djv::app::App", e.what(), feather_tk::LogType::Error);
                            }
                        }
                    }
                }
            }
            if (player)
            {
                const OTIO_NS::RationalTime currentTime = activeFiles.front()->currentTime;
                const OTIO_NS::TimeRange inOutRange = activeFiles.front()->inOutRange;
                if (!tl::time::compareExact(inOutRange, tl::time::invalidTimeRange))
                {
                    player->setInOutRange(inOutRange);
                }
                if (!currentTime.strictly_equal(tl::time::invalidTime))
                {
                    player->seek(currentTime);
                }
                std::vector<std::shared_ptr<tl::timeline::Timeline> > compare;
                for (size_t i = 1; i < activeFiles.size(); ++i)
                {
                    auto j = std::find(p.files.begin(), p.files.end(), activeFiles[i]);
                    if (j != p.files.end())
                    {
                        auto timeline = p.timelines[j - p.files.begin()];
                        compare.push_back(timeline);
                    }
                }
                player->setCompare(compare);
                player->setCompareTime(p.filesModel->getCompareTime());
            }

            p.activeFiles = activeFiles;
            p.player->setIfChanged(player);
#if defined(TLRENDER_BMD)
            p.bmdOutputDevice->setPlayer(player);
#endif // TLRENDER_BMD

            _layersUpdate(p.filesModel->observeLayers()->get());
            _audioUpdate();
        }

        void App::_layersUpdate(const std::vector<int>& value)
        {
            FEATHER_TK_P();
            if (auto player = p.player->get())
            {
                int videoLayer = 0;
                std::vector<int> compareVideoLayers;
                if (!value.empty() && value.size() == p.files.size() && !p.activeFiles.empty())
                {
                    auto i = std::find(p.files.begin(), p.files.end(), p.activeFiles.front());
                    if (i != p.files.end())
                    {
                        videoLayer = value[i - p.files.begin()];
                    }
                    for (size_t j = 1; j < p.activeFiles.size(); ++j)
                    {
                        i = std::find(p.files.begin(), p.files.end(), p.activeFiles[j]);
                        if (i != p.files.end())
                        {
                            compareVideoLayers.push_back(value[i - p.files.begin()]);
                        }
                    }
                }
                player->setVideoLayer(videoLayer);
                player->setCompareVideoLayers(compareVideoLayers);
            }
        }

        void App::_viewUpdate(const feather_tk::V2I& pos, double zoom, bool frame)
        {
            FEATHER_TK_P();
            const feather_tk::Box2I& g = p.mainWindow->getViewport()->getGeometry();
            float scale = 1.F;
            if (p.secondaryWindow)
            {
                const feather_tk::Size2I& secondarySize = p.secondaryWindow->getViewport()->getGeometry().size();
                if (g.isValid() && secondarySize.isValid())
                {
                    scale = secondarySize.w / static_cast<float>(g.w());
                }
                p.secondaryWindow->setView(pos * scale, zoom * scale, frame);
            }
#if defined(TLRENDER_BMD)
            scale = 1.F;
            const feather_tk::Size2I& bmdSize = p.bmdOutputDevice->getSize();
            if (g.isValid() && bmdSize.isValid())
            {
                scale = bmdSize.w / static_cast<float>(g.w());
            }
            p.bmdOutputDevice->setView(pos * scale, zoom * scale, frame);
#endif // TLRENDER_BMD
        }

        void App::_audioUpdate()
        {
            FEATHER_TK_P();
            const float volume = p.audioModel->getVolume();
            const bool mute = p.audioModel->isMuted();
            const std::vector<bool> channelMute = p.audioModel->getChannelMute();
            const double audioOffset = p.audioModel->getSyncOffset();
            if (auto player = p.player->get())
            {
                player->setVolume(volume);
                player->setMute(mute || p.bmdDeviceActive);
                player->setChannelMute(channelMute);
                player->setAudioOffset(audioOffset);
            }
#if defined(TLRENDER_BMD)
            p.bmdOutputDevice->setVolume(volume);
            p.bmdOutputDevice->setMute(mute);
            p.bmdOutputDevice->setChannelMute(channelMute);
            p.bmdOutputDevice->setAudioOffset(audioOffset);
#endif // TLRENDER_BMD
        }
    }
}
