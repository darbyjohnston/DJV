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

//! \file djvViewColorPickerTool.cpp

#include <djvViewColorPickerTool.h>

#include <djvViewApplication.h>
#include <djvViewDisplayProfile.h>
#include <djvViewImageView.h>
#include <djvViewMainWindow.h>

#include <djvColorSwatch.h>
#include <djvColorWidget.h>
#include <djvIconLibrary.h>
#include <djvIntEditSlider.h>
#include <djvPrefs.h>
#include <djvToolButton.h>

#include <djvDebug.h>
#include <djvError.h>
#include <djvOpenGlImage.h>
#include <djvOpenGlOffscreenBuffer.h>
#include <djvSignalBlocker.h>
#include <djvStyle.h>

#include <QHBoxLayout>
#include <QScopedPointer>
#include <QTimer>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvViewColorPickerTool::P
//------------------------------------------------------------------------------

struct djvViewColorPickerTool::P
{
    P() :
        value               (djvPixel::RGBA_F32),
        size                (3),
        colorProfile        (true),
        displayProfile      (true),
        lock                (false),
        swatchInit          (false),
        widget              (0),
        swatch              (0),
        sizeSlider          (0),
        colorProfileButton  (0),
        displayProfileButton(0),
        lockWidget          (0)
    {}
    
    djvVector2i                              pick;
    djvColor                                 value;
    int                                      size;
    bool                                     colorProfile;
    bool                                     displayProfile;
    bool                                     lock;

    QScopedPointer<djvOpenGlOffscreenBuffer> buffer;
    djvOpenGlImageState                      state;
    bool                                     swatchInit;

    djvColorWidget *                         widget;
    djvColorSwatch *                         swatch;
    djvIntEditSlider *                       sizeSlider;
    djvToolButton *                          colorProfileButton;
    djvToolButton *                          displayProfileButton;
    djvToolButton *                          lockWidget;
};

//------------------------------------------------------------------------------
// djvViewColorPickerTool
//------------------------------------------------------------------------------

djvViewColorPickerTool::djvViewColorPickerTool(
    djvViewMainWindow * mainWindow,
    QWidget *           parent) :
    djvViewAbstractTool(mainWindow, parent),
    _p(new P)
{
    //DJV_DEBUG(gdjvViewColorPickerTool::djvViewColorPickerTool");

    // Create the widgets.

    _p->widget = new djvColorWidget;

    _p->swatch = new djvColorSwatch;
    _p->swatch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _p->sizeSlider = new djvIntEditSlider;
    _p->sizeSlider->setRange(1, 100);
    _p->sizeSlider->setResetToDefault(false);
    _p->sizeSlider->setToolTip(tr("Color picker sample size"));

    _p->colorProfileButton = new djvToolButton(
        djvIconLibrary::global()->pixmap("djvDisplayProfileIcon.png"));
    _p->colorProfileButton->setCheckable(true);
    _p->colorProfileButton->setToolTip(tr("Set whether the color profile is enabled"));

    _p->displayProfileButton = new djvToolButton(
        djvIconLibrary::global()->pixmap("djvDisplayProfileIcon.png"));
    _p->displayProfileButton->setCheckable(true);
    _p->displayProfileButton->setToolTip(tr("Set whether the display profile is enabled"));

    _p->lockWidget = new djvToolButton(
        djvIconLibrary::global()->icon("djvUnlockIcon.png", "djvLockIcon.png"));
    _p->lockWidget->setCheckable(true);
    _p->lockWidget->setToolTip(tr("Lock the pixel format and type"));

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);

    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->setSpacing(djvStyle::global()->sizeMetric().spacing);
    hLayout->addWidget(_p->swatch);
    hLayout->addWidget(_p->widget, 1);
    layout->addLayout(hLayout);

    hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->addWidget(_p->sizeSlider);
    
    QHBoxLayout * hLayout2 = new QHBoxLayout;
    hLayout2->setMargin(0);
    hLayout2->setSpacing(0);
    hLayout2->addWidget(_p->colorProfileButton);
    hLayout2->addWidget(_p->displayProfileButton);
    hLayout->addLayout(hLayout2);
    
    layout->addLayout(hLayout);

    _p->widget->bottomLayout()->insertWidget(1, _p->lockWidget);

    // Preferences.

    djvPrefs prefs("djvViewColorPickerTool");
    prefs.get("size", _p->size);
    djvPixel::PIXEL pixel = _p->value.pixel();
    prefs.get("pixel", pixel);
    _p->value.setPixel(pixel);
    prefs.get("colorProfile", _p->colorProfile);
    prefs.get("displayProfile", _p->displayProfile);
    prefs.get("lock", _p->lock);

    // Initialize.
    
    setWindowTitle(tr("Color Picker"));

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

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
        _p->widget,
        SIGNAL(colorChanged(const djvColor &)),
        SLOT(widgetCallback(const djvColor &)));
    
    connect(
        _p->sizeSlider,
        SIGNAL(valueChanged(int)),
        SLOT(sizeCallback(int)));
    
    connect(
        _p->colorProfileButton,
        SIGNAL(toggled(bool)),
        SLOT(colorProfileCallback(bool)));
    
    connect(
        _p->displayProfileButton,
        SIGNAL(toggled(bool)),
        SLOT(displayProfileCallback(bool)));
    
    connect(
        _p->lockWidget,
        SIGNAL(toggled(bool)),
        SLOT(lockCallback(bool)));
}

djvViewColorPickerTool::~djvViewColorPickerTool()
{
    //DJV_DEBUG("djvViewColorPickerTool::~djvViewColorPickerTool");

    djvPrefs prefs("djvViewColorPickerTool");
    prefs.set("size", _p->size);
    prefs.set("pixel", _p->value.pixel());
    prefs.set("colorProfile", _p->colorProfile);
    prefs.set("displayProfile", _p->displayProfile);
    prefs.set("lock", _p->lock);
    
    viewWidget()->makeCurrent();

    delete _p;
}

void djvViewColorPickerTool::showEvent(QShowEvent *)
{
    widgetUpdate();
}

void djvViewColorPickerTool::pickCallback(const djvVector2i & in)
{
    _p->pick = in;

    widgetUpdate();
}

void djvViewColorPickerTool::widgetCallback(const djvColor & color)
{
    _p->value = color;

    _p->swatch->setColor(color);
}

void djvViewColorPickerTool::sizeCallback(int value)
{
    _p->size = value;

    widgetUpdate();
}

void djvViewColorPickerTool::colorProfileCallback(bool in)
{
    _p->colorProfile = in;

    widgetUpdate();
}

void djvViewColorPickerTool::displayProfileCallback(bool in)
{
    _p->displayProfile = in;

    widgetUpdate();
}

void djvViewColorPickerTool::lockCallback(bool in)
{
    _p->lock = in;

    widgetUpdate();
}

void djvViewColorPickerTool::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->sizeSlider <<
        _p->colorProfileButton <<
        _p->displayProfileButton <<
        _p->lockWidget);

    _p->sizeSlider->setValue(_p->size);
    _p->colorProfileButton->setChecked(_p->colorProfile);
    _p->displayProfileButton->setChecked(_p->displayProfile);
    _p->lockWidget->setChecked(_p->lock);

    if (! _p->swatchInit && isVisible())
    {
        _p->swatchInit = true;

        QTimer::singleShot(0, this, SLOT(swatchUpdate()));
    }
}

void djvViewColorPickerTool::swatchUpdate()
{
    //DJV_DEBUG("djvViewColorPickerTool::swatchUpdate");
    //DJV_DEBUG_PRINT("color profile = " << _p->colorProfile);
    //DJV_DEBUG_PRINT("display profile = " << _p->displayProfile);
    //DJV_DEBUG_PRINT("lock = " << _p->lock);

    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->widget <<
        _p->swatch);

    if (const djvPixelData * data = viewWidget()->data())
    {
        //DJV_DEBUG_PRINT("data = " << *data);

        if (! _p->lock && data)
        {
            _p->value.setPixel(data->pixel());
        }
        else
        {
            _p->value.setPixel(_p->widget->color().pixel());
        }

        // Pick.

        const djvVector2i pick = djvVectorUtil::floor<double, int>(
            djvVector2f(_p->pick - viewWidget()->viewPos()) / viewWidget()->viewZoom());

        //DJV_DEBUG_PRINT("pick = " << _p->pick);

        // Render color sample.

        //DJV_DEBUG_PRINT("pick size = " << _p->size);

        djvPixelData tmp(djvPixelDataInfo(djvVector2i(_p->size), _p->value.pixel()));

        //DJV_DEBUG_PRINT("tmp = " << tmp);

        try
        {
            viewWidget()->makeCurrent();

            if (! _p->buffer || _p->buffer->info() != tmp.info())
            {
                _p->buffer.reset(new djvOpenGlOffscreenBuffer(tmp.info()));
            }

            djvOpenGlImageOptions options = viewWidget()->options();
            options.xform.position -= pick - djvVector2i((_p->size - 1) / 2);

            if (! _p->colorProfile)
            {
                options.colorProfile = djvColorProfile();
            }

            if (! _p->displayProfile)
            {
                options.displayProfile = djvViewDisplayProfile();
            }

            //DJV_DEBUG_PRINT("color profile = " << options.colorProfile);

            djvPixelData empty;

            if (! data)
            {
                data = &empty;
            }

            djvOpenGlImage::copy(
                *data,
                tmp,
                options,
                &_p->state,
                _p->buffer.data());

            djvOpenGlImage::average(tmp, _p->value);
        }
        catch (djvError error)
        {
            error.add(
                djvViewUtil::errorLabels()[djvViewUtil::ERROR_PICK_COLOR]);

            DJV_VIEW_APP->printError(error);
        }

        //DJV_DEBUG_PRINT("value = " << _p->value);
    }

    _p->widget->setColor(_p->value);
    _p->swatch->setColor(_p->value);

    _p->swatchInit = false;
}

