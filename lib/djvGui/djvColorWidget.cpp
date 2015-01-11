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

//! \file djvColorWidget.cpp

#include <djvColorWidget.h>

#include <djvFloatEditSlider.h>
#include <djvIntEditSlider.h>

#include <djvColorUtil.h>
#include <djvFloatObject.h>
#include <djvListUtil.h>
#include <djvSignalBlocker.h>

#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvColorWidget::P
//------------------------------------------------------------------------------

struct djvColorWidget::P
{
    P() :
        color       (djvPixel::RGB_U8),
        formatWidget(0),
        typeWidget  (0),
        bottomLayout(0)
    {}
    
    djvColor                      color;
    QVector<djvIntEditSlider *>   intWidgets;
    QVector<djvFloatEditSlider *> floatWidgets;
    QComboBox *                   formatWidget;
    QComboBox *                   typeWidget;
    QHBoxLayout *                 bottomLayout;
};

//------------------------------------------------------------------------------
// djvColorWidget
//------------------------------------------------------------------------------

djvColorWidget::djvColorWidget(QWidget * parent) :
    QWidget(parent),
    _p(new P)
{
    //DJV_DEBUG("djvColorWidget::djvColorWidget");

    // Create the widgets.

    for (int i = 0; i < djvPixel::channelsMax; ++i)
    {
        djvIntEditSlider * intSlider = new djvIntEditSlider;
        intSlider->setResetToDefault(false);
        _p->intWidgets += intSlider;
        
        djvFloatEditSlider * floatSlider = new djvFloatEditSlider;
        floatSlider->setResetToDefault(false);
        floatSlider->editObject()->setClamp(false);
        _p->floatWidgets += floatSlider;
    }

    _p->formatWidget = new QComboBox;
    _p->formatWidget->addItems(djvPixel::formatLabels());
    _p->formatWidget->setToolTip(tr("Pixel format"));

    _p->typeWidget = new QComboBox;
    _p->typeWidget->addItems(djvPixel::typeLabels());
    _p->typeWidget->setToolTip(tr("Pixel type"));

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setMargin(0);

    for (int i = 0; i < djvPixel::channelsMax; ++i)
    {
        layout->addWidget(_p->intWidgets[i]);
        layout->addWidget(_p->floatWidgets[i]);
    }

    _p->bottomLayout = new QHBoxLayout;
    _p->bottomLayout->setMargin(0);
    _p->bottomLayout->setSpacing(0);
    _p->bottomLayout->addStretch();
    _p->bottomLayout->addWidget(_p->formatWidget);
    _p->bottomLayout->addWidget(_p->typeWidget);
    layout->addLayout(_p->bottomLayout);

    // Initialize.

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    widgetUpdate();
    valueUpdate();

    // Setup the callbacks.

    for (int i = 0; i < djvPixel::channelsMax; ++i)
    {
        connect(
            _p->intWidgets[i],
            SIGNAL(valueChanged(int)),
            SLOT(intCallback(int)));
        
        connect(
            _p->floatWidgets[i],
            SIGNAL(valueChanged(double)),
            SLOT(floatCallback(double)));
    }

    connect(
        _p->formatWidget,
        SIGNAL(activated(int)),
        SLOT(formatCallback(int)));
    
    connect(
        _p->typeWidget,
        SIGNAL(activated(int)),
        SLOT(typeCallback(int)));
}

djvColorWidget::~djvColorWidget()
{
    delete _p;
}

const djvColor & djvColorWidget::color() const
{
    return _p->color;
}

void djvColorWidget::setColor(const djvColor & color)
{
    if (color == _p->color)
        return;

    //DJV_DEBUG("djvColorWidget::setColor");
    //DJV_DEBUG_PRINT("color = " << color);

    const djvColor prev = _p->color;
    
    _p->color = color;

    if (_p->color.pixel() != prev.pixel())
    {
        widgetUpdate();
    }

    valueUpdate();

    Q_EMIT colorChanged(_p->color);
}

QHBoxLayout * djvColorWidget::bottomLayout()
{
    return _p->bottomLayout;
}

void djvColorWidget::intCallback(int)
{
    for (int i = 0; i < djvPixel::channels(_p->color.pixel()); ++i)
    {
        switch (djvPixel::type(_p->color.pixel()))
        {
            case djvPixel::U8:
                _p->color.setU8 (_p->intWidgets[i]->value(), i);
                break;

            case djvPixel::U10:
                _p->color.setU10(_p->intWidgets[i]->value(), i);
                break;

            case djvPixel::U16:
                _p->color.setU16(_p->intWidgets[i]->value(), i);
                break;

            case djvPixel::F16:
                _p->color.setF16(
                    static_cast<djvPixel::F16_T>(_p->floatWidgets[i]->value()),
                    i);
                break;

            case djvPixel::F32:
                _p->color.setF32(
                    static_cast<djvPixel::F32_T>(_p->floatWidgets[i]->value()),
                    i);
                break;

            default: break;
        }
    }

    valueUpdate();

    Q_EMIT colorChanged(_p->color);
}

void djvColorWidget::floatCallback(double)
{
    intCallback(0);
}

void djvColorWidget::formatCallback(int in)
{
    djvColor tmp(djvPixel::pixel(
        static_cast<djvPixel::FORMAT>(in),
        djvPixel::type(_p->color.pixel())));

    djvColorUtil::convert(_p->color, tmp);

    setColor(tmp);

    widgetUpdate();
}

void djvColorWidget::typeCallback(int in)
{
    djvColor tmp(djvPixel::pixel(
        djvPixel::format(_p->color.pixel()),
        static_cast<djvPixel::TYPE>(in)));

    djvColorUtil::convert(_p->color, tmp);

    setColor(tmp);

    widgetUpdate();
}

void djvColorWidget::widgetUpdate()
{
    //DJV_DEBUG("djvColorWidget::widgetUpdate");
    //DJV_DEBUG_PRINT("color = " << _p->color);
    
    djvSignalBlocker signalBlocker(QVector<QObject *>() <<
        djvListUtil::convert<djvIntEditSlider *, QObject *>(_p->intWidgets) <<
        djvListUtil::convert<djvFloatEditSlider *, QObject *>(
            _p->floatWidgets) <<
        _p->formatWidget <<
        _p->typeWidget);

    const int channels = djvPixel::channels(_p->color.pixel());

    static const QString toolTip0[] =
    {
        tr("Luminance channel"),
        tr("Alpha channel")
    };

    static const QString toolTip1[] =
    {
        tr("Red channel"),
        tr("Green channel"),
        tr("Blue channel"),
        tr("Alpha channel")
    };

    const QString * toolTip = 0;

    switch (djvPixel::format(_p->color.pixel()))
    {
        case djvPixel::L:
        case djvPixel::LA:
        
            toolTip = toolTip0;
            
            break;

        case djvPixel::RGB:
        case djvPixel::RGBA:
        
            toolTip = toolTip1;
            
            break;

        default: break;
    }

    for (int i = 0; i < channels; ++i)
    {
        _p->intWidgets[i]->setRange(0, djvPixel::max(_p->color.pixel()));
        _p->intWidgets[i]->setToolTip(toolTip[i]);
        
        _p->floatWidgets[i]->setToolTip(toolTip[i]);
    }

    for (int i = 0; i < djvPixel::channelsMax; ++i)
    {
        switch (djvPixel::type(_p->color.pixel()))
        {
            case djvPixel::U8:
            case djvPixel::U10:
            case djvPixel::U16:
            
                if (i < channels)
                {
                    _p->intWidgets[i]->show();
                }
                else
                {
                    _p->intWidgets[i]->hide();
                }

                _p->floatWidgets[i]->hide();
                
                break;

            case djvPixel::F16:
            case djvPixel::F32:
            
                _p->intWidgets[i]->hide();

                if (i < channels)
                {
                    _p->floatWidgets[i]->show();
                }
                else
                {
                    _p->floatWidgets[i]->hide();
                }

                break;

            default: break;
        }
    }

    _p->formatWidget->setCurrentIndex(djvPixel::format(_p->color.pixel()));
    _p->typeWidget->setCurrentIndex(djvPixel::type(_p->color.pixel()));

    update();
}

void djvColorWidget::valueUpdate()
{
    //DJV_DEBUG("djvColorWidget::valueUpdate");

    djvSignalBlocker signalBlocker(QVector<QObject *>() <<
        djvListUtil::convert<djvIntEditSlider *, QObject *>(_p->intWidgets) <<
        djvListUtil::convert<djvFloatEditSlider *, QObject *>(
            _p->floatWidgets));

    for (int i = 0; i < djvPixel::channels(_p->color.pixel()); ++i)
    {
        switch (djvPixel::type(_p->color.pixel()))
        {
            case djvPixel::U8:
                _p->intWidgets[i]->setValue(_p->color.u8 (i));
                break;

            case djvPixel::U10:
                _p->intWidgets[i]->setValue(_p->color.u10(i));
                break;

            case djvPixel::U16:
                _p->intWidgets[i]->setValue(_p->color.u16(i));
                break;

            case djvPixel::F16:
                _p->floatWidgets[i]->setValue(_p->color.f16(i));
                break;

            case djvPixel::F32:
                _p->floatWidgets[i]->setValue(_p->color.f32(i));
                break;

            default: break;
        }
    }
}
