// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Models/SettingsModel.h>

#include <feather-tk/ui/Settings.h>
#include <feather-tk/core/Error.h>
#include <feather-tk/core/String.h>

#include <sstream>

namespace djv
{
    namespace app
    {
        bool AdvancedSettings::operator == (const AdvancedSettings& other) const
        {
            return
                compat == other.compat &&
                audioBufferFrameCount == other.audioBufferFrameCount &&
                videoRequestMax == other.videoRequestMax &&
                audioRequestMax == other.audioRequestMax;
        }

        bool AdvancedSettings::operator != (const AdvancedSettings& other) const
        {
            return !(*this == other);
        }

        FEATHER_TK_ENUM_IMPL(
            ExportRenderSize,
            "Default",
            "1920x1080",
            "3840x2160",
            "4096x2160",
            "Custom");

        feather_tk::Size2I getSize(ExportRenderSize value)
        {
            const std::array<feather_tk::Size2I, static_cast<size_t>(ExportRenderSize::Count)> data =
            {
                feather_tk::Size2I(),
                feather_tk::Size2I(1920, 1080),
                feather_tk::Size2I(3840, 2160),
                feather_tk::Size2I(4096, 2160),
                feather_tk::Size2I()
            };
            return data[static_cast<size_t>(value)];
        }

        FEATHER_TK_ENUM_IMPL(
            ExportFileType,
            "Image",
            "Sequence",
            "Movie");

        bool ExportSettings::operator == (const ExportSettings& other) const
        {
            return
                directory == other.directory &&
                renderSize == other.renderSize &&
                customSize == other.customSize &&
                fileType == other.fileType &&
                imageBaseName == other.imageBaseName &&
                imageZeroPad == other.imageZeroPad &&
                imageExtension == other.imageExtension &&
                movieBaseName == other.movieBaseName &&
                movieExtension == other.movieExtension &&
                movieCodec == other.movieCodec;
        }

        bool ExportSettings::operator != (const ExportSettings& other) const
        {
            return !(*this == other);
        }

        bool FileBrowserSettings::operator == (const FileBrowserSettings& other) const
        {
            return
                nativeFileDialog == other.nativeFileDialog &&
                path == other.path &&
                options == other.options &&
                extension == other.extension;
        }

        bool FileBrowserSettings::operator != (const FileBrowserSettings& other) const
        {
            return !(*this == other);
        }

        bool ImageSequenceSettings::operator == (const ImageSequenceSettings& other) const
        {
            return
                audio == other.audio &&
                audioExtensions == other.audioExtensions &&
                audioFileName == other.audioFileName &&
                maxDigits == other.maxDigits &&
                io == other.io;
        }

        bool ImageSequenceSettings::operator != (const ImageSequenceSettings& other) const
        {
            return !(*this == other);
        }

        ShortcutsSettings::ShortcutsSettings()
        {
            shortcuts =
            {
                Shortcut("Audio/VolumeUp", "Volume up", feather_tk::Key::Period),
                Shortcut("Audio/VolumeDown", "Volume down", feather_tk::Key::Comma),
                Shortcut("Audio/Mute", "Mute", feather_tk::Key::M),

                Shortcut("Compare/Next", "Next", feather_tk::Key::PageDown, static_cast<int>(feather_tk::KeyModifier::Shift)),
                Shortcut("Compare/Prev", "Previous", feather_tk::Key::PageUp, static_cast<int>(feather_tk::KeyModifier::Shift)),
                Shortcut("Compare/A", "A", feather_tk::Key::A, static_cast<int>(feather_tk::KeyModifier::Control)),
                Shortcut("Compare/B", "B", feather_tk::Key::B, static_cast<int>(feather_tk::KeyModifier::Control)),
                Shortcut("Compare/Wipe", "Wipe", feather_tk::Key::W, static_cast<int>(feather_tk::KeyModifier::Control)),
                Shortcut("Compare/Overlay", "Overlay"),
                Shortcut("Compare/Difference", "Difference"),
                Shortcut("Compare/Horizontal", "Horizontal"),
                Shortcut("Compare/Vertical", "Vertical"),
                Shortcut("Compare/Tile", "Tile", feather_tk::Key::T, static_cast<int>(feather_tk::KeyModifier::Control)),
                Shortcut("Compare/Relative", "Relative"),
                Shortcut("Compare/Absolute", "Absolute"),

                Shortcut("File/Open", "Open", feather_tk::Key::O, static_cast<int>(feather_tk::commandKeyModifier)),
                Shortcut(
                    "File/OpenSeparateAudio",
                    "Open separate audio",
                    feather_tk::Key::O,
                    static_cast<int>(feather_tk::KeyModifier::Shift) | static_cast<int>(feather_tk::commandKeyModifier)),
                Shortcut("File/Close", "Close", feather_tk::Key::E, static_cast<int>(feather_tk::commandKeyModifier)),
                Shortcut(
                    "File/CloseAll",
                    "Close all",
                    feather_tk::Key::E,
                    static_cast<int>(feather_tk::KeyModifier::Shift) | static_cast<int>(feather_tk::commandKeyModifier)),
                Shortcut(
                    "File/Reload",
                    "Reload",
                    feather_tk::Key::R,
                    static_cast<int>(feather_tk::KeyModifier::Shift) | static_cast<int>(feather_tk::commandKeyModifier)),
                Shortcut("File/Next", "Next", feather_tk::Key::PageDown, static_cast<int>(feather_tk::KeyModifier::Control)),
                Shortcut("File/Prev", "Previous", feather_tk::Key::PageUp, static_cast<int>(feather_tk::KeyModifier::Control)),
                Shortcut("File/NextLayer", "Next layer", feather_tk::Key::Equal, static_cast<int>(feather_tk::KeyModifier::Control)),
                Shortcut("File/PrevLayer", "Previous layer", feather_tk::Key::Minus, static_cast<int>(feather_tk::KeyModifier::Control)),
                Shortcut("File/Exit", "Exit", feather_tk::Key::Q, static_cast<int>(feather_tk::commandKeyModifier)),

                Shortcut("Frame/Start", "Start", feather_tk::Key::Home),
                Shortcut("Frame/End", "End", feather_tk::Key::End),
                Shortcut("Frame/Prev", "Previous", feather_tk::Key::Left),
                Shortcut("Frame/PrevX10", "Previous X10", feather_tk::Key::Left, static_cast<int>(feather_tk::KeyModifier::Shift)),
                Shortcut("Frame/PrevX100", "Previous X100", feather_tk::Key::Left, static_cast<int>(feather_tk::KeyModifier::Control)),
                Shortcut("Frame/Next", "Next", feather_tk::Key::Right),
                Shortcut("Frame/NextX10", "Next X10", feather_tk::Key::Right, static_cast<int>(feather_tk::KeyModifier::Shift)),
                Shortcut("Frame/NextX100", "Next X100", feather_tk::Key::Right, static_cast<int>(feather_tk::KeyModifier::Control)),
                Shortcut("Frame/FocusCurrent", "Focus current", feather_tk::Key::F, static_cast<int>(feather_tk::KeyModifier::Control)),

                Shortcut("Playback/Stop", "Stop", feather_tk::Key::K),
                Shortcut("Playback/Forward", "Forward", feather_tk::Key::L),
                Shortcut("Playback/Reverse", "Reverse", feather_tk::Key::J),
                Shortcut("Playback/Toggle", "Toggle", feather_tk::Key::Space),
                Shortcut("Playback/JumpBack1s", "Jump back 1s", feather_tk::Key::J, static_cast<int>(feather_tk::KeyModifier::Shift)),
                Shortcut("Playback/JumpBack10s", "Jump back 10s", feather_tk::Key::J, static_cast<int>(feather_tk::KeyModifier::Control)),
                Shortcut("Playback/JumpForward1s", "Jump forward 1s", feather_tk::Key::L, static_cast<int>(feather_tk::KeyModifier::Shift)),
                Shortcut("Playback/JumpForward10s", "Jump forward 10s", feather_tk::Key::L, static_cast<int>(feather_tk::KeyModifier::Control)),
                Shortcut("Playback/Loop", "Loop"),
                Shortcut("Playback/Once", "Once"),
                Shortcut("Playback/PingPong", "Ping pong"),
                Shortcut("Playback/SetInPoint", "Set in point", feather_tk::Key::I),
                Shortcut("Playback/ResetInPoint", "Reset in point", feather_tk::Key::I, static_cast<int>(feather_tk::KeyModifier::Shift)),
                Shortcut("Playback/SetOutPoint", "Set out point", feather_tk::Key::O),
                Shortcut("Playback/ResetOutPoint", "Reset out point", feather_tk::Key::O, static_cast<int>(feather_tk::KeyModifier::Shift)),

                Shortcut("Timeline/FrameView", "Frame view"),
                Shortcut("Timeline/Scroll", "Scroll"),
                Shortcut("Timeline/StopOnScrub", "Stop on scrub"),
                Shortcut("Timeline/Thumbnails", "Thumbnails"),
                Shortcut("Timeline/ThumbnailsSmall", "Thumbnails small"),
                Shortcut("Timeline/ThumbnailsMedium", "Thumbnails medium"),
                Shortcut("Timeline/ThumbnailsLarge", "Thumbnails large"),

                Shortcut("Tools/Files", "Files", feather_tk::Key::F1),
                Shortcut("Tools/Export", "Export", feather_tk::Key::F2),
                Shortcut("Tools/View", "View", feather_tk::Key::F3),
                Shortcut("Tools/ColorPicker", "Color picker", feather_tk::Key::F4),
                Shortcut("Tools/ColorControls", "Color controls", feather_tk::Key::F5),
                Shortcut("Tools/Info", "Information", feather_tk::Key::F6),
                Shortcut("Tools/Audio", "Audio", feather_tk::Key::F7),
                Shortcut("Tools/Devices", "Devices", feather_tk::Key::F8),
                Shortcut("Tools/Settings", "Settings", feather_tk::Key::F9),
                Shortcut("Tools/Messages", "Messages", feather_tk::Key::F10),
                Shortcut("Tools/SystemLog", "System log", feather_tk::Key::F11),

                Shortcut("View/Frame", "Frame", feather_tk::Key::Backspace),
                Shortcut("View/ZoomReset", "Zoom reset", feather_tk::Key::_0),
                Shortcut("View/ZoomIn", "Zoom in", feather_tk::Key::Equal),
                Shortcut("View/ZoomOut", "Zoom out", feather_tk::Key::Minus),
                Shortcut("View/Red", "Red", feather_tk::Key::R),
                Shortcut("View/Green", "Green", feather_tk::Key::G),
                Shortcut("View/Blue", "Blue", feather_tk::Key::B),
                Shortcut("View/Alpha", "Alpha", feather_tk::Key::A),
                Shortcut("View/MirrorHorizontal", "Mirror horizontal", feather_tk::Key::H),
                Shortcut("View/MirrorVertical", "Mirror vertical", feather_tk::Key::V),
                Shortcut("View/MinifyNearest", "Minify nearest"),
                Shortcut("View/MinifyLinear", "Minify linear"),
                Shortcut("View/MagnifyNearest", "Magnify nearest"),
                Shortcut("View/MagnifyLinear", "Magnify linear"),
                Shortcut("View/FromFile", "From file"),
                Shortcut("View/FullRange", "Full range"),
                Shortcut("View/LegalRange", "Legal range"),
                Shortcut("View/AlphaBlendNone", "Alpha blend none"),
                Shortcut("View/AlphaBlendStraight", "Alpha blend straight"),
                Shortcut("View/AlphaBlendPremultiplied", "Alpha blend premultiplied"),
                Shortcut("View/HUD", "HUD", feather_tk::Key::H, static_cast<int>(feather_tk::KeyModifier::Control)),

                Shortcut("Window/FullScreen", "Full screen", feather_tk::Key::U),
                Shortcut("Window/FloatOnTop", "Float on top"),
                Shortcut("Window/Secondary", "Secondary", feather_tk::Key::Y),
                Shortcut("Window/FileToolBar", "File tool bar"),
                Shortcut("Window/CompareToolBar", "Compare tool bar"),
                Shortcut("Window/WindowToolBar", "Window tool bar"),
                Shortcut("Window/ViewToolBar", "View tool bar"),
                Shortcut("Window/ToolsToolBar", "Tools tool bar"),
                Shortcut("Window/TabBar", "Tab bar"),
                Shortcut("Window/Timeline", "Timeline"),
                Shortcut("Window/BottomToolBar", "Bottom tool bar"),
                Shortcut("Window/StatusToolBar", "Status tool bar")
            };
        }

        bool ShortcutsSettings::operator == (const ShortcutsSettings& other) const
        {
            return shortcuts == other.shortcuts;
        }

        bool ShortcutsSettings::operator != (const ShortcutsSettings& other) const
        {
            return !(*this == other);
        }

        bool MiscSettings::operator == (const MiscSettings& other) const
        {
            return
                tooltipsEnabled == other.tooltipsEnabled &&
                showSetup == other.showSetup;
        }

        bool MiscSettings::operator != (const MiscSettings& other) const
        {
            return !(*this == other);
        }

        FEATHER_TK_ENUM_IMPL(
            MouseAction,
            "PanView",
            "CompareWipe",
            "ColorPicker",
            "FrameShuttle");

        bool MouseSettings::operator == (const MouseSettings& other) const
        {
            return actions == other.actions;
        }

        bool MouseSettings::operator != (const MouseSettings& other) const
        {
            return !(*this == other);
        }

        bool StyleSettings::operator == (const StyleSettings& other) const
        {
            return
                displayScale == other.displayScale &&
                colorControls == other.colorControls &&
                colorStyle == other.colorStyle &&
                customColorRoles == other.customColorRoles;
        }

        bool StyleSettings::operator != (const StyleSettings& other) const
        {
            return !(*this == other);
        }

        bool TimelineSettings::operator == (const TimelineSettings& other) const
        {
            return
                editable == other.editable &&
                frameView == other.frameView &&
                scrollBars == other.scrollBars &&
                autoScroll == other.autoScroll &&
                stopOnScrub == other.stopOnScrub &&
                item == other.item &&
                display == other.display &&
                firstTrack == other.firstTrack;
        }

        bool TimelineSettings::operator != (const TimelineSettings& other) const
        {
            return !(*this == other);
        }

        bool WindowSettings::operator == (const WindowSettings& other) const
        {
            return
                size == other.size &&
                fileToolBar == other.fileToolBar &&
                compareToolBar == other.compareToolBar &&
                windowToolBar == other.windowToolBar &&
                viewToolBar == other.viewToolBar &&
                toolsToolBar == other.toolsToolBar &&
                tabBar == other.tabBar &&
                timeline == other.timeline &&
                bottomToolBar == other.bottomToolBar &&
                statusToolBar == other.statusToolBar &&
                splitter == other.splitter &&
                splitter2 == other.splitter2;
        }

        bool WindowSettings::operator != (const WindowSettings& other) const
        {
            return !(*this == other);
        }

        struct SettingsModel::Private
        {
            std::weak_ptr<feather_tk::Context> context;
            std::shared_ptr<feather_tk::Settings> settings;

            std::shared_ptr<feather_tk::ObservableValue<AdvancedSettings> > advanced;
            std::shared_ptr<feather_tk::ObservableValue<tl::timeline::PlayerCacheOptions> > cache;
            std::shared_ptr<feather_tk::ObservableValue<ExportSettings> > exportSettings;
            std::shared_ptr<feather_tk::ObservableValue<FileBrowserSettings> > fileBrowser;
            std::shared_ptr<feather_tk::ObservableValue<ImageSequenceSettings> > imageSequence;
            std::shared_ptr<feather_tk::ObservableValue<ShortcutsSettings> > Shortcuts;
            std::shared_ptr<feather_tk::ObservableValue<MiscSettings> > misc;
            std::shared_ptr<feather_tk::ObservableValue<MouseSettings> > mouse;
            std::shared_ptr<feather_tk::ObservableValue<StyleSettings> > style;
            std::shared_ptr<feather_tk::ObservableValue<TimelineSettings> > timeline;
            std::shared_ptr<feather_tk::ObservableValue<WindowSettings> > window;
#if defined(TLRENDER_FFMPEG)
            std::shared_ptr<feather_tk::ObservableValue<tl::ffmpeg::Options> > ffmpeg;
#endif // TLRENDER_FFMPEG
#if defined(TLRENDER_USD)
            std::shared_ptr<feather_tk::ObservableValue<tl::usd::Options> > usd;
#endif // TLRENDER_USD
        };

        void SettingsModel::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<feather_tk::Settings>& settings)
        {
            FEATHER_TK_P();

            p.context = context;
            p.settings = settings;

            AdvancedSettings advanced;
            settings->getT("/Advanced", advanced);
            p.advanced = feather_tk::ObservableValue<AdvancedSettings>::create(advanced);

            tl::timeline::PlayerCacheOptions cache;
            settings->getT("/Cache", cache);
            p.cache = feather_tk::ObservableValue<tl::timeline::PlayerCacheOptions>::create(cache);

            ExportSettings exportSettings;
            settings->getT("/Export", exportSettings);
            p.exportSettings = feather_tk::ObservableValue<ExportSettings>::create(exportSettings);

            FileBrowserSettings fileBrowser;
            settings->getT("/FileBrowser", fileBrowser);
            p.fileBrowser = feather_tk::ObservableValue<FileBrowserSettings>::create(fileBrowser);
            auto fileBrowserSystem = context->getSystem<feather_tk::FileBrowserSystem>();
            fileBrowserSystem->setNativeFileDialog(fileBrowser.nativeFileDialog);
            fileBrowserSystem->getModel()->setPath(fileBrowser.path);
            fileBrowserSystem->getModel()->setOptions(fileBrowser.options);
            fileBrowserSystem->getModel()->setExtension(fileBrowser.extension);

            ImageSequenceSettings imageSequence;
            settings->getT("/ImageSequence", imageSequence);
            p.imageSequence = feather_tk::ObservableValue<ImageSequenceSettings>::create(imageSequence);

            ShortcutsSettings Shortcuts;
            settings->getT("/Shortcuts", Shortcuts);
            p.Shortcuts = feather_tk::ObservableValue<ShortcutsSettings>::create(Shortcuts);

            MiscSettings misc;
            settings->getT("/Misc", misc);
            p.misc = feather_tk::ObservableValue<MiscSettings>::create(misc);

            MouseSettings mouse;
            settings->getT("/Mouse", mouse);
            p.mouse = feather_tk::ObservableValue<MouseSettings>::create(mouse);

            StyleSettings style;
            settings->getT("/Style", style);
            p.style = feather_tk::ObservableValue<StyleSettings>::create(style);

            TimelineSettings timeline;
            settings->getT("/Timeline", timeline);
            p.timeline = feather_tk::ObservableValue<TimelineSettings>::create(timeline);

            WindowSettings window;
            settings->getT("/Window", window);
            p.window = feather_tk::ObservableValue<WindowSettings>::create(window);

#if defined(TLRENDER_FFMPEG)
            tl::ffmpeg::Options ffmpeg;
            settings->getT("/FFmpeg", ffmpeg);
            p.ffmpeg = feather_tk::ObservableValue<tl::ffmpeg::Options>::create(ffmpeg);
#endif // TLRENDER_FFMPEG

#if defined(TLRENDER_USD)
            tl::usd::Options usd;
            settings->getT("/USD", usd);
            p.usd = feather_tk::ObservableValue<tl::usd::Options>::create(usd);
#endif // TLRENDER_USD
        }

        SettingsModel::SettingsModel() :
            _p(new Private)
        {}

        SettingsModel::~SettingsModel()
        {
            FEATHER_TK_P();
            p.settings->setT("/Advanced", p.advanced->get());

            p.settings->setT("/Cache", p.cache->get());

            p.settings->setT("/Export", p.exportSettings->get());

            FileBrowserSettings fileBrowser = p.fileBrowser->get();
            if (auto context = p.context.lock())
            {
                auto fileBrowserSystem = context->getSystem<feather_tk::FileBrowserSystem>();
                fileBrowser.path = fileBrowserSystem->getModel()->getPath().u8string();
                fileBrowser.options = fileBrowserSystem->getModel()->getOptions();
                fileBrowser.extension = fileBrowserSystem->getModel()->getExtension();
            }
            p.settings->setT("/FileBrowser", fileBrowser);

            p.settings->setT("/ImageSequence", p.imageSequence->get());

            p.settings->setT("/Shortcuts", p.Shortcuts->get());

            p.settings->setT("/Misc", p.misc->get());

            p.settings->setT("/Mouse", p.mouse->get());

            p.settings->setT("/Style", p.style->get());

            p.settings->setT("/Timeline", p.timeline->get());

            p.settings->setT("/Window", p.window->get());

#if defined(TLRENDER_FFMPEG)
            p.settings->setT("/FFmpeg", p.ffmpeg->get());
#endif // TLRENDER_FFMPEG

#if defined(TLRENDER_USD)
            p.settings->setT("/USD", p.usd->get());
#endif // TLRENDER_USD
        }

        std::shared_ptr<SettingsModel> SettingsModel::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<feather_tk::Settings>& settings)
        {
            auto out = std::shared_ptr<SettingsModel>(new SettingsModel);
            out->_init(context, settings);
            return out;
        }

        void SettingsModel::reset()
        {
            setAdvanced(AdvancedSettings());
            setCache(tl::timeline::PlayerCacheOptions());
            setExport(ExportSettings());
            setFileBrowser(FileBrowserSettings());
            setImageSequence(ImageSequenceSettings());
            setShortcuts(ShortcutsSettings());
            MiscSettings miscSettings;
            miscSettings.showSetup = false;
            setMisc(miscSettings);
            setMouse(MouseSettings());
            setStyle(StyleSettings());
            setTimeline(TimelineSettings());
            setWindow(WindowSettings());
#if defined(TLRENDER_FFMPEG)
            setFFmpeg(tl::ffmpeg::Options());
#endif // TLRENDER_FFMPEG
#if defined(TLRENDER_USD)
            setUSD(tl::usd::Options());
#endif // TLRENDER_USD
        }

        const AdvancedSettings& SettingsModel::getAdvanced() const
        {
            return _p->advanced->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<AdvancedSettings> > SettingsModel::observeAdvanced() const
        {
            return _p->advanced;
        }

        void SettingsModel::setAdvanced(const AdvancedSettings& value)
        {
            _p->advanced->setIfChanged(value);
        }

        const tl::timeline::PlayerCacheOptions& SettingsModel::getCache() const
        {
            return _p->cache->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<tl::timeline::PlayerCacheOptions> > SettingsModel::observeCache() const
        {
            return _p->cache;
        }

        void SettingsModel::setCache(const tl::timeline::PlayerCacheOptions& value)
        {
            _p->cache->setIfChanged(value);
        }

        const ExportSettings& SettingsModel::getExport() const
        {
            return _p->exportSettings->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<ExportSettings> > SettingsModel::observeExport() const
        {
            return _p->exportSettings;
        }

        void SettingsModel::setExport(const ExportSettings& value)
        {
            _p->exportSettings->setIfChanged(value);
        }

        const FileBrowserSettings& SettingsModel::getFileBrowser() const
        {
            return _p->fileBrowser->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<FileBrowserSettings> > SettingsModel::observeFileBrowser() const
        {
            return _p->fileBrowser;
        }

        void SettingsModel::setFileBrowser(const FileBrowserSettings& value)
        {
            FEATHER_TK_P();
            if (p.fileBrowser->setIfChanged(value))
            {
                if (auto context = p.context.lock())
                {
                    auto fileBrowserSystem = context->getSystem<feather_tk::FileBrowserSystem>();
                    fileBrowserSystem->setNativeFileDialog(value.nativeFileDialog);
                }
            }
        }

        const ImageSequenceSettings& SettingsModel::getImageSequence() const
        {
            return _p->imageSequence->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<ImageSequenceSettings> > SettingsModel::observeImageSequence() const
        {
            return _p->imageSequence;
        }

        void SettingsModel::setImageSequence(const ImageSequenceSettings& value)
        {
            _p->imageSequence->setIfChanged(value);
        }

        const ShortcutsSettings& SettingsModel::getShortcuts() const
        {
            return _p->Shortcuts->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<ShortcutsSettings> > SettingsModel::observeShortcuts() const
        {
            return _p->Shortcuts;
        }

        void SettingsModel::setShortcuts(const ShortcutsSettings& value)
        {
            _p->Shortcuts->setIfChanged(value);
        }

        const MiscSettings& SettingsModel::getMisc() const
        {
            return _p->misc->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<MiscSettings> > SettingsModel::observeMisc() const
        {
            return _p->misc;
        }

        void SettingsModel::setMisc(const MiscSettings& value)
        {
            _p->misc->setIfChanged(value);
        }

        const MouseSettings& SettingsModel::getMouse() const
        {
            return _p->mouse->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<MouseSettings> > SettingsModel::observeMouse() const
        {
            return _p->mouse;
        }

        void SettingsModel::setMouse(const MouseSettings& value)
        {
            _p->mouse->setIfChanged(value);
        }

        const StyleSettings& SettingsModel::getStyle() const
        {
            return _p->style->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<StyleSettings> > SettingsModel::observeStyle() const
        {
            return _p->style;
        }

        void SettingsModel::setStyle(const StyleSettings& value)
        {
            _p->style->setIfChanged(value);
        }

        const TimelineSettings& SettingsModel::getTimeline() const
        {
            return _p->timeline->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<TimelineSettings> > SettingsModel::observeTimeline() const
        {
            return _p->timeline;
        }

        void SettingsModel::setTimeline(const TimelineSettings& value)
        {
            _p->timeline->setIfChanged(value);
        }

        const WindowSettings& SettingsModel::getWindow() const
        {
            return _p->window->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<WindowSettings> > SettingsModel::observeWindow() const
        {
            return _p->window;
        }

        void SettingsModel::setWindow(const WindowSettings& value)
        {
            _p->window->setIfChanged(value);
        }

#if defined(TLRENDER_FFMPEG)
        const tl::ffmpeg::Options& SettingsModel::getFFmpeg() const
        {
            return _p->ffmpeg->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<tl::ffmpeg::Options> > SettingsModel::observeFFmpeg() const
        {
            return _p->ffmpeg;
        }

        void SettingsModel::setFFmpeg(const tl::ffmpeg::Options& value)
        {
            _p->ffmpeg->setIfChanged(value);
        }
#endif // TLRENDER_FFMPEG

#if defined(TLRENDER_USD)
        const tl::usd::Options& SettingsModel::getUSD() const
        {
            return _p->usd->get();
        }

        std::shared_ptr<feather_tk::IObservableValue<tl::usd::Options> > SettingsModel::observeUSD() const
        {
            return _p->usd;
        }

        void SettingsModel::setUSD(const tl::usd::Options& value)
        {
            _p->usd->setIfChanged(value);
        }
#endif // TLRENDER_USD

        void to_json(nlohmann::json& json, const AdvancedSettings& value)
        {
            json["Compat"] = value.compat;
            json["AudioBufferFrameCount"] = value.audioBufferFrameCount;
            json["VideoRequestMax"] = value.videoRequestMax;
            json["AudioRequestMax"] = value.audioRequestMax;
        }

        void to_json(nlohmann::json& json, const ExportSettings& value)
        {
            json["Directory"] = value.directory;
            json["RenderSize"] = to_string(value.renderSize);
            json["CustomSize"] = value.customSize;
            json["FileType"] = to_string(value.fileType);
            json["ImageBaseName"] = value.imageBaseName;
            json["ImageZeroPad"] = value.imageZeroPad;
            json["ImageExtension"] = value.imageExtension;
            json["MovieBaseName"] = value.movieBaseName;
            json["MovieExtension"] = value.movieExtension;
            json["MovieCodec"] = value.movieCodec;
        }

        void to_json(nlohmann::json& json, const FileBrowserSettings& value)
        {
            json["NativeFileDialog"] = value.nativeFileDialog;
            json["Path"] = value.path;
            json["Options"] = value.options;
            json["Extension"] = value.extension;
        }

        void to_json(nlohmann::json& json, const ImageSequenceSettings& value)
        {
            json["Audio"] = tl::timeline::to_string(value.audio);
            json["AudioExtensions"] = value.audioExtensions;
            json["AudioFileName"] = value.audioFileName;
            json["MaxDigits"] = value.maxDigits;
            json["IO"] = value.io;
        }

        void to_json(nlohmann::json& json, const ShortcutsSettings& value)
        {
            for (const auto shortcut : value.shortcuts)
            {
                json["Shortcuts"].push_back(shortcut);
            }
        }

        void to_json(nlohmann::json& json, const MiscSettings& value)
        {
            json["TooltipsEnabled"] = value.tooltipsEnabled;
            json["ShowSetup"] = value.showSetup;
        }

        void to_json(nlohmann::json& json, const MouseSettings& value)
        {
            for (const auto& i : value.actions)
            {
                json["Actions"][to_string(i.first)] = to_string(i.second);
            }
        }

        void to_json(nlohmann::json& json, const StyleSettings& value)
        {
            json["DisplayScale"] = value.displayScale;
            json["ColorControls"] = value.colorControls;
            json["ColorStyle"] = to_string(value.colorStyle);
            for (auto i : value.customColorRoles)
            {
                json["CustomColorRoles"][feather_tk::getLabel(i.first)] = i.second;
            }
        }

        void to_json(nlohmann::json& json, const TimelineSettings& value)
        {
            json["Editable"] = value.editable;
            json["FrameView"] = value.frameView;
            json["ScrollBars"] = value.scrollBars;
            json["AutoScroll"] = value.autoScroll;
            json["StopOnScrub"] = value.stopOnScrub;
            json["Item"] = value.item;
            json["Display"] = value.display;
            json["FirstTrack"] = value.firstTrack;
        }

        void to_json(nlohmann::json& json, const WindowSettings& in)
        {
            json = nlohmann::json
            {
                { "Size", in.size },
                { "FileToolBar", in.fileToolBar },
                { "CompareToolBar", in.compareToolBar },
                { "WindowToolBar", in.windowToolBar },
                { "ViewToolBar", in.viewToolBar },
                { "ToolsToolBar", in.toolsToolBar },
                { "TabBar", in.tabBar },
                { "Timeline", in.timeline },
                { "BottomToolBar", in.bottomToolBar },
                { "StatusToolBar", in.statusToolBar },
                { "Splitter", in.splitter },
                { "Splitter2", in.splitter2 }
            };
        }

        void from_json(const nlohmann::json& json, AdvancedSettings& value)
        {
            json.at("Compat").get_to(value.compat);
            json.at("AudioBufferFrameCount").get_to(value.audioBufferFrameCount);
            json.at("VideoRequestMax").get_to(value.videoRequestMax);
            json.at("AudioRequestMax").get_to(value.audioRequestMax);
        }

        void from_json(const nlohmann::json& json, ExportSettings& value)
        {
            json.at("Directory").get_to(value.directory);
            from_string(json.at("RenderSize").get<std::string>(), value.renderSize);
            json.at("CustomSize").get_to(value.customSize);
            from_string(json.at("FileType").get<std::string>(), value.fileType);
            json.at("ImageBaseName").get_to(value.imageBaseName);
            json.at("ImageZeroPad").get_to(value.imageZeroPad);
            json.at("ImageExtension").get_to(value.imageExtension);
            json.at("MovieBaseName").get_to(value.movieBaseName);
            json.at("MovieExtension").get_to(value.movieExtension);
            json.at("MovieCodec").get_to(value.movieCodec);
        }

        void from_json(const nlohmann::json& json, FileBrowserSettings& value)
        {
            json.at("NativeFileDialog").get_to(value.nativeFileDialog);
            json.at("Path").get_to(value.path);
            json.at("Options").get_to(value.options);
            json.at("Extension").get_to(value.extension);
        }

        void from_json(const nlohmann::json& json, ImageSequenceSettings& value)
        {
            tl::timeline::from_string(json.at("Audio").get<std::string>(), value.audio);
            json.at("AudioExtensions").get_to(value.audioExtensions);
            json.at("AudioFileName").get_to(value.audioFileName);
            json.at("MaxDigits").get_to(value.maxDigits);
            json.at("IO").get_to(value.io);
        }

        void from_json(const nlohmann::json& json, MiscSettings& value)
        {
            json.at("TooltipsEnabled").get_to(value.tooltipsEnabled);
            json.at("ShowSetup").get_to(value.showSetup);
        }

        void from_json(const nlohmann::json& json, MouseSettings& value)
        {
            for (auto i = json.at("Actions").begin(); i != json.at("Actions").end(); ++i)
            {
                MouseAction mouseAction = MouseAction::First;
                from_string(i.key(), mouseAction);
                from_string(i.value().get<std::string>(), value.actions[mouseAction]);
            }
        }

        void from_json(const nlohmann::json& json, ShortcutsSettings& value)
        {
            for (auto i = json.at("Shortcuts").begin(); i != json.at("Shortcuts").end(); ++i)
            {
                const Shortcut shortcut = i->get<Shortcut>();
                const auto j = std::find_if(
                    value.shortcuts.begin(),
                    value.shortcuts.end(),
                    [shortcut](const Shortcut& value)
                    {
                        return shortcut.name == value.name;
                    });
                if (j != value.shortcuts.end())
                {
                    *j = shortcut;
                }
                else
                {
                    value.shortcuts.push_back(shortcut);
                }
            }
        }

        void from_json(const nlohmann::json& json, StyleSettings& value)
        {
            json.at("DisplayScale").get_to(value.displayScale);
            from_string(json.at("ColorStyle").get<std::string>(), value.colorStyle);
            json.at("ColorControls").get_to(value.colorControls);
            for (auto i = json.at("CustomColorRoles").begin(); i != json.at("CustomColorRoles").end(); ++i)
            {
                feather_tk::ColorRole colorRole = feather_tk::ColorRole::None;
                from_string(i.key(), colorRole);
                i.value().get_to(value.customColorRoles[colorRole]);
            }
        }

        void from_json(const nlohmann::json& json, TimelineSettings& value)
        {
            json.at("Editable").get_to(value.editable);
            json.at("FrameView").get_to(value.frameView);
            json.at("ScrollBars").get_to(value.scrollBars);
            json.at("AutoScroll").get_to(value.autoScroll);
            json.at("StopOnScrub").get_to(value.stopOnScrub);
            json.at("Item").get_to(value.item);
            json.at("Display").get_to(value.display);
            json.at("FirstTrack").get_to(value.firstTrack);
        }

        void from_json(const nlohmann::json& json, WindowSettings& out)
        {
            json.at("Size").get_to(out.size);
            json.at("FileToolBar").get_to(out.fileToolBar);
            json.at("CompareToolBar").get_to(out.compareToolBar);
            json.at("WindowToolBar").get_to(out.windowToolBar);
            json.at("ViewToolBar").get_to(out.viewToolBar);
            json.at("ToolsToolBar").get_to(out.toolsToolBar);
            json.at("TabBar").get_to(out.tabBar);
            json.at("Timeline").get_to(out.timeline);
            json.at("BottomToolBar").get_to(out.bottomToolBar);
            json.at("StatusToolBar").get_to(out.statusToolBar);
            json.at("Splitter").get_to(out.splitter);
            json.at("Splitter2").get_to(out.splitter2);
        }
    }
}
