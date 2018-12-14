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
            std::shared_ptr<StackLayout> layout;
        };

        void Window::_init(Context * context)
        {
            IContainerWidget::_init(context);
            
            setClassName("djv::UI::Window");
            setVisible(false);
            setBackgroundRole(ColorRole::Background);

            _p->layout = StackLayout::create(context);
            IContainerWidget::addWidget(_p->layout);

            if (auto system = context->getSystemT<IWindowSystem>().lock())
            {
                system->_addWindow(std::dynamic_pointer_cast<Window>(shared_from_this()));
            }
        }

        Window::Window() :
            _p(new Private)
        {}

        Window::~Window()
        {
            if (auto system = getContext()->getSystemT<IWindowSystem>().lock())
            {
                system->_removeWindow(std::dynamic_pointer_cast<Window>(shared_from_this()));
            }
        }

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

        void Window::raiseToTop()
        {
            if (auto system = getContext()->getSystemT<IWindowSystem>().lock())
            {
                system->raiseToTop(std::dynamic_pointer_cast<Window>(shared_from_this()));
            }
        }

        void Window::preLayoutEvent(PreLayoutEvent&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void Window::layoutEvent(LayoutEvent&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void Window::pointerEnterEvent(Core::PointerEnterEvent& event)
        {
            event.accept();
        }

        void Window::pointerLeaveEvent(Core::PointerLeaveEvent& event)
        {
            event.accept();
        }

        void Window::pointerMoveEvent(Core::PointerMoveEvent& event)
        {
            event.accept();
        }

    } // namespace UI
} // namespace djv
