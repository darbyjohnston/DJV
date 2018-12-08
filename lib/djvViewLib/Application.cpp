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

#include <djvViewLib/Application.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/MainWindow.h>

#include <djvCore/Timer.h>

namespace djv
{
    namespace ViewLib
    {
        struct Application::Private
        {
            std::shared_ptr<Context> context;
            QScopedPointer<MainWindow> mainWindow;
            std::chrono::time_point<std::chrono::system_clock> time;
            int timer = 0;
        };
        
        Application::Application(int & argc, char ** argv) :
            QApplication(argc, argv),
            _p(new Private)
        {
            DJV_PRIVATE_PTR();
            p.context = Context::create(argc, argv);
            
            p.mainWindow.reset(new MainWindow(p.context));
            p.mainWindow->resize(800, 600);
            p.mainWindow->show();

            p.time = std::chrono::system_clock::now();
            p.timer = startTimer(Core::Timer::getValue(Core::Timer::Value::Fast), Qt::PreciseTimer);
        }
        
        Application::~Application()
        {
            _p->context->exit();
        }

        void Application::timerEvent(QTimerEvent * event)
        {
            DJV_PRIVATE_PTR();
            const auto now = std::chrono::system_clock::now();
            const std::chrono::duration<float> delta = now - p.time;
            p.time = now;
            const float dt = delta.count();
            p.context->tick(dt);
        }

    } // namespace ViewLib
} // namespace djv

