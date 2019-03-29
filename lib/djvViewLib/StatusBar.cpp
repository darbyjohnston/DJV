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

#include <djvViewLib/StatusBar.h>

#include <djvViewLib/FileCache.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/ColorSwatch.h>

#include <djvAV/OpenGLImage.h>
#include <djvAV/OpenGLOffscreenBuffer.h>

#include <QCoreApplication>
#include <QLabel>

namespace djv
{
    namespace ViewLib
    {
        struct StatusBar::Private
        {
            std::shared_ptr<AV::Image> image;
            glm::ivec2 viewPos = glm::ivec2(0, 0);
            float viewZoom = 0.f;
            glm::ivec2 pick = glm::ivec2(0, 0);
            AV::Color sample;
            std::unique_ptr<AV::OpenGLImage> openGLImage;
            AV::OpenGLImageOptions openGLImageOptions;

            QPointer<UI::ColorSwatch> swatch;
            QPointer<QLabel> pixelLabel;
            QPointer<QLabel> imageLabel;
            QPointer<QLabel> cacheLabel;

            QPointer<ViewContext> context;
        };

        StatusBar::StatusBar(
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context,
            QWidget * parent) :
            QStatusBar(parent),
            _p(new Private)
        {
            _p->context = context;

            _p->swatch = new UI::ColorSwatch(context.data());
            _p->swatch->setFixedSize(20, 20);

            _p->pixelLabel = new QLabel;
            _p->pixelLabel->setToolTip(
                qApp->translate("djv::ViewLib::StatusBar", "Pixel information\n\nClick and drag inside the image."));

            _p->imageLabel = new QLabel;
            _p->imageLabel->setToolTip(
                qApp->translate("djv::ViewLib::StatusBar", "Image information."));

            _p->cacheLabel = new QLabel;
            _p->cacheLabel->setToolTip(
                qApp->translate("djv::ViewLib::StatusBar", "File cache information."));

            addWidget(_p->swatch);
            addWidget(_p->pixelLabel);
            addWidget(new QWidget, 1);
            addWidget(_p->imageLabel);
            addWidget(_p->cacheLabel);

            auto viewWidget = session->viewWidget();
            _p->viewPos = viewWidget->viewPos();
            _p->viewZoom = viewWidget->viewZoom();

            connect(
                context->fileCache(),
                SIGNAL(cacheChanged()),
                SLOT(widgetUpdate()));
            connect(
                session,
                &Session::imageChanged,
                [this](const std::shared_ptr<djv::AV::Image> & value)
            {
                _p->image = value;
                widgetUpdate();
            });
            connect(
                session,
                &Session::imageOptionsChanged,
                [this](const AV::OpenGLImageOptions & value)
            {
                _p->openGLImageOptions = value;
                widgetUpdate();
            });
            connect(
                viewWidget,
                &ImageView::viewPosChanged,
                [this](const glm::ivec2 & value)
            {
                _p->viewPos = value;
                widgetUpdate();
            });
            connect(
                viewWidget,
                &ImageView::viewZoomChanged,
                [this](float value)
            {
                _p->viewZoom = value;
                widgetUpdate();
            });
            connect(
                viewWidget,
                SIGNAL(pickChanged(const glm::ivec2 &)),
                SLOT(pickCallback(const glm::ivec2 &)));
        }

        StatusBar::~StatusBar()
        {
            if (_p->openGLImage)
            {
                _p->context->makeGLContextCurrent();
                _p->openGLImage.reset();
            }
        }

        void StatusBar::pickCallback(const glm::ivec2 & value)
        {
            //DJV_DEBUG("StatusBar::pickCallback");

            _p->pick = value;

            _p->context->makeGLContextCurrent();
            if (!_p->openGLImage)
            {
                _p->openGLImage.reset(new AV::OpenGLImage);
            }

            const glm::ivec2 pick = Core::VectorUtil::floor(glm::vec2(_p->pick - _p->viewPos) / _p->viewZoom);
            //DJV_DEBUG_PRINT("pick = " << pick);

            _p->sample = _p->openGLImageOptions.background;
            if (_p->image)
            {
                try
                {
                    AV::PixelData tmp(AV::PixelDataInfo(glm::ivec2(1, 1), _p->image->pixel()));
                    AV::OpenGLImageOptions _options = _p->openGLImageOptions;
                    _options.xform.position -= pick;
                    _p->openGLImage->copy(*_p->image, tmp, _options);
                    _p->openGLImage->average(tmp, _p->sample);
                    //DJV_DEBUG_PRINT("sample = " << _p->sample);
                }
                catch (Core::Error error)
                {
                    error.add(Enum::errorLabels()[Enum::ERROR_PICK_COLOR]);
                    _p->context->printError(error);
                }
            }

            widgetUpdate();
        }

        void StatusBar::widgetUpdate()
        {
            const float sizeGB = _p->context->fileCache()->currentSizeGB();
            const float maxSizeGB = _p->context->fileCache()->maxSizeGB();
            _p->cacheLabel->setText(
                qApp->translate("djv::ViewLib::StatusBar", "Cache: %1% %2/%3GB").
                arg(static_cast<int>(sizeGB / maxSizeGB * 100)).
                arg(sizeGB, 0, 'f', 2).
                arg(maxSizeGB, 0, 'f', 2));

            AV::PixelDataInfo info;
            if (_p->image)
            {
                info = _p->image->info();
            }
            QStringList pixelLabel;
            pixelLabel << info.pixel;
            _p->imageLabel->setText(
                qApp->translate("djv::ViewLib::StatusBar", "Image: %1x%2:%3 %4").
                arg(info.size.x).
                arg(info.size.y).
                arg(Core::VectorUtil::aspect(info.size), 0, 'f', 2).
                arg(pixelLabel.join(" ")));

            _p->swatch->setColor(_p->sample);

            QStringList sampleLabel;
            sampleLabel << _p->sample;
            _p->pixelLabel->setText(
                qApp->translate("djv::ViewLib::StatusBar", "Pixel: %1, %2, %3").
                arg(_p->pick.x).
                arg(_p->pick.y).
                arg(sampleLabel.join(" ")));
        }

    } // namespace ViewLib
} // namespace djv
