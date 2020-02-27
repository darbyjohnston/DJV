//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvCore/IEventSystem.h>

namespace djv
{
    namespace UI
    {
        class Widget;
        class Window;

        //! This class provides a window system interface.
        class EventSystem : public Core::Event::IEventSystem
        {
            DJV_NON_COPYABLE(EventSystem);
            
        protected:
            void _init(const std::string &, const std::shared_ptr<Core::Context>&);

            EventSystem();

        public:
            virtual ~EventSystem() = 0;

            void tick(const std::chrono::steady_clock::time_point&, const Core::Time::Unit& dt) override;

        protected:
            virtual void _pushClipRect(const Core::BBox2f &);
            virtual void _popClipRect();

            bool _resizeRequest(const std::shared_ptr<Widget> &) const;
            bool _redrawRequest(const std::shared_ptr<Widget> &) const;

            void _initLayoutRecursive(const std::shared_ptr<Widget>&, Core::Event::InitLayout&);
            void _preLayoutRecursive(const std::shared_ptr<Widget>&, Core::Event::PreLayout&);
            void _layoutRecursive(const std::shared_ptr<Widget> &, Core::Event::Layout &);
            void _clipRecursive(const std::shared_ptr<Widget> &, Core::Event::Clip &);
            void _paintRecursive(
                const std::shared_ptr<Widget> &,
                Core::Event::Paint &,
                Core::Event::PaintOverlay&);

            void _initObject(const std::shared_ptr<Core::IObject> &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
