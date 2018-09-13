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

#include <djvViewLib/ImageView.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/FilePrefs.h>
#include <djvViewLib/HudInfo.h>
#include <djvViewLib/InputPrefs.h>
#include <djvViewLib/ViewPrefs.h>
#include <djvViewLib/WindowPrefs.h>

#include <djvUI/Style.h>
#include <djvUI/TimePrefs.h>

#include <djvGraphics/OpenGL.h>
#include <djvGraphics/Pixel.h>

#include <djvCore/Assert.h>
#include <djvCore/FileInfo.h>
#include <djvCore/FileInfoUtil.h>
#include <djvCore/Time.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QUrl>
#include <QWheelEvent>

namespace djv
{
    namespace ViewLib
    {
        struct ImageView::Private
        {
            Private(Context * context) :
                grid(context->viewPrefs()->grid()),
                gridColor(context->viewPrefs()->gridColor()),
                hudEnabled(context->viewPrefs()->isHudEnabled()),
                hudColor(context->viewPrefs()->hudColor()),
                hudBackground(context->viewPrefs()->hudBackground()),
                hudBackgroundColor(context->viewPrefs()->hudBackgroundColor()),
                context(context)
            {}

            glm::ivec2           viewPosTmp = glm::ivec2(0, 0);
            float                viewZoomTmp = 0.f;
            Util::GRID           grid = static_cast<Util::GRID>(0);
            Graphics::Color      gridColor;
            bool                 hudEnabled = false;
            HudInfo              hudInfo;
            Graphics::Color      hudColor;
            Util::HUD_BACKGROUND hudBackground = static_cast<Util::HUD_BACKGROUND>(0);
            Graphics::Color      hudBackgroundColor;
            bool                 inside = false;
            glm::ivec2           mousePos = glm::ivec2(0, 0);
            glm::ivec2           mouseStartPos = glm::ivec2(0, 0);
            bool                 mouseWheel = false;
            int                  mouseWheelTmp = 0;
            int                  timer = -1;
            Context *            context = nullptr;
        };

        ImageView::ImageView(Context * context, QWidget * parent) :
            UI::ImageView(context, parent),
            _p(new Private(context))
        {
            //DJV_DEBUG("ImageView::ImageView");

            setMouseTracking(true);
            setAcceptDrops(true);

            connect(
                context->viewPrefs(),
                SIGNAL(gridColorChanged(const djvColor &)),
                SLOT(setGridColor(const djvColor &)));
            connect(
                context->viewPrefs(),
                SIGNAL(hudInfoChanged(const QVector<bool> &)),
                SLOT(hudInfoCallback(const QVector<bool> &)));
            connect(
                context->viewPrefs(),
                SIGNAL(hudColorChanged(const djvColor &)),
                SLOT(setHudColor(const djvColor &)));
            connect(
                context->viewPrefs(),
                SIGNAL(hudBackgroundChanged(djv::ViewLib::Util::HUD_BACKGROUND)),
                SLOT(setHudBackground(djv::ViewLib::Util::HUD_BACKGROUND)));
            connect(
                context->viewPrefs(),
                SIGNAL(hudBackgroundColorChanged(const djvColor &)),
                SLOT(setHudBackgroundColor(const djvColor &)));
            connect(
                context->timePrefs(),
                SIGNAL(timeUnitsChanged(djvTime::UNITS)),
                SLOT(update()));
        }

        ImageView::~ImageView()
        {
            //DJV_DEBUG("ImageView::~ImageView");
        }

        bool ImageView::isMouseInside() const
        {
            return _p->inside;
        }

        const glm::ivec2 & ImageView::mousePos() const
        {
            return _p->mousePos;
        }

        QSize ImageView::sizeHint() const
        {
            //DJV_DEBUG("ImageView::sizeHint");
            //DJV_DEBUG_PRINT("zoom = " << viewZoom());
            glm::ivec2 size = djvVectorUtil::ceil(bbox().size);
            if (djvVectorUtil::isSizeValid(size))
            {
                size = glm::ivec2(640, 300);
            }
            glm::ivec2 maxSize(0, 0);
            const QSize desktopGeom = qApp->desktop()->availableGeometry().size();
            const glm::ivec2 screenSize(desktopGeom.width(), desktopGeom.height());
            switch (_p->context->windowPrefs()->viewMax())
            {
            case Util::VIEW_MAX_25:
            case Util::VIEW_MAX_50:
            case Util::VIEW_MAX_75:
            {
                float v = 0.f;
                switch (_p->context->windowPrefs()->viewMax())
                {
                case Util::VIEW_MAX_25: v = 0.25; break;
                case Util::VIEW_MAX_50: v = 0.5;  break;
                case Util::VIEW_MAX_75: v = 0.75; break;
                default: break;
                }
                maxSize = djvVectorUtil::ceil(glm::vec2(screenSize) * v);
            }
            break;
            case Util::VIEW_MAX_USER:
                maxSize = _p->context->windowPrefs()->viewMaxUser();
                break;
            default: break;
            }
            if (djvVectorUtil::isSizeValid(maxSize))
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
            setViewZoom(
                in,
                _p->inside ? _p->mousePos : (glm::ivec2(width(), height()) / 2));
        }

        void ImageView::setGrid(Util::GRID grid)
        {
            if (grid == _p->grid)
                return;
            _p->grid = grid;
            update();
        }

        void ImageView::setGridColor(const Graphics::Color & color)
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

        void ImageView::setHudColor(const Graphics::Color & color)
        {
            if (color == _p->hudColor)
                return;
            _p->hudColor = color;
            update();
        }

        void ImageView::setHudBackground(Util::HUD_BACKGROUND background)
        {
            if (background == _p->hudBackground)
                return;
            _p->hudBackground = background;
            update();
        }

        void ImageView::setHudBackgroundColor(const Graphics::Color & color)
        {
            if (color == _p->hudBackgroundColor)
                return;
            _p->hudBackgroundColor = color;
            update();
        }

        void ImageView::timerEvent(QTimerEvent *)
        {
            // Reset cursor.
            setCursor(QCursor());

            // Reset mouse wheel.
            if (_p->mouseWheel)
            {
                _p->mouseWheelTmp = 0;
                _p->mouseWheel = false;
            }

            killTimer(_p->timer);
            _p->timer = -1;
        }

        void ImageView::enterEvent(QEvent * event)
        {
            UI::ImageView::enterEvent(event);
            _p->inside = true;
        }

        void ImageView::leaveEvent(QEvent * event)
        {
            UI::ImageView::leaveEvent(event);
            _p->inside = false;
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
            if (Qt::LeftButton == event->button())
            {
                setCursor(Qt::CrossCursor);
                Q_EMIT pickChanged(_p->mousePos);
            }
            else if (Qt::MidButton == event->button() &&
                event->modifiers() & Qt::ControlModifier)
            {
                setViewZoom(viewZoom() * 2.f, _p->mousePos);
            }
            if (Qt::RightButton == event->button() &&
                event->modifiers() & Qt::ControlModifier)
            {
                setViewZoom(viewZoom() * 0.5, _p->mousePos);
            }
            else if (Qt::MidButton == event->button())
            {
                //setCursor(Qt::ClosedHandCursor);
            }
            else if (Qt::RightButton == event->button())
            {
                setCursor(Qt::SizeHorCursor);
                Q_EMIT mouseWheelChanged(Util::MOUSE_WHEEL_PLAYBACK_SHUTTLE);
            }
        }

        void ImageView::mouseReleaseEvent(QMouseEvent *)
        {
            setCursor(QCursor());
        }

        void ImageView::mouseMoveEvent(QMouseEvent * event)
        {
            _p->mousePos = glm::ivec2(event->pos().x(), height() - event->pos().y() - 1);
            if (event->buttons() & Qt::LeftButton)
            {
                setCursor(Qt::CrossCursor);
                Q_EMIT pickChanged(_p->mousePos);
            }
            if (event->buttons() & Qt::MidButton)
            {
                setViewPos(_p->viewPosTmp + (_p->mousePos - _p->mouseStartPos));
                setCursor(Qt::ClosedHandCursor);
            }
            else if (event->buttons() & Qt::RightButton)
            {
                setCursor(Qt::SizeHorCursor);
                Q_EMIT mouseWheelValueChanged((_p->mousePos.x - _p->mouseStartPos.x) / 5);
            }
        }

        void ImageView::wheelEvent(QWheelEvent * event)
        {
            Util::MOUSE_WHEEL mouseWheel =
                _p->context->inputPrefs()->mouseWheel();
            if (event->modifiers() & Qt::ShiftModifier)
            {
                mouseWheel = _p->context->inputPrefs()->mouseWheelShift();
            }
            if (event->modifiers() & Qt::ControlModifier)
            {
                mouseWheel = _p->context->inputPrefs()->mouseWheelCtrl();
            }
            const float speed = Util::zoomFactor(_p->context->viewPrefs()->zoomFactor());
            const int delta = event->angleDelta().y();
            switch (mouseWheel)
            {
            case Util::MOUSE_WHEEL_VIEW_ZOOM:
                setZoomFocus(viewZoom() * (delta / 120.f > 0 ? speed : (1.f / speed)));
                break;
            case Util::MOUSE_WHEEL_PLAYBACK_SHUTTLE:

                if (!_p->mouseWheel)
                {
                    setCursor(Qt::SizeHorCursor);
                    Q_EMIT mouseWheelChanged(mouseWheel);
                    _p->mouseWheel = true;
                }
                _p->mouseWheelTmp += delta / 120.f;
                Q_EMIT mouseWheelValueChanged(_p->mouseWheelTmp);
                break;
            case Util::MOUSE_WHEEL_PLAYBACK_SPEED:
                if (!_p->mouseWheel)
                {
                    Q_EMIT mouseWheelChanged(mouseWheel);
                    _p->mouseWheel = true;
                }
                _p->mouseWheelTmp += delta / 120.f;
                Q_EMIT mouseWheelValueChanged(_p->mouseWheelTmp);
                break;
            default: break;
            }
            if (_p->timer == -1)
            {
                _p->timer = startTimer(0);
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
            const bool autoSequence = _p->context->filePrefs()->hasAutoSequence();
            //DJV_DEBUG_PRINT("autoSequence = " << autoSequence);
            djvFileInfo fileInfo;
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
                fileInfo = djvFileInfoUtil::parse(
                    fileName,
                    autoSequence ? djvSequence::COMPRESS_RANGE : djvSequence::COMPRESS_OFF,
                    autoSequence);
            }
            else if (event->mimeData()->hasFormat("text/plain"))
            {
                //DJV_DEBUG_PRINT("text/plain");
                fileInfo = djvFileInfoUtil::parse(
                    event->mimeData()->text(),
                    autoSequence ? djvSequence::COMPRESS_RANGE : djvSequence::COMPRESS_OFF,
                    autoSequence);
            }
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            Q_EMIT fileDropped(fileInfo);
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

        void ImageView::hudInfoCallback(const QVector<bool> & info)
        {
            _p->hudInfo.visible = info;
            update();
        }

        void ImageView::drawGrid()
        {
            //DJV_DEBUG("ImageView::drawGrid");
            //DJV_DEBUG_PRINT("size = " << geom().size);
            //DJV_DEBUG_PRINT("grid = " << labelGrid()[_p->grid]);
            int inc = 0;
            switch (_p->grid)
            {
            case Util::GRID_1x1:     inc = 1;   break;
            case Util::GRID_10x10:   inc = 10;  break;
            case Util::GRID_100x100: inc = 100; break;
            default: break;
            }

            // Bail if too small.
            if ((inc * viewZoom()) <= 2)
                return;

            // Compute the view area.
            djvBox2i area(
                djvVectorUtil::floor(
                    glm::vec2(-viewPos()) / viewZoom() / static_cast<float>(inc)) - 1,
                djvVectorUtil::ceil(
                    glm::vec2(width(), height()) / viewZoom() / static_cast<float>(inc)) + 2);
            area *= inc;
            //DJV_DEBUG_PRINT("area = " << area);

            // Draw.
            //! \todo
            /*djvOpenGLUtil::color(_p->gridColor);
            glPushMatrix();
            glTranslated(viewPos().x, viewPos().y, 0);
            glScaled(viewZoom(), viewZoom(), 1.f);
            glBegin(GL_LINES);
            for (int y = 0; y <= area.h; y += inc)
            {
                glVertex2i(area.x, area.y + y);
                glVertex2i(area.x + area.w, area.y + y);
            }
            for (int x = 0; x <= area.w; x += inc)
            {
                glVertex2i(area.x + x, area.y);
                glVertex2i(area.x + x, area.y + area.h);
            }
            glEnd();
            glPopMatrix();*/
        }

        void ImageView::drawHud()
        {
            //DJV_DEBUG("ImageView::drawHud");

            QStringList upperLeft;
            QStringList lowerLeft;
            QStringList upperRight;
            QStringList lowerRight;

            // Generate the upper left contents.
            if (_p->hudInfo.visible[Util::HUD_FILE_NAME])
            {
                upperLeft += qApp->translate("djv::ViewLib::ImageView", "File  = %1").
                    arg(_p->hudInfo.info.fileName);
            }
            if (_p->hudInfo.visible[Util::HUD_LAYER])
            {
                upperLeft += qApp->translate("djv::ViewLib::ImageView", "Layer = %1").
                    arg(_p->hudInfo.info.layerName);
            }
            if (_p->hudInfo.visible[Util::HUD_SIZE])
            {
                upperLeft += qApp->translate("djv::ViewLib::ImageView", "Size  = %1x%2:%3").
                    arg(_p->hudInfo.info.size.x).
                    arg(_p->hudInfo.info.size.y).
                    arg(djvVectorUtil::aspect(_p->hudInfo.info.size), 0, 'f', 2);
            }
            if (_p->hudInfo.visible[Util::HUD_PROXY])
            {
                upperLeft += qApp->translate("djv::ViewLib::ImageView", "Proxy = %1").
                    arg(_p->hudInfo.info.proxy);
            }
            if (_p->hudInfo.visible[Util::HUD_PIXEL])
            {
                upperLeft += qApp->translate("djv::ViewLib::ImageView", "Pixel = %1").
                    arg(djvStringUtil::label(_p->hudInfo.info.pixel).join(", "));
            }

            // Generate the lower left contents.
            if (_p->hudInfo.visible[Util::HUD_TAG] && _p->hudInfo.tags.count())
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
            if (_p->hudInfo.visible[Util::HUD_FRAME])
            {
                upperRight += qApp->translate("djv::ViewLib::ImageView", "Frame = %1").
                    arg(djvTime::frameToString(_p->hudInfo.frame, _p->hudInfo.speed));
            }
            if (_p->hudInfo.visible[Util::HUD_SPEED])
            {
                upperRight += qApp->translate("djv::ViewLib::ImageView", "Speed = %1/%2").
                    arg(djvSpeed::speedToFloat(_p->hudInfo.speed), 0, 'f', 2).
                    arg(_p->hudInfo.realSpeed, 0, 'f', 2);
            }

            // Draw the HUD.
            drawHud(
                upperLeft,
                lowerLeft,
                upperRight,
                lowerRight);
        }

        void ImageView::drawHud(
            const QStringList & upperLeft,
            const QStringList & lowerLeft,
            const QStringList & upperRight,
            const QStringList & lowerRight)
        {
            //DJV_DEBUG("ImageView::drawHud");

            // Setup.
            const djvBox2i geom(width(), height());
            const int  margin = _p->context->style()->sizeMetric().widgetMargin;
            QSize      size;
            glm::ivec2 p;
            QString    s;

            // Draw the upper left contents.
            p = glm::ivec2(margin, margin);
            for (int i = 0; i < upperLeft.count(); ++i)
            {
                size = drawHudSize(upperLeft[i]);

                drawHud(upperLeft[i], p);

                p.y += size.height();
            }

            // Draw the lower left contents.
            p = glm::ivec2(margin, height() - size.height() * lowerLeft.count() - margin);
            for (int i = 0; i < lowerLeft.count(); ++i)
            {
                size = drawHudSize(lowerLeft[i]);

                drawHud(lowerLeft[i], p);

                p.y += size.height();
            }

            // Draw the upper right contents.
            p = glm::ivec2(geom.w - margin, margin);
            for (int i = 0; i < upperRight.count(); ++i)
            {
                size = drawHudSize(upperRight[i]);

                drawHud(upperRight[i], glm::ivec2(p.x - size.width(), p.y));

                p.y += size.height();
            }
        }

        namespace
        {
            const int margin = 2;

        } // namespace

        djvBox2i ImageView::drawHud(
            const QString &    in,
            const glm::ivec2 & position)
        {
            const int h = height();
            const int w = fontMetrics().width(in);
            const int a = fontMetrics().ascent();
            const int d = fontMetrics().descent();
            djvBox2i box(position.x, position.y, w + margin * 2, a + d + margin * 2);

            // Draw the background.
            //! \todo
            /*djvOpenGLUtil::color(_p->hudBackgroundColor);
            switch (_p->hudBackground)
            {
            case Util::HUD_BACKGROUND_NONE: break;
            case Util::HUD_BACKGROUND_SOLID:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBegin(GL_QUADS);
                glVertex2i(box.x, h - box.y - box.h);
                glVertex2i(box.x + box.w, h - box.y - box.h);
                glVertex2i(box.x + box.w, h - box.y + box.h - box.h);
                glVertex2i(box.x, h - box.y + box.h - box.h);
                glEnd();
                glDisable(GL_BLEND);
                break;
            case Util::HUD_BACKGROUND_SHADOW:
                //! \todo
                //renderText(
                //    position.x + margin + 1,
                //    position.y + margin + a + 1,
                //    in,
                //    font());
                break;
            default: break;
            }

            // Draw the foreground.
            djvOpenGLUtil::color(_p->hudColor);
            renderText(
                position.x + margin,
                position.y + a + margin,
                in,
                font());*/

            return box;
        }

        QSize ImageView::drawHudSize(const QString & s) const
        {
            const QSize size = fontMetrics().size(Qt::TextSingleLine, s);
            return QSize(size.width() + margin * 2, size.height() + margin * 2);
        }

    } // namespace ViewLib
} // namespace djv
