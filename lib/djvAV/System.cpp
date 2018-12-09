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

#include <djvAV/System.h>

#include <djvAV/AudioSystem.h>
#include <djvAV/FontSystem.h>
#include <djvAV/IconSystem.h>
#include <djvAV/IO.h>
#include <djvAV/OpenGL.h>

#include <djvCore/Error.h>
#include <djvCore/OS.h>
#include <djvCore/String.h>

#include <QGuiApplication>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QScopedPointer>
#include <QScreen>

namespace djv
{
    namespace AV
    {
        struct System::Private
        {
            QScopedPointer<QOffscreenSurface> offscreenSurface;
            QScopedPointer<QOpenGLContext> openGLContext;
            QScopedPointer<QOpenGLDebugLogger> openGLDebugLogger;
        };

        void System::_init(const std::shared_ptr<Core::Context> & context)
        {
            Core::ISystem::_init("djv::AV::System", context);

            // Create the default OpenGL context.
            QSurfaceFormat defaultFormat;
            defaultFormat.setRenderableType(QSurfaceFormat::OpenGL);
            defaultFormat.setMajorVersion(4);
            defaultFormat.setMinorVersion(1);
            defaultFormat.setProfile(QSurfaceFormat::CoreProfile);
            //! \todo Document this environment variable.
            if (!Core::OS::getEnv("DJV_OPENGL_DEBUG").empty())
            {
                defaultFormat.setOption(QSurfaceFormat::DebugContext);
            }
            QSurfaceFormat::setDefaultFormat(defaultFormat);

            DJV_PRIVATE_PTR();
            p.offscreenSurface.reset(new QOffscreenSurface);
            QSurfaceFormat surfaceFormat = QSurfaceFormat::defaultFormat();
            surfaceFormat.setSwapBehavior(QSurfaceFormat::SingleBuffer);
            surfaceFormat.setSamples(1);
            p.offscreenSurface->setFormat(surfaceFormat);
            p.offscreenSurface->create();
            p.openGLContext.reset(new QOpenGLContext);
            p.openGLContext->setFormat(surfaceFormat);
            if (!p.openGLContext->create())
            {
                std::stringstream ss;
                ss << "djv::AV::System: " << DJV_TEXT("cannot create OpenGL context; found version") << " " <<
                    p.openGLContext->format().majorVersion() << "." << p.openGLContext->format().minorVersion();
                throw std::runtime_error(ss.str());
            }

            p.openGLContext->makeCurrent(p.offscreenSurface.data());
            std::stringstream ss;
            ss << "OpenGL context valid = " << p.openGLContext->isValid();
            _log(ss.str());
            ss.str(std::string());
            ss << "OpenGL version = " <<
                p.openGLContext->format().majorVersion() << "." <<
                p.openGLContext->format().minorVersion();
            _log(ss.str());
            if (!p.openGLContext->versionFunctions<QOpenGLFunctions_3_3_Core>())
            {
                std::stringstream ss;
                ss << "djv::AV::System: " << DJV_TEXT("cannot find OpenGL 3.3 functions; found version") << " " <<
                    p.openGLContext->format().majorVersion() << "." << p.openGLContext->format().minorVersion();
                throw std::runtime_error(ss.str());
            }

            p.openGLDebugLogger.reset(new QOpenGLDebugLogger);
            QObject::connect(
                p.openGLDebugLogger.data(),
                &QOpenGLDebugLogger::messageLogged,
                [this](const QOpenGLDebugMessage & message)
            {
                _log(message.message().toStdString());
            });
            if (p.openGLContext->format().testOption(QSurfaceFormat::DebugContext))
            {
                p.openGLDebugLogger->initialize();
                p.openGLDebugLogger->startLogging();
            }

            // Create the systems.
            IO::System::create(context);
            AudioSystem::create(context);
            glm::vec2 dpi;
            auto screen = qApp->primaryScreen();
            dpi.x = screen->physicalDotsPerInchX();
            dpi.y = screen->physicalDotsPerInchY();
            FontSystem::create(dpi, context);
            IconSystem::create(context);
        }

        System::System() :
            _p(new Private)
        {}

        System::~System()
        {}

        std::shared_ptr<System> System::create(const std::shared_ptr<Core::Context> & context)
        {
            auto out = std::shared_ptr<System>(new System);
            out->_init(context);
            return out;
        }

        QPointer<QOpenGLContext> System::openGLContext() const
        {
            return _p->openGLContext.data();
        }

        void System::makeGLContextCurrent()
        {
            DJV_PRIVATE_PTR();
            p.openGLContext->makeCurrent(p.offscreenSurface.data());
        }

    } // namespace AV
} // namespace djv

