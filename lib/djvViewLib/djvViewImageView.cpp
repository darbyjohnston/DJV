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

//! \file djvViewImageView.cpp

#include <djvViewImageView.h>

#include <djvViewApplication.h>
#include <djvViewHudInfo.h>
#include <djvViewInputPrefs.h>
#include <djvViewViewPrefs.h>
#include <djvViewWindowPrefs.h>

#include <djvMiscPrefs.h>
#include <djvStyle.h>

#include <djvAssert.h>
#include <djvFileInfo.h>
#include <djvOpenGl.h>
#include <djvPixel.h>
#include <djvTime.h>

#include <QDesktopWidget>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QUrl>
#include <QWheelEvent>

//------------------------------------------------------------------------------
// djvViewImageView::P
//------------------------------------------------------------------------------

struct djvViewImageView::P
{
    P() :
        viewZoomTmp       (0.0),
        grid              (djvViewViewPrefs::global()->grid()),
        gridColor         (djvViewViewPrefs::global()->gridColor()),
        hudEnabled        (djvViewViewPrefs::global()->isHudEnabled()),
        hudColor          (djvViewViewPrefs::global()->hudColor()),
        hudBackground     (djvViewViewPrefs::global()->hudBackground()),
        hudBackgroundColor(djvViewViewPrefs::global()->hudBackgroundColor()),
        inside            (false),
        mouseWheel        (false),
        mouseWheelTmp     (0),
        timer             (-1)
    {}
    
    djvVector2i                 viewPosTmp;
    double                      viewZoomTmp;
    djvViewUtil::GRID           grid;
    djvColor                    gridColor;
    bool                        hudEnabled;
    djvViewHudInfo              hudInfo;
    djvColor                    hudColor;
    djvViewUtil::HUD_BACKGROUND hudBackground;
    djvColor                    hudBackgroundColor;
    bool                        inside;
    djvVector2i                 mousePos;
    djvVector2i                 mouseStartPos;
    bool                        mouseWheel;
    int                         mouseWheelTmp;
    int                         timer;
};

//------------------------------------------------------------------------------
// djvViewImageView
//------------------------------------------------------------------------------

djvViewImageView::djvViewImageView(QWidget * parent) :
    djvImageView(parent),
    _p(new P)
{
    //DJV_DEBUG("djvViewImageView::djvViewImageView");
    
    setMouseTracking(true);
    setAcceptDrops(true);

    connect(
        djvViewViewPrefs::global(),
        SIGNAL(gridColorChanged(const djvColor &)),
        SLOT(setGridColor(const djvColor &)));

    connect(
        djvViewViewPrefs::global(),
        SIGNAL(hudInfoChanged(const QVector<bool> &)),
        SLOT(hudInfoCallback(const QVector<bool> &)));

    connect(
        djvViewViewPrefs::global(),
        SIGNAL(hudColorChanged(const djvColor &)),
        SLOT(setHudColor(const djvColor &)));

    connect(
        djvViewViewPrefs::global(),
        SIGNAL(hudBackgroundChanged(djvViewUtil::HUD_BACKGROUND)),
        SLOT(setHudBackground(djvViewUtil::HUD_BACKGROUND)));

    connect(
        djvViewViewPrefs::global(),
        SIGNAL(hudBackgroundColorChanged(const djvColor &)),
        SLOT(setHudBackgroundColor(const djvColor &)));

    connect(
        djvMiscPrefs::global(),
        SIGNAL(timeUnitsChanged(djvTime::UNITS)),
        SLOT(update()));
}

djvViewImageView::~djvViewImageView()
{
    //DJV_DEBUG("djvViewImageView::~djvViewImageView");

    delete _p;
}

bool djvViewImageView::isMouseInside() const
{
    return _p->inside;
}

const djvVector2i & djvViewImageView::mousePos() const
{
    return _p->mousePos;
}

QSize djvViewImageView::sizeHint() const
{
    //DJV_DEBUG("djvViewImageView::sizeHint");
    //DJV_DEBUG_PRINT("zoom = " << viewZoom());

    djvVector2i size = djvVectorUtil::ceil<double, int>(bbox().size);
        
    if (! djvVectorUtil::isSizeValid(size))
    {
        size = djvVector2i(640, 300);
    }

    djvVector2i maxSize;
    
    const djvVector2i screenSize = djvVectorUtil::fromQSize(
        qApp->desktop()->availableGeometry().size());
    
    switch (djvViewWindowPrefs::global()->viewMax())
    {
        case djvViewUtil::VIEW_MAX_25:
        case djvViewUtil::VIEW_MAX_50:
        case djvViewUtil::VIEW_MAX_75:
        {
            double v = 0.0f;
            
            switch (djvViewWindowPrefs::global()->viewMax())
            {
                case djvViewUtil::VIEW_MAX_25: v = 0.25; break;
                case djvViewUtil::VIEW_MAX_50: v = 0.5;  break;
                case djvViewUtil::VIEW_MAX_75: v = 0.75; break;
                
                default: break;
            }
            
            maxSize = djvVectorUtil::ceil<double, int>(djvVector2f(screenSize) * v);
        }
        break;
        
        case djvViewUtil::VIEW_MAX_USER:
        
            maxSize = djvViewWindowPrefs::global()->viewMaxUser();
            
            break;
            
        default: break;
    }
    
    if (djvVectorUtil::isSizeValid(maxSize))
    {
        if (size.x > maxSize.x || size.y > maxSize.y)
        {
            const double aspect = size.x / static_cast<double>(size.y);
            
            const djvVector2i a(maxSize.x, maxSize.x / aspect);
            const djvVector2i b(maxSize.y * aspect, maxSize.y);

            size = a < b ? a : b;
        }
    }

    //DJV_DEBUG_PRINT("size = " << size);

    return QSize(size.x, size.y);
}

QSize djvViewImageView::minimumSizeHint() const
{
    return QSize(100, 100);
}

void djvViewImageView::setZoomFocus(double in)
{
    //DJV_DEBUG("djvViewImageView::setZoomFocus");
    //DJV_DEBUG_PRINT("in = " << in);
    
    setViewZoom(
        in,
        _p->inside ? _p->mousePos : (djvVector2i(width(), height()) / 2));
}

void djvViewImageView::setGrid(djvViewUtil::GRID grid)
{
    if (grid == _p->grid)
        return;

    _p->grid = grid;

    update();
}

void djvViewImageView::setGridColor(const djvColor & color)
{
    if (color == _p->gridColor)
        return;

    _p->gridColor = color;

    update();
}

void djvViewImageView::setHudEnabled(bool in)
{
    if (in == _p->hudEnabled)
        return;

    _p->hudEnabled = in;

    update();
}

void djvViewImageView::setHudInfo(const djvViewHudInfo & info)
{
    if (info == _p->hudInfo)
        return;

    _p->hudInfo = info;

    update();
}

void djvViewImageView::setHudColor(const djvColor & color)
{
    if (color == _p->hudColor)
        return;

    _p->hudColor = color;

    update();
}

void djvViewImageView::setHudBackground(djvViewUtil::HUD_BACKGROUND background)
{
    if (background == _p->hudBackground)
        return;

    _p->hudBackground = background;

    update();
}

void djvViewImageView::setHudBackgroundColor(const djvColor & color)
{
    if (color == _p->hudBackgroundColor)
        return;

    _p->hudBackgroundColor = color;

    update();
}

void djvViewImageView::timerEvent(QTimerEvent *)
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

void djvViewImageView::enterEvent(QEvent * event)
{
    djvImageView::enterEvent(event);

    _p->inside = true;
}

void djvViewImageView::leaveEvent(QEvent * event)
{
    djvImageView::leaveEvent(event);

    _p->inside = false;
}

void djvViewImageView::resizeEvent(QResizeEvent * event)
{
    //DJV_DEBUG("djvViewImageView::resizeEvent");

    djvImageView::resizeEvent(event);

    if (hasViewFit())
    {
        //DJV_DEBUG_PRINT("fit");
    
        viewFit();
    }
}

void djvViewImageView::mousePressEvent(QMouseEvent * event)
{
    setFocus(Qt::MouseFocusReason);
    
    _p->viewPosTmp  = viewPos();
    _p->viewZoomTmp = viewZoom();

    _p->mousePos      = djvVector2i(event->pos().x(), height() - event->pos().y() - 1);
    _p->mouseStartPos = _p->mousePos;

    if (Qt::LeftButton == event->button())
    {
        setCursor(Qt::CrossCursor);

        Q_EMIT pickChanged(_p->mousePos);
    }
    else if (Qt::MidButton == event->button() &&
        event->modifiers() & Qt::ControlModifier)
    {
        setViewZoom(viewZoom() * 2.0, _p->mousePos);
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
        
        Q_EMIT mouseWheelChanged(djvViewUtil::MOUSE_WHEEL_PLAYBACK_SHUTTLE);
    }
}

void djvViewImageView::mouseReleaseEvent(QMouseEvent *)
{
    setCursor(QCursor());
}

void djvViewImageView::mouseMoveEvent(QMouseEvent * event)
{
    _p->mousePos = djvVector2i(event->pos().x(), height() - event->pos().y() - 1);

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

        Q_EMIT mouseWheelValueChanged(
            (_p->mousePos.x - _p->mouseStartPos.x) / 5);
    }
}

void djvViewImageView::wheelEvent(QWheelEvent * event)
{
    djvViewUtil::MOUSE_WHEEL mouseWheel =
        djvViewInputPrefs::global()->mouseWheel();

    if (event->modifiers() & Qt::ShiftModifier)
    {
        mouseWheel = djvViewInputPrefs::global()->mouseWheelShift();
    }
    if (event->modifiers() & Qt::ControlModifier)
    {
        mouseWheel = djvViewInputPrefs::global()->mouseWheelCtrl();
    }

    const int delta =
#       if QT_VERSION < 0x050000
        event->delta();
#       else
        event->angleDelta().y();
#       endif

    switch (mouseWheel)
    {
        case djvViewUtil::MOUSE_WHEEL_VIEW_ZOOM:

            setZoomFocus(viewZoom() * (delta / 120.0 > 0 ? 1.1 : 0.9));

            break;

        case djvViewUtil::MOUSE_WHEEL_PLAYBACK_SHUTTLE:

            if (! _p->mouseWheel)
            {
                setCursor(Qt::SizeHorCursor);

                Q_EMIT mouseWheelChanged(mouseWheel);

                _p->mouseWheel = true;
            }

            _p->mouseWheelTmp += delta / 120.0;

            Q_EMIT mouseWheelValueChanged(_p->mouseWheelTmp);

            break;

        case djvViewUtil::MOUSE_WHEEL_PLAYBACK_SPEED:

            if (! _p->mouseWheel)
            {
                Q_EMIT mouseWheelChanged(mouseWheel);

                _p->mouseWheel = true;
            }

            _p->mouseWheelTmp += delta / 120.0;

            Q_EMIT mouseWheelValueChanged(_p->mouseWheelTmp);

            break;

        default: break;
    }

    if (_p->timer == -1)
    {
        _p->timer = startTimer(0);
    }
}

void djvViewImageView::dragEnterEvent(QDragEnterEvent * event)
{
    //DJV_DEBUG("djvViewImageView::dragEnterEvent");
    //Q_FOREACH(const QString & format, event->mimeData()->formats())
    //    DJV_DEBUG_PRINT("mime = " << format);

    if (event->mimeData()->hasFormat("application/x-filebrowser") ||
        event->mimeData()->hasFormat("text/uri-list") ||
        event->mimeData()->hasFormat("text/plain"))
    {
        event->acceptProposedAction();
    }
}

void djvViewImageView::dropEvent(QDropEvent * event)
{
    //DJV_DEBUG("djvViewImageView::dropEvent");
    //Q_FOREACH(const QString & format, event->mimeData()->formats())
    //    DJV_DEBUG_PRINT("mime = " << format);

    event->acceptProposedAction();

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
         
         fileInfo = fileName;
    }
    else if (event->mimeData()->hasFormat("text/plain"))
    {
         //DJV_DEBUG_PRINT("text/plain");

        fileInfo = event->mimeData()->text();
    }

     //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);

    Q_EMIT fileDropped(fileInfo);
}

void djvViewImageView::paintGL()
{
    //DJV_DEBUG("djvViewImageView::draw");

    djvImageView::paintGL();

    if (_p->grid)
    {
        drawGrid();
    }

    if (_p->hudEnabled)
    {
        drawHud();
    }
}

void djvViewImageView::hudInfoCallback(const QVector<bool> & info)
{
    _p->hudInfo.visible = info;

    update();
}

void djvViewImageView::drawGrid()
{
    //DJV_DEBUG("djvViewImageView::drawGrid");
    //DJV_DEBUG_PRINT("size = " << geom().size);
    //DJV_DEBUG_PRINT("grid = " << labelGrid()[_p->grid]);

    int inc = 0;

    switch (_p->grid)
    {
        case djvViewUtil::GRID_1x1:     inc = 1;   break;
        case djvViewUtil::GRID_10x10:   inc = 10;  break;
        case djvViewUtil::GRID_100x100: inc = 100; break;

        default: break;
    }

    // Bail if too small.

    if ((inc * viewZoom()) <= 2)
        return;

    // Compute the view area.

    djvBox2i area(
        djvVectorUtil::floor<double, int>(
            djvVector2f(-viewPos()) / viewZoom() / static_cast<double>(inc)) - 1,
        djvVectorUtil::ceil<double, int>(
            djvVector2f(width(), height()) / viewZoom() / static_cast<double>(inc)) + 2);

    area *= inc;
    
    //DJV_DEBUG_PRINT("area = " << area);

    // Draw.

    djvOpenGlUtil::color(_p->gridColor);

    glPushMatrix();
    glTranslated(viewPos().x, viewPos().y, 0);
    glScaled(viewZoom(), viewZoom(), 1.0);

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

    glPopMatrix();
}

void djvViewImageView::drawHud()
{
    //DJV_DEBUG("djvViewImageView::drawHud");

    QStringList upperLeft;
    QStringList lowerLeft;
    QStringList upperRight;
    QStringList lowerRight;

    // Generate the upper left contents.

    if (_p->hudInfo.visible[djvViewUtil::HUD_FILE_NAME])
    {
        upperLeft += QString(tr("File  = %1")).arg(_p->hudInfo.info.fileName);
    }

    if (_p->hudInfo.visible[djvViewUtil::HUD_LAYER])
    {
        upperLeft += QString(tr("Layer = %1")).arg(_p->hudInfo.info.layerName);
    }

    if (_p->hudInfo.visible[djvViewUtil::HUD_SIZE])
    {
        upperLeft += QString(tr("Size  = %1x%2:%3")).
            arg(_p->hudInfo.info.size.x).
            arg(_p->hudInfo.info.size.y).
            arg(djvVectorUtil::aspect(_p->hudInfo.info.size), 0, 'f', 2);
    }

    if (_p->hudInfo.visible[djvViewUtil::HUD_PROXY])
    {
        upperLeft += QString(tr("Proxy = %1")).arg(_p->hudInfo.info.proxy);
    }

    if (_p->hudInfo.visible[djvViewUtil::HUD_PIXEL])
    {
        upperLeft += QString(tr("Pixel = %1")).
            arg(djvStringUtil::label(_p->hudInfo.info.pixel).join(", "));
    }

    // Generate the lower left contents.

    if (_p->hudInfo.visible[djvViewUtil::HUD_TAG] && _p->hudInfo.tags.count())
    {
        const QStringList keys = _p->hudInfo.tags.keys();

        for (int i = 0; i < keys.count(); ++i)
        {
            lowerLeft += QString(tr("%1 = %2")).arg(keys[i]).
                arg(_p->hudInfo.tags[keys[i]]);
        }
    }

    // Generate the upper right contents.

    if (_p->hudInfo.visible[djvViewUtil::HUD_FRAME])
    {
        upperRight += QString(tr("Frame = %1")).
            arg(djvTime::frameToString(_p->hudInfo.frame, _p->hudInfo.speed));
    }

    if (_p->hudInfo.visible[djvViewUtil::HUD_SPEED])
    {
        upperRight += QString(tr("Speed = %1/%2")).
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

void djvViewImageView::drawHud(
    const QStringList & upperLeft,
    const QStringList & lowerLeft,
    const QStringList & upperRight,
    const QStringList & lowerRight)
{
    //DJV_DEBUG("djvViewImageView::drawHud");
    
    // Setup.

    const djvBox2i geom(width(), height());

    const int   margin = djvStyle::global()->sizeMetric().widgetMargin;
    QSize       size;
    djvVector2i p;
    QString     s;

    // Draw the upper left contents.

    p = djvVector2i(margin, margin);

    for (int i = 0; i < upperLeft.count(); ++i)
    {
        size = drawHudSize(upperLeft[i]);

        drawHud(upperLeft[i], p);

        p.y += size.height();
    }

    // Draw the lower left contents.

    p = djvVector2i(margin, height() - size.height() * lowerLeft.count() - margin);
    
    for (int i = 0; i < lowerLeft.count(); ++i)
    {
        size = drawHudSize(lowerLeft[i]);

        drawHud(lowerLeft[i], p);

        p.y += size.height();
    }

    // Draw the upper right contents.

    p = djvVector2i(geom.w - margin, margin);

    for (int i = 0; i < upperRight.count(); ++i)
    {
        size = drawHudSize(upperRight[i]);

        drawHud(upperRight[i], djvVector2i(p.x - size.width(), p.y));

        p.y += size.height();
    }
}

namespace
{

const int margin = 2;

}

djvBox2i djvViewImageView::drawHud(
    const QString &     in,
    const djvVector2i & position)
{
    const int h = height();
    const int w = fontMetrics().width(in);
    const int a = fontMetrics().ascent();
    const int d = fontMetrics().descent();

    djvBox2i box(position.x, position.y, w + margin * 2, a + d + margin * 2);

    // Draw the background.

    djvOpenGlUtil::color(_p->hudBackgroundColor);

    switch (_p->hudBackground)
    {
        case djvViewUtil::HUD_BACKGROUND_NONE: break;

        case djvViewUtil::HUD_BACKGROUND_SOLID:

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glBegin(GL_QUADS);
            glVertex2i(box.x,         h - box.y - box.h);
            glVertex2i(box.x + box.w, h - box.y - box.h);
            glVertex2i(box.x + box.w, h - box.y + box.h - box.h);
            glVertex2i(box.x,         h - box.y + box.h - box.h);
            glEnd();

            glDisable(GL_BLEND);

            break;

        case djvViewUtil::HUD_BACKGROUND_SHADOW:
        
            renderText(
                position.x + margin + 1,
                position.y + margin + a + 1,
                in,
                font());
            
            break;

        default: break;
    }

    // Draw the foreground.

    djvOpenGlUtil::color(_p->hudColor);

    renderText(
        position.x + margin,
        position.y + a + margin,
        in,
        font());

    return box;
}

QSize djvViewImageView::drawHudSize(const QString & s) const
{
    const QSize size = fontMetrics().size(Qt::TextSingleLine, s);

    return QSize(size.width() + margin * 2, size.height() + margin * 2);

}
