// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>
#include <djvViewApp/IViewAppSystem.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        class MediaCanvas;
        class MediaWidget;

        //! This class provides the window system.
        class WindowSystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(WindowSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            WindowSystem();

        public:
            ~WindowSystem() override;

            static std::shared_ptr<WindowSystem> create(const std::shared_ptr<System::Context>&);

            //! \name Active Widget
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<std::shared_ptr<MediaWidget> > > observeActiveWidget() const;

            void setActiveWidget(const std::shared_ptr<MediaWidget>&);

            ///@}

            //! \name Full Screen
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeFullScreen() const;

            void setFullScreen(bool);

            ///@}

            //! \name Presentation Mode
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observePresentation() const;

            void setPresentation(bool);

            ///@}

            //! \name Window State
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeFloatOnTop() const;
            
            void setFloatOnTop(bool);

            ///@}

            //! \name Fade
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<float> > observeFade() const;

            ///@}

            int getSortKey() const override;
            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            std::vector<std::shared_ptr<UI::Menu> > getMenus() const override;

        protected:
            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            void _fadeStart();
            void _fadeStop();

            void _presentationUpdate();
            void _actionsUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

