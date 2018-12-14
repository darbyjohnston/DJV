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

#include <djvCore/IEventSystem.h>

#include <glm/vec2.hpp>

struct GLFWwindow;

namespace djv
{
    namespace UI
    {
        class Widget;

    } // namespace UI

    namespace Desktop
    {
        class EventSystem : public Core::IEventSystem
        {
            DJV_NON_COPYABLE(EventSystem);

        protected:
            void _init(GLFWwindow*, Core::Context *);
            EventSystem();

        public:
            virtual ~EventSystem();

            static std::shared_ptr<EventSystem> create(GLFWwindow*, Core::Context *);

        private:
            void _pointerMove(const Core::PointerInfo&);
            void _buttonPress(const Core::PointerInfo&);
            void _buttonRelease(const Core::PointerInfo&);
            void _hover(const std::shared_ptr<UI::Widget>&, Core::PointerMoveEvent&, std::shared_ptr<UI::Widget>&);
            static std::shared_ptr<UI::Widget> _underPointer(const std::shared_ptr<UI::Widget>&, const glm::vec2&);

            static void _pointerCallback(GLFWwindow*, double, double);
            static void _buttonCallback(GLFWwindow*, int button, int action, int mods);
            static void _dropCallback(GLFWwindow*, int, const char**);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace Desktop
} // namespace djv
