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

#include <Dir.h>
#include <Info.h>
#include <Thumbnail.h>
#include <Util.h>
#include <Widget.h>

#include <djvError.h>

#include <QTimer>

//------------------------------------------------------------------------------
// Application
//------------------------------------------------------------------------------

Application::Application(int & argc, char ** argv) :
    QApplication(argc, argv)
{
    setStyle(new UI::ProxyStyle);
    
    qRegisterMetaType<DirRequest>("DirRequest");
    qRegisterMetaType<DirResult>("DirResult");
    qRegisterMetaType<InfoRequest>("InfoRequest");
    qRegisterMetaType<InfoResult>("InfoResult");
    qRegisterMetaType<ThumbnailRequest>("ThumbnailRequest");
    qRegisterMetaType<ThumbnailResult>("ThumbnailResult");
    qRegisterMetaType<Util::THUMBNAILS>("Util::THUMBNAILS");

    _context.reset(new djvUIContext);
    
    if (argc != 2)
    {
        _context->printMessage("Usage: djvFileBrowserExperiment (path)");
        QTimer::singleShot(0, this, SLOT(commandLineExit()));
    }
    else
    {
        _path = argv[1];
        QTimer::singleShot(0, this, SLOT(work()));
    }
}

void Application::commandLineExit()
{
    exit(1);
}

void Application::work()
{
    _widget.reset(new Widget(_context.data()));
    _widget->setPath(_path);
    _widget->show();
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------

int main(int argc, char ** argv)
{
    return Application(argc, argv).exec();
}

