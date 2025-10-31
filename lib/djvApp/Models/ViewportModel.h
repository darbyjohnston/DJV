// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the DJV project.

#pragma once

#include <tlTimeline/BackgroundOptions.h>
#include <tlTimeline/DisplayOptions.h>
#include <tlTimeline/ForegroundOptions.h>

#include <ftk/Core/ObservableValue.h>

namespace ftk
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
            FTK_NON_COPYABLE(ViewportModel);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ftk::Settings>&);

            ViewportModel();

        public:
            ~ViewportModel();

            //! Create a new model.
            static std::shared_ptr<ViewportModel> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ftk::Settings>&);

            //! Get the color picker.
            const ftk::Color4F& getColorPicker() const;

            //! Observe the color picker.
            std::shared_ptr<ftk::IObservableValue<ftk::Color4F> > observeColorPicker() const;

            //! Set the color picker.
            void setColorPicker(const ftk::Color4F&);

            //! Get the image options.
            const ftk::ImageOptions& getImageOptions() const;

            //! Observe the image options.
            std::shared_ptr<ftk::IObservableValue<ftk::ImageOptions> > observeImageOptions() const;

            //! Set the image options.
            void setImageOptions(const ftk::ImageOptions&);

            //! Get the display options.
            const tl::timeline::DisplayOptions& getDisplayOptions() const;

            //! Observe the display options.
            std::shared_ptr<ftk::IObservableValue<tl::timeline::DisplayOptions> > observeDisplayOptions() const;

            //! Set the display options.
            void setDisplayOptions(const tl::timeline::DisplayOptions&);

            //! Get the background options.
            const tl::timeline::BackgroundOptions& getBackgroundOptions() const;

            //! Observe the background options.
            std::shared_ptr<ftk::IObservableValue<tl::timeline::BackgroundOptions> > observeBackgroundOptions() const;

            //! Set the background options.
            void setBackgroundOptions(const tl::timeline::BackgroundOptions&);

            //! Get the foreground options.
            const tl::timeline::ForegroundOptions& getForegroundOptions() const;

            //! Observe the foreground options.
            std::shared_ptr<ftk::IObservableValue<tl::timeline::ForegroundOptions> > observeForegroundOptions() const;

            //! Set the foreground options.
            void setForegroundOptions(const tl::timeline::ForegroundOptions&);

            //! Get the color buffer type.
            ftk::ImageType getColorBuffer() const;

            //! Observe the color buffer type.
            std::shared_ptr<ftk::IObservableValue<ftk::ImageType> > observeColorBuffer() const;

            //! Set the color buffer type.
            void setColorBuffer(ftk::ImageType);

            //! Get whether the HUD is enabled.
            bool getHUD() const;

            //! Observe whether the HUD is enabled.
            std::shared_ptr<ftk::IObservableValue<bool> > observeHUD() const;

            //! Set whether the HUD is enabled.
            void setHUD(bool);

        private:
            FTK_PRIVATE();
        };
    }
}
