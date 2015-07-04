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

//! \file djvImagePlayThreadedTest.h

#include <djvGuiContext.h>
#include <djvOpenGlWidget.h>

#include <djvFileInfo.h>
#include <djvImage.h>
#include <djvImageIo.h>

#include <QApplication>
#include <QScopedPointer>
#include <QThread>

class djvViewFrameSlider;

//------------------------------------------------------------------------------
// djvImagePlayThreadedTestLoad
//------------------------------------------------------------------------------

class djvImagePlayThreadedTestLoad : public QObject
{
    Q_OBJECT

public:

    explicit djvImagePlayThreadedTestLoad(djvImageContext *);
    
public Q_SLOTS:

    void open(const djvFileInfo &);
    
    void read(qint64);

Q_SIGNALS:

    void fileChanged(const djvImageIoInfo &);

    void imageRead(const djvImage &);

protected:

    virtual void timerEvent(QTimerEvent *);
    
private:
    
    void readInternal(qint64);

    djvImageContext *            _context;
    djvFileInfo                  _fileInfo;
    QScopedPointer<djvImageLoad> _imageLoad;
    djvImageIoInfo               _imageIoInfo;
    djvImage                     _image;
    qint64                       _frame;
};

//------------------------------------------------------------------------------
// djvImagePlayThreadedTestShader
//------------------------------------------------------------------------------

class djvImagePlayThreadedTestShader
{
public:

    djvImagePlayThreadedTestShader();

    ~djvImagePlayThreadedTestShader();

    void init(const QString & vertex, const QString & fragment);

    void bind();

    GLuint program() const;

private:

    void del();

    QString _vertex;
    QString _fragment;
    GLuint  _vertexId;
    GLuint  _fragmentId;
    GLuint  _program;
};

//------------------------------------------------------------------------------
// djvImagePlayThreadedTestView
//------------------------------------------------------------------------------

class djvImagePlayThreadedTestView : public djvOpenGlWidget
{
    Q_OBJECT

public:

    explicit djvImagePlayThreadedTestView(djvGuiContext *);
    
    virtual ~djvImagePlayThreadedTestView();
    
public Q_SLOTS:

    void setImage(const djvImage &);

protected:

    virtual void initializeGL();
    virtual void paintGL();

private:

    djvGuiContext *                                _context;
    QScopedPointer<djvImagePlayThreadedTestShader> _shader;
    djvPixelDataInfo                               _info;
    GLuint                                         _id;
};

//------------------------------------------------------------------------------
// djvImagePlayThreadedTestWindow
//------------------------------------------------------------------------------

class djvImagePlayThreadedTestWindow : public QWidget
{
    Q_OBJECT

public:

    explicit djvImagePlayThreadedTestWindow(
        djvImagePlayThreadedTestLoad *,
        djvGuiContext *);
    
    virtual ~djvImagePlayThreadedTestWindow();

private Q_SLOTS:

    void fileCallback(const djvImageIoInfo &);
    void imageCallback(const djvImage &);

private:

    djvGuiContext *                _context;
    djvImagePlayThreadedTestLoad * _load;
    djvImagePlayThreadedTestView * _view;
    djvViewFrameSlider *           _frameSlider;
};

//------------------------------------------------------------------------------
// djvImagePlayThreadedTestApplication
//------------------------------------------------------------------------------

class djvImagePlayThreadedTestApplication : public QApplication
{
    Q_OBJECT
    
public:

    djvImagePlayThreadedTestApplication(int & argc, char ** argv);
    
    virtual ~djvImagePlayThreadedTestApplication();

Q_SIGNALS:

    void frameChanged(qint64);

private Q_SLOTS:

    void commandLineExit();

private:

    QScopedPointer<djvGuiContext>                  _context;
    djvImagePlayThreadedTestLoad *                 _load;
    QThread                                        _thread;
    QScopedPointer<djvImagePlayThreadedTestWindow> _window;
};
