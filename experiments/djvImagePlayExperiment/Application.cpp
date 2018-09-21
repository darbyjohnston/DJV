//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <Application.h>

#include <Load.h>
#include <Playback.h>

#include <QTimer>

//------------------------------------------------------------------------------
// Application
//------------------------------------------------------------------------------

Application::Application(int & argc, char ** argv) :
    QApplication(argc, argv),
    _context(new Context)
{
    //DJV_DEBUG("Application");
    
    if (argc != 2)
    {
        _context->printMessage("Usage: djvImagePlayExperiment (input)");
        
        QTimer::singleShot(0, this, SLOT(commandLineExit()));
    }
    else
    {
        setStyle(new UI::ProxyStyle);
        
        _window.reset(new Window(_context.data()));
        _window->setWindowTitle("djvImagePlayExperiment");
        _window->show();
        
        djvFileInfo fileInfo(argv[1]);
        if (fileInfo.isSequenceValid())
        {
            fileInfo.setType(djvFileInfo::SEQUENCE);
        }
        
        _context->load()->setShareContext(_window->glContext());
        _context->load()->connect(
            this,
            SIGNAL(open(const djvFileInfo &)),
            SLOT(open(const djvFileInfo &)));
        _context->thread()->start();
        
        Q_EMIT open(fileInfo);
    }
}

Application::~Application()
{}

void Application::commandLineExit()
{
    exit(1);
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------

int main(int argc, char ** argv)
{
    return Application(argc, argv).exec();
}

