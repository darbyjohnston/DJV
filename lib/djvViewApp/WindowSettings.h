// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/ISettings.h>

#include <djvMath/BBox.h>

#include <djvCore/ValueObserver.h>
#include <djvCore/ListObserver.h>

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
            void _init(const std::shared_ptr<System::Context>&);

            WindowSettings();

        public:
            ~WindowSettings() override;

            static std::shared_ptr<WindowSettings> create(const std::shared_ptr<System::Context>&);

            //! \name Position and Size
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeRestorePos() const;
            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeRestoreSize() const;

            const glm::ivec2& getWindowPos() const;
            const glm::ivec2& getWindowSize() const;
            const glm::ivec2& getWindowSizeDefault() const;

            void setRestorePos(bool);
            void setRestoreSize(bool);
            void setWindowPos(const glm::ivec2&);
            void setWindowSize(const glm::ivec2&);

            ///@}

            //! \name Drawer
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<float> > observeDrawerSplit() const;

            void setDrawerSplit(float);

            ///@}

            //! \name Full Screen
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeFullScreen() const;
            std::shared_ptr<Core::Observer::IValueSubject<int> > observeFullScreenMonitor() const;

            void setFullScreen(bool);
            void setFullScreenMonitor(int);

            ///@}

            //! \name Window State
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeFloatOnTop() const;
            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeMaximize() const;

            void setFloatOnTop(bool);
            void setMaximize(bool);
            
            ///@}

            //! \name Background Image
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<std::string> > observeBackgroundImage() const;
            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeBackgroundImageScale() const;
            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeBackgroundImageColorize() const;

            void setBackgroundImage(const std::string&);
            void setBackgroundImageScale(bool);
            void setBackgroundImageColorize(bool);

            ///@}

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

