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

#include <djvUI/Window.h>

#include <djvUI/IWindowSystem.h>
#include <djvUI/StackLayout.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Window::Private
        {
            std::shared_ptr<Layout::Stack> layout;
        };

        void Window::_init(Context * context)
        {
            IContainer::_init(context);
            
            setClassName("djv::UI::Window");
            setVisible(false);
            setBackgroundRole(Style::ColorRole::Background);
            setPointerEnabled(true);

            _p->layout = Layout::Stack::create(context);
            IContainer::addWidget(_p->layout);

            if (auto system = context->getSystemT<IWindowSystem>().lock())
            {
                system->_addWindow(std::dynamic_pointer_cast<Window>(shared_from_this()));
            }
        }

        Window::Window() :
            _p(new Private)
        {}

        Window::~Window()
        {}

        std::shared_ptr<Window> Window::create(Context * context)
        {
            auto out = std::shared_ptr<Window>(new Window);
            out->_init(context);
            return out;
        }

        void Window::addWidget(const std::shared_ptr<Widget>& value)
        {
            _p->layout->addWidget(value);
        }

        void Window::removeWidget(const std::shared_ptr<Widget>& value)
        {
            _p->layout->removeWidget(value);
        }

        void Window::clearWidgets()
        {
            _p->layout->clearWidgets();
        }

        void Window::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void Window::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv
