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

//! \file djvViewMiscWidget.h

#ifndef DJV_VIEW_MISC_WIDGET_H
#define DJV_VIEW_MISC_WIDGET_H

#include <djvViewLibExport.h>

#include <djvSequence.h>
#include <djvSpeed.h>
#include <djvUtil.h>

#include <QAbstractSpinBox>

struct djvViewCacheSizeWidgetPrivate;
struct djvViewFrameDisplayPrivate;
struct djvViewFrameSliderPrivate;
struct djvViewFrameWidgetPrivate;
struct djvViewSpeedButtonPrivate;
struct djvViewSpeedDisplayPrivate;
struct djvViewSpeedWidgetPrivate;

class djvGuiContext;

class QAbstractButton;

//! \addtogroup djvViewMisc
//@{

//------------------------------------------------------------------------------
//! \class djvViewCacheSizeWidget
//!
//! This class provides a cache size widget.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewCacheSizeWidget : public QWidget
{
    Q_OBJECT

    //! This property holds the cache sizes.
    
    Q_PROPERTY(
        QVector<double> cacheSizes
        READ            cacheSizes
        WRITE           setCacheSizes
        NOTIFY          cacheSizesChanged)

    //! This property holds the cache size.
    
    Q_PROPERTY(
        double cacheSize
        READ   cacheSize
        WRITE  setCacheSize
        NOTIFY cacheSizeChanged)

public:

    //! Constructor.

    explicit djvViewCacheSizeWidget(djvGuiContext *, QWidget * parent = 0);

    //! Destructor.

    virtual ~djvViewCacheSizeWidget();

    //! Get the cache sizes.

    const QVector<double> & cacheSizes() const;

    //! Get the current cache size.

    double cacheSize() const;

public Q_SLOTS:

    //! Set the cache sizes.

    void setCacheSizes(const QVector<double> &);

    //! Set the current cache size.

    void setCacheSize(double);

Q_SIGNALS:

    //! This signal is emitted when the cache sizes are changed.

    void cacheSizesChanged(const QVector<double> &);

    //! This signal is emitted when the current cache size is changed.

    void cacheSizeChanged(double);

private Q_SLOTS:

    void buttonCallback();
    void menuCallback();

private Q_SLOTS:

    void widgetUpdate();

private:

    DJV_PRIVATE_COPY(djvViewCacheSizeWidget);
    
    djvViewCacheSizeWidgetPrivate * _p;
};

//------------------------------------------------------------------------------
//! \class djvViewFrameWidget
//!
//! This class provides a frame number widget.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewFrameWidget : public QAbstractSpinBox
{
    Q_OBJECT

    //! This property holds the frame list.
    
    Q_PROPERTY(
        djvFrameList frameList
        READ         frameList
        WRITE        setFrameList)

    //! This property holds the frame.
    
    Q_PROPERTY(
        qint64 frame
        READ   frame
        WRITE  setFrame
        NOTIFY frameChanged)

    //! This property holds the speed.
    
    Q_PROPERTY(
        djvSpeed speed
        READ     speed
        WRITE    setSpeed)

public:

    //! Constructor.

    explicit djvViewFrameWidget(djvGuiContext *, QWidget * parent = 0);

    //! Destructor.

    virtual ~djvViewFrameWidget();

    //! Get the frame list.

    const djvFrameList & frameList() const;

    //! Get the frame.

    qint64 frame() const;

    //! Get the speed.

    const djvSpeed & speed() const;

    virtual void stepBy(int);

    virtual QSize sizeHint() const;

public Q_SLOTS:

    //! Set the frame list.

    void setFrameList(const djvFrameList &);

    //! Set the frame.

    void setFrame(qint64);

    //! Set the speed.

    void setSpeed(const djvSpeed &);

Q_SIGNALS:

    //! This signal is emitted when the frame is changed.

    void frameChanged(qint64);

protected:

    virtual QAbstractSpinBox::StepEnabled stepEnabled() const;

    private Q_SLOTS:

    void editingFinishedCallback();
    void timeUnitsCallback();

    void textUpdate();
    void widgetUpdate();

private:

    DJV_PRIVATE_COPY(djvViewFrameWidget);
    
    djvViewFrameWidgetPrivate * _p;
};

//------------------------------------------------------------------------------
//! \class djvViewFrameSlider
//!
//! This class provides a frame number slider.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewFrameSlider : public QWidget
{
    Q_OBJECT

    //! This property holds the frame list.
    
    Q_PROPERTY(
        djvFrameList frameList
        READ         frameList
        WRITE        setFrameList)

    //! This property holds the current frame.
    
    Q_PROPERTY(
        qint64 frame
        READ   frame
        WRITE  setFrame
        NOTIFY frameChanged)

    //! This property holds the speed.
    
    Q_PROPERTY(
        djvSpeed speed
        READ     speed
        WRITE    setSpeed)

    //! This property holds whether in/out points are enabled.
    
    Q_PROPERTY(
        bool   inOutEnabled
        READ   isInOutEnabled
        WRITE  setInOutEnabled
        NOTIFY inOutEnabledChanged)
    
    //! This property holds the in point.
    
    Q_PROPERTY(
        qint64 inPoint
        READ   inPoint
        WRITE  setInPoint
        NOTIFY inPointChanged)
    
    //! This property holds the out point.
    
    Q_PROPERTY(
        qint64 outPoint
        READ   outPoint
        WRITE  setOutPoint
        NOTIFY outPointChanged)
    
public:

    //! Constructor.

    explicit djvViewFrameSlider(djvGuiContext *, QWidget * parent = 0);

    //! Destructor.

    virtual ~djvViewFrameSlider();

    //! Get the frame list.

    const djvFrameList & frameList() const;

    //! Get the current frame.

    qint64 frame() const;

    //! Get the speed.

    const djvSpeed & speed() const;

    //! Get whether the in/out points are enabled.

    bool isInOutEnabled() const;

    //! Get the in point.

    qint64 inPoint() const;

    //! Get the out point.

    qint64 outPoint() const;
    
    virtual QSize sizeHint() const;

public Q_SLOTS:

    //! Set the frame list.

    void setFrameList(const djvFrameList &);

    //! Set the current frame.

    void setFrame(qint64);

    //! Set the speed.

    void setSpeed(const djvSpeed &);

    //! Set whether the in/out points are enabled.

    void setInOutEnabled(bool);

    //! Set the in/out points.

    void setInOutPoints(qint64, qint64);

    //! Set the in point.

    void setInPoint(qint64);

    //! Set the out point.

    void setOutPoint(qint64);

    //! Mark the in point. This sets the in point to the current frame.

    void markInPoint();

    //! Mark the out point. This sets the out point to the current frame.

    void markOutPoint();

    //! Reset the in/out points.

    void resetInOutPoints();

    //! Reset the in point.

    void resetInPoint();

    //! Reset the out point.

    void resetOutPoint();

    //! Go to the start frame.

    void gotoStartFrame();

    //! Go to the end frame.

    void gotoEndFrame();

    //! Set the list of cached frames.

    void setCachedFrames(const djvFrameList &);

Q_SIGNALS:

    //! This signal is emitted when the frame is changed.

    void frameChanged(qint64);

    //! This signal is emitted when the in/out points are enabled or disabled.

    void inOutEnabledChanged(bool);

    //! This signal is emitted when the in point is changed.

    void inPointChanged(qint64);

    //! This signal is emitted when the out point is changed.

    void outPointChanged(qint64);

    //! This signal is emitted when the slider is pressed.

    void pressed(bool);

protected:

    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);

private Q_SLOTS:

    void timeUnitsCallback();
    
private:

    qint64 end() const;

    qint64 posToFrame(int) const;
    double frameToPosF(qint64) const;
    int frameToPos(qint64) const;

    DJV_PRIVATE_COPY(djvViewFrameSlider);
    
    djvViewFrameSliderPrivate * _p;
};

//------------------------------------------------------------------------------
//! \class djvViewFrameDisplay
//!
//! This class provides frame number display.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewFrameDisplay : public QWidget
{
    Q_OBJECT

    //! This property holds the frame.
    
    Q_PROPERTY(
        qint64 frame
        READ   frame
        WRITE  setFrame)

    //! This property holds the speed.
    
    Q_PROPERTY(
        djvSpeed speed
        READ     speed
        WRITE    setSpeed)

    //! This property holds whether in/out points are enabled.
    
    Q_PROPERTY(
        bool   inOutEnabled
        READ   isInOutEnabled
        WRITE  setInOutEnabled)
    
public:

    //! Constructor.

    explicit djvViewFrameDisplay(djvGuiContext *, QWidget * parent = 0);

    //! Destructor.

    virtual ~djvViewFrameDisplay();

    //! Get the frame.

    qint64 frame() const;

    //! Get the speed.

    const djvSpeed & speed() const;

    //! Get whether the in/out points are enabled.

    bool isInOutEnabled() const;

    virtual QSize sizeHint() const;

public Q_SLOTS:

    //! Set the frame.

    void setFrame(qint64);

    //! Set the speed.

    void setSpeed(const djvSpeed &);

    //! Set whether in/out points are enabled.

    void setInOutEnabled(bool);

private Q_SLOTS:

    void timeUnitsCallback();
    
    void textUpdate();
    void widgetUpdate();

private:

    DJV_PRIVATE_COPY(djvViewFrameDisplay);
    
    djvViewFrameDisplayPrivate * _p;
};

//------------------------------------------------------------------------------
//! \class djvViewSpeedButton
//!
//! This class provides a speed button.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewSpeedButton : public QWidget
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvViewSpeedButton(djvGuiContext *, QWidget * parent = 0);

    //! Destructor.

    virtual ~djvViewSpeedButton();

    //! Set the default speed.

    void setDefaultSpeed(const djvSpeed &);

Q_SIGNALS:

    //! This signal is emitted when the speed is changed.

    void speedChanged(const djvSpeed &);

private Q_SLOTS:

    void pressedCallback();
    void menuCallback();

private:

    DJV_PRIVATE_COPY(djvViewSpeedButton);
    
    djvViewSpeedButtonPrivate * _p;
};

//------------------------------------------------------------------------------
//! \class djvViewSpeedWidget
//!
//! This class provides a speed widget.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewSpeedWidget : public QWidget
{
    Q_OBJECT

    //! This property holds the speed.
    
    Q_PROPERTY(
        djvSpeed speed
        READ     speed
        WRITE    setSpeed
        NOTIFY   speedChanged)

    //! This property holds the default speed.
    
    Q_PROPERTY(
        djvSpeed defaultSpeed
        READ     defaultSpeed
        WRITE    setDefaultSpeed)
    
public:

    //! Constructor.

    explicit djvViewSpeedWidget(djvGuiContext *, QWidget * parent = 0);

    //! Destructor.

    virtual ~djvViewSpeedWidget();

    //! Get the speed.

    const djvSpeed & speed() const;

    //! Get the default speed.

    const djvSpeed & defaultSpeed() const;

public Q_SLOTS:

    //! Set the speed.

    void setSpeed(const djvSpeed &);

    //! Set the default speed.

    void setDefaultSpeed(const djvSpeed &);

Q_SIGNALS:

    //! This signal is emitted when the speed is changed.

    void speedChanged(const djvSpeed &);

private Q_SLOTS:

    void editCallback(double);

    void widgetUpdate();

private:

    DJV_PRIVATE_COPY(djvViewSpeedWidget);
    
    djvViewSpeedWidgetPrivate * _p;
};

//------------------------------------------------------------------------------
//! \class djvViewSpeedDisplay
//!
//! This class provides a speed display.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewSpeedDisplay : public QWidget
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvViewSpeedDisplay(QWidget * parent = 0);

    //! Destructor.

    virtual ~djvViewSpeedDisplay();

    virtual QSize sizeHint() const;

public Q_SLOTS:

    //! Set the speed.

    void setSpeed(double);

    //! Set whether frames were dropped.

    void setDroppedFrames(bool);

private Q_SLOTS:

    void widgetUpdate();

private:

    DJV_PRIVATE_COPY(djvViewSpeedDisplay);
    
    djvViewSpeedDisplayPrivate * _p;
};

//@} // djvViewMisc

#endif // DJV_VIEW_MISC_WIDGET_H

