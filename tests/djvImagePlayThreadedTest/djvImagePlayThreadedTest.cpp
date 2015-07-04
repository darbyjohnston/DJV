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

//! \file djvImagePlayThreadedTest.cpp

#include <djvImagePlayThreadedTest.h>

#include <djvViewMiscWidget.h>

#include <djvWindowUtil.h>

#include <djvDebug.h>
#include <djvError.h>
#include <djvOpenGlImage.h>
#include <djvPixel.h>
#include <djvTimer.h>

#include <QCoreApplication>
#include <QCursor>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSlider>
#include <QTimer>
#include <QToolButton>

//------------------------------------------------------------------------------
// djvImagePlayThreadedTestLoad
//------------------------------------------------------------------------------

djvImagePlayThreadedTestLoad::djvImagePlayThreadedTestLoad(djvImageContext * context) :
    _context (context),
    _frame   (0),
    _frameTmp(0),
    _accum   (0)
{
    startTimer(0);
}

void djvImagePlayThreadedTestLoad::open(const djvFileInfo & fileInfo)
{
    try
    {
        _imageLoad.reset(
            _context->imageIoFactory()->load(fileInfo, _imageIoInfo));
        
        _fileInfo = fileInfo;
    }
    catch (const djvError & error)
    {
    }

    Q_EMIT fileChanged(_imageIoInfo);
}

void djvImagePlayThreadedTestLoad::read(qint64 frame)
{
    _frameTmp = frame;
    
    ++_accum;
}

void djvImagePlayThreadedTestLoad::timerEvent(QTimerEvent *)
{
    qApp->processEvents();
    
    if (_frameTmp != _frame)
    {
        _frame = _frameTmp;
        
        readInternal(_frame);
    }
}

void djvImagePlayThreadedTestLoad::readInternal(qint64 frame)
{
    if (! _imageLoad.data())
        return;

    //DJV_DEBUG("djvImagePlayThreadedTestLoad::readInternal");
    //DJV_DEBUG_PRINT("frame = " << frame);
    //DJV_DEBUG_PRINT("accum = " << _accum);
    
    try
    {
        _imageLoad->read(_image, _frame);
    }
    catch (const djvError & error)
    {
    }
    
    _accum = 0;
    
    Q_EMIT imageRead(_image);
}

//------------------------------------------------------------------------------
// djvImagePlayThreadedTestPlayback
//------------------------------------------------------------------------------

djvImagePlayThreadedTestPlayback::djvImagePlayThreadedTestPlayback(QObject * parent) :
    QObject(parent),
    _frame(0)
{}

qint64 djvImagePlayThreadedTestPlayback::frame() const
{
    return _frame;
}
    
void djvImagePlayThreadedTestPlayback::setFrame(qint64 frame)
{
    if (frame == _frame)
        return;

    _frame = frame;
    
    Q_EMIT frameChanged(_frame);
}

//------------------------------------------------------------------------------
// djvImagePlayThreadedTestShader
//------------------------------------------------------------------------------

djvImagePlayThreadedTestShader::djvImagePlayThreadedTestShader() :
    _vertexId  (0),
    _fragmentId(0),
    _program   (0)
{}

djvImagePlayThreadedTestShader::~djvImagePlayThreadedTestShader()
{
    del();
}

namespace
{

void shaderCompile(GLuint id, const QString & source)
{
    //DJV_DEBUG("shaderCompile");
    //DJV_DEBUG_PRINT("source = " << source);

    djvMemoryBuffer<char> buf(source.length());
    djvMemory::copy(source.toLatin1().data(), buf.data(), buf.size());

    const char * sources       [] = { buf.data() };
    const GLint  sourceLengths [] = { buf.size() };

    DJV_DEBUG_OPEN_GL(glShaderSource(id, 1, sources, sourceLengths));

    DJV_DEBUG_OPEN_GL(glCompileShader(id));

    GLint error = GL_FALSE;

    glGetShaderiv(id, GL_COMPILE_STATUS, &error);

    char log [4096] = "";
    GLsizei logSize = 0;
    glGetShaderInfoLog(id, 4096, &logSize, log);

    //DJV_DEBUG_PRINT("log = " << QString(log));
}

} // namespace

void djvImagePlayThreadedTestShader::init(const QString & vertex, const QString & fragment)
{
    if (vertex == _vertex && fragment == _fragment)
        return;

    //DJV_DEBUG("djvImagePlayThreadedTestShader::init");
    //DJV_DEBUG_PRINT("fragment = " << fragment);

    del();

    _vertex   = vertex;
    _fragment = fragment;

    GLint error = GL_FALSE;

    _vertexId   = glCreateShader(GL_VERTEX_SHADER);
    _fragmentId = glCreateShader(GL_FRAGMENT_SHADER);

    shaderCompile(_vertexId,   _vertex);
    shaderCompile(_fragmentId, _fragment);

    DJV_DEBUG_OPEN_GL(_program = glCreateProgram());

    DJV_DEBUG_OPEN_GL(glAttachShader(_program, _vertexId));
    DJV_DEBUG_OPEN_GL(glAttachShader(_program, _fragmentId));
    DJV_DEBUG_OPEN_GL(glLinkProgram (_program));

    glGetProgramiv(_program, GL_LINK_STATUS, &error);

    char log [4096] = "";
    GLsizei logSize = 0;
    glGetProgramInfoLog(_program, 4096, &logSize, log);

    //DJV_DEBUG_PRINT("log = " << QString(log));
}

void djvImagePlayThreadedTestShader::bind()
{
    //DJV_DEBUG("djvImagePlayThreadedTestShader::bind");

    DJV_DEBUG_OPEN_GL(glUseProgram(_program));
}

GLuint djvImagePlayThreadedTestShader::program() const
{
    return _program;
}

void djvImagePlayThreadedTestShader::del()
{
    if (_vertexId)
    {
        glDeleteShader(_vertexId);
    }

    if (_fragmentId)
    {
        glDeleteShader(_fragmentId);
    }

    if (_program)
    {
        glDeleteProgram(_program);
    }
}

//------------------------------------------------------------------------------
// djvImagePlayThreadedTestView
//------------------------------------------------------------------------------

djvImagePlayThreadedTestView::djvImagePlayThreadedTestView(djvGuiContext * context) :
    _context(context),
    _shader (new djvImagePlayThreadedTestShader),
    _id     (0)
{}

djvImagePlayThreadedTestView::~djvImagePlayThreadedTestView()
{
    if (_id)
    {
        glDeleteTextures(1, &_id);
    }
}

void djvImagePlayThreadedTestView::setImage(const djvImage & image)
{
    //DJV_DEBUG("djvImagePlayThreadedTestView::setImage");
    //DJV_DEBUG_PRINT("image = " << image.info());
    
    if (image.info() != _info)
    {
        _info = image.info();
        
        if (_id)
        {
            glDeleteTextures(1, &_id);
        }
        
        DJV_DEBUG_OPEN_GL(glGenTextures(1, &_id));
        
        //DJV_DEBUG_PRINT("id = " << _id);
        
        DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_2D, _id));
        DJV_DEBUG_OPEN_GL(
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        DJV_DEBUG_OPEN_GL(
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        DJV_DEBUG_OPEN_GL(
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        DJV_DEBUG_OPEN_GL(
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GLenum format = GL_RGBA;

        if (djvPixel::F16 == djvPixel::type(_info.pixel))
        {
            format = GL_RGBA16F;
        }
        else if (djvPixel::F32 == djvPixel::type(_info.pixel))
        {
            format = GL_RGBA32F;
        }

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            _info.size.x,
            _info.size.y,
            0,
            djvOpenGlUtil::format(_info.pixel, _info.bgr),
            djvOpenGlUtil::type(_info.pixel),
            0);
    }
    
    DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_2D, _id));
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, _info.align);
    glPixelStorei(GL_UNPACK_SWAP_BYTES, _info.endian != djvMemory::endian());

    DJV_DEBUG_OPEN_GL(
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            _info.size.x,
            _info.size.y,
            djvOpenGlUtil::format(_info.pixel, _info.bgr),
            djvOpenGlUtil::type(_info.pixel),
            image.data()));
    
    updateGL();
}

void djvImagePlayThreadedTestView::initializeGL()
{
    //DJV_DEBUG("djvImagePlayThreadedTestView::initializeGL");
    
    _shader->init(
        "void main(void)\n"
        "{\n"
        "    gl_FrontColor  = gl_Color;\n"
        "    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
        "    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
        "}\n",
        
        "uniform sampler2D inTexture;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    vec4 color;\n"
        "\n"
        "    color = texture2D(inTexture, gl_TexCoord[0].st);"
        "\n"
        "    gl_FragColor = color;\n"
        "}\n");
}

void djvImagePlayThreadedTestView::paintGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    djvOpenGlUtil::ortho(djvVector2i(width(), height()));
    glViewport(0, 0, width(), height());

    _shader->bind();

    glActiveTexture(GL_TEXTURE0);

    glUniform1f(
        glGetUniformLocation(_shader->program(), "inTexture"),
        0);
    
    DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_2D, _id));

    double u [] = { 0.0, 1.0 };
    double v [] = { 0.0, 1.0 };

    const djvVector2f uv[] =
    {
        djvVector2f(u[0], v[0]),
        djvVector2f(u[0], v[1]),
        djvVector2f(u[1], v[1]),
        djvVector2f(u[1], v[0])
    };

    glBegin(GL_QUADS);

    djvOpenGlUtil::drawBox(_info.size, uv);

    glEnd();
}

//------------------------------------------------------------------------------
// djvImagePlayThreadedTestWindow
//------------------------------------------------------------------------------

djvImagePlayThreadedTestWindow::djvImagePlayThreadedTestWindow(
    djvImagePlayThreadedTestLoad *     load,
    djvImagePlayThreadedTestPlayback * playback,
    djvGuiContext *                    context) :
    _context    (context),
    _view       (0),
    _frameWidget(0),
    _frameSlider(0)
{
    _view = new djvImagePlayThreadedTestView(_context);
    
    _frameWidget = new djvViewFrameWidget(_context);
    
    _frameSlider = new djvViewFrameSlider(_context);
    
    QWidget * playBar = new QWidget;
    QHBoxLayout * hLayout = new QHBoxLayout(playBar);
    hLayout->addWidget(_frameWidget);
    hLayout->addWidget(_frameSlider);
    
    QGridLayout * layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->addWidget(_view, 1, 1);
    layout->addWidget(playBar, 2, 0, 1, 3);
    layout->setRowStretch(1, 1);
    layout->setColumnStretch(1, 1);
    
    connect(
        load,
        SIGNAL(fileChanged(const djvImageIoInfo &)),
        SLOT(fileCallback(const djvImageIoInfo &)));
    
    connect(
        load,
        SIGNAL(imageRead(const djvImage &)),
        SLOT(imageCallback(const djvImage &)));
    
    connect(
        playback,
        SIGNAL(frameChanged(qint64)),
        SLOT(updateWidget(qint64)));
    
    playback->connect(
        _frameWidget,
        SIGNAL(frameChanged(qint64)),
        SLOT(setFrame(qint64)));
    
    playback->connect(
        _frameSlider,
        SIGNAL(frameChanged(qint64)),
        SLOT(setFrame(qint64)));
}

djvImagePlayThreadedTestWindow::~djvImagePlayThreadedTestWindow()
{
}

void djvImagePlayThreadedTestWindow::fileCallback(const djvImageIoInfo & info)
{
    resize(info.size.x, info.size.y);
    
    _frameWidget->setFrameList(info.sequence.frames);
    _frameWidget->setSpeed(info.sequence.speed);

    _frameSlider->setFrameList(info.sequence.frames);
    _frameSlider->setSpeed(info.sequence.speed);
}

void djvImagePlayThreadedTestWindow::imageCallback(const djvImage & image)
{
    _view->setImage(image);
}

void djvImagePlayThreadedTestWindow::updateWidget(qint64 frame)
{
    _frameWidget->setFrame(frame);
    _frameSlider->setFrame(frame);
}

//------------------------------------------------------------------------------
// djvImagePlayThreadedTestApplication
//------------------------------------------------------------------------------

djvImagePlayThreadedTestApplication::djvImagePlayThreadedTestApplication(int & argc, char ** argv) :
    QApplication(argc, argv),
    _load(0)
{
    //DJV_DEBUG("djvImagePlayThreadedTestApplication");
    
    _context.reset(new djvGuiContext);

    if (argc != 2)
    {
        _context->printMessage("Usage: djvImagePlayThreadedTest (input)");
        
        QTimer::singleShot(0, this, SLOT(commandLineExit()));
    }
    else
    {
#       if QT_VERSION < 0x050000
        setStyle(new QPlastiqueStyle);
#       else
        setStyle("fusion");
#       endif

        _load = new djvImagePlayThreadedTestLoad(_context.data());
        _load->moveToThread(&_thread);
        _thread.start();
        
        _playback.reset(new djvImagePlayThreadedTestPlayback);
        
        _window.reset(new djvImagePlayThreadedTestWindow(
            _load, _playback.data(), _context.data()));
        _window->setWindowTitle("djvImagePlayThreadedTest");
        _window->show();
        
        _load->connect(
            _playback.data(),
            SIGNAL(frameChanged(qint64)),
            SLOT(read(qint64)));

        djvFileInfo fileInfo(argv[1]);
    
        if (fileInfo.isSequenceValid())
        {
            fileInfo.setType(djvFileInfo::SEQUENCE);
        }
        
        _load->open(fileInfo);
    }
}

djvImagePlayThreadedTestApplication::~djvImagePlayThreadedTestApplication()
{
    _thread.quit();
    _thread.wait();
}

void djvImagePlayThreadedTestApplication::commandLineExit()
{
    exit(1);
}

int main(int argc, char ** argv)
{
    return djvImagePlayThreadedTestApplication(argc, argv).exec();
}

