//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvCore/Event.h>
#include <djvCore/ISystem.h>
#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace UI
    {
        class Widget;
        class Window;

        //! This class provides a window system interface.
        class IWindowSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(IWindowSystem);
            
        protected:
            void _init(const std::string &, Core::Context *);

            IWindowSystem();

        public:
            virtual ~IWindowSystem() = 0;

            std::shared_ptr<Core::IListSubject<std::shared_ptr<Window> > > observeWindows() const;
            std::shared_ptr<Core::IValueSubject<std::shared_ptr<Window> > > observeCurrentWindow() const;

            void tick(float dt) override;

        protected:
            virtual void _addWindow(const std::shared_ptr<Window>&);
            virtual void _removeWindow(const std::shared_ptr<Window>&);

            virtual void _pushClipRect(const Core::BBox2f &);
            virtual void _popClipRect();

            bool _resizeRequest(const std::shared_ptr<Widget>&) const;
            bool _redrawRequest(const std::shared_ptr<Widget>&) const;

            void _styleChangedRecursive(const std::shared_ptr<Widget>&, Core::Event::StyleChanged&);
            void _preLayoutRecursive(const std::shared_ptr<Widget>&, Core::Event::PreLayout&);
            void _layoutRecursive(const std::shared_ptr<Widget>&, Core::Event::Layout&);
            void _clipRecursive(const std::shared_ptr<Widget>&, Core::Event::Clip&);
            void _paintRecursive(const std::shared_ptr<Widget>&, Core::Event::Paint&);

        private:
            void _getFirstTick(const std::shared_ptr<Widget> &, std::vector<std::shared_ptr<Widget> > &);

            struct Private;
            std::unique_ptr<Private> _p;

            friend class Window;
        };

    } // namespace UI
} // namespace djv
