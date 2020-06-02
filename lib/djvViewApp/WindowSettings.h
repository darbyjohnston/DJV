// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/ISettings.h>

#include <djvCore/ValueObserver.h>

#include <glm/vec2.hpp>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the window settings.
        class WindowSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(WindowSettings);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);

            WindowSettings();

        public:
            virtual ~WindowSettings();

            static std::shared_ptr<WindowSettings> create(const std::shared_ptr<Core::Context>&);

            const glm::ivec2& getWindowSize() const;
            void setWindowSize(const glm::ivec2&);

            std::shared_ptr<Core::IValueSubject<int> > observeFullscreenMonitor() const;
            void setFullscreenMonitor(int);

            std::shared_ptr<Core::IValueSubject<bool> > observeFloatOnTop() const;
            void setFloatOnTop(bool);

            std::shared_ptr<Core::IValueSubject<bool> > observeMaximize() const;
            void setMaximize(bool);

            std::shared_ptr<Core::IValueSubject<bool> > observeAutoHide() const;
            void setAutoHide(bool);
            
            std::shared_ptr<Core::IValueSubject<std::string> > observeBackgroundImage() const;
            std::shared_ptr<Core::IValueSubject<bool> > observeBackgroundImageScale() const;
            std::shared_ptr<Core::IValueSubject<bool> > observeBackgroundImageColorize() const;
            void setBackgroundImage(const std::string&);
            void setBackgroundImageScale(bool);
            void setBackgroundImageColorize(bool);

            void load(const picojson::value &) override;
            picojson::value save() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

