//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvWglContextPrivate.cpp

//#define DJV_DEBUG

#include <djvWglContextPrivate.h>

#include <djvDebug.h>
#include <djvDebugLog.h>
#include <djvError.h>

#include <QStringList>

//------------------------------------------------------------------------------
// djvWglContextPrivate
//------------------------------------------------------------------------------

djvWglContextPrivate::djvWglContextPrivate() throw (djvError) :
    _id     (0),
    _device (0),
    _context(0)
{
    //DJV_DEBUG("djvWglContextPrivate::djvWglContextPrivate");

    // XXX Create a dummy window and OpenGL context for glewInit.
    // According to the docs, glewInit can be called just once per-process?

    DJV_LOG("djvWglContextPrivate", "Creating dummy window...");

    HINSTANCE hinstance = GetModuleHandle(0);

    if (! hinstance)
    {
        DJV_THROW_ERROR(
            QString("GetModuleHandle: #%1").arg(int(GetLastError())));
    }
    
    static const char name [] = "djv";
    WNDCLASS wc;
    if (! GetClassInfo(hinstance, name, &wc))
    {
        wc.style = CS_OWNDC;
        //wc.lpfnWndProc = (WNDPROC)MainWndProc;
        wc.lpfnWndProc = DefWindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hinstance;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = 0;
        wc.lpszMenuName = 0;
        wc.lpszClassName = name;

        if (! RegisterClass(&wc))
        {
            DJV_THROW_ERROR(
                QString("RegisterClass: #%1").arg(int(GetLastError())));
        }
    }

    _id = CreateWindow(name, 0, 0, 0, 0, 0, 0, 0, 0, hinstance, 0);

    if (! _id)
    {
        DJV_THROW_ERROR(QString("CreateWindow: #%1").arg(int(GetLastError())));
    }

    _device = GetDC(_id);

    if (! _device)
    {
        DJV_THROW_ERROR(QString("GetDC: #%1").arg(int(GetLastError())));
    }

    PIXELFORMATDESCRIPTOR pixelFormatInfo;

    const int pixelFormatCount = DescribePixelFormat(_device, 0, 0, 0);

    //DJV_DEBUG_PRINT("pixel format count = " << pixelFormatCount);

    DJV_LOG("djvWglContextPrivate",
        QString("Pixel format count: %1").arg(pixelFormatCount));

    for (int i = 1; i < pixelFormatCount; ++i)
    {
        DescribePixelFormat(_device, i, sizeof(PIXELFORMATDESCRIPTOR),
            &pixelFormatInfo);

        //DJV_DEBUG_PRINT("  id " << i << ": " <<
        //    ((PFD_SUPPORT_OPENGL & pixelFormatInfo.dwFlags) ? "gl " : "") <<
        //    ((PFD_GENERIC_FORMAT & pixelFormatInfo.dwFlags) ? "" : "accel ") <<
        //    ((PFD_TYPE_RGBA == pixelFormatInfo.iPixelType) ? "rgba " : "") <<
        //    "depth = " << pixelFormatInfo.cColorBits << "/" <<
        //    pixelFormatInfo.cRedBits << "/" <<
        //    pixelFormatInfo.cGreenBits << "/" <<
        //    pixelFormatInfo.cBlueBits << "/" <<
        //    pixelFormatInfo.cAlphaBits << " ");

        QStringList tmp;
        if (PFD_SUPPORT_OPENGL & pixelFormatInfo.dwFlags)
            tmp += "gl";
        if (! (PFD_GENERIC_FORMAT & pixelFormatInfo.dwFlags))
            tmp += "accel";
        if (PFD_TYPE_RGBA == pixelFormatInfo.iPixelType)
            tmp += "rgba";

        DJV_LOG("djvWglContextPrivate",
            QString("Pixel format %1: %2 %3/%4/%5/%6/%7").
            arg(i).
            arg(tmp.join(" ")).
            arg(pixelFormatInfo.cColorBits).
            arg(pixelFormatInfo.cRedBits).
            arg(pixelFormatInfo.cGreenBits).
            arg(pixelFormatInfo.cBlueBits).
            arg(pixelFormatInfo.cAlphaBits));
    }

    PIXELFORMATDESCRIPTOR pixelFormat =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        0,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    int pixelFormatId = ChoosePixelFormat(_device, &pixelFormat);

    //DJV_DEBUG_PRINT("pixel format = " << pixelFormatId);

    DJV_LOG("djvWglContextPrivate",
        QString("Chosen pixel format: %1").arg(pixelFormatId));

    if (! pixelFormatId)
    {
        DJV_THROW_ERROR(
            QString("ChoosePixelFormat: #%1").arg(int(GetLastError())));
    }

    if (! SetPixelFormat(_device, pixelFormatId, &pixelFormat))
    {
        DJV_THROW_ERROR(
            QString("SetPixelFormat: #%1").arg(int(GetLastError())));
    }

    // Create OpengGL context.

    DJV_LOG("djvWglContextPrivate", "Creating OpenGL context...");

    _context = wglCreateContext(_device);

    if (! _context)
    {
        DJV_THROW_ERROR(
            QString("Cannot create context: #%1").arg(int(GetLastError())));
    }

    if (! wglMakeCurrent(_device, _context))
    {
        DJV_THROW_ERROR(
            QString("Cannot bind context: #%1").arg(int(GetLastError())));
    }

    // Initialize GLEW.

    DJV_LOG("djvWglContextPrivate", "Initializing GLEW...");

    GLenum err = glewInit();

    if (err != GLEW_OK)
    {
        DJV_THROW_ERROR(QString("Cannot initialize: #%1").
            arg((char *)glewGetErrorString(err)));
    }

    setVendor(QString((const char *)glGetString(GL_VENDOR)));
    setRenderer(QString((const char *)glGetString(GL_RENDERER)));
    setVersion(QString((const char *)glGetString(GL_VERSION)));

    //DJV_DEBUG_PRINT("OpenGL vendor string = " << vendor());
    //DJV_DEBUG_PRINT("OpenGL renderer string = " << renderer());
    //DJV_DEBUG_PRINT("OpenGL version string = " << version());
    //DJV_DEBUG_PRINT("OpenGL extensions = " <<
    //    (const char *)glGetString(GL_EXTENSIONS));
    //DJV_DEBUG_PRINT("glu version = " <<
    //    (const char *)gluGetString(GLU_VERSION));
    //DJV_DEBUG_PRINT("glu extensions = " <<
    //    (const char *)gluGetString(GLU_EXTENSIONS));

    DJV_LOG("djvWglContextPrivate", QString("GL vendor: \"%1\"").arg(vendor()));
    DJV_LOG("djvWglContextPrivate", QString("GL renderer: \"%1\"").arg(renderer()));
    DJV_LOG("djvWglContextPrivate", QString("GL version: \"%1\"").arg(version()));
}

djvWglContextPrivate::~djvWglContextPrivate()
{
    //DJV_DEBUG("djvWglContextPrivate::~djvWglContextPrivate");
  
    if (_context)
    {
        //DJV_DEBUG_PRINT("context");

        wglDeleteContext(_context);
    }
  
    if (_device)
    {
        //DJV_DEBUG_PRINT("device");

        ReleaseDC(_id, _device);
    }
}

void djvWglContextPrivate::bind() throw (djvError)
{
    if (! wglMakeCurrent(_device, _context))
    {
        DJV_THROW_ERROR(
            QString("Cannot bind context: #%1").arg(int(GetLastError())));
    }
}

void djvWglContextPrivate::unbind()
{
    if (! wglMakeCurrent(_device, 0))
    {
        DJV_THROW_ERROR(
            QString("Cannot unbind context: #%1").arg(int(GetLastError())));
    }
}
