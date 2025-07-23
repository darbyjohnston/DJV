// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlTimeline/Player.h>

#include <feather-tk/ui/App.h>

#include <filesystem>

namespace feather_tk
{
    class Settings;
}

namespace tl
{
#if defined(TLRENDER_BMD)
    namespace bmd
    {
        class OutputDevice;
    }
#endif // TLRENDER_BMD
}

namespace djv
{
    //! DJV application
    namespace app
    {
        struct FilesModelItem;

        class AudioModel;
        class ColorModel;
        class FilesModel;
        class MainWindow;
        class RecentFilesModel;
        class SettingsModel;
        class TimeUnitsModel;
        class ToolsModel;
        class ViewportModel;
#if defined(TLRENDER_BMD)
        class BMDDevicesModel;
#endif // TLRENDER_BMD

        //! Application.
        class App : public feather_tk::App
        {
            FEATHER_TK_NON_COPYABLE(App);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                std::vector<std::string>&);

            App();

        public:
            ~App();

            //! Create a new application.
            static std::shared_ptr<App> create(
                const std::shared_ptr<feather_tk::Context>&,
                std::vector<std::string>&);

            //! Open a file.
            void open(
                const tl::file::Path& path,
                const tl::file::Path& audioPath = tl::file::Path());

            //! Open a file dialog.
            void openDialog();

            //! Open a file and separate audio dialog.
            void openSeparateAudioDialog();

            //! Get the settings.
            const std::shared_ptr<feather_tk::Settings>& getSettings() const;

            //! Get the settings model.
            const std::shared_ptr<SettingsModel>& getSettingsModel() const;

            //! Get the time units model.
            const std::shared_ptr<TimeUnitsModel>& getTimeUnitsModel() const;

            //! Get the files model.
            const std::shared_ptr<FilesModel>& getFilesModel() const;

            //! Get the recent files model.
            const std::shared_ptr<RecentFilesModel>& getRecentFilesModel() const;

            //! Reload the active files.
            void reload();

            //! Observe the timeline player.
            std::shared_ptr<feather_tk::IObservableValue<std::shared_ptr<tl::timeline::Player> > > observePlayer() const;

            //! Get the color model.
            const std::shared_ptr<ColorModel>& getColorModel() const;

            //! Get the viewport model.
            const std::shared_ptr<ViewportModel>& getViewportModel() const;

            //! Get the audio model.
            const std::shared_ptr<AudioModel>& getAudioModel() const;

            //! Get the tools model.
            const std::shared_ptr<ToolsModel>& getToolsModel() const;

            //! Get the main window.
            const std::shared_ptr<MainWindow>& getMainWindow() const;

            //! Observe whether the secondary window is active.
            std::shared_ptr<feather_tk::IObservableValue<bool> > observeSecondaryWindow() const;

            //! Set whether the secondary window is active.
            void setSecondaryWindow(bool);

#if defined(TLRENDER_BMD)
            //! Get the BMD devices model.
            const std::shared_ptr<BMDDevicesModel>& getBMDDevicesModel() const;

            //! Get the BMD output device.
            const std::shared_ptr<tl::bmd::OutputDevice>& getBMDOutputDevice() const;
#endif // TLRENDER_BMD

            void run() override;

        protected:
            void _tick() override;

        private:
            void _modelsInit();
            void _devicesInit();
            void _observersInit();
            void _inputFilesInit();
            void _windowsInit();

            std::filesystem::path _appDocsPath();
            std::filesystem::path _getLogFilePath(
                const std::string& appName,
                const std::filesystem::path& appDocsPath);
            std::filesystem::path _getSettingsPath(
                const std::string& appName,
                const std::filesystem::path& appDocsPath);
            tl::io::Options _getIOOptions() const;

            void _filesUpdate(const std::vector<std::shared_ptr<FilesModelItem> >&);
            void _activeUpdate(const std::vector<std::shared_ptr<FilesModelItem> >&);
            void _layersUpdate(const std::vector<int>&);
            void _viewUpdate(const feather_tk::V2I& pos, double zoom, bool frame);
            void _audioUpdate();

            FEATHER_TK_PRIVATE();
        };
    }
}
