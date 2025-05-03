// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Shortcuts.h>

#include <tlTimelineUI/IItem.h>

#include <tlTimeline/Player.h>

#include <dtk/ui/App.h>
#include <dtk/ui/FileBrowser.h>
#include <dtk/core/ObservableValue.h>

#include <tlIO/SequenceIO.h>
#if defined(TLRENDER_FFMPEG)
#include <tlIO/FFmpeg.h>
#endif // TLRENDER_FFMPEG
#if defined(TLRENDER_USD)
#include <tlIO/USD.h>
#endif // TLRENDER_USD

#include <nlohmann/json.hpp>

namespace dtk
{
    class Context;
    class Settings;
}

namespace djv
{
    namespace app
    {
        //! Advanced settings.
        struct AdvancedSettings
        {
            bool compat = true;
            size_t audioBufferFrameCount = tl::timeline::PlayerOptions().audioBufferFrameCount;
            size_t videoRequestMax = 16;
            size_t audioRequestMax = 16;

            bool operator == (const AdvancedSettings&) const;
            bool operator != (const AdvancedSettings&) const;
        };

        //! Export render size.
        enum class ExportRenderSize
        {
            Default,
            _1920_1080,
            _3840_2160,
            _4096_2160,
            Custom,

            Count,
            First = Default
        };
        DTK_ENUM(ExportRenderSize);

        //! Get an export render size.
        dtk::Size2I getSize(ExportRenderSize);

        //! Export file type.
        enum class ExportFileType
        {
            Image,
            Sequence,
            Movie,

            Count,
            First = Image
        };
        DTK_ENUM(ExportFileType);

        //! Export settings.
        struct ExportSettings
        {
            std::string directory;
            ExportRenderSize renderSize = ExportRenderSize::Default;
            dtk::Size2I customSize = dtk::Size2I(1920, 1080);
            ExportFileType fileType = ExportFileType::Image;

            std::string imageBaseName = "render.";
            size_t imageZeroPad = 0;
            std::string imageExtension = ".tif";

            std::string movieBaseName = "render";
            std::string movieExtension = ".mov";
            std::string movieCodec = "mjpeg";

            bool operator == (const ExportSettings&) const;
            bool operator != (const ExportSettings&) const;
        };

        //! File browser settings.
        struct FileBrowserSettings
        {
            bool nativeFileDialog = true;
            std::string path;
            dtk::FileBrowserOptions options;
            std::string extension;

            bool operator == (const FileBrowserSettings&) const;
            bool operator != (const FileBrowserSettings&) const;
        };

        //! File sequence settings.
        struct FileSequenceSettings
        {
            tl::timeline::FileSequenceAudio audio = tl::timeline::FileSequenceAudio::Extension;
            std::vector<std::string> audioExtensions;
            std::string audioFileName;
            size_t maxDigits = 9;
            tl::io::SequenceOptions io;

            bool operator == (const FileSequenceSettings&) const;
            bool operator != (const FileSequenceSettings&) const;
        };

        //! Miscellaneous settings.
        struct MiscSettings
        {
            bool tooltipsEnabled = true;

            bool operator == (const MiscSettings&) const;
            bool operator != (const MiscSettings&) const;
        };

        //! Mouse actions.
        enum class MouseAction
        {
            PanView,
            CompareWipe,
            ColorPicker,
            FrameShuttle,

            Count,
            First = ColorPicker
        };
        DTK_ENUM(MouseAction);

        //! Mouse settings.
        struct MouseSettings
        {
            std::map<MouseAction, dtk::KeyModifier> actions =
            {
                { MouseAction::ColorPicker, dtk::KeyModifier::None },
                { MouseAction::PanView, dtk::KeyModifier::Control },
                { MouseAction::FrameShuttle, dtk::KeyModifier::Shift },
                { MouseAction::CompareWipe, dtk::KeyModifier::Alt }
            };

            bool operator == (const MouseSettings&) const;
            bool operator != (const MouseSettings&) const;
        };

        //! Keyboard shortcuts settings.
        struct ShortcutsSettings
        {
            ShortcutsSettings();

            std::vector<Shortcut> shortcuts;

            bool operator == (const ShortcutsSettings&) const;
            bool operator != (const ShortcutsSettings&) const;
        };

        //! Style settings.
        struct StyleSettings
        {
            float displayScale = 0.F;
            dtk::ColorControls colorControls;
            dtk::ColorStyle colorStyle = dtk::ColorStyle::Dark;
            std::map<dtk::ColorRole, dtk::Color4F> customColorRoles = dtk::getCustomColorRoles();

            bool operator == (const StyleSettings&) const;
            bool operator != (const StyleSettings&) const;
        };

        //! Timeline settings.
        struct TimelineSettings
        {
            bool editable = false;
            bool frameView = true;
            bool scroll = true;
            bool stopOnScrub = false;
            tl::timelineui::ItemOptions item;
            tl::timelineui::DisplayOptions display;
            bool firstTrack = false;

            bool operator == (const TimelineSettings&) const;
            bool operator != (const TimelineSettings&) const;
        };

        //! Window settings.
        struct WindowSettings
        {
            dtk::Size2I size = dtk::Size2I(1920, 1080);
            bool fileToolBar = true;
            bool compareToolBar = true;
            bool windowToolBar = true;
            bool viewToolBar = true;
            bool toolsToolBar = true;
            bool tabBar = true;
            bool timeline = true;
            bool bottomToolBar = true;
            bool statusToolBar = true;
            float splitter = .7F;
            float splitter2 = .7F;

            bool operator == (const WindowSettings&) const;
            bool operator != (const WindowSettings&) const;
        };

        //! Settings model.
        class SettingsModel : public std::enable_shared_from_this<SettingsModel>
        {
            DTK_NON_COPYABLE(SettingsModel);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<dtk::Settings>&);

            SettingsModel();

        public:
            ~SettingsModel();

            //! Create a new model.
            static std::shared_ptr<SettingsModel> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<dtk::Settings>&);

            //! Reset to default values.
            void reset();

            //! \name Advanced
            ///@{

            const AdvancedSettings& getAdvanced() const;
            std::shared_ptr<dtk::IObservableValue<AdvancedSettings> > observeAdvanced() const;
            void setAdvanced(const AdvancedSettings&);

            ///@}

            //! \name Cache
            ///@{

            const tl::timeline::PlayerCacheOptions& getCache() const;
            std::shared_ptr<dtk::IObservableValue<tl::timeline::PlayerCacheOptions> > observeCache() const;
            void setCache(const tl::timeline::PlayerCacheOptions&);

            ///@}

            //! \name Export
            ///@{

            const ExportSettings& getExport() const;
            std::shared_ptr<dtk::IObservableValue<ExportSettings> > observeExport() const;
            void setExport(const ExportSettings&);

            ///@}

            //! \name File Browser
            ///@{

            const FileBrowserSettings& getFileBrowser() const;
            std::shared_ptr<dtk::IObservableValue<FileBrowserSettings> > observeFileBrowser() const;
            void setFileBrowser(const FileBrowserSettings&);

            ///@}

            //! \name File Sequences
            ///@{

            const FileSequenceSettings& getFileSequence() const;
            std::shared_ptr<dtk::IObservableValue<FileSequenceSettings> > observeFileSequence() const;
            void setFileSequence(const FileSequenceSettings&);

            ///@}

            //! \name Miscellaneous
            ///@{

            const MiscSettings& getMisc() const;
            std::shared_ptr<dtk::IObservableValue<MiscSettings> > observeMisc() const;
            void setMisc(const MiscSettings&);

            ///@}

            //! \name Mouse
            ///@{

            const MouseSettings& getMouse() const;
            std::shared_ptr<dtk::IObservableValue<MouseSettings> > observeMouse() const;
            void setMouse(const MouseSettings&);

            ///@}

            //! \name Keyboard Shortcuts
            ///@{

            const ShortcutsSettings& getShortcuts() const;
            std::shared_ptr<dtk::IObservableValue<ShortcutsSettings> > observeShortcuts() const;
            void setShortcuts(const ShortcutsSettings&);

            ///@}

            //! \name Style
            ///@{

            const StyleSettings& getStyle() const;
            std::shared_ptr<dtk::IObservableValue<StyleSettings> > observeStyle() const;
            void setStyle(const StyleSettings&);

            ///@}

            //! \name Timeline
            ///@{

            const TimelineSettings& getTimeline() const;
            std::shared_ptr<dtk::IObservableValue<TimelineSettings> > observeTimeline() const;
            void setTimeline(const TimelineSettings&);

            ///@}

            //! \name Window
            ///@{

            const WindowSettings& getWindow() const;
            std::shared_ptr<dtk::IObservableValue<WindowSettings> > observeWindow() const;
            void setWindow(const WindowSettings&);

            ///@}

#if defined(TLRENDER_FFMPEG)
            //! \name FFmpeg
            ///@{

            const tl::ffmpeg::Options& getFFmpeg() const;
            std::shared_ptr<dtk::IObservableValue<tl::ffmpeg::Options> > observeFFmpeg() const;
            void setFFmpeg(const tl::ffmpeg::Options&);

            ///@}
#endif // TLRENDER_FFMPEG

#if defined(TLRENDER_USD)
            //! \name USD
            ///@{

            const tl::usd::Options& getUSD() const;
            std::shared_ptr<dtk::IObservableValue<tl::usd::Options> > observeUSD() const;
            void setUSD(const tl::usd::Options&);

            ///@}
#endif // TLRENDER_USD

        private:
            DTK_PRIVATE();
        };

        //! \name Serialize
        ///@{

        void to_json(nlohmann::json&, const AdvancedSettings&);
        void to_json(nlohmann::json&, const ExportSettings&);
        void to_json(nlohmann::json&, const FileBrowserSettings&);
        void to_json(nlohmann::json&, const FileSequenceSettings&);
        void to_json(nlohmann::json&, const MiscSettings&);
        void to_json(nlohmann::json&, const MouseSettings&);
        void to_json(nlohmann::json&, const ShortcutsSettings&);
        void to_json(nlohmann::json&, const StyleSettings&);
        void to_json(nlohmann::json&, const TimelineSettings&);
        void to_json(nlohmann::json&, const WindowSettings&);

        void from_json(const nlohmann::json&, AdvancedSettings&);
        void from_json(const nlohmann::json&, ExportSettings&);
        void from_json(const nlohmann::json&, FileBrowserSettings&);
        void from_json(const nlohmann::json&, FileSequenceSettings&);
        void from_json(const nlohmann::json&, MiscSettings&);
        void from_json(const nlohmann::json&, MouseSettings&);
        void from_json(const nlohmann::json&, ShortcutsSettings&);
        void from_json(const nlohmann::json&, StyleSettings&);
        void from_json(const nlohmann::json&, TimelineSettings&);
        void from_json(const nlohmann::json&, WindowSettings&);

        ///@}
    }
}
