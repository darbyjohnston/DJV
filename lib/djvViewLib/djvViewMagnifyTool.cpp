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

//! \file djvViewMagnifyTool.cpp

#include <djvViewMagnifyTool.h>

#include <djvViewApplication.h>
#include <djvViewDisplayProfile.h>
#include <djvViewImageView.h>
#include <djvViewMainWindow.h>

#include <djvIconLibrary.h>
#include <djvIntEditSlider.h>
#include <djvPrefs.h>
#include <djvToolButton.h>

#include <djvDebug.h>
#include <djvOpenGlImage.h>
#include <djvOpenGlOffscreenBuffer.h>
#include <djvPixelDataUtil.h>
#include <djvSignalBlocker.h>

#include <QHBoxLayout>
#include <QScopedPointer>
#include <QTimer>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// Widget
//------------------------------------------------------------------------------

namespace
{

class Widget : public QWidget
{
public:

    Widget();

    void setPixmap(const QPixmap &);

protected:

    virtual void paintEvent(QPaintEvent *);

private:

    QPixmap _pixmap;
};

Widget::Widget()
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void Widget::setPixmap(const QPixmap & pixmap)
{
    _pixmap = pixmap;

    update();
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::black);

    if (! _pixmap.isNull())
    {
        painter.drawPixmap(0, 0, _pixmap);
    }
}

} // namespace

//------------------------------------------------------------------------------
// djvViewMagnifyTool::P
//------------------------------------------------------------------------------

struct djvViewMagnifyTool::P
{
    P() :
        zoom                (2),
        colorProfile        (true),
        displayProfile      (true),
        pixelDataInit       (false),
        widget              (0),
        slider              (0),
        colorProfileButton  (0),
        displayProfileButton(0)
    {}
    
    djvVector2i                              pick;
    int                                      zoom;
    bool                                     colorProfile;
    bool                                     displayProfile;

    QScopedPointer<djvOpenGlOffscreenBuffer> magnifyBuffer;
    djvOpenGlImageState                      magnifyState;
    QScopedPointer<djvOpenGlOffscreenBuffer> convertBuffer;
    djvOpenGlImageState                      convertState;
    bool                                     pixelDataInit;

    Widget *                                 widget;
    djvIntEditSlider *                       slider;
    djvToolButton *                          colorProfileButton;
    djvToolButton *                          displayProfileButton;
};

//------------------------------------------------------------------------------
// djvViewMagnifyTool
//------------------------------------------------------------------------------

djvViewMagnifyTool::djvViewMagnifyTool(
    djvViewMainWindow * mainWindow,
    QWidget *           parent) :
    djvViewAbstractTool(mainWindow, parent),
    _p(new P)
{
    // Create the widgets.

    _p->widget = new Widget;

    _p->slider = new djvIntEditSlider;
    _p->slider->setRange(1, 10);
    _p->slider->setResetToDefault(false);

    _p->colorProfileButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvDisplayProfileIcon.png"));
    _p->colorProfileButton->setCheckable(true);
    _p->colorProfileButton->setToolTip(
        tr("Set whether the color profile is enabled"));

    _p->displayProfileButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvDisplayProfileIcon.png"));
    _p->displayProfileButton->setCheckable(true);
    _p->displayProfileButton->setToolTip(
        tr("Set whether the display profile is enabled"));

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);

    layout->addWidget(_p->widget, 1);

    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->addWidget(_p->slider);
    
    QHBoxLayout * hLayout2 = new QHBoxLayout;
    hLayout2->setMargin(0);
    hLayout2->setSpacing(0);
    hLayout2->addWidget(_p->colorProfileButton);
    hLayout2->addWidget(_p->displayProfileButton);
    hLayout->addLayout(hLayout2);
    
    layout->addLayout(hLayout);

    // Preferences.

    djvPrefs prefs("djvViewMagnifyTool");
    prefs.get("zoom", _p->zoom);
    prefs.get("colorProfile", _p->colorProfile);
    prefs.get("displayProfile", _p->displayProfile);

    // Initialize.
    
    setWindowTitle(tr("Magnify"));

    widgetUpdate();

    // Setup the callbacks.

    connect(
        mainWindow,
        SIGNAL(imageChanged()),
        SLOT(widgetUpdate()));

    connect(
        mainWindow->viewWidget(),
        SIGNAL(pickChanged(const djvVector2i &)),
        SLOT(pickCallback(const djvVector2i &)));

    connect(
        _p->slider,
        SIGNAL(valueChanged(int)),
        SLOT(sliderCallback(int)));
    
    connect(
        _p->colorProfileButton,
        SIGNAL(toggled(bool)),
        SLOT(colorProfileCallback(bool)));
    
    connect(
        _p->displayProfileButton,
        SIGNAL(toggled(bool)),
        SLOT(displayProfileCallback(bool)));
}

djvViewMagnifyTool::~djvViewMagnifyTool()
{
    //DJV_DEBUG("djvViewMagnifyTool::~djvViewMagnifyTool");

    djvPrefs prefs("djvViewMagnifyTool");
    prefs.set("zoom", _p->zoom);
    prefs.set("colorProfile", _p->colorProfile);
    prefs.set("displayProfile", _p->displayProfile);
    
    viewWidget()->makeCurrent();
    
    delete _p;
}

void djvViewMagnifyTool::pickCallback(const djvVector2i & in)
{
    _p->pick = in;

    widgetUpdate();
}

void djvViewMagnifyTool::sliderCallback(int in)
{
    _p->zoom = in - 1;

    widgetUpdate();
}

void djvViewMagnifyTool::colorProfileCallback(bool in)
{
    _p->colorProfile = in;

    widgetUpdate();
}

void djvViewMagnifyTool::displayProfileCallback(bool in)
{
    _p->displayProfile = in;

    widgetUpdate();
}

void djvViewMagnifyTool::showEvent(QShowEvent *)
{
    widgetUpdate();
}

void djvViewMagnifyTool::widgetUpdate()
{
    //DJV_DEBUG("djvViewMagnifyTool::widgetUpdate");

    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->slider <<
        _p->colorProfileButton <<
        _p->displayProfileButton);

    _p->slider->setValue(_p->zoom + 1);
    _p->colorProfileButton->setChecked(_p->colorProfile);
    _p->displayProfileButton->setChecked(_p->displayProfile);

    if (! _p->pixelDataInit && isVisible())
    {
        _p->pixelDataInit = true;

        QTimer::singleShot(0, this, SLOT(pixelDataUpdate()));
    }
}

void djvViewMagnifyTool::pixelDataUpdate()
{
    //DJV_DEBUG("djvViewMagnifyTool::pixelDataUpdate");

    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->widget);

    djvPixelData tmp(djvPixelDataInfo(
        _p->widget->width(),
        _p->widget->height(),
        djvPixel::RGB_U8));

    if (const djvPixelData * data = viewWidget()->data())
    {
        //DJV_DEBUG_PRINT("data = " << *data);

        const double zoom = djvMath::pow(2, _p->zoom);

        djvVector2i pick = djvVectorUtil::floor<double, int>(
            djvVector2f(_p->pick - viewWidget()->viewPos()) * zoom -
            djvVector2f(tmp.info().size) / 2.0);

        //DJV_DEBUG_PRINT("zoom = " << zoom);
        //DJV_DEBUG_PRINT("pick = " << pick);

        try
        {
            viewWidget()->makeCurrent();

            if (! _p->magnifyBuffer || _p->magnifyBuffer->info() != tmp.info())
            {
                _p->magnifyBuffer.reset(new djvOpenGlOffscreenBuffer(tmp.info()));
            }

            djvOpenGlImageOptions options = viewWidget()->options();
            options.xform.position -= pick;
            options.xform.scale *= zoom * viewWidget()->viewZoom();

            if (! _p->colorProfile)
            {
                options.colorProfile = djvColorProfile();
            }

            if (! _p->displayProfile)
            {
                options.displayProfile = djvViewDisplayProfile();
            }

            djvOpenGlImage::copy(
                *data,
                tmp,
                options,
                &_p->magnifyState,
                _p->magnifyBuffer.data());
            
            _p->widget->setPixmap(
                djvPixelDataUtil::toQt(
                    tmp,
                    djvOpenGlImageOptions(),
                    &_p->convertState,
                    _p->convertBuffer.data()));
        }
        catch (djvError error)
        {
            error.add(
                djvViewUtil::errorLabels()[djvViewUtil::ERROR_MAGNIFY]);

            DJV_VIEW_APP->printError(error);
        }
    }

    //_p->widget->setPixelData(tmp);

    _p->pixelDataInit = false;
}

