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

#include <djvUI/MainWindow.h>

#include <djvUI/MenuBar.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct MainWindow::Private
        {
            std::shared_ptr<MenuBar> menuBar;
            std::shared_ptr<Widget> centralWidget;
            std::shared_ptr<Layout::Stack> childLayout;
            std::shared_ptr<VerticalLayout> layout;
        };

        void MainWindow::_init(Context * context)
        {
            Window::_init(context);

            setClassName("djv::UI::MainWindow");

            _p->menuBar = MenuBar::create(context);

            _p->childLayout = Layout::Stack::create(context);
            
            _p->layout = VerticalLayout::create(context);
            _p->layout->setSpacing(Style::MetricsRole::None);
            _p->layout->addWidget(_p->menuBar);
            _p->layout->addSeparator();
            _p->layout->addWidget(_p->childLayout, Layout::RowStretch::Expand);
            Window::addWidget(_p->layout);
        }

        MainWindow::MainWindow() :
            _p(new Private)
        {}

        MainWindow::~MainWindow()
        {}

        std::shared_ptr<MainWindow> MainWindow::create(Context * context)
        {
            auto out = std::shared_ptr<MainWindow>(new MainWindow);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<MenuBar> & MainWindow::getMenuBar() const
        {
            return _p->menuBar;
        }

        const std::shared_ptr<Widget> & MainWindow::getCentralWidget() const
        {
            return _p->centralWidget;
        }

        void MainWindow::setCentralWidget(const std::shared_ptr<Widget> & centralWidget)
        {
            if (_p->centralWidget)
            {
                _p->childLayout->removeWidget(_p->centralWidget);
            }
            _p->centralWidget = centralWidget;
            if (_p->centralWidget)
            {
                _p->childLayout->addWidget(_p->centralWidget);
            }
        }

    } // namespace UI
} // namespace djv
