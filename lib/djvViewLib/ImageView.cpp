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

#include <djvViewLib/ImageView.h>

#include <djvViewLib/FilePrefs.h>
#include <djvViewLib/HudInfo.h>
#include <djvViewLib/MousePrefs.h>
#include <djvViewLib/ViewContext.h>
#include <djvViewLib/ViewPrefs.h>
#include <djvViewLib/WindowPrefs.h>

#include <djvUI/SequencePrefs.h>
#include <djvUI/TimePrefs.h>

#include <djvAV/OpenGL.h>
#include <djvAV/Pixel.h>

#include <djvCore/Assert.h>
#include <djvCore/FileInfo.h>
#include <djvCore/FileInfoUtil.h>
#include <djvCore/Time.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QPointer>
#include <QStyle>
#include <QUrl>
#include <QWheelEvent>

#include <glm/gtc/matrix_transform.hpp>

namespace djv
{
    namespace ViewLib
    {
        struct ImageView::Private
        {
            Private(const QPointer<ViewContext> & context) :
                grid(context->viewPrefs()->grid()),
                gridColor(context->viewPrefs()->gridColor()),
                hudEnabled(context->viewPrefs()->isHudEnabled()),
                hudColor(context->viewPrefs()->hudColor()),
                hudBackground(context->viewPrefs()->hudBackground()),
                hudBackgroundColor(context->viewPrefs()->hudBackgroundColor()),
                context(context)
            {}

            glm::ivec2                             viewPosTmp = glm::ivec2(0, 0);
            float                                  viewZoomTmp = 0.f;
            Enum::GRID                             grid = static_cast<Enum::GRID>(0);
            AV::Color                        gridColor;
            AV::PixelData                    gridPixelData;
            std::unique_ptr<AV::OpenGLImage> gridOpenGLImage;
            bool                                   hudEnabled = false;
            HudInfo                                hudInfo;
            AV::Color                        hudColor;
            Enum::HUD_BACKGROUND                   hudBackground = static_cast<Enum::HUD_BACKGROUND>(0);
            AV::Color                        hudBackgroundColor;
            AV::PixelData                    hudPixelData;
            std::unique_ptr<AV::OpenGLImage> hudOpenGLImage;
            bool                                   mouseInside = false;
            glm::ivec2                             mousePos = glm::ivec2(0, 0);
            glm::ivec2                             mouseStartPos = glm::ivec2(0, 0);
            MouseButtonAction                      mouseButtonAction;
            bool                                   mouseWheel = false;
            int                                    mouseWheelTmp = 0;
            int                                    mouseWheelTimer = -1;
            QPointer<ViewContext>                  context;
        };

        ImageView::ImageView(const QPointer<ViewContext> & context, QWidget * parent) :
            UI::ImageView(context.data(), parent),
            _p(new Private(context))
        {
            //DJV_DEBUG("ImageView::ImageView");

            setMouseTracking(true);
            setAcceptDrops(true);

            connect(
                context->viewPrefs(),
                SIGNAL(gridColorChanged(const djv::AV::Color &)),
                SLOT(setGridColor(const djv::AV::Color &)));
            connect(
                context->viewPrefs(),
                SIGNAL(hudInfoChanged(const QMap<djv::ViewLib::Enum::HUD, bool> &)),
                SLOT(hudInfoCallback(const QMap<djv::ViewLib::Enum::HUD, bool> &)));
            connect(
                context->viewPrefs(),
                SIGNAL(hudColorChanged(const djv::AV::Color &)),
                SLOT(setHudColor(const djv::AV::Color &)));
            connect(
                context->viewPrefs(),
                SIGNAL(hudBackgroundChanged(djv::ViewLib::Enum::HUD_BACKGROUND)),
                SLOT(setHudBackground(djv::ViewLib::Enum::HUD_BACKGROUND)));
            connect(
                context->viewPrefs(),
                SIGNAL(hudBackgroundColorChanged(const djv::AV::Color &)),
                SLOT(setHudBackgroundColor(const djv::AV::Color &)));
            connect(
                context->timePrefs(),
                SIGNAL(unitsChanged(djv::Core::Time::UNITS)),
                SLOT(update()));
        }

        ImageView::~ImageView()
        {
            //DJV_DEBUG("ImageView::~ImageView");
            makeCurrent();
            _p->gridOpenGLImage.reset();
            _p->hudOpenGLImage.reset();
        }

        bool ImageView::isMouseInside() const
        {
            return _p->mouseInside;
        }

        const glm::ivec2 & ImageView::mousePos() const
        {
            return _p->mousePos;
        }

        QSize ImageView::sizeHint() const
        {
            //DJV_DEBUG("ImageView::sizeHint");
            //DJV_DEBUG_PRINT("zoom = " << viewZoom());
            glm::ivec2 size = Core::VectorUtil::ceil(bbox().size);
            if (!Core::VectorUtil::isSizeValid(size))
            {
                size = glm::ivec2(800, 400);
            }
            glm::ivec2 maxSize(0, 0);
            const QSize desktopGeom = qApp->desktop()->availableGeometry().size();
            const glm::ivec2 screenSize(desktopGeom.width(), desktopGeom.height());
            switch (_p->context->windowPrefs()->viewMax())
            {
            case Enum::VIEW_MAX_25:
            case Enum::VIEW_MAX_50:
            case Enum::VIEW_MAX_75:
            {
                float v = 0.f;
                switch (_p->context->windowPrefs()->viewMax())
                {
                case Enum::VIEW_MAX_25: v = 0.25; break;
                case Enum::VIEW_MAX_50: v = 0.5;  break;
                case Enum::VIEW_MAX_75: v = 0.75; break;
                default: break;
                }
                maxSize = Core::VectorUtil::ceil(glm::vec2(screenSize) * v);
            }
            break;
            case Enum::VIEW_MAX_USER:
                maxSize = _p->context->windowPrefs()->viewMaxUser();
                break;
            default: break;
            }
            if (Core::VectorUtil::isSizeValid(maxSize))
            {
                if (size.x > maxSize.x || size.y > maxSize.y)
                {
                    const float aspect = size.x / static_cast<float>(size.y);
                    const glm::ivec2 a(maxSize.x, maxSize.x / aspect);
                    const glm::ivec2 b(maxSize.y * aspect, maxSize.y);
                    size = a.x < b.x && a.y < b.y ? a : b;
                }
            }
            //DJV_DEBUG_PRINT("size = " << size);
            return QSize(size.x, size.y);
        }

        QSize ImageView::minimumSizeHint() const
        {
            return QSize(100, 100);
        }

        void ImageView::setZoomFocus(float in)
        {
            //DJV_DEBUG("ImageView::setZoomFocus");
            //DJV_DEBUG_PRINT("in = " << in);    
            setViewZoom(in, _p->mouseInside ? _p->mousePos : (glm::ivec2(width(), height()) / 2));
        }

        void ImageView::setGrid(Enum::GRID grid)
        {
            if (grid == _p->grid)
                return;
            _p->grid = grid;
            update();
        }

        void ImageView::setGridColor(const AV::Color & color)
        {
            if (color == _p->gridColor)
                return;
            _p->gridColor = color;
            update();
        }

        void ImageView::setHudEnabled(bool in)
        {
            if (in == _p->hudEnabled)
                return;
            _p->hudEnabled = in;
            update();
        }

        void ImageView::setHudInfo(const HudInfo & info)
        {
            //if (info == _p->hudInfo)
            //    return;
            _p->hudInfo = info;
            update();
        }

        void ImageView::setHudColor(const AV::Color & color)
        {
            if (color == _p->hudColor)
                return;
            _p->hudColor = color;
            update();
        }

        void ImageView::setHudBackground(Enum::HUD_BACKGROUND background)
        {
            if (background == _p->hudBackground)
                return;
            _p->hudBackground = background;
            update();
        }

        void ImageView::setHudBackgroundColor(const AV::Color & color)
        {
            if (color == _p->hudBackgroundColor)
                return;
            _p->hudBackgroundColor = color;
            update();
        }

        void ImageView::timerEvent(QTimerEvent *)
        {
            setCursor(QCursor());
            if (_p->mouseWheel)
            {
                _p->mouseWheelTmp = 0;
                _p->mouseWheel = false;
            }
            killTimer(_p->mouseWheelTimer);
            _p->mouseWheelTimer = -1;
        }

        void ImageView::enterEvent(QEvent * event)
        {
            UI::ImageView::enterEvent(event);
            _p->mouseInside = true;
        }

        void ImageView::leaveEvent(QEvent * event)
        {
            UI::ImageView::leaveEvent(event);
            _p->mouseInside = false;
        }

        void ImageView::resizeEvent(QResizeEvent * event)
        {
            //DJV_DEBUG("ImageView::resizeEvent");
            UI::ImageView::resizeEvent(event);
            if (hasViewFit())
            {
                //DJV_DEBUG_PRINT("fit");
                viewFit();
            }
        }

        void ImageView::mousePressEvent(QMouseEvent * event)
        {
            setFocus(Qt::MouseFocusReason);
            _p->viewPosTmp = viewPos();
            _p->viewZoomTmp = viewZoom();
            _p->mousePos = glm::ivec2(event->pos().x(), height() - event->pos().y() - 1);
            _p->mouseStartPos = _p->mousePos;
            _p->mouseButtonAction.action = Enum::MOUSE_BUTTON_ACTION_NONE;
            Q_FOREACH(auto action, _p->context->mousePrefs()->mouseButtonActions())
            {
                if (event->button() == Enum::qtMouseButtons()[action.button])
                {
                    if (event->modifiers())
                    {
                        if (event->modifiers() & Enum::qtKeyboardModifiers()[action.modifier])
                        {
                            _p->mouseButtonAction = action;
                            break;
                        }
                    }
                    else if (Enum::KEYBOARD_MODIFIER_NONE == action.modifier)
                    {
                        _p->mouseButtonAction = action;
                        break;
                    }
                }
            }
            switch (_p->mouseButtonAction.action)
            {
            case Enum::MOUSE_BUTTON_ACTION_COLOR_PICK:
                setCursor(Qt::CrossCursor);
                Q_EMIT pickChanged(_p->mousePos);
                break;
            case Enum::MOUSE_BUTTON_ACTION_VIEW_ZOOM_IN:
                setViewZoom(viewZoom() * 2.f, _p->mousePos);
                break;
            case Enum::MOUSE_BUTTON_ACTION_VIEW_ZOOM_OUT:
                setViewZoom(viewZoom() * 0.5, _p->mousePos);
                break;
            case Enum::MOUSE_BUTTON_ACTION_PLAYBACK_SHUTTLE:
                setCursor(Qt::SizeHorCursor);
                Q_EMIT mouseWheelActionChanged(Enum::MOUSE_WHEEL_ACTION_PLAYBACK_SHUTTLE);
                break;
            case Enum::MOUSE_BUTTON_ACTION_CONTEXT_MENU:
                Q_EMIT contextMenuRequested(event->globalPos());
                break;
            default: break;
            }
        }

        void ImageView::mouseReleaseEvent(QMouseEvent *)
        {
            setCursor(QCursor());
            _p->mouseButtonAction.action = Enum::MOUSE_BUTTON_ACTION_NONE;
        }

        void ImageView::mouseMoveEvent(QMouseEvent * event)
        {
            //DJV_DEBUG("ImageView::mouseMoveEvent");
            _p->mousePos = glm::ivec2(event->pos().x(), height() - event->pos().y() - 1);
            //DJV_DEBUG_PRINT("pos = " << _p->mousePos);
            switch (_p->mouseButtonAction.action)
            {
            case Enum::MOUSE_BUTTON_ACTION_COLOR_PICK:
                setCursor(Qt::CrossCursor);
                Q_EMIT pickChanged(_p->mousePos);
                break;
            case Enum::MOUSE_BUTTON_ACTION_VIEW_MOVE:
                setCursor(Qt::ClosedHandCursor);
                setViewPos(_p->viewPosTmp + (_p->mousePos - _p->mouseStartPos));
                break;
            case Enum::MOUSE_BUTTON_ACTION_PLAYBACK_SHUTTLE:
                setCursor(Qt::SizeHorCursor);
                Q_EMIT mouseWheelValueChanged((_p->mousePos.x - _p->mouseStartPos.x) / 5);
                break;
            default: break;
            }
        }

        void ImageView::wheelEvent(QWheelEvent * event)
        {
            const float speed = Enum::zoomFactor(_p->context->viewPrefs()->zoomFactor());
            const int delta = event->angleDelta().y();
            MouseWheelAction mouseWheelAction;
            Q_FOREACH(auto action, _p->context->mousePrefs()->mouseWheelActions())
            {
                if (event->modifiers())
                {
                    if (event->modifiers() & Enum::qtKeyboardModifiers()[action.modifier])
                    {
                        mouseWheelAction = action;
                        break;
                    }
                }
                else if (Enum::KEYBOARD_MODIFIER_NONE == action.modifier)
                {
                    mouseWheelAction = action;
                    break;
                }
            }
            switch (mouseWheelAction.action)
            {
            case Enum::MOUSE_WHEEL_ACTION_VIEW_ZOOM:
                setZoomFocus(viewZoom() * (delta / 120.f > 0 ? speed : (1.f / speed)));
                break;
            case Enum::MOUSE_WHEEL_ACTION_PLAYBACK_SHUTTLE:
                if (!_p->mouseWheel)
                {
                    setCursor(Qt::SizeHorCursor);
                    Q_EMIT mouseWheelActionChanged(mouseWheelAction.action);
                    _p->mouseWheel = true;
                }
                _p->mouseWheelTmp += delta / 120.f;
                Q_EMIT mouseWheelValueChanged(_p->mouseWheelTmp);
                break;
            case Enum::MOUSE_WHEEL_ACTION_PLAYBACK_SPEED:
                if (!_p->mouseWheel)
                {
                    Q_EMIT mouseWheelActionChanged(mouseWheelAction.action);
                    _p->mouseWheel = true;
                }
                _p->mouseWheelTmp += delta / 120.f;
                Q_EMIT mouseWheelValueChanged(_p->mouseWheelTmp);
                break;
            default: break;
            }
            if (-1 == _p->mouseWheelTimer)
            {
                _p->mouseWheelTimer = startTimer(0);
            }
        }

        void ImageView::dragEnterEvent(QDragEnterEvent * event)
        {
            //DJV_DEBUG("ImageView::dragEnterEvent");
            //Q_FOREACH(const QString & format, event->mimeData()->formats())
            //    DJV_DEBUG_PRINT("mime = " << format);
            if (event->mimeData()->hasFormat("application/x-filebrowser") ||
                event->mimeData()->hasFormat("text/uri-list") ||
                event->mimeData()->hasFormat("text/plain"))
            {
                event->acceptProposedAction();
            }
        }

        void ImageView::dropEvent(QDropEvent * event)
        {
            //DJV_DEBUG("ImageView::dropEvent");
            //Q_FOREACH(const QString & format, event->mimeData()->formats())
            //    DJV_DEBUG_PRINT("mime = " << format);
            event->acceptProposedAction();
            //DJV_DEBUG_PRINT("autoSequence = " << autoSequence);
            Core::FileInfo fileInfo;
            if (event->mimeData()->hasFormat("application/x-filebrowser"))
            {
                //DJV_DEBUG_PRINT("application/x-filebrowser");
                const QByteArray data = event->mimeData()->data("application/x-filebrowser");
                QDataStream stream(data);
                QStringList tmp;
                stream >> tmp;
                //DJV_DEBUG_PRINT("tmp = " << tmp);
                tmp >> fileInfo;
            }
            else if (event->mimeData()->hasFormat("text/uri-list"))
            {
                //DJV_DEBUG_PRINT("text/uri-list");
                QString fileName = QUrl(event->mimeData()->text()).toLocalFile();
                //! \todo Why do we have to strip new-lines off the end of the URI?
                while (
                    fileName.count() &&
                    ('\n' == fileName[fileName.count() - 1] ||
                        '\r' == fileName[fileName.count() - 1]))
                {
                    fileName.chop(1);
                }
                //DJV_DEBUG_PRINT("URI = \"" << fileName << "\"");         
                fileInfo = Core::FileInfoUtil::parse(
                    fileName,
                    _p->context->sequencePrefs()->format(),
                    _p->context->sequencePrefs()->isAutoEnabled());
            }
            else if (event->mimeData()->hasFormat("text/plain"))
            {
                //DJV_DEBUG_PRINT("text/plain");
                fileInfo = Core::FileInfoUtil::parse(
                    event->mimeData()->text(),
                    _p->context->sequencePrefs()->format(),
                    _p->context->sequencePrefs()->isAutoEnabled());
            }
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            Q_EMIT fileDropped(fileInfo);
        }

        void ImageView::initializeGL()
        {
            //DJV_DEBUG("ImageView::initializeGL");
            UI::ImageView::initializeGL();
            _p->gridOpenGLImage.reset(new AV::OpenGLImage);
            _p->hudOpenGLImage.reset(new AV::OpenGLImage);
        }

        void ImageView::paintGL()
        {
            //DJV_DEBUG("ImageView::paintGL");
            UI::ImageView::paintGL();
            if (_p->grid)
            {
                drawGrid();
            }
            if (_p->hudEnabled)
            {
                drawHud();
            }
        }

        void ImageView::hudInfoCallback(const QMap<Enum::HUD, bool> & info)
        {
            _p->hudInfo.visible = info;
            update();
        }

        void ImageView::drawGrid()
        {
            //DJV_DEBUG("ImageView::drawGrid");
            //DJV_DEBUG_PRINT("grid = " << Enum::gridLabels()[_p->grid]);

            const glm::ivec2 size(width(), height());
            const glm::ivec2& viewPos = this->viewPos();
            const float viewZoom = this->viewZoom();
            //DJV_DEBUG_PRINT("size = " << size);
            //DJV_DEBUG_PRINT("view pos = " << viewPos);
            //DJV_DEBUG_PRINT("view zoom = " << viewZoom);

            _p->gridPixelData.set(AV::PixelDataInfo(size, AV::Pixel::RGBA_U8));
            _p->gridPixelData.zero();
            QImage image(_p->gridPixelData.data(), size.x, size.y, QImage::Format_RGBA8888_Premultiplied);
            QPainter painter(&image);

            int inc = 0;
            switch (_p->grid)
            {
            case Enum::GRID_1x1:     inc = 1;   break;
            case Enum::GRID_10x10:   inc = 10;  break;
            case Enum::GRID_100x100: inc = 100; break;
            default: break;
            }
            if ((inc * viewZoom) <= 2)
                return;

            Core::Box2i area(
                Core::VectorUtil::floor(
                    glm::vec2(-viewPos) / viewZoom / static_cast<float>(inc)) - 1,
                Core::VectorUtil::ceil(
                    glm::vec2(width(), height()) / viewZoom / static_cast<float>(inc)) + 2);
            area *= inc;
            //DJV_DEBUG_PRINT("area = " << area);

            painter.setPen(QPen(AV::ColorUtil::toQt(_p->gridColor), 1));
            for (int y = 0; y <= area.h; y += inc)
            {
                painter.drawLine(
                    (area.x) * viewZoom + viewPos.x,
                    (area.y + y) * viewZoom + viewPos.y,
                    (area.x + area.w) * viewZoom + viewPos.x,
                    (area.y + y) * viewZoom + viewPos.y);
            }
            for (int x = 0; x <= area.w; x += inc)
            {
                painter.drawLine(
                    (area.x + x) * viewZoom + viewPos.x,
                    (area.y) * viewZoom + viewPos.y,
                    (area.x + x) * viewZoom + viewPos.x,
                    (area.y + area.h) * viewZoom + viewPos.y);
            }

            try
            {
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glEnable(GL_BLEND);
                glFuncs->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                const auto viewMatrix = glm::ortho(
                    0.f,
                    static_cast<float>(size.x),
                    0.f,
                    static_cast<float>(size.y),
                    -1.f,
                    1.f);
                _p->gridOpenGLImage->draw(_p->gridPixelData, viewMatrix);
            }
            catch (const Core::Error &)
            {}
        }

        void ImageView::drawHud()
        {
            //DJV_DEBUG("ImageView::drawHud");

            QStringList upperLeft;
            QStringList lowerLeft;
            QStringList upperRight;
            QStringList lowerRight;

            // Generate the upper left contents.
            if (_p->hudInfo.visible[Enum::HUD_FILE_NAME])
            {
                upperLeft += qApp->translate("djv::ViewLib::ImageView", "File  = %1").
                    arg(_p->hudInfo.info.fileName);
            }
            if (_p->hudInfo.visible[Enum::HUD_LAYER])
            {
                upperLeft += qApp->translate("djv::ViewLib::ImageView", "Layer = %1").
                    arg(_p->hudInfo.info.layerName);
            }
            if (_p->hudInfo.visible[Enum::HUD_SIZE])
            {
                upperLeft += qApp->translate("djv::ViewLib::ImageView", "Size  = %1x%2:%3").
                    arg(_p->hudInfo.info.size.x).
                    arg(_p->hudInfo.info.size.y).
                    arg(Core::VectorUtil::aspect(_p->hudInfo.info.size), 0, 'f', 2);
            }
            if (_p->hudInfo.visible[Enum::HUD_PROXY])
            {
                upperLeft += qApp->translate("djv::ViewLib::ImageView", "Proxy = %1").
                    arg(_p->hudInfo.info.proxy);
            }
            if (_p->hudInfo.visible[Enum::HUD_PIXEL])
            {
                QStringList pixelLabel;
                pixelLabel << _p->hudInfo.info.pixel;
                upperLeft += qApp->translate("djv::ViewLib::ImageView", "Pixel = %1").
                    arg(pixelLabel.join(", "));
            }

            // Generate the lower left contents.
            if (_p->hudInfo.visible[Enum::HUD_TAG] && _p->hudInfo.tags.count())
            {
                const QStringList keys = _p->hudInfo.tags.keys();
                for (int i = 0; i < keys.count(); ++i)
                {
                    lowerLeft += qApp->translate("djv::ViewLib::ImageView", "%1 = %2").
                        arg(keys[i]).
                        arg(_p->hudInfo.tags[keys[i]]);
                }
            }

            // Generate the upper right contents.
            if (_p->hudInfo.visible[Enum::HUD_FRAME])
            {
                upperRight += qApp->translate("djv::ViewLib::ImageView", "Frame = %1").
                    arg(Core::Time::frameToString(_p->hudInfo.frame, _p->hudInfo.speed));
            }
            if (_p->hudInfo.visible[Enum::HUD_SPEED])
            {
                upperRight += qApp->translate("djv::ViewLib::ImageView", "Speed = %1/%2").
                    arg(Core::Speed::speedToFloat(_p->hudInfo.speed), 0, 'f', 2).
                    arg(_p->hudInfo.actualSpeed, 0, 'f', 2);
            }

            const glm::ivec2 size(width(), height());
            _p->hudPixelData.set(AV::PixelDataInfo(size, AV::Pixel::RGBA_U8));
            _p->hudPixelData.zero();
            QImage image(_p->hudPixelData.data(), size.x, size.y, QImage::Format_RGBA8888_Premultiplied);
            QPainter painter(&image);
            painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

            // Draw the upper left contents.
            const int m = style()->pixelMetric(QStyle::PM_LayoutLeftMargin);
            const int th = fontMetrics().height();
            glm::ivec2 p = glm::ivec2(m, m);
            for (int i = 0; i < upperLeft.count(); ++i)
            {
                const int tw = fontMetrics().width(upperLeft[i]);
                drawHudItem(painter, upperLeft[i], Core::Box2i(p.x, p.y, tw + m, th + m));
                p.y += th + m;
            }

            // Draw the lower left contents.
            p = glm::ivec2(m, height() - th * lowerLeft.count() - m);
            for (int i = 0; i < lowerLeft.count(); ++i)
            {
                const int tw = fontMetrics().width(lowerLeft[i]);
                drawHudItem(painter, lowerLeft[i], Core::Box2i(p.x, p.y, tw + m, th + m));
                p.y += th + m;
            }

            // Draw the upper right contents.
            p = glm::ivec2(size.x - m, m);
            for (int i = 0; i < upperRight.count(); ++i)
            {
                const int tw = fontMetrics().width(upperRight[i]);
                drawHudItem(painter, upperRight[i], Core::Box2i(p.x - tw - m, p.y, tw + m, th + m));
                p.y += th + m;
            }

            try
            {
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glEnable(GL_BLEND);
                glFuncs->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                const auto viewMatrix = glm::ortho(
                    0.f,
                    static_cast<float>(size.x),
                    0.f,
                    static_cast<float>(size.y),
                    -1.f,
                    1.f);
                AV::OpenGLImageOptions options;
                options.xform.mirror.y = true;
                _p->hudOpenGLImage->draw(_p->hudPixelData, viewMatrix, options);
            }
            catch (const Core::Error &)
            {}
        }

       void ImageView::drawHudItem(
            QPainter & painter,
            const QString & in,
            const Core::Box2i & box)
        {
            switch (_p->hudBackground)
            {
            case Enum::HUD_BACKGROUND_NONE: break;
            case Enum::HUD_BACKGROUND_SOLID:
                painter.fillRect(box.x, box.y, box.w, box.h, AV::ColorUtil::toQt(_p->hudBackgroundColor));
                break;
            case Enum::HUD_BACKGROUND_SHADOW:
                painter.setPen(AV::ColorUtil::toQt(_p->hudBackgroundColor));
                painter.drawText(box.x + 1, box.y + 1, box.w, box.h, Qt::AlignCenter, in);
                break;
            default: break;
            }
            painter.setPen(AV::ColorUtil::toQt(_p->hudColor));
            painter.drawText(box.x, box.y, box.w, box.h, Qt::AlignCenter, in);
        }

    } // namespace ViewLib
} // namespace djv
