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

//! \file djvViewHistogramTool.cpp

#include <djvViewHistogramTool.h>

#include <djvViewImageView.h>

#include <djvViewApplication.h>
#include <djvViewDisplayProfile.h>
#include <djvViewImageView.h>
#include <djvViewMainWindow.h>

#include <djvIconLibrary.h>
#include <djvPixelMaskWidget.h>
#include <djvPrefs.h>
#include <djvToolButton.h>

#include <djvDebug.h>
#include <djvError.h>
#include <djvSignalBlocker.h>

#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPainter>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvViewHistogramWidget
//------------------------------------------------------------------------------

class djvViewHistogramWidget : public QWidget
{
public:

    djvViewHistogramWidget();

    void set(
        const djvPixelData * data,
        const djvColor &     min,
        const djvColor &     max);

    virtual QSize sizeHint() const;
    
protected:

    virtual void resizeEvent(QResizeEvent *);
    virtual void paintEvent(QPaintEvent *);

private:

    void updatePixmap();

    const djvPixelData * _data;
    djvColor             _min;
    djvColor             _max;
    QPixmap              _pixmap;
    bool                 _dirty;
};

djvViewHistogramWidget::djvViewHistogramWidget() :
    _data (0),
    _dirty(true)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void djvViewHistogramWidget::set(
    const djvPixelData * data,
    const djvColor &     min,
    const djvColor &     max)
{
    _data  = data;
    _min   = min;
    _max   = max;
    _dirty = true;

    update();
}

QSize djvViewHistogramWidget::sizeHint() const
{
    return QSize(256, 256);
}

void djvViewHistogramWidget::resizeEvent(QResizeEvent *)
{
    if (isVisible() && _data)
    {
        _dirty = true;
    }
}

void djvViewHistogramWidget::paintEvent(QPaintEvent *)
{
    if (_dirty)
    {
        updatePixmap();
    }

    QPainter painter(this);

    painter.drawPixmap(0, 0, _pixmap);
}

namespace
{

void drawHistrogramBar(
    QPainter &     painter,
    int            width,
    int            height,
    int            dataWidth,
    int            index,
    int            y,
    const QColor & color)
{
    const int x = djvMath::floor(
        index / static_cast<double>(dataWidth)* width);

    const int w = djvMath::max(
        djvMath::floor(
            (index + 1) / static_cast<double>(dataWidth)* width) - x,
        1);

    painter.fillRect(x, height - 1 - y, w, y, color);
}

} // namespace

void djvViewHistogramWidget::updatePixmap()
{
    if (! _data)
        return;

    //DJV_DEBUG("djvViewHistogramWidget::updatePixmap");
    //DJV_DEBUG_PRINT("data = " << *_data);

    const int width  = this->width();
    const int height = this->height();

    //DJV_DEBUG_PRINT("size = " << w << " " << h);

    _pixmap = QPixmap(width, height);

    QPainter painter(&_pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(0, 0, width, height, palette().color(QPalette::Background));

    const djvPixel::U16_T * p =
        reinterpret_cast<const djvPixel::U16_T *>(_data->data());

    const int dataW = _data->w();
    const int dataC = _data->channels();

    //DJV_DEBUG_PRINT("dataW = " << dataW);
    //DJV_DEBUG_PRINT("dataC = " << dataC);

    int dataMax = 0;

    for (int i = 0; i < dataW; ++i, p += dataC)
    {
        for (int c = 0; c < dataC; ++c)
        {
            dataMax = djvMath::max(static_cast<int>(p[c]), dataMax);
        }
    }

    //DJV_DEBUG_PRINT("dataMax = " << dataMax);

    p = reinterpret_cast<const djvPixel::U16_T *>(_data->data());

    const QColor colors[] =
    {
        QColor(255,   0,   0),
        QColor(  0, 255,   0),
        QColor(  0,   0, 255)
    };

    for (int i = 0; i < dataW; ++i, p += dataC)
    {
        int yC [djvPixel::channelsMax] = { 0, 0, 0, 0 };
        int yMax = 0;

        for (int c = dataC - 1; c >= 0; --c)
        {
            const int y = djvMath::floor(
                p[c] / static_cast<double>(dataMax) * (height - 1));

            yC[c] = y;

            yMax = djvMath::max(y, yMax);
        }

        drawHistrogramBar(painter, width, height, dataW, i, yMax, Qt::black);

        painter.setCompositionMode(QPainter::CompositionMode_Plus);

        for (int c = dataC - 1; c >= 0; --c)
        {
            drawHistrogramBar(painter, width, height, dataW, i, yC[c], colors[c]);
        }
    }
}

//------------------------------------------------------------------------------
// djvViewHistogramTool::P
//------------------------------------------------------------------------------

struct djvViewHistogramTool::P
{
    P() :
        size                (static_cast<djvViewUtil::HISTOGRAM>(0)),
        colorProfile        (true),
        displayProfile      (true),
        widget              (0),
        minWidget           (0),
        maxWidget           (0),
        sizeWidget          (0),
        maskWidget          (0),
        colorProfileButton  (0),
        displayProfileButton(0)
    {}
    
    djvViewUtil::HISTOGRAM   size;
    bool                     colorProfile;
    bool                     displayProfile;
    djvPixelData             histogram;
    djvColor                 min;
    djvColor                 max;
    djvPixel::Mask           mask;
    
    djvViewHistogramWidget * widget;
    QLineEdit *              minWidget;
    QLineEdit *              maxWidget;
    QComboBox *              sizeWidget;
    djvPixelMaskWidget *     maskWidget;
    djvToolButton *          colorProfileButton;
    djvToolButton *          displayProfileButton;
};

//------------------------------------------------------------------------------
// djvViewHistogramTool
//------------------------------------------------------------------------------

djvViewHistogramTool::djvViewHistogramTool(
    djvViewMainWindow * mainWindow,
    QWidget *           parent) :
    djvViewAbstractTool(mainWindow, parent),
    _p(new P)
{
    // Create the widgets.

    _p->widget = new djvViewHistogramWidget;

    _p->minWidget = new QLineEdit;
    _p->minWidget->setReadOnly(true);

    _p->maxWidget = new QLineEdit;
    _p->maxWidget->setReadOnly(true);

    _p->sizeWidget = new QComboBox;
    _p->sizeWidget->addItems(djvViewUtil::histogramLabels());
    _p->sizeWidget->setToolTip(tr("The size of the histogram"));

    _p->maskWidget = new djvPixelMaskWidget;
    
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

    QFormLayout * formLayout = new QFormLayout;
    formLayout->addRow(tr("Min:"), _p->minWidget);
    formLayout->addRow(tr("Max:"), _p->maxWidget);
    layout->addLayout(formLayout);
    
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->addWidget(_p->sizeWidget);
    hLayout->addStretch();
    hLayout->addWidget(_p->maskWidget);
    QHBoxLayout * hLayout2 = new QHBoxLayout;
    hLayout2->setMargin(0);
    hLayout2->setSpacing(0);
    hLayout2->addWidget(_p->colorProfileButton);
    hLayout2->addWidget(_p->displayProfileButton);
    hLayout->addLayout(hLayout2);
    layout->addLayout(hLayout);

    // Preferences.

    djvPrefs prefs("djvViewHistogramTool");
    prefs.get("colorProfile", _p->colorProfile);
    prefs.get("displayProfile", _p->displayProfile);

    // Initialize.
    
    setWindowTitle(tr("Histogram"));

    widgetUpdate();

    // Setup the callbacks.

    connect(
        mainWindow,
        SIGNAL(imageChanged()),
        SLOT(widgetUpdate()));

    connect(
        _p->sizeWidget,
        SIGNAL(activated(int)),
        SLOT(sizeCallback(int)));

    connect(
        _p->maskWidget,
        SIGNAL(maskChanged(const djvPixel::Mask &)),
        SLOT(maskCallback(const djvPixel::Mask &)));
    
    connect(
        _p->colorProfileButton,
        SIGNAL(toggled(bool)),
        SLOT(colorProfileCallback(bool)));
    
    connect(
        _p->displayProfileButton,
        SIGNAL(toggled(bool)),
        SLOT(displayProfileCallback(bool)));
}

djvViewHistogramTool::~djvViewHistogramTool()
{
    djvPrefs prefs("djvViewHistogramTool");
    prefs.set("colorProfile", _p->colorProfile);
    prefs.set("displayProfile", _p->displayProfile);
    
    delete _p;
}

void djvViewHistogramTool::sizeCallback(int in)
{
    _p->size = static_cast<djvViewUtil::HISTOGRAM>(in);
    
    widgetUpdate();
}

void djvViewHistogramTool::maskCallback(const djvPixel::Mask & mask)
{
    _p->mask = mask;
    
    widgetUpdate();
}

void djvViewHistogramTool::colorProfileCallback(bool in)
{
    _p->colorProfile = in;

    widgetUpdate();
}

void djvViewHistogramTool::displayProfileCallback(bool in)
{
    _p->displayProfile = in;

    widgetUpdate();
}

void djvViewHistogramTool::showEvent(QShowEvent * event)
{
    djvViewAbstractTool::showEvent(event);
    
    widgetUpdate();
}

void djvViewHistogramTool::resizeEvent(QResizeEvent * event)
{
    djvViewAbstractTool::resizeEvent(event);
    
    widgetUpdate();
}

void djvViewHistogramTool::widgetUpdate()
{
    //DJV_DEBUG("djvViewHistogramTool::widgetUpdate");

    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->widget <<
        _p->minWidget <<
        _p->maxWidget <<
        _p->sizeWidget <<
        _p->maskWidget <<
        _p->colorProfileButton <<
        _p->displayProfileButton);

    if (isVisible())
    {
        if (const djvPixelData * data = viewWidget()->data())
        {
            try
            {
                viewWidget()->makeCurrent();

                djvOpenGlImageOptions options = viewWidget()->options();
    
                //! \todo Why do we need to reverse the rotation here?
                
                options.xform.rotate = options.xform.rotate;

                const djvBox2f bbox =
                    djvOpenGlImageXform::xformMatrix(options.xform) *
                    djvBox2f(data->size());

                //DJV_DEBUG_PRINT("bbox = " << bbox);

                options.xform.position = -bbox.position;
                
                if (! _p->colorProfile)
                {
                    options.colorProfile = djvColorProfile();
                }

                if (! _p->displayProfile)
                {
                    options.displayProfile = djvViewDisplayProfile();
                }
            
                djvPixelData tmp(djvPixelDataInfo(bbox.size, data->pixel()));

                djvOpenGlImage::copy(*data, tmp, options);
                
                djvOpenGlImage::histogram(
                    tmp,
                    _p->histogram,
                    djvViewUtil::histogramSize(_p->size),
                    _p->min,
                    _p->max,
                    _p->mask);
            }
            catch (djvError error)
            {
                error.add(
                    djvViewUtil::errorLabels()[djvViewUtil::ERROR_HISTOGRAM]);
                
                DJV_VIEW_APP->printError(error);
            }
        }
        else
        {
            _p->histogram.zero();
            _p->min.zero();
            _p->max.zero();
        }
    }

    _p->widget->set(&_p->histogram, _p->min, _p->max);
    _p->minWidget->setText(djvStringUtil::label(_p->min).join(", "));
    _p->maxWidget->setText(djvStringUtil::label(_p->max).join(", "));
    _p->maskWidget->setMask(_p->mask);
    _p->colorProfileButton->setChecked(_p->colorProfile);
    _p->displayProfileButton->setChecked(_p->displayProfile);
}
