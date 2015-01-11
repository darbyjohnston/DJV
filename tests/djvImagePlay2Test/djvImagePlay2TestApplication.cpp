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

//! \file djvImagePlay2TestApplication.cpp

#include <djvImagePlay2TestApplication.h>

#include <djvError.h>
#include <djvErrorUtil.h>
#include <djvTimer.h>

djvImagePlay2TestApplication::djvImagePlay2TestApplication(
    int     argc,
    char ** argv) :
    djvApplication("djvImagePlay2Test", argc, argv),
    _frame (0),
    _image0(0),
    _image1(0)
{
    // Command line.

    if (argc != 2)
    {
        print("Usage: djvImagePlay2Test (input)");
        
        setExitValue(djvApplicationEnum::EXIT_DEFAULT);
        
        return;
    }

    _file.setFileName(argv[1]);

    if (_file.isSequenceValid())
    {
        _file.setType(djvFileInfo::SEQUENCE);
    }

    setValid(true);

    // Widgets.

    _widget = new djvImagePlay2TestWidget;
    _widget->setWindowTitle(name());

    // Initialize.

    _imageBuf.resize(20);
    
    int i = 0;

    for (; i < _imageBuf.count(); ++i)
    {
        _imageBuf[i].p = new djvImagePlay2TestImagePpm;
    }

    i = 0;

    for (; i < _imageBuf.count() - 1; ++i)
    {
        _imageBuf[i].next = &_imageBuf[i + 1];
    }

    _imageBuf[i].next = &_imageBuf[0];

    _image0 = &_imageBuf[0];
    _image0->p->load(_file.fileName(_file.sequence().frames[_frame]));
    _image0->frame = _frame;

    _image1 = &_imageBuf[0];
    
    startTimer(0);

    const djvVector2i size = _image0->p->info().size;
    
    _widget->resize(size.x, size.y);
    _widget->show();
}

djvImagePlay2TestApplication::~djvImagePlay2TestApplication()
{
    if (_widget)
    {
        delete _widget;
        
        _widget = 0;
    }

    for (int i = 0; i < _imageBuf.count(); ++i)
    {
        if (_imageBuf[i].p)
        {
            delete _imageBuf[i].p;
        }
    }
}

void djvImagePlay2TestApplication::timerEvent(QTimerEvent *)
{
    //DJV_DEBUG("djvImagePlay2TestApplication::timerEvent");

    static djvTimer t;
    static double average = 0.0;
    static int accum = 0;

    t.check();
    const double fps = t.fps();
    t.start();

    if (fps < 1000.0)
    {
        average += fps;
        ++accum;
    }

    print(QString("FPS = %1 (%2)").
        arg(fps).
        arg(average / static_cast<double>(accum)));

    try
    {
        //_image->load(_file.get(_file.sequence().list[_frame]));

        _widget->set(_image0->p);
        _widget->update();

        int frame = _image1->frame + 1;

        if (_imageBuf.count() > 1)
        {
            while (_image1->next != _image0 &&
                frame < _file.sequence().frames.count())
            {
                //DJV_DEBUG_PRINT("advance = " << frame);

                _image1 = _image1->next;
                _image1->p->load(_file.fileName(_file.sequence().frames[frame]));
                _image1->frame = frame;

                ++frame;
            }
        }
        else
        {
            if (frame < _file.sequence().frames.count())
            {
                _image1 = _image1->next;
                _image1->p->load(_file.fileName(_file.sequence().frames[frame]));
                _image1->frame = frame;
            }
        }

        _image0 = _image0->next;
    }
    catch (const djvError & error)
    {
        printError(error);
    }

    ++_frame;

    if (_frame >= _file.sequence().frames.count())
    {
        setExitValue(djvApplicationEnum::EXIT_DEFAULT);
        
        quit();
    }
}

int main(int argc, char ** argv)
{
    int r = 1;

    try
    {
        r = djvImagePlay2TestApplication(argc, argv).run();
    }
    catch (const djvError & error)
    {
        djvErrorUtil::print(error);
    }

    return r;
}

