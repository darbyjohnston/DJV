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

#include <QTimer>

Application::Application(int & argc, char ** argv) :
    QApplication(argc, argv)
{
    //DJV_DEBUG("Application::Application");

    setStyle(new UI::ProxyStyle);

    // Create the context.
    _context.reset(new Context);

    // Parse the command line.
    if (! _context->commandLine(argc, argv))
    {
        QTimer::singleShot(0, this, SLOT(commandLineExit()));
    }
    else
    {
        QTimer::singleShot(0, this, SLOT(work()));
    }
}

void Application::commandLineExit()
{
    exit(1);
}

void Application::work()
{
    //DJV_DEBUG("Application::work");
    _imageLoad.reset(new ImageLoad(_context.data()));
    try
    {
        djvFileInfo fileInfo = _context->fileInfo();
        if (fileInfo.isSequenceValid())
        {
            fileInfo.setType(djvFileInfo::SEQUENCE);
        }
        _imageLoad->load(fileInfo);
    }
    catch (const djvError & error)
    {
        _context->printError(error);
        exit(1);
        return;
    }
    _context->setValid(true);
    _opsManager.reset(new OpsManager(_context.data()));
    _playback.reset(new Playback);
    _playback->setSequence(_imageLoad->info().sequence);
    _mainWindow.reset(new MainWindow(
        _imageLoad.data(),
        _opsManager.data(),
        _playback.data(),
        _context.data()));
    //_playback->setPlayback(Playback::FORWARD);
    _mainWindow->show();
}

int main(int argc, char ** argv)
{
    return Application(argc, argv).exec();
}

