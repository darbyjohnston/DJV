//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvUI/OpenGLWidget.h>

#include <djvUI/UIContext.h>

#include <djvAV/AVContext.h>

#include <djvCore/DebugLog.h>

#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QPointer>
#include <QSurface>

namespace djv
{
    namespace UI
    {
        struct OpenGLWidget::Private
        {
            QPointer<UIContext> context;
            QScopedPointer<QOpenGLDebugLogger> openGLDebugLogger;
        };

        OpenGLWidget::OpenGLWidget(
            const QPointer<UIContext> & context,
            QWidget * parent,
            Qt::WindowFlags flags) :
            QOpenGLWidget(parent, flags),
            _p(new Private)
        {
            _p->context = context;
            _p->openGLDebugLogger.reset(new QOpenGLDebugLogger);
            connect(
                _p->openGLDebugLogger.data(),
                &QOpenGLDebugLogger::messageLogged,
                this,
                &OpenGLWidget::debugLogMessage);
        }

        OpenGLWidget::~OpenGLWidget()
        {}

        void OpenGLWidget::initializeGL()
        {
            if (QOpenGLContext::currentContext()->format().testOption(QSurfaceFormat::DebugContext))
            {
                _p->openGLDebugLogger->initialize();
                _p->openGLDebugLogger->startLogging();
            }
        }

        void OpenGLWidget::debugLogMessage(const QOpenGLDebugMessage & message)
        {
            DJV_LOG(_p->context->debugLog(), "djv::UI::OpenGLWidget", message.message());
        }

    } // namespace UI
} // namespace djv
