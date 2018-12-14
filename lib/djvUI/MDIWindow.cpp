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

#include <djvUI/MDIWindow.h>

#include <djvUI/Button.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace MDI
        {
            struct Window::Private
            {
                std::shared_ptr<VerticalLayout> layout;
                std::shared_ptr<Label> titleLabel;
                std::shared_ptr<HorizontalLayout> titleBar;
                std::shared_ptr<StackLayout> stackLayout;
                std::shared_ptr<Icon> resizeHandle;
                std::shared_ptr<HorizontalLayout> bottomBar;
                std::function<void(void)> closedCallback;
            };

            void Window::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::MDI::Window");

                auto closeButton = Button::create(context);
                closeButton->setIcon(context->getPath(ResourcePath::IconsDirectory, "djvIconClose90DPI.png"));

                _p->titleLabel = Label::create(context);
                _p->titleLabel->setMargin(MetricsRole::Margin);
                
                _p->titleBar = HorizontalLayout::create(context);
                _p->titleBar->setClassName("djv::UI::MDI::TitleBar");
                _p->titleBar->setPointerEnabled(true);
                _p->titleBar->setBackgroundRole(ColorRole::BackgroundHeader);
                _p->titleBar->addWidget(_p->titleLabel);
                _p->titleBar->addExpander();
                _p->titleBar->addWidget(closeButton);

                _p->stackLayout = StackLayout::create(context);
                
                _p->resizeHandle = Icon::create(context);
                _p->resizeHandle->setPointerEnabled(true);
                _p->resizeHandle->setIcon(context->getPath(ResourcePath::IconsDirectory, "djvIconWindowResizeHandle90DPI.png"));

                _p->bottomBar = HorizontalLayout::create(context);
                _p->bottomBar->setPointerEnabled(true);
                _p->bottomBar->setBackgroundRole(ColorRole::BackgroundHeader);
                _p->bottomBar->addExpander();
                _p->bottomBar->addWidget(_p->resizeHandle);

                _p->layout = VerticalLayout::create(context);
                _p->layout->setSpacing(MetricsRole::None);
                _p->layout->addWidget(_p->titleBar);
                _p->layout->addWidget(_p->stackLayout, RowLayoutStretch::Expand);
                _p->layout->addWidget(_p->bottomBar);
                IContainerWidget::addWidget(_p->layout);

                auto weak = std::weak_ptr<Window>(std::dynamic_pointer_cast<Window>(shared_from_this()));
                closeButton->setClickedCallback(
                    [weak]
                {
                    if (auto window = weak.lock())
                    {
                        if (window->_p->closedCallback)
                        {
                            window->_p->closedCallback();
                        }
                    }
                });
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
            
            const std::string & Window::getTitle() const
            {
                return _p->titleLabel->getText();
            }
            
            void Window::setTitle(const std::string & text)
            {
                _p->titleLabel->setText(text);
            }
            
            std::shared_ptr<Widget> Window::getTitleBar() const
            {
                return _p->titleBar;
            }
            
            std::shared_ptr<Widget> Window::getBottomBar() const
            {
                return _p->bottomBar;
            }
            
            std::shared_ptr<Widget> Window::getResizeHandle() const
            {
                return _p->resizeHandle;
            }

            void Window::setClosedCallback(const std::function<void(void)> & value)
            {
                _p->closedCallback = value;
            }

            void Window::addWidget(const std::shared_ptr<Widget>& value)
            {
                _p->stackLayout->addWidget(value);
            }

            void Window::removeWidget(const std::shared_ptr<Widget>& value)
            {
                _p->stackLayout->addWidget(value);
            }

            void Window::clearWidgets()
            {
                _p->stackLayout->clearWidgets();
            }

            void Window::preLayoutEvent(PreLayoutEvent& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void Window::layoutEvent(LayoutEvent&)
            {
                _p->layout->setGeometry(getGeometry());
            }

        } // namespace MDI
    } // namespace UI
} // namespace djdv
