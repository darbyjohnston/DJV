//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvUI/ImageView.h>

#include <djvUI/UIContext.h>

#include <djvGraphics/ColorUtil.h>
#include <djvGraphics/Image.h>
#include <djvGraphics/OpenGLImage.h>
#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/DebugLog.h>
#include <djvCore/Error.h>
#include <djvCore/ErrorUtil.h>

#include <QPointer>

#include <glm/gtc/matrix_transform.hpp>

namespace djv
{
    namespace UI
    {
        struct ImageView::Private
        {
            Private(const QPointer<UIContext> & context) :
                context(context)
            {}

            const Graphics::PixelData *  data = nullptr;
            Graphics::OpenGLImageOptions options;
            glm::ivec2 viewPos = glm::ivec2(0, 0);
            float viewZoom = 1.f;
            bool viewFit = false;
            std::unique_ptr<Graphics::OpenGLImage> openGLImage;
            QPointer<UIContext> context;
        };

        ImageView::ImageView(
            const QPointer<UIContext> & context,
            QWidget * parent,
            Qt::WindowFlags flags) :
            OpenGLWidget(parent, flags),
            _p(new Private(context))
        {}

        ImageView::~ImageView()
        {
            makeCurrent();
            _p->openGLImage.reset();
        }

        const Graphics::PixelData * ImageView::data() const
        {
            return _p->data;
        }

        const Graphics::OpenGLImageOptions & ImageView::options() const
        {
            return _p->options;
        }

        const glm::ivec2 & ImageView::viewPos() const
        {
            return _p->viewPos;
        }

        float ImageView::viewZoom() const
        {
            return _p->viewZoom;
        }

        Core::Box2f ImageView::bbox() const
        {
            return bbox(_p->viewPos, _p->viewZoom);
        }

        bool ImageView::hasViewFit() const
        {
            return _p->viewFit;
        }

        QSize ImageView::sizeHint() const
        {
            return QSize(200, 200);
        }

        void ImageView::setData(const Graphics::PixelData * data)
        {
            if (data == _p->data)
                return;
            //DJV_DEBUG("ImageView::setData");
            _p->data = data;
            update();
            Q_EMIT dataChanged(_p->data);
            Q_EMIT viewChanged();
        }

        void ImageView::setOptions(const Graphics::OpenGLImageOptions & options)
        {
            if (options == _p->options)
                return;
            _p->options = options;
            update();
            Q_EMIT optionsChanged(_p->options);
            Q_EMIT viewChanged();
        }

        void ImageView::setViewPos(const glm::ivec2 & pos)
        {
            setViewPosZoom(pos, _p->viewZoom);
        }

        void ImageView::setViewZoom(float zoom)
        {
            //DJV_DEBUG("ImageView::setViewZoom");
            //DJV_DEBUG_PRINT("zoom = " << zoom);
            setViewPosZoom(_p->viewPos, zoom);
        }

        void ImageView::setViewZoom(float zoom, const glm::ivec2 & focus)
        {
            //DJV_DEBUG("ImageView::setViewZoom");
            //DJV_DEBUG_PRINT("zoom = " << zoom);
            //DJV_DEBUG_PRINT("focus = " << focus);
            //DJV_DEBUG_PRINT("viewZoom = " << _p->viewZoom);
            setViewPosZoom(
                focus + glm::ivec2(glm::vec2(_p->viewPos - focus) * (zoom / _p->viewZoom)),
                zoom);
        }

        void ImageView::setViewPosZoom(const glm::ivec2 & pos, float zoom)
        {
            const bool posSame = pos == _p->viewPos;
            const bool zoomSame = Core::Math::fuzzyCompare(zoom, _p->viewZoom);
            if (posSame && zoomSame)
                return;
            //DJV_DEBUG("ImageView::setViewPosZoom");
            //DJV_DEBUG_PRINT("pos = " << pos);
            //DJV_DEBUG_PRINT("zoom = " << zoom);
            _p->viewPos = pos;
            _p->viewZoom = zoom;
            _p->viewFit = false;
            update();
            if (!posSame)
                Q_EMIT viewPosChanged(_p->viewPos);
            if (!zoomSame)
                Q_EMIT viewZoomChanged(_p->viewZoom);
            Q_EMIT viewChanged();
        }

        void ImageView::viewZero()
        {
            setViewPosZoom(glm::ivec2(), 1.f);
        }

        void ImageView::viewCenter()
        {
            setViewPos(glm::vec2(width(), height()) / 2.f - bbox().size / 2.f);
        }

        void ImageView::viewFit()
        {
            if (!_p->data)
                return;
            //DJV_DEBUG("ImageView::viewFit");
            const Core::Box2i geom(width(), height());
            //DJV_DEBUG_PRINT("geom = " << geom.size);
            const Core::Box2f bbox = this->bbox(glm::ivec2(), 1.f);
            //DJV_DEBUG_PRINT("bbox = " << bbox);
            const float zoom = Core::VectorUtil::isSizeValid(bbox.size) ?
                Core::Math::min(geom.w / bbox.size.x, geom.h / bbox.size.y) : 1.f;
            //DJV_DEBUG_PRINT("zoom = " << zoom);
            glm::ivec2 pos =
                glm::vec2(geom.size) / 2.f -
                bbox.size * zoom / 2.f -
                this->bbox(glm::ivec2(), zoom).position;
            //DJV_DEBUG_PRINT("pos = " << pos);
            setViewPosZoom(pos, zoom);
            //DJV_DEBUG_PRINT("fit");
            _p->viewFit = true;
        }

        void ImageView::initializeGL()
        {
            OpenGLWidget::initializeGL();

            //DJV_DEBUG("ImageView::initializeGL");

            QSurfaceFormat surfaceFormat = this->format();
            //DJV_DEBUG_PRINT("major = " << surfaceFormat.majorVersion());
            //DJV_DEBUG_PRINT("minor = " << surfaceFormat.minorVersion());
            //DJV_DEBUG_PRINT("profile = " << surfaceFormat.profile());

            _p->openGLImage.reset(new Graphics::OpenGLImage);
        }

        void ImageView::paintGL()
        {
            OpenGLWidget::paintGL();

            //DJV_DEBUG("ImageView::paintGL");

            const int devicePixelRatio = this->devicePixelRatio();
            //DJV_DEBUG_PRINT("devicePixelRatio = " << devicePixelRatio);
            const Core::Box2i geom(
                width() * devicePixelRatio,
                height() * devicePixelRatio);
            //DJV_DEBUG_PRINT("geom = " << geom.size);

            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
            glFuncs->glViewport(0, 0, geom.w, geom.h);
            Graphics::Color background(Graphics::Pixel::RGB_F32);
            Graphics::ColorUtil::convert(_p->options.background, background);
            glFuncs->glClearColor(
                background.f32(0),
                background.f32(1),
                background.f32(2),
                0.f);
            //glFuncs->glClearColor(0, 1, 0, 0);
            glFuncs->glClear(GL_COLOR_BUFFER_BIT);
            if (!_p->data)
                return;

            //DJV_DEBUG_PRINT("data = " << *_p->data);
            Graphics::OpenGLImageOptions options = _p->options;
            options.xform.position += _p->viewPos;
            options.xform.scale *= _p->viewZoom * devicePixelRatio;
            try
            {
                auto viewMatrix = glm::ortho(
                    0.f,
                    static_cast<float>(width()),
                    0.f,
                    static_cast<float>(height()),
                    -1.f,
                    1.f);
                _p->openGLImage->draw(*_p->data, viewMatrix, options);
            }
            catch (const Core::Error & error)
            {
                DJV_LOG(_p->context->debugLog(), "djv::UI::ImageView",
                    Core::ErrorUtil::format(error).join("\n"));
            }
        }

        Core::Box2f ImageView::bbox(const glm::ivec2 & pos, float zoom) const
        {
            if (!_p->data)
                return Core::Box2f();

            //DJV_DEBUG("ImageView::bbox");
            //DJV_DEBUG_PRINT("pos = " << pos);
            //DJV_DEBUG_PRINT("zoom = " << zoom);

            const Graphics::PixelDataInfo & info = _p->data->info();
            //DJV_DEBUG_PRINT("info = " << info);

            Graphics::OpenGLImageXform xform = _p->options.xform;
            xform.position += pos;
            xform.scale *= zoom;
            //DJV_DEBUG_PRINT("xform = " << xform);

            const glm::mat3x3 m = Graphics::OpenGLImageXform::xformMatrix(xform);
            //DJV_DEBUG_PRINT("m = " << m);

            Core::Box2f box(_p->data->info().size * Graphics::PixelDataUtil::proxyScale(info.proxy));
            //DJV_DEBUG_PRINT("box = " << box);

            box = m * box;
            //DJV_DEBUG_PRINT("box = " << box);
            return box;
        }

    } // namespace UI
} // namespace djv
