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
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE=
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

//! \file djvViewMiscWidget.cpp

#include <djvViewMiscWidget.h>

#include <djvViewApplication.h>
#include <djvViewFileCache.h>
#include <djvViewShortcutPrefs.h>

#include <djvFloatEdit.h>
#include <djvFloatObject.h>
#include <djvIconLibrary.h>
#include <djvMiscPrefs.h>
#include <djvPrefs.h>
#include <djvStyle.h>
#include <djvToolButton.h>

#include <djvBox.h>
#include <djvBoxUtil.h>
#include <djvColorUtil.h>
#include <djvDebug.h>
#include <djvPixel.h>
#include <djvRange.h>
#include <djvRangeUtil.h>
#include <djvSequence.h>
#include <djvSequenceUtil.h>
#include <djvSignalBlocker.h>

#include <QCursor>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvViewCacheSizeWidget::P
//------------------------------------------------------------------------------

struct djvViewCacheSizeWidget::P
{
    P() :
        cacheSize(0.0),
        edit     (0),
        button   (0)
    {}

    QVector<double> cacheSizes;
    double          cacheSize;

    djvFloatEdit *  edit;
    djvToolButton * button;
};

//------------------------------------------------------------------------------
// djvViewCacheSizeWidget
//------------------------------------------------------------------------------

djvViewCacheSizeWidget::djvViewCacheSizeWidget(QWidget * parent) :
    QWidget(parent),
    _p(new P)
{
    // Create widgets.

    _p->edit = new djvFloatEdit;
    _p->edit->setRange(0.0, 1024.0);
    _p->edit->object()->setInc(1.0, 5.0);

    _p->button = new djvToolButton;
    _p->button->setIcon(djvIconLibrary::global()->icon("djvSubMenuIcon.png"));
    _p->button->setIconSize(QSize(20, 20));

    // Layout the widgets.

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(_p->edit);
    layout->addWidget(_p->button);

    // Initialize.

    widgetUpdate();

    // Setup the callbacks.

    connect(
        _p->edit,
        SIGNAL(valueChanged(double)),
        SLOT(setCacheSize(double)));

    connect(
        _p->button,
        SIGNAL(pressed()),
        SLOT(buttonCallback()));
}

djvViewCacheSizeWidget::~djvViewCacheSizeWidget()
{
    delete _p;
}

const QVector<double> & djvViewCacheSizeWidget::cacheSizes() const
{
    return _p->cacheSizes;
}

double djvViewCacheSizeWidget::cacheSize() const
{
    return _p->cacheSize;
}

void djvViewCacheSizeWidget::setCacheSizes(const QVector<double> & in)
{
    if (in == _p->cacheSizes)
        return;

    _p->cacheSizes = in;

    widgetUpdate();

    Q_EMIT cacheSizesChanged(_p->cacheSizes);
}

void djvViewCacheSizeWidget::setCacheSize(double size)
{
    if (size == _p->cacheSize)
        return;

    _p->cacheSize = size;

    widgetUpdate();

    Q_EMIT cacheSizeChanged(_p->cacheSize);
}

void djvViewCacheSizeWidget::buttonCallback()
{
    QMenu menu;

    const QStringList & labels = djvViewFileCache::sizeLabels();

    for (int i = 0; i < labels.count(); ++i)
    {
        QAction * action = menu.addAction(
            labels[i],
            this,
            SLOT(menuCallback()));
        action->setData(djvViewFileCache::sizeDefaults()[i]);
    }

    menu.exec(mapToGlobal(
        QPoint(_p->button->x(), _p->button->y() + _p->button->height())));

    _p->button->setDown(false);
}

void djvViewCacheSizeWidget::menuCallback()
{
    QAction * action = qobject_cast<QAction *>(sender());

    setCacheSize(action->data().toInt());
}

void djvViewCacheSizeWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->edit);

    _p->edit->setValue(_p->cacheSize);
}

//------------------------------------------------------------------------------
// djvViewFrameWidget::P
//------------------------------------------------------------------------------

struct djvViewFrameWidget::P
{
    P() :
        frame(0)
    {}

    qint64       frame;
    djvFrameList frameList;
    djvSpeed     speed;
    QString      text;
};

//------------------------------------------------------------------------------
// djvViewFrameWidget
//------------------------------------------------------------------------------

djvViewFrameWidget::djvViewFrameWidget(QWidget * parent) :
    QAbstractSpinBox(parent),
    _p(new P)
{
    // Initialize.

    textUpdate();
    widgetUpdate();

    // Setup callbacks.

    connect(
        this,
        SIGNAL(editingFinished()),
        SLOT(editingFinishedCallback()));

    connect(
        djvMiscPrefs::global(),
        SIGNAL(timeUnitsChanged(djvTime::UNITS)),
        SLOT(timeUnitsCallback()));
}

djvViewFrameWidget::~djvViewFrameWidget()
{
    delete _p;
}

qint64 djvViewFrameWidget::frame() const
{
    return _p->frame;
}

const djvFrameList & djvViewFrameWidget::frameList() const
{
    return _p->frameList;
}

const djvSpeed & djvViewFrameWidget::speed() const
{
    return _p->speed;
}

void djvViewFrameWidget::stepBy(int steps)
{
    setFrame(_p->frame + steps);
}

QSize djvViewFrameWidget::sizeHint() const
{
    QString sizeString;

    switch (djvTime::units())
    {
        case djvTime::UNITS_TIMECODE: sizeString = "00:00:00:00"; break;
        case djvTime::UNITS_FRAMES:   sizeString = "000000";      break;

        default: break;
    }

    return QSize(
        fontMetrics().width(sizeString) + 25,
        QAbstractSpinBox::sizeHint().height());
}

void djvViewFrameWidget::setFrameList(const djvFrameList & in)
{
    if (in == _p->frameList)
        return;

    _p->frameList = in;

    setFrame(_p->frame);

    textUpdate();
    widgetUpdate();
}

void djvViewFrameWidget::setFrame(qint64 frame)
{
    const int tmp = djvMath::min(
        frame,
        static_cast<qint64>(_p->frameList.count() - 1));

    if (tmp == _p->frame)
        return;

    //DJV_DEBUG("djvViewFrameWidget::setFrame");
    //DJV_DEBUG_PRINT("frame = " << tmp);

    _p->frame = tmp;

    textUpdate();
    widgetUpdate();
    update();

    Q_EMIT frameChanged(_p->frame);
}

void djvViewFrameWidget::setSpeed(const djvSpeed & in)
{
    if (in == _p->speed)
        return;

    _p->speed = in;

    textUpdate();
    widgetUpdate();
}

QAbstractSpinBox::StepEnabled djvViewFrameWidget::stepEnabled() const
{
    QAbstractSpinBox::StepEnabled step = QAbstractSpinBox::StepNone;

    const int count = _p->frameList.count();

    if (count > 0)
    {
        if (_p->frame > 0)
        {
            step |= QAbstractSpinBox::StepDownEnabled;
        }

        if (_p->frame < count - 1)
        {
            step |= QAbstractSpinBox::StepUpEnabled;
        }
    }

    return step;
}

void djvViewFrameWidget::editingFinishedCallback()
{
    //DJV_DEBUG("djvViewFrameWidget::editingFinishedCallback");

    const QString text = lineEdit()->text();

    //DJV_DEBUG_PRINT("text = " << text);

    setFrame(djvSequenceUtil::findClosest(
        djvTime::stringToFrame(text, _p->speed), _p->frameList));

    lineEdit()->setText(_p->text);
}

void djvViewFrameWidget::timeUnitsCallback()
{
    textUpdate();
    widgetUpdate();

    updateGeometry();
}

void djvViewFrameWidget::textUpdate()
{
    qint64 frame = 0;

    if (_p->frame >= 0 &&
        _p->frame < static_cast<qint64>(_p->frameList.count()))
    {
        frame = _p->frameList[_p->frame];
    }

    _p->text = djvTime::frameToString(frame, _p->speed);
}

void djvViewFrameWidget::widgetUpdate()
{
    //djvSignalBlocker signalBlocker(lineEdit());

    lineEdit()->setText(_p->text);
}

//------------------------------------------------------------------------------
// djvViewFrameSlider::P
//------------------------------------------------------------------------------

struct djvViewFrameSlider::P
{
    P() :
        frame       (0),
        inOutEnabled(false),
        inPoint     (0),
        outPoint    (0)
    {}

    qint64       frame;
    djvFrameList frameList;
    djvSpeed     speed;
    bool         inOutEnabled;
    qint64       inPoint;
    qint64       outPoint;
    djvFrameList cachedFrames;
};

//------------------------------------------------------------------------------
// djvViewFrameSlider
//------------------------------------------------------------------------------

djvViewFrameSlider::djvViewFrameSlider(QWidget * parent) :
    QWidget(parent),
    _p(new P)
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    connect(
        djvMiscPrefs::global(),
        SIGNAL(timeUnitsChanged(djvTime::UNITS)),
        SLOT(timeUnitsCallback()));
}

djvViewFrameSlider::~djvViewFrameSlider()
{
    delete _p;
}

qint64 djvViewFrameSlider::frame() const
{
    return _p->frame;
}

const djvFrameList & djvViewFrameSlider::frameList() const
{
    return _p->frameList;
}

const djvSpeed & djvViewFrameSlider::speed() const
{
    return _p->speed;
}

bool djvViewFrameSlider::isInOutEnabled() const
{
    return _p->inOutEnabled;
}

qint64 djvViewFrameSlider::inPoint() const
{
    return _p->inPoint;
}

qint64 djvViewFrameSlider::outPoint() const
{
    return _p->outPoint;
}
    
QSize djvViewFrameSlider::sizeHint() const
{
    return QSize(200, fontMetrics().height() * 2 + 5 * 2);
}

void djvViewFrameSlider::setFrameList(const djvFrameList & in)
{
    if (in == _p->frameList)
        return;

    _p->frameList = in;
    
    setFrame(_p->frame);

    update();
}

void djvViewFrameSlider::setFrame(qint64 frame)
{
    const int tmp = djvMath::clamp(
        frame,
        static_cast<qint64>(0),
        end());
    
    if (tmp == _p->frame)
        return;
    
    //DJV_DEBUG("djvViewFrameSlider::setFrame");
    //DJV_DEBUG_PRINT("frame = " << tmp);

    _p->frame = tmp;

    update();

    Q_EMIT frameChanged(_p->frame);
}

void djvViewFrameSlider::setSpeed(const djvSpeed & in)
{
    if (in == _p->speed)
        return;

    _p->speed = in;

    update();
}

void djvViewFrameSlider::setInOutEnabled(bool in)
{
    if (in == _p->inOutEnabled)
        return;
    
    _p->inOutEnabled = in;

    update();

    Q_EMIT inOutEnabledChanged(_p->inOutEnabled);
}

void djvViewFrameSlider::setInOutPoints(qint64 inPoint, qint64 outPoint)
{
    if (inPoint == _p->inPoint && outPoint == _p->outPoint)
        return;
    
    _p->inPoint  = inPoint;
    _p->outPoint = outPoint;

    update();

    Q_EMIT inPointChanged(_p->inPoint);
    Q_EMIT outPointChanged(_p->outPoint);
}

void djvViewFrameSlider::setInPoint(qint64 frame)
{
    setInOutPoints(frame, _p->outPoint);
}

void djvViewFrameSlider::setOutPoint(qint64 frame)
{
    setInOutPoints(_p->inPoint, frame);
}

void djvViewFrameSlider::markInPoint()
{
    setInPoint(_p->frame);
}

void djvViewFrameSlider::markOutPoint()
{
    setOutPoint(_p->frame);
}

void djvViewFrameSlider::resetInOutPoints()
{
    setInOutPoints(0, end());
}

void djvViewFrameSlider::resetInPoint()
{
    setInPoint(0);
}

void djvViewFrameSlider::resetOutPoint()
{
    setOutPoint(end());
}

void djvViewFrameSlider::gotoStartFrame()
{
    setFrame(_p->inPoint);
}

void djvViewFrameSlider::gotoEndFrame()
{
    setFrame(_p->outPoint);
}

void djvViewFrameSlider::setCachedFrames(const djvFrameList & in)
{
    if (in == _p->cachedFrames)
        return;
    
    _p->cachedFrames = in;

    update();
}

void djvViewFrameSlider::mousePressEvent(QMouseEvent * event)
{
    setFrame(posToFrame(event->pos().x()));
    
    Q_EMIT pressed(true);
}

void djvViewFrameSlider::mouseReleaseEvent(QMouseEvent * event)
{
    Q_EMIT pressed(false);
}

void djvViewFrameSlider::mouseMoveEvent(QMouseEvent * event)
{
    setFrame(posToFrame(event->pos().x()));
}

namespace
{
    
struct Tick
{
    enum TYPE
    {
        FRAME,
        SECOND,
        CURRENT
    };

    Tick() :
        type(static_cast<TYPE>(0)),
        x   (0.0)
    {}
    
    TYPE    type;
    double  x;
    QString label;
    QRect   labelRect;

    void draw(QPainter * painter, const QPalette & palette, int h)
    {
        switch (type)
        {
            case FRAME:

                painter->setPen(djvColorUtil::lerp(
                    0.2,
                    palette.color(QPalette::Base),
                    palette.color(QPalette::Text)));
                painter->drawLine(x, 0, x, 5);
                painter->drawLine(x, h - 5, x, h - 1);

                break;

            case SECOND:

                if (label.isEmpty())
                {
                    painter->setPen(djvColorUtil::lerp(
                        0.2,
                        palette.color(QPalette::Base),
                        palette.color(QPalette::Text)));
                    painter->drawLine(x, 0, x, 5);
                    painter->drawLine(x, h - 5, x, h - 1);
                }
                else
                {
                    painter->setPen(djvColorUtil::lerp(
                        0.2,
                        palette.color(QPalette::Base),
                        palette.color(QPalette::Text)));
                    painter->drawLine(x, 0, x, h - 1);
                    painter->setPen(djvColorUtil::lerp(
                        0.4,
                        palette.color(QPalette::Base),
                        palette.color(QPalette::Text)));
                    painter->drawText(labelRect, label);
                }

                break;

            case CURRENT:

                painter->setPen(palette.color(QPalette::Text));
                painter->drawLine(x, 0, x, h - 1);
                painter->drawText(labelRect, label);

                break;
        }
    }
};

} // namespace

void djvViewFrameSlider::paintEvent(QPaintEvent * event)
{
    //DJV_DEBUG("djvViewFrameSlider::paintEvent");

    const djvBox2i box(0, 0, width(), height());
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QPalette palette(this->palette());

    // Fill the background.
    
    painter.fillRect(
        box.x,
        box.y,
        box.w,
        box.h,
        palette.color(QPalette::Base));

    // Draw the in/out points.

    if (_p->inPoint != 0 || _p->outPoint != end())
    {
        const QRectF r(
            frameToPosF(_p->inPoint),
            0,
            frameToPosF(_p->outPoint - _p->inPoint + 1),
            2);

        painter.fillRect(r, palette.color(QPalette::Highlight));
    }

    // Draw the cached frames.

    if (_p->cachedFrames.count())
    {
        //DJV_DEBUG_PRINT("cached frames = " << _p->cachedFrames.count());

        const djvFrameRangeList list = djvRangeUtil::range(_p->cachedFrames);

        for (int i = 0; i < list.count(); ++i)
        {
            const QRectF r(
                frameToPosF(list[i].min),
                box.h - 2,
                frameToPosF(list[i].max - list[i].min + 1),
                2);

            painter.fillRect(r, QColor(90, 90, 255));
        }
    }

    // Draw the frame ticks.
    
    QVector<Tick> ticks;

    const double speed       = djvSpeed::speedToFloat(_p->speed);
    const bool   drawFrames  = frameToPosF(1) > 3;
    const bool   drawSeconds = frameToPosF(static_cast<int>(speed)) > 3;
    
    for (int i = 0; i < _p->frameList.count(); ++i)
    {
        Tick tick;
        tick.type = Tick::FRAME;

        bool drawTick = drawFrames;
        bool drawLabel = false;

        if (speed > 0.0 && 0 == i % static_cast<int>(speed))
        {
            drawTick  = drawSeconds;
            drawLabel = true;
            tick.type = Tick::SECOND;
        }

        if (drawTick)
        {
            tick.x = (frameToPosF(i) + frameToPosF(i + 1)) / 2.0;
            
            if (drawLabel)
            {
                tick.label = djvTime::frameToString(
                    _p->frameList[i],
                    _p->speed);
            }
            
            ticks.append(tick);
        }
    }
    
    int labelWidthMax = 0;

    const int spacing = djvStyle::global()->sizeMetric().spacing;

    for (int i = 0; i < ticks.count(); ++i)
    {
        const QRect labelBounds =
            fontMetrics().boundingRect(ticks[i].label);

        ticks[i].labelRect = QRect(
            ticks[i].x + spacing,
            box.h / 2,
            labelBounds.width(),
            labelBounds.height());

        labelWidthMax = djvMath::max(
            labelWidthMax,
            ticks[i].labelRect.width());
    }

    for (int i = 0; i < ticks.count(); ++i)
    {
        ticks[i].labelRect.setWidth(labelWidthMax);
    }
    
    int j = 0;
    
    for (int i = 1; i < ticks.count(); ++i)
    {
        if (! ticks[i].label.isEmpty())
        {
            if (ticks[i].labelRect.adjusted(-spacing, 0, 0, 0).intersects(
                ticks[j].labelRect.adjusted(0, 0, spacing, 0)))
            {
                ticks[i].label = QString();
            }
            else
            {
                j = i;
            }
        }
    }
    
    for (int i = 0; i < ticks.count(); ++i)
    {
        ticks[i].draw(&painter, palette, box.h);
    }

    // Draw the current frame.
    
    Tick current;
    current.type = Tick::CURRENT;
    current.x = (frameToPosF(_p->frame) + frameToPosF(_p->frame + 1)) / 2.0;
    current.label = djvTime::frameToString(
        _p->frameList.count() ? _p->frameList[_p->frame] : 0, speed);
    const QRect labelBounds =
        fontMetrics().boundingRect(current.label);
    current.labelRect = QRect(
        current.x + spacing,
        5,
        labelWidthMax,
        labelBounds.height());
    if (current.labelRect.right() > box.w)
        current.labelRect.moveLeft(current.x - spacing - labelWidthMax);
    current.draw(&painter, palette, box.h);
}

void djvViewFrameSlider::timeUnitsCallback()
{
    updateGeometry();
    update();
}

qint64 djvViewFrameSlider::end() const
{
    return _p->frameList.count() ? (_p->frameList.count() - 1) : 0;
}

qint64 djvViewFrameSlider::posToFrame(int pos) const
{
    const double fw = width() / static_cast<double>(_p->frameList.count());

    const double v = pos / static_cast<double>(width() - fw);
    
    return static_cast<qint64>(v * end());
}

double djvViewFrameSlider::frameToPosF(qint64 frame) const
{
    const double fw =
        _p->frameList.count() ?
        (width() / static_cast<double>(_p->frameList.count())) :
        0.0;

    const qint64 end = this->end();

    const double v =
        end ?
        (frame / static_cast<double>(end)) :
        0.0;

    return v * (width() - fw);
}

int djvViewFrameSlider::frameToPos(qint64 frame) const
{
    const double fw = _p->frameList.count() ?
        width() / static_cast<double>(_p->frameList.count()) :
        0.0;

    const qint64 end = this->end();

    const double v =
        end ?
        frame / static_cast<double>(end) :
        0.0;

    return static_cast<int>(v * (width() - fw));
}

//------------------------------------------------------------------------------
// djvViewFrameDisplay::P
//------------------------------------------------------------------------------

struct djvViewFrameDisplay::P
{
    P() :
        frame       (0),
        inOutEnabled(false)
    {}

    qint64   frame;
    djvSpeed speed;
    bool     inOutEnabled;
    QString  text;
    
    QLineEdit * lineEdit;
};

//------------------------------------------------------------------------------
// djvViewFrameDisplay
//------------------------------------------------------------------------------

djvViewFrameDisplay::djvViewFrameDisplay(QWidget * parent) :
    QWidget(parent),
    _p(new P)
{
    // Create the widgets.

    _p->lineEdit = new QLineEdit;

    // Layout the widgets.

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(_p->lineEdit);

    // Initialize.

    textUpdate();
    widgetUpdate();
    
    // Setup the callbacks.

    connect(
        djvMiscPrefs::global(),
        SIGNAL(timeUnitsChanged(djvTime::UNITS)),
        SLOT(timeUnitsCallback()));
}

djvViewFrameDisplay::~djvViewFrameDisplay()
{
    delete _p;
}

qint64 djvViewFrameDisplay::frame() const
{
    return _p->frame;
}

const djvSpeed & djvViewFrameDisplay::speed() const
{
    return _p->speed;
}

bool djvViewFrameDisplay::isInOutEnabled() const
{
    return _p->inOutEnabled;
}

QSize djvViewFrameDisplay::sizeHint() const
{
    QString sizeString;

    switch (djvTime::units())
    {
        case djvTime::UNITS_TIMECODE: sizeString = "00:00:00:00"; break;
        case djvTime::UNITS_FRAMES:   sizeString = "000000";      break;

        default: break;
    }
    
    return QSize(
        fontMetrics().width(sizeString) + 10,
        QWidget::sizeHint().height());
}

void djvViewFrameDisplay::setFrame(qint64 in)
{
    if (in == _p->frame)
        return;

    _p->frame = in;

    textUpdate();
    widgetUpdate();
}

void djvViewFrameDisplay::setSpeed(const djvSpeed & in)
{
    if (in == _p->speed)
        return;

    _p->speed = in;

    textUpdate();
    widgetUpdate();
}

void djvViewFrameDisplay::setInOutEnabled(bool in)
{
    if (in == _p->inOutEnabled)
        return;

    _p->inOutEnabled = in;

    widgetUpdate();
}

void djvViewFrameDisplay::timeUnitsCallback()
{
    textUpdate();
    widgetUpdate();

    updateGeometry();
}

void djvViewFrameDisplay::textUpdate()
{
    _p->text = djvTime::frameToString(_p->frame, _p->speed);
}

void djvViewFrameDisplay::widgetUpdate()
{
    djvSignalBlocker signalBlocker(_p->lineEdit);

    _p->lineEdit->setText(_p->text);

    QPalette palette = this->palette();
    if (_p->inOutEnabled)
        palette.setColor(QPalette::Text, palette.color(QPalette::Highlight));
    _p->lineEdit->setPalette(palette);
}

//------------------------------------------------------------------------------
// djvViewSpeedButton::P
//------------------------------------------------------------------------------

struct djvViewSpeedButton::P
{
    P() :
        button(0)
    {}
    
    djvSpeed speed;
    djvSpeed defaultSpeed;
    
    djvToolButton * button;
};

//------------------------------------------------------------------------------
// djvViewSpeedButton
//------------------------------------------------------------------------------

djvViewSpeedButton::djvViewSpeedButton(QWidget * parent) :
    QWidget(parent),
    _p(new P)
{
    // Create the widgets.
    
    _p->button = new djvToolButton;
    _p->button->setIcon(djvIconLibrary::global()->icon("djvSubMenuIcon.png"));
    _p->button->setIconSize(QSize(20, 20));

    // Layout the widgets.
    
    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(_p->button);

    // Setup the callbacks.
    
    connect(
        _p->button,
        SIGNAL(pressed()),
        SLOT(pressedCallback()));
}

djvViewSpeedButton::~djvViewSpeedButton()
{
    delete _p;
}

void djvViewSpeedButton::setDefaultSpeed(const djvSpeed & in)
{
    _p->defaultSpeed = in;
}

void djvViewSpeedButton::pressedCallback()
{
    QMenu menu;
    
    if (_p->defaultSpeed.isValid())
    {
        const QString text = tr("Default: %1").
            arg(djvSpeed::speedToFloat(_p->defaultSpeed), 0, 'f', 2);

        QAction * action = menu.addAction(
            text,
            this,
            SLOT(menuCallback()));
        action->setData(-1);
        
        menu.addSeparator();
    }
    
    const QStringList & labels = djvSpeed::fpsLabels();

    for (int i = 0; i < labels.count(); ++i)
    {
        QAction * action = menu.addAction(
            labels[i],
            this,
            SLOT(menuCallback()));
        action->setData(i);
    }
    
    menu.exec(mapToGlobal(QPoint(0, height())));
    
    _p->button->setDown(false);
}

void djvViewSpeedButton::menuCallback()
{
    QAction * action = qobject_cast<QAction *>(sender());
    
    const int index = action->data().toInt();
    
    Q_EMIT speedChanged(
        -1 == index ? _p->defaultSpeed : static_cast<djvSpeed::FPS>(index));
}

//------------------------------------------------------------------------------
// djvViewSpeedWidget::P
//------------------------------------------------------------------------------

struct djvViewSpeedWidget::P
{
    P() :
        floatEdit(0),
        button   (0)
    {}
    
    djvSpeed speed;
    djvSpeed defaultSpeed;
    
    djvFloatEdit *       floatEdit;
    djvViewSpeedButton * button;
};

//------------------------------------------------------------------------------
// djvViewSpeedWidget
//------------------------------------------------------------------------------

djvViewSpeedWidget::djvViewSpeedWidget(QWidget * parent) :
    QWidget(parent),
    _p(new P)
{
    // Create the widgets.
    
    _p->floatEdit = new djvFloatEdit;
    _p->floatEdit->setRange(1.0, 1000.0);
    _p->floatEdit->object()->setInc(1.0, 10.0);
    
    _p->button = new djvViewSpeedButton;
    
    // Layout the widgets.
    
    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(_p->floatEdit, 1);
    layout->addWidget(_p->button);

    // Initialize.
    
    widgetUpdate();

    // Setup the callbacks.
    
    connect(
        _p->floatEdit,
        SIGNAL(valueChanged(double)),
        SLOT(editCallback(double)));
    
    connect(
        _p->button,
        SIGNAL(speedChanged(const djvSpeed &)),
        SLOT(setSpeed(const djvSpeed &)));
}

djvViewSpeedWidget::~djvViewSpeedWidget()
{
    delete _p;
}

const djvSpeed & djvViewSpeedWidget::speed() const
{
    return _p->speed;
}

const djvSpeed & djvViewSpeedWidget::defaultSpeed() const
{
    return _p->defaultSpeed;
}

void djvViewSpeedWidget::setSpeed(const djvSpeed & in)
{
    if (in == _p->speed)
        return;

    _p->speed = in;

    widgetUpdate();

    Q_EMIT speedChanged(_p->speed);
}

void djvViewSpeedWidget::setDefaultSpeed(const djvSpeed & in)
{
    if (in == _p->defaultSpeed)
        return;

    _p->defaultSpeed = in;
    
    widgetUpdate();
}

void djvViewSpeedWidget::editCallback(double in)
{
    const djvSpeed speed = djvSpeed::floatToSpeed(in);

    setSpeed(speed);
    
    widgetUpdate();
}

void djvViewSpeedWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(_p->floatEdit);
    
    _p->floatEdit->setValue(djvSpeed::speedToFloat(_p->speed));
    
    _p->button->setDefaultSpeed(_p->defaultSpeed);
}

//------------------------------------------------------------------------------
// djvViewSpeedDisplay::P
//------------------------------------------------------------------------------

struct djvViewSpeedDisplay::P
{
    P() :
        speed        (0.0),
        droppedFrames(false),
        lineEdit     (0)
    {}

    double      speed;
    bool        droppedFrames;
    QLineEdit * lineEdit;
};

//------------------------------------------------------------------------------
// djvViewSpeedDisplay
//------------------------------------------------------------------------------

djvViewSpeedDisplay::djvViewSpeedDisplay(QWidget * parent) :
    QWidget(parent),
    _p(new P)
{
    _p->lineEdit = new QLineEdit;
    _p->lineEdit->setReadOnly(true);

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(_p->lineEdit);

    widgetUpdate();
}

djvViewSpeedDisplay::~djvViewSpeedDisplay()
{
    delete _p;
}

QSize djvViewSpeedDisplay::sizeHint() const
{
    QString sizeString("000.00");

    return QSize(
        fontMetrics().width(sizeString) + 10,
        QWidget::sizeHint().height());
}

void djvViewSpeedDisplay::setSpeed(double speed)
{
    if (djvMath::fuzzyCompare(speed, _p->speed))
        return;

    _p->speed = speed;

    widgetUpdate();
}

void djvViewSpeedDisplay::setDroppedFrames(bool in)
{
    if (in == _p->droppedFrames)
        return;

    _p->droppedFrames = in;

    widgetUpdate();
}

void djvViewSpeedDisplay::widgetUpdate()
{
    djvSignalBlocker signalBlocker(_p->lineEdit);

    QPalette palette = this->palette();
    if (_p->droppedFrames)
        palette.setColor(QPalette::Text, palette.color(QPalette::Highlight));
    _p->lineEdit->setPalette(palette);

    _p->lineEdit->setText(QString("%1").arg(_p->speed, 0, 'f', 2));
}
