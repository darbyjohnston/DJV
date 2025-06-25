// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <tlTimeline/BackgroundOptions.h>
#include <tlTimeline/DisplayOptions.h>
#include <tlTimeline/ForegroundOptions.h>

#include <feather-tk/core/ObservableValue.h>

namespace feather_tk
{
    class Context;
    class Settings;
}

namespace djv
{
    namespace app
    {
        //! Viewport model.
        class ViewportModel : public std::enable_shared_from_this<ViewportModel>
        {
            FEATHER_TK_NON_COPYABLE(ViewportModel);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<feather_tk::Settings>&);

            ViewportModel();

        public:
            ~ViewportModel();

            //! Create a new model.
            static std::shared_ptr<ViewportModel> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<feather_tk::Settings>&);

            //! Get the color picker.
            const feather_tk::Color4F& getColorPicker() const;

            //! Observe the color picker.
            std::shared_ptr<feather_tk::IObservableValue<feather_tk::Color4F> > observeColorPicker() const;

            //! Set the color picker.
            void setColorPicker(const feather_tk::Color4F&);

            //! Get the image options.
            const feather_tk::ImageOptions& getImageOptions() const;

            //! Observe the image options.
            std::shared_ptr<feather_tk::IObservableValue<feather_tk::ImageOptions> > observeImageOptions() const;

            //! Set the image options.
            void setImageOptions(const feather_tk::ImageOptions&);

            //! Get the display options.
            const tl::timeline::DisplayOptions& getDisplayOptions() const;

            //! Observe the display options.
            std::shared_ptr<feather_tk::IObservableValue<tl::timeline::DisplayOptions> > observeDisplayOptions() const;

            //! Set the display options.
            void setDisplayOptions(const tl::timeline::DisplayOptions&);

            //! Get the background options.
            const tl::timeline::BackgroundOptions& getBackgroundOptions() const;

            //! Observe the background options.
            std::shared_ptr<feather_tk::IObservableValue<tl::timeline::BackgroundOptions> > observeBackgroundOptions() const;

            //! Set the background options.
            void setBackgroundOptions(const tl::timeline::BackgroundOptions&);

            //! Get the foreground options.
            const tl::timeline::ForegroundOptions& getForegroundOptions() const;

            //! Observe the foreground options.
            std::shared_ptr<feather_tk::IObservableValue<tl::timeline::ForegroundOptions> > observeForegroundOptions() const;

            //! Set the foreground options.
            void setForegroundOptions(const tl::timeline::ForegroundOptions&);

            //! Get the color buffer type.
            feather_tk::ImageType getColorBuffer() const;

            //! Observe the color buffer type.
            std::shared_ptr<feather_tk::IObservableValue<feather_tk::ImageType> > observeColorBuffer() const;

            //! Set the color buffer type.
            void setColorBuffer(feather_tk::ImageType);

            //! Get whether the HUD is enabled.
            bool getHUD() const;

            //! Observe whether the HUD is enabled.
            std::shared_ptr<feather_tk::IObservableValue<bool> > observeHUD() const;

            //! Set whether the HUD is enabled.
            void setHUD(bool);

            //! Get the HUD information regular expression.
            const std::string& getHUDInfo() const;

            //! Observe the HUD information regular expression.
            std::shared_ptr<feather_tk::IObservableValue<std::string> > observeHUDInfo() const;

            //! Set the HUD information regular expression.
            void setHUDInfo(const std::string&);

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
