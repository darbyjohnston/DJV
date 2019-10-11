//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include <djvViewApp/Enum.h>
#include <djvViewApp/IViewSystem.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Core
    {
        namespace Event
        {
            struct PointerInfo;
    
        } // namespace Event
    } // namespace Core

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
            void _pointerUpdate(const Core::Event::PointerInfo&);
            void _actionsUpdate();

            void _textUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

