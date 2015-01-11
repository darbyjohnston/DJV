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

//! \file djvViewDisplayProfileWidget.cpp

#include <djvViewDisplayProfileWidget.h>

#include <djvViewImagePrefs.h>
#include <djvViewUtil.h>

#include <djvFileEdit.h>
#include <djvFloatEditSlider.h>
#include <djvFloatObject.h>
#include <djvIconLibrary.h>
#include <djvInputDialog.h>
#include <djvToolButton.h>

#include <djvImage.h>
#include <djvSignalBlocker.h>

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvViewDisplayProfileWidget::P
//------------------------------------------------------------------------------

struct djvViewDisplayProfileWidget::P
{
    P(const djvViewImageView * viewWidget) :
        viewWidget      (viewWidget),
        lutWidget       (0),
        brightnessWidget(0),
        contrastWidget  (0),
        saturationWidget(0),
        gammaWidget     (0),
        softClipWidget  (0)
    {}
    
    const djvViewImageView * viewWidget;
    djvViewDisplayProfile    displayProfile;
    djvFileEdit *            lutWidget;
    djvFloatEditSlider *     brightnessWidget;
    djvFloatEditSlider *     contrastWidget;
    djvFloatEditSlider *     saturationWidget;
    djvFloatEditSlider *     levelsInWidget[2];
    djvFloatEditSlider *     gammaWidget;
    djvFloatEditSlider *     levelsOutWidget[2];
    djvFloatEditSlider *     softClipWidget;
};

//------------------------------------------------------------------------------
// djvViewDisplayProfileWidget
//------------------------------------------------------------------------------

djvViewDisplayProfileWidget::djvViewDisplayProfileWidget(
    const djvViewImageView * viewWidget,
    QWidget *                parent) :
    QWidget(parent),
    _p(new P(viewWidget))
{
    // Create the LUT widgets.

    QGroupBox * lutGroup = new QGroupBox(tr("Lookup Table"));

    _p->lutWidget = new djvFileEdit;

    // Create the color widgets.

    QGroupBox * colorGroup = new QGroupBox(tr("Color"));

    _p->brightnessWidget = new djvFloatEditSlider;
    _p->brightnessWidget->setDefaultValue(_p->displayProfile.color.brightness);
    _p->brightnessWidget->editObject()->setClamp(false);
    _p->brightnessWidget->sliderObject()->setRange(0.0, 4.0);

    _p->contrastWidget = new djvFloatEditSlider;
    _p->contrastWidget->setDefaultValue(_p->displayProfile.color.contrast);
    _p->contrastWidget->editObject()->setClamp(false);
    _p->contrastWidget->sliderObject()->setRange(0.0, 4.0);

    _p->saturationWidget = new djvFloatEditSlider;
    _p->saturationWidget->setDefaultValue(_p->displayProfile.color.saturation);
    _p->saturationWidget->editObject()->setClamp(false);
    _p->saturationWidget->sliderObject()->setRange(0.0, 4.0);

    // Create the levels widgets.

    QGroupBox * levelsGroup = new QGroupBox(tr("Levels"));

    _p->levelsInWidget[0] = new djvFloatEditSlider;
    _p->levelsInWidget[0]->setDefaultValue(_p->displayProfile.levels.inLow);
    _p->levelsInWidget[0]->editObject()->setClamp(false);
    _p->levelsInWidget[0]->sliderObject()->setRange(0.0, 1.0);

    _p->levelsInWidget[1] = new djvFloatEditSlider;
    _p->levelsInWidget[1]->setDefaultValue(_p->displayProfile.levels.inHigh);
    _p->levelsInWidget[1]->editObject()->setClamp(false);
    _p->levelsInWidget[1]->sliderObject()->setRange(0.0, 1.0);

    _p->gammaWidget = new djvFloatEditSlider;
    _p->gammaWidget->setDefaultValue(_p->displayProfile.levels.gamma);
    _p->gammaWidget->editObject()->setClamp(false);
    _p->gammaWidget->sliderObject()->setRange(0.0, 4.0);

    _p->levelsOutWidget[0] = new djvFloatEditSlider;
    _p->levelsOutWidget[0]->setDefaultValue(_p->displayProfile.levels.outLow);
    _p->levelsOutWidget[0]->editObject()->setClamp(false);
    _p->levelsOutWidget[0]->sliderObject()->setRange(0.0, 1.0);

    _p->levelsOutWidget[1] = new djvFloatEditSlider;
    _p->levelsOutWidget[1]->setDefaultValue(_p->displayProfile.levels.outHigh);
    _p->levelsOutWidget[1]->editObject()->setClamp(false);
    _p->levelsOutWidget[1]->sliderObject()->setRange(0.0, 1.0);

    _p->softClipWidget = new djvFloatEditSlider;
    _p->softClipWidget->setDefaultValue(_p->displayProfile.softClip);
    _p->softClipWidget->editObject()->setClamp(false);
    _p->softClipWidget->sliderObject()->setRange(0.0, 1.0);

    // Create the other widgets.

    djvToolButton * addButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvAddIcon.png"));
    addButton->setToolTip(tr("Add this display profile to the favorites list"));

    djvToolButton * resetButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvRemoveIcon.png"));
    resetButton->setToolTip(tr("Reset the display profile"));

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);

    layout->addWidget(lutGroup);
    QFormLayout * formLayout = new QFormLayout(lutGroup);
    formLayout->addRow(_p->lutWidget);

    layout->addWidget(colorGroup);
    formLayout = new QFormLayout(colorGroup);
    formLayout->addRow(tr("Brightness:"), _p->brightnessWidget);
    formLayout->addRow(tr("Contrast:"), _p->contrastWidget);
    formLayout->addRow(tr("Saturation:"), _p->saturationWidget);
    
    layout->addWidget(levelsGroup);
    formLayout = new QFormLayout(levelsGroup);
    QVBoxLayout * vLayout = new QVBoxLayout;
    vLayout->addWidget(_p->levelsInWidget[0]);
    vLayout->addWidget(_p->levelsInWidget[1]);
    formLayout->addRow(tr("Input:"), vLayout);
    formLayout->addRow(tr("Gamma:"), _p->gammaWidget);
    vLayout = new QVBoxLayout;
    vLayout->addWidget(_p->levelsOutWidget[0]);
    vLayout->addWidget(_p->levelsOutWidget[1]);
    formLayout->addRow(tr("Output:"), vLayout);
    formLayout->addRow(tr("Soft clip:"), _p->softClipWidget);

    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(addButton);
    hLayout->addWidget(resetButton);
    layout->addLayout(hLayout);

    layout->addStretch();
    
    // Initialize.
    
    widgetUpdate();

    // Setup the LUT callbacks.

    connect(
        _p->lutWidget,
        SIGNAL(fileInfoChanged(const djvFileInfo &)),
        SLOT(lutCallback(const djvFileInfo &)));

    // Setup the color callbacks.

    connect(
        _p->brightnessWidget,
        SIGNAL(valueChanged(double)),
        SLOT(brightnessCallback(double)));

    connect(
        _p->contrastWidget,
        SIGNAL(valueChanged(double)),
        SLOT(contrastCallback(double)));

    connect(
        _p->saturationWidget,
        SIGNAL(valueChanged(double)),
        SLOT(saturationCallback(double)));

    // Setup the levels callbacks.

    connect(
        _p->levelsInWidget[0],
        SIGNAL(valueChanged(double)),
        SLOT(levelsIn0Callback(double)));

    connect(
        _p->levelsInWidget[1],
        SIGNAL(valueChanged(double)),
        SLOT(levelsIn1Callback(double)));

    connect(
        _p->gammaWidget,
        SIGNAL(valueChanged(double)),
        SLOT(gammaCallback(double)));

    connect(
        _p->levelsOutWidget[0],
        SIGNAL(valueChanged(double)),
        SLOT(levelsOut0Callback(double)));

    connect(
        _p->levelsOutWidget[1],
        SIGNAL(valueChanged(double)),
        SLOT(levelsOut1Callback(double)));

    connect(
        _p->softClipWidget,
        SIGNAL(valueChanged(double)),
        SLOT(softClipCallback(double)));

    // Setup the other callbacks.

    connect(
        addButton,
        SIGNAL(clicked()),
        SLOT(addCallback()));

    connect(
        resetButton,
        SIGNAL(clicked()),
        SLOT(resetCallback()));
}

djvViewDisplayProfileWidget::~djvViewDisplayProfileWidget()
{
    delete _p;
}

const djvViewDisplayProfile & djvViewDisplayProfileWidget::displayProfile() const
{
    return _p->displayProfile;
}

void djvViewDisplayProfileWidget::setDisplayProfile(
    const djvViewDisplayProfile & displayProfile)
{
    if (displayProfile == _p->displayProfile)
        return;
    
    _p->displayProfile = displayProfile;

    widgetUpdate();
    
    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::lutCallback(const djvFileInfo & in)
{
    _p->displayProfile.lutFile = in;
    
    djvViewUtil::loadLut(_p->displayProfile.lutFile, _p->displayProfile.lut);

    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::brightnessCallback(double value)
{
    //DJV_DEBUG("djvViewDisplayProfileWidget::brightnessCallback");
    //DJV_DEBUG_PRINT("value = " << value);
    
    _p->displayProfile.color.brightness = value;

    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::contrastCallback(double in)
{
    _p->displayProfile.color.contrast = in;

    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::saturationCallback(double in)
{
    _p->displayProfile.color.saturation = in;

    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::levelsIn0Callback(double in)
{
    _p->displayProfile.levels.inLow = in;

    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::levelsIn1Callback(double in)
{
    _p->displayProfile.levels.inHigh = in;

    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::gammaCallback(double in)
{
    _p->displayProfile.levels.gamma = in;

    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::levelsOut0Callback(double in)
{
    _p->displayProfile.levels.outLow = in;

    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::levelsOut1Callback(double in)
{
    _p->displayProfile.levels.outHigh = in;

    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::softClipCallback(double in)
{
    _p->displayProfile.softClip = in;

    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::addCallback()
{
    djvInputDialog dialog(tr("Name this display profile:"));

    if (QDialog::Accepted == dialog.exec())
    {
        QVector<djvViewDisplayProfile> displayProfiles =
            djvViewImagePrefs::global()->displayProfiles();

        djvViewDisplayProfile displayProfile(_p->displayProfile);
        displayProfile.name = dialog.text();
        displayProfiles += displayProfile;

        djvViewImagePrefs::global()->setDisplayProfiles(displayProfiles);
    }
}

void djvViewDisplayProfileWidget::resetCallback()
{
    _p->displayProfile = djvViewDisplayProfile();

    widgetUpdate();

    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->lutWidget <<
        _p->brightnessWidget <<
        _p->contrastWidget <<
        _p->saturationWidget <<
        _p->levelsInWidget[0] <<
        _p->levelsInWidget[1] <<
        _p->gammaWidget <<
        _p->levelsOutWidget[0] <<
        _p->levelsOutWidget[1] <<
        _p->softClipWidget);

    _p->lutWidget->setFileInfo(_p->displayProfile.lutFile);

    _p->brightnessWidget->setValue(_p->displayProfile.color.brightness);
    _p->contrastWidget->setValue(_p->displayProfile.color.contrast);
    _p->saturationWidget->setValue(_p->displayProfile.color.saturation);

    _p->levelsInWidget[0]->setValue(_p->displayProfile.levels.inLow);
    _p->levelsInWidget[1]->setValue(_p->displayProfile.levels.inHigh);
    _p->gammaWidget->setValue(_p->displayProfile.levels.gamma);
    _p->levelsOutWidget[0]->setValue(_p->displayProfile.levels.outLow);
    _p->levelsOutWidget[1]->setValue(_p->displayProfile.levels.outHigh);
    _p->softClipWidget->setValue(_p->displayProfile.softClip);
}
