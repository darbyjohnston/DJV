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

#include <djvUI/EventSystem.h>

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
        //! This class provides a desktop application event system.
        class EventSystem : public UI::EventSystem
        {
            DJV_NON_COPYABLE(EventSystem);

        protected:
            void _init(GLFWwindow *, const std::shared_ptr<Core::Context>&);
            EventSystem();

        public:
            ~EventSystem() override;

            static std::shared_ptr<EventSystem> create(GLFWwindow *, const std::shared_ptr<Core::Context>&);

            void setClipboard(const std::string&) override;
            std::string getClipboard() const override;

            void tick(float dt) override;

        protected:
            void _pushClipRect(const Core::BBox2f &) override;
            void _popClipRect() override;

            void _hover(Core::Event::PointerMove &, std::shared_ptr<Core::IObject> &) override;

        private:
            void _resize(const glm::ivec2 &);
            void _redraw();
            void _hover(const std::shared_ptr<UI::Widget> &, Core::Event::PointerMove &, std::shared_ptr<Core::IObject> &);

            static void _resizeCallback(GLFWwindow*, int, int);
            static void _contentScaleCallback(GLFWwindow*, float, float);
            static void _redrawCallback(GLFWwindow *);
            static void _pointerCallback(GLFWwindow *, double, double);
            static void _buttonCallback(GLFWwindow *, int button, int action, int mods);
            static void _dropCallback(GLFWwindow *, int, const char **);
            static void _keyCallback(GLFWwindow *, int key, int scancode, int action, int modifiers);
            static void _charCallback(GLFWwindow *, unsigned int character, int modifiers);
            static void _scrollCallback(GLFWwindow *, double, double);

            DJV_PRIVATE();
        };

    } // namespace Desktop
} // namespace djv
