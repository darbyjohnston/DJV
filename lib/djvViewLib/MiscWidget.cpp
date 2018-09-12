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

#include <djvViewLib/MiscWidget.h>

#include <djvViewLib/FileCache.h>

#include <djvUI/FloatEdit.h>
#include <djvUI/FloatObject.h>
#include <djvUI/UIContext.h>
#include <djvUI/IconLibrary.h>
#include <djvUI/Prefs.h>
#include <djvUI/Style.h>
#include <djvUI/TimePrefs.h>
#include <djvUI/ToolButton.h>

#include <djvGraphics/ColorUtil.h>
#include <djvGraphics/Pixel.h>

#include <djvCore/Box.h>
#include <djvCore/BoxUtil.h>
#include <djvCore/Debug.h>
#include <djvCore/Range.h>
#include <djvCore/RangeUtil.h>
#include <djvCore/Sequence.h>
#include <djvCore/SequenceUtil.h>
#include <djvCore/SignalBlocker.h>
#include <djvCore/Time.h>

#include <QApplication>
#include <QCursor>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QVBoxLayout>

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

        TYPE    type = static_cast<TYPE>(0);
        float   x = 0.f;
        QString label;
        QRect   labelRect;

        void draw(QPainter * painter, const QPalette & palette, int h)
        {
            switch (type)
            {
            case FRAME:
                painter->setPen(djvColorUtil::lerp(
                    .2f,
                    palette.color(QPalette::Base),
                    palette.color(QPalette::Text)));
                painter->drawLine(x, 0, x, 5);
                painter->drawLine(x, h - 5, x, h - 1);
                break;
            case SECOND:
                if (label.isEmpty())
                {
                    painter->setPen(djvColorUtil::lerp(
                        .2f,
                        palette.color(QPalette::Base),
                        palette.color(QPalette::Text)));
                    painter->drawLine(x, 0, x, 5);
                    painter->drawLine(x, h - 5, x, h - 1);
                }
                else
                {
                    painter->setPen(djvColorUtil::lerp(
                        .2f,
                        palette.color(QPalette::Base),
                        palette.color(QPalette::Text)));
                    painter->drawLine(x, 0, x, h - 1);
                    painter->setPen(djvColorUtil::lerp(
                        .4f,
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

namespace djv
{

    namespace ViewLib
    {
        struct CacheSizeWidget::Private
        {
            QVector<float> cacheSizes;
            float cacheSize = 0.f;
            UI::FloatEdit *  edit = nullptr;
            UI::ToolButton * button = nullptr;
        };

        CacheSizeWidget::CacheSizeWidget(UI::UIContext * context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            // Create widgets.
            _p->edit = new UI::FloatEdit;
            _p->edit->setRange(0.f, 1024.f);
            _p->edit->object()->setInc(1.f, 5.f);

            _p->button = new UI::ToolButton;
            _p->button->setIcon(context->iconLibrary()->icon("djvSubMenuIcon.png"));
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
                SIGNAL(valueChanged(float)),
                SLOT(setCacheSize(float)));
            connect(
                _p->button,
                SIGNAL(pressed()),
                SLOT(buttonCallback()));
        }

        CacheSizeWidget::~CacheSizeWidget()
        {}

        const QVector<float> & CacheSizeWidget::cacheSizes() const
        {
            return _p->cacheSizes;
        }

        float CacheSizeWidget::cacheSize() const
        {
            return _p->cacheSize;
        }

        void CacheSizeWidget::setCacheSizes(const QVector<float> & in)
        {
            if (in == _p->cacheSizes)
                return;
            _p->cacheSizes = in;
            widgetUpdate();
            Q_EMIT cacheSizesChanged(_p->cacheSizes);
        }

        void CacheSizeWidget::setCacheSize(float size)
        {
            if (size == _p->cacheSize)
                return;
            _p->cacheSize = size;
            widgetUpdate();
            Q_EMIT cacheSizeChanged(_p->cacheSize);
        }

        void CacheSizeWidget::buttonCallback()
        {
            QMenu menu;
            const QStringList & labels = FileCache::sizeLabels();
            for (int i = 0; i < labels.count(); ++i)
            {
                QAction * action = menu.addAction(
                    labels[i],
                    this,
                    SLOT(menuCallback()));
                action->setData(FileCache::sizeDefaults()[i]);
            }
            menu.exec(mapToGlobal(
                QPoint(_p->button->x(), _p->button->y() + _p->button->height())));
            _p->button->setDown(false);
        }

        void CacheSizeWidget::menuCallback()
        {
            QAction * action = qobject_cast<QAction *>(sender());
            setCacheSize(action->data().toInt());
        }

        void CacheSizeWidget::widgetUpdate()
        {
            djvSignalBlocker signalBlocker(QObjectList() <<
                _p->edit);
            _p->edit->setValue(_p->cacheSize);
        }

        struct FrameWidget::Private
        {
            qint64       frame = 0;
            djvFrameList frameList;
            djvSpeed     speed;
            QString      text;
        };

        FrameWidget::FrameWidget(UI::UIContext * context, QWidget * parent) :
            QAbstractSpinBox(parent),
            _p(new Private)
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
                context->timePrefs(),
                SIGNAL(timeUnitsChanged(djvTime::UNITS)),
                SLOT(timeUnitsCallback()));
        }

        FrameWidget::~FrameWidget()
        {}

        qint64 FrameWidget::frame() const
        {
            return _p->frame;
        }

        const djvFrameList & FrameWidget::frameList() const
        {
            return _p->frameList;
        }

        const djvSpeed & FrameWidget::speed() const
        {
            return _p->speed;
        }

        void FrameWidget::stepBy(int steps)
        {
            setFrame(_p->frame + steps);
        }

        QSize FrameWidget::sizeHint() const
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

        void FrameWidget::setFrameList(const djvFrameList & in)
        {
            if (in == _p->frameList)
                return;
            _p->frameList = in;
            setFrame(_p->frame);
            textUpdate();
            widgetUpdate();
        }

        void FrameWidget::setFrame(qint64 frame)
        {
            const int tmp = djvMath::min(
                frame,
                static_cast<qint64>(_p->frameList.count() - 1));
            if (tmp == _p->frame)
                return;
            //DJV_DEBUG("FrameWidget::setFrame");
            //DJV_DEBUG_PRINT("frame = " << tmp);
            _p->frame = tmp;
            textUpdate();
            widgetUpdate();
            update();
            Q_EMIT frameChanged(_p->frame);
        }

        void FrameWidget::setSpeed(const djvSpeed & in)
        {
            if (in == _p->speed)
                return;
            _p->speed = in;
            textUpdate();
            widgetUpdate();
        }

        QAbstractSpinBox::StepEnabled FrameWidget::stepEnabled() const
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

        void FrameWidget::editingFinishedCallback()
        {
            //DJV_DEBUG("FrameWidget::editingFinishedCallback");
            const QString text = lineEdit()->text();
            //DJV_DEBUG_PRINT("text = " << text);
            setFrame(djvSequenceUtil::findClosest(
                djvTime::stringToFrame(text, _p->speed), _p->frameList));
            lineEdit()->setText(_p->text);
        }

        void FrameWidget::timeUnitsCallback()
        {
            textUpdate();
            widgetUpdate();
            updateGeometry();
        }

        void FrameWidget::textUpdate()
        {
            qint64 frame = 0;
            if (_p->frame >= 0 &&
                _p->frame < static_cast<qint64>(_p->frameList.count()))
            {
                frame = _p->frameList[_p->frame];
            }
            _p->text = djvTime::frameToString(frame, _p->speed);
        }

        void FrameWidget::widgetUpdate()
        {
            //djvSignalBlocker signalBlocker(lineEdit());
            lineEdit()->setText(_p->text);
        }

        struct FrameSlider::Private
        {
            Private(UI::UIContext * context) :
                context(context)
            {}

            qint64 frame = 0;
            djvFrameList frameList;
            djvSpeed speed;
            bool inOutEnabled = false;
            qint64 inPoint = 0;
            qint64 outPoint = 0;
            djvFrameList cachedFrames;
            UI::UIContext * context;
        };

        FrameSlider::FrameSlider(UI::UIContext * context, QWidget * parent) :
            QWidget(parent),
            _p(new Private(context))
        {
            setAttribute(Qt::WA_OpaquePaintEvent);
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            connect(
                context->timePrefs(),
                SIGNAL(timeUnitsChanged(djvTime::UNITS)),
                SLOT(timeUnitsCallback()));
        }

        FrameSlider::~FrameSlider()
        {}

        qint64 FrameSlider::frame() const
        {
            return _p->frame;
        }

        const djvFrameList & FrameSlider::frameList() const
        {
            return _p->frameList;
        }

        const djvSpeed & FrameSlider::speed() const
        {
            return _p->speed;
        }

        bool FrameSlider::isInOutEnabled() const
        {
            return _p->inOutEnabled;
        }

        qint64 FrameSlider::inPoint() const
        {
            return _p->inPoint;
        }

        qint64 FrameSlider::outPoint() const
        {
            return _p->outPoint;
        }

        QSize FrameSlider::sizeHint() const
        {
            return QSize(200, fontMetrics().height() * 2 + 5 * 2);
        }

        void FrameSlider::setFrameList(const djvFrameList & in)
        {
            if (in == _p->frameList)
                return;
            _p->frameList = in;
            setFrame(_p->frame);
            update();
        }

        void FrameSlider::setFrame(qint64 frame)
        {
            const int tmp = djvMath::clamp(
                frame,
                static_cast<qint64>(0),
                end());
            if (tmp == _p->frame)
                return;
            //DJV_DEBUG("FrameSlider::setFrame");
            //DJV_DEBUG_PRINT("frame = " << tmp);
            _p->frame = tmp;
            update();
            Q_EMIT frameChanged(_p->frame);
        }

        void FrameSlider::setSpeed(const djvSpeed & in)
        {
            if (in == _p->speed)
                return;
            _p->speed = in;
            update();
        }

        void FrameSlider::setInOutEnabled(bool in)
        {
            if (in == _p->inOutEnabled)
                return;
            _p->inOutEnabled = in;
            update();
            Q_EMIT inOutEnabledChanged(_p->inOutEnabled);
        }

        void FrameSlider::setInOutPoints(qint64 inPoint, qint64 outPoint)
        {
            if (inPoint == _p->inPoint && outPoint == _p->outPoint)
                return;
            _p->inPoint = inPoint;
            _p->outPoint = outPoint;
            update();
            Q_EMIT inPointChanged(_p->inPoint);
            Q_EMIT outPointChanged(_p->outPoint);
        }

        void FrameSlider::setInPoint(qint64 frame)
        {
            setInOutPoints(frame, _p->outPoint);
        }

        void FrameSlider::setOutPoint(qint64 frame)
        {
            setInOutPoints(_p->inPoint, frame);
        }

        void FrameSlider::markInPoint()
        {
            setInPoint(_p->frame);
        }

        void FrameSlider::markOutPoint()
        {
            setOutPoint(_p->frame);
        }

        void FrameSlider::resetInOutPoints()
        {
            setInOutPoints(0, end());
        }

        void FrameSlider::resetInPoint()
        {
            setInPoint(0);
        }

        void FrameSlider::resetOutPoint()
        {
            setOutPoint(end());
        }

        void FrameSlider::gotoStartFrame()
        {
            setFrame(_p->inPoint);
        }

        void FrameSlider::gotoEndFrame()
        {
            setFrame(_p->outPoint);
        }

        void FrameSlider::setCachedFrames(const djvFrameList & in)
        {
            if (in == _p->cachedFrames)
                return;
            _p->cachedFrames = in;
            update();
        }

        void FrameSlider::mousePressEvent(QMouseEvent * event)
        {
            setFrame(posToFrame(event->pos().x()));
            Q_EMIT pressed(true);
        }

        void FrameSlider::mouseReleaseEvent(QMouseEvent * event)
        {
            Q_EMIT pressed(false);
        }

        void FrameSlider::mouseMoveEvent(QMouseEvent * event)
        {
            setFrame(posToFrame(event->pos().x()));
        }

        void FrameSlider::paintEvent(QPaintEvent * event)
        {
            //DJV_DEBUG("FrameSlider::paintEvent");

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
            const float  speed = djvSpeed::speedToFloat(_p->speed);
            const bool   drawFrames = frameToPosF(1) > 3;
            const bool   drawSeconds = frameToPosF(static_cast<int>(speed)) > 3;
            for (int i = 0; i < _p->frameList.count(); ++i)
            {
                Tick tick;
                tick.type = Tick::FRAME;
                bool drawTick = drawFrames;
                bool drawLabel = false;
                if (speed > 0.f && 0 == i % static_cast<int>(speed))
                {
                    drawTick = drawSeconds;
                    drawLabel = true;
                    tick.type = Tick::SECOND;
                }
                if (drawTick)
                {
                    tick.x = (frameToPosF(i) + frameToPosF(i + 1)) / 2.f;
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
            const int spacing = _p->context->style()->sizeMetric().spacing;
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
                if (!ticks[i].label.isEmpty())
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
            current.x = (frameToPosF(_p->frame) + frameToPosF(_p->frame + 1)) / 2.f;
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

        void FrameSlider::timeUnitsCallback()
        {
            updateGeometry();
            update();
        }

        qint64 FrameSlider::end() const
        {
            return _p->frameList.count() ? (_p->frameList.count() - 1) : 0;
        }

        qint64 FrameSlider::posToFrame(int pos) const
        {
            const float fw = width() / static_cast<float>(_p->frameList.count());
            const float v = pos / static_cast<float>(width() - fw);
            return static_cast<qint64>(v * end());
        }

        float FrameSlider::frameToPosF(qint64 frame) const
        {
            const float fw =
                _p->frameList.count() ?
                (width() / static_cast<float>(_p->frameList.count())) :
                0.f;
            const qint64 end = this->end();
            const float v =
                end ?
                (frame / static_cast<float>(end)) :
                0.f;
            return v * (width() - fw);
        }

        int FrameSlider::frameToPos(qint64 frame) const
        {
            const float fw = _p->frameList.count() ?
                width() / static_cast<float>(_p->frameList.count()) :
                0.f;
            const qint64 end = this->end();
            const float v =
                end ?
                frame / static_cast<float>(end) :
                0.f;
            return static_cast<int>(v * (width() - fw));
        }

        struct FrameDisplay::Private
        {
            qint64      frame = 0;
            djvSpeed    speed;
            bool        inOutEnabled = false;
            QString     text;
            QLineEdit * lineEdit = nullptr;
        };

        FrameDisplay::FrameDisplay(UI::UIContext * context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
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
                context->timePrefs(),
                SIGNAL(timeUnitsChanged(djvTime::UNITS)),
                SLOT(timeUnitsCallback()));
        }

        FrameDisplay::~FrameDisplay()
        {}

        qint64 FrameDisplay::frame() const
        {
            return _p->frame;
        }

        const djvSpeed & FrameDisplay::speed() const
        {
            return _p->speed;
        }

        bool FrameDisplay::isInOutEnabled() const
        {
            return _p->inOutEnabled;
        }

        QSize FrameDisplay::sizeHint() const
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

        void FrameDisplay::setFrame(qint64 in)
        {
            if (in == _p->frame)
                return;
            _p->frame = in;
            textUpdate();
            widgetUpdate();
        }

        void FrameDisplay::setSpeed(const djvSpeed & in)
        {
            if (in == _p->speed)
                return;
            _p->speed = in;
            textUpdate();
            widgetUpdate();
        }

        void FrameDisplay::setInOutEnabled(bool in)
        {
            if (in == _p->inOutEnabled)
                return;
            _p->inOutEnabled = in;
            widgetUpdate();
        }

        void FrameDisplay::timeUnitsCallback()
        {
            textUpdate();
            widgetUpdate();
            updateGeometry();
        }

        void FrameDisplay::textUpdate()
        {
            _p->text = djvTime::frameToString(_p->frame, _p->speed);
        }

        void FrameDisplay::widgetUpdate()
        {
            djvSignalBlocker signalBlocker(_p->lineEdit);
            _p->lineEdit->setText(_p->text);
            QPalette palette = this->palette();
            if (_p->inOutEnabled)
                palette.setColor(QPalette::Text, palette.color(QPalette::Highlight));
            _p->lineEdit->setPalette(palette);
        }

        struct SpeedButton::Private
        {
            djvSpeed speed;
            djvSpeed defaultSpeed;
            UI::ToolButton * button = nullptr;
        };

        SpeedButton::SpeedButton(UI::UIContext * context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->button = new UI::ToolButton;
            _p->button->setIcon(context->iconLibrary()->icon("djvSubMenuIcon.png"));
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

        SpeedButton::~SpeedButton()
        {}

        void SpeedButton::setDefaultSpeed(const djvSpeed & in)
        {
            _p->defaultSpeed = in;
        }

        void SpeedButton::pressedCallback()
        {
            QMenu menu;
            if (_p->defaultSpeed.isValid())
            {
                const QString text =
                    qApp->translate("djv::ViewLib::SpeedButton", "Default: %1").
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

        void SpeedButton::menuCallback()
        {
            QAction * action = qobject_cast<QAction *>(sender());
            const int index = action->data().toInt();
            Q_EMIT speedChanged(-1 == index ? _p->defaultSpeed : static_cast<djvSpeed::FPS>(index));
        }

        struct SpeedWidget::Private
        {
            djvSpeed speed;
            djvSpeed defaultSpeed;
            UI::FloatEdit * floatEdit = nullptr;
            SpeedButton * button = nullptr;
        };

        SpeedWidget::SpeedWidget(UI::UIContext * context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->floatEdit = new UI::FloatEdit;
            _p->floatEdit->setRange(1.f, 1000.f);
            _p->floatEdit->object()->setInc(1.f, 10.f);

            _p->button = new SpeedButton(context);

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
                SIGNAL(valueChanged(float)),
                SLOT(editCallback(float)));
            connect(
                _p->button,
                SIGNAL(speedChanged(const djvSpeed &)),
                SLOT(setSpeed(const djvSpeed &)));
        }

        SpeedWidget::~SpeedWidget()
        {}

        const djvSpeed & SpeedWidget::speed() const
        {
            return _p->speed;
        }

        const djvSpeed & SpeedWidget::defaultSpeed() const
        {
            return _p->defaultSpeed;
        }

        void SpeedWidget::setSpeed(const djvSpeed & in)
        {
            if (in == _p->speed)
                return;
            _p->speed = in;
            widgetUpdate();
            Q_EMIT speedChanged(_p->speed);
        }

        void SpeedWidget::setDefaultSpeed(const djvSpeed & in)
        {
            if (in == _p->defaultSpeed)
                return;
            _p->defaultSpeed = in;
            widgetUpdate();
        }

        void SpeedWidget::editCallback(float in)
        {
            const djvSpeed speed = djvSpeed::floatToSpeed(in);
            setSpeed(speed);
            widgetUpdate();
        }

        void SpeedWidget::widgetUpdate()
        {
            djvSignalBlocker signalBlocker(_p->floatEdit);
            _p->floatEdit->setValue(djvSpeed::speedToFloat(_p->speed));
            _p->button->setDefaultSpeed(_p->defaultSpeed);
        }

        struct SpeedDisplay::Private
        {
            float       speed = 0.f;
            bool        droppedFrames = false;
            QLineEdit * lineEdit = nullptr;
        };

        SpeedDisplay::SpeedDisplay(QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->lineEdit = new QLineEdit;
            _p->lineEdit->setReadOnly(true);

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->lineEdit);

            widgetUpdate();
        }

        SpeedDisplay::~SpeedDisplay()
        {}

        QSize SpeedDisplay::sizeHint() const
        {
            QString sizeString("000.00");
            return QSize(
                fontMetrics().width(sizeString) + 10,
                QWidget::sizeHint().height());
        }

        void SpeedDisplay::setSpeed(float speed)
        {
            if (djvMath::fuzzyCompare(speed, _p->speed))
                return;
            _p->speed = speed;
            widgetUpdate();
        }

        void SpeedDisplay::setDroppedFrames(bool in)
        {
            if (in == _p->droppedFrames)
                return;
            _p->droppedFrames = in;
            widgetUpdate();
        }

        void SpeedDisplay::widgetUpdate()
        {
            djvSignalBlocker signalBlocker(_p->lineEdit);
            QPalette palette = this->palette();
            if (_p->droppedFrames)
                palette.setColor(QPalette::Text, palette.color(QPalette::Highlight));
            _p->lineEdit->setPalette(palette);
            _p->lineEdit->setText(QString("%1").arg(_p->speed, 0, 'f', 2));
        }
    
    } // namespace ViewLib
} // namespace djv
