// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>
#include <djvViewApp/IViewSystem.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        class MediaCanvas;
        class MediaWidget;

        //! This class provides the window system.
        class WindowSystem : public IViewSystem
        {
            DJV_NON_COPYABLE(WindowSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            WindowSystem();

        public:
            ~WindowSystem() override;

            static std::shared_ptr<WindowSystem> create(const std::shared_ptr<Core::Context>&);

            void setMediaCanvas(const std::shared_ptr<MediaCanvas>&);

            std::shared_ptr<Core::IValueSubject<std::shared_ptr<MediaWidget> > > observeActiveWidget() const;

            std::shared_ptr<Core::IValueSubject<bool> > observeFullScreen() const;
            void setFullScreen(bool);

            std::shared_ptr<Core::IValueSubject<bool> > observeMaximize() const;
            void setMaximize(bool);

            std::shared_ptr<Core::IValueSubject<float> > observeFade() const;

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            MenuData getMenu() const override;

        protected:
            void _pointerUpdate();
            void _actionsUpdate();

            void _textUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

