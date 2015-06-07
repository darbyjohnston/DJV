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

//! \file djvGlslTestApplication.h

#ifndef DJV_GLSL_TEST_APPLICATION_H
#define DJV_GLSL_TEST_APPLICATION_H

#include <djvGlslTestContext.h>
#include <djvGlslTestImageLoad.h>
#include <djvGlslTestOpManager.h>
#include <djvGlslTestPlayback.h>
#include <djvGlslTestWindow.h>

#include <QApplication>

class djvGlslTestOp;
class djvGlslTestWidget;

class djvIntSlider;
class djvToolButton;

//------------------------------------------------------------------------------
// djvGlslTestApplication
//------------------------------------------------------------------------------

class djvGlslTestApplication : public QApplication
{
    Q_OBJECT
    
public:

    djvGlslTestApplication(int & argc, char ** argv);
    
private Q_SLOTS:

    void commandLineExit();
    void work();
    /*void playCallback(bool);
    void frameCallback(int);
    void currentImageOpCallback(int);
    void updateCallback();*/

private:

    /*void playUpdate();
    void frameUpdate();
    void update();*/

    QScopedPointer<djvGlslTestContext>   _context;
    QScopedPointer<djvGlslTestImageLoad> _imageLoad;
    QScopedPointer<djvGlslTestOpManager> _opManager;
    QScopedPointer<djvGlslTestPlayback>  _playback;
    QScopedPointer<djvGlslTestWindow>    _window;
    
    djvImage _image;
    
    /*QScopedPointer<djvImageLoad>  _imageLoad;
    djvImageIoInfo                _imageLoadInfo;
    djvImage                      _image;
    bool                          _play;
    int                           _frame;
    int                           _timerId;
    QVector<djvGlslTestOp *>      _imageOpList;
    int                           _currentImageOp;
    QWidget *                     _window;
    djvGlslTestWidget *           _view;
    djvToolButton *               _playWidget;
    djvIntSlider *                _slider;*/
};

#endif // DJV_GLSL_TEST_APPLICATION_H
