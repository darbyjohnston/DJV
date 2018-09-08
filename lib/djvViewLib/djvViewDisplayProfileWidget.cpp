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

#include <djvViewDisplayProfileWidget.h>

#include <djvViewContext.h>
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

#include <QApplication>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvViewDisplayProfileWidget::Private
//------------------------------------------------------------------------------

struct djvViewDisplayProfileWidget::Private
{
    Private(
        const djvViewImageView * viewWidget,
        djvViewContext *         context) :
        viewWidget      (viewWidget),
        lutWidget       (0),
        brightnessWidget(0),
        contrastWidget  (0),
        saturationWidget(0),
        gammaWidget     (0),
        softClipWidget  (0),
        context         (context)
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
    djvViewContext *         context;
};

//------------------------------------------------------------------------------
// djvViewDisplayProfileWidget
//------------------------------------------------------------------------------

djvViewDisplayProfileWidget::djvViewDisplayProfileWidget(
    const djvViewImageView * viewWidget,
    djvViewContext *         context,
    QWidget *                parent) :
    QWidget(parent),
    _p(new Private(viewWidget, context))
{
    // Create the LUT widgets.
    QGroupBox * lutGroup = new QGroupBox(
        qApp->translate("djvViewDisplayProfileWidget", "Lookup Table"));

    _p->lutWidget = new djvFileEdit(context);

    // Create the color widgets.
    QGroupBox * colorGroup = new QGroupBox(
        qApp->translate("djvViewDisplayProfileWidget", "Color"));

    _p->brightnessWidget = new djvFloatEditSlider(context);
    _p->brightnessWidget->setDefaultValue(_p->displayProfile.color.brightness);
    _p->brightnessWidget->editObject()->setClamp(false);
    _p->brightnessWidget->sliderObject()->setRange(0.f, 4.f);

    _p->contrastWidget = new djvFloatEditSlider(context);
    _p->contrastWidget->setDefaultValue(_p->displayProfile.color.contrast);
    _p->contrastWidget->editObject()->setClamp(false);
    _p->contrastWidget->sliderObject()->setRange(0.f, 4.f);

    _p->saturationWidget = new djvFloatEditSlider(context);
    _p->saturationWidget->setDefaultValue(_p->displayProfile.color.saturation);
    _p->saturationWidget->editObject()->setClamp(false);
    _p->saturationWidget->sliderObject()->setRange(0.f, 4.f);

    // Create the levels widgets.
    QGroupBox * levelsGroup = new QGroupBox(
        qApp->translate("djvViewDisplayProfileWidget", "Levels"));

    _p->levelsInWidget[0] = new djvFloatEditSlider(context);
    _p->levelsInWidget[0]->setDefaultValue(_p->displayProfile.levels.inLow);
    _p->levelsInWidget[0]->editObject()->setClamp(false);
    _p->levelsInWidget[0]->sliderObject()->setRange(0.f, 1.f);

    _p->levelsInWidget[1] = new djvFloatEditSlider(context);
    _p->levelsInWidget[1]->setDefaultValue(_p->displayProfile.levels.inHigh);
    _p->levelsInWidget[1]->editObject()->setClamp(false);
    _p->levelsInWidget[1]->sliderObject()->setRange(0.f, 1.f);

    _p->gammaWidget = new djvFloatEditSlider(context);
    _p->gammaWidget->setDefaultValue(_p->displayProfile.levels.gamma);
    _p->gammaWidget->editObject()->setClamp(false);
    _p->gammaWidget->sliderObject()->setRange(0.f, 4.f);

    _p->levelsOutWidget[0] = new djvFloatEditSlider(context);
    _p->levelsOutWidget[0]->setDefaultValue(_p->displayProfile.levels.outLow);
    _p->levelsOutWidget[0]->editObject()->setClamp(false);
    _p->levelsOutWidget[0]->sliderObject()->setRange(0.f, 1.f);

    _p->levelsOutWidget[1] = new djvFloatEditSlider(context);
    _p->levelsOutWidget[1]->setDefaultValue(_p->displayProfile.levels.outHigh);
    _p->levelsOutWidget[1]->editObject()->setClamp(false);
    _p->levelsOutWidget[1]->sliderObject()->setRange(0.f, 1.f);

    _p->softClipWidget = new djvFloatEditSlider(context);
    _p->softClipWidget->setDefaultValue(_p->displayProfile.softClip);
    _p->softClipWidget->editObject()->setClamp(false);
    _p->softClipWidget->sliderObject()->setRange(0.f, 1.f);

    // Create the other widgets.
    djvToolButton * addButton = new djvToolButton(
        context->iconLibrary()->icon("djvAddIcon.png"));
    addButton->setToolTip(
        qApp->translate("djvViewDisplayProfileWidget", "Add this display profile to the favorites list"));

    djvToolButton * resetButton = new djvToolButton(
        context->iconLibrary()->icon("djvRemoveIcon.png"));
    resetButton->setToolTip(
        qApp->translate("djvViewDisplayProfileWidget", "Reset the display profile"));

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);

    layout->addWidget(lutGroup);
    QFormLayout * formLayout = new QFormLayout(lutGroup);
    formLayout->addRow(_p->lutWidget);

    layout->addWidget(colorGroup);
    formLayout = new QFormLayout(colorGroup);
    formLayout->addRow(
        qApp->translate("djvViewDisplayProfileWidget", "Brightness:"),
        _p->brightnessWidget);
    formLayout->addRow(
        qApp->translate("djvViewDisplayProfileWidget", "Contrast:"),
        _p->contrastWidget);
    formLayout->addRow(
        qApp->translate("djvViewDisplayProfileWidget", "Saturation:"),
        _p->saturationWidget);
    
    layout->addWidget(levelsGroup);
    formLayout = new QFormLayout(levelsGroup);
    QVBoxLayout * vLayout = new QVBoxLayout;
    vLayout->addWidget(_p->levelsInWidget[0]);
    vLayout->addWidget(_p->levelsInWidget[1]);
    formLayout->addRow(
        qApp->translate("djvViewDisplayProfileWidget", "Input:"),
        vLayout);
    formLayout->addRow(
        qApp->translate("djvViewDisplayProfileWidget", "Gamma:"),
        _p->gammaWidget);
    vLayout = new QVBoxLayout;
    vLayout->addWidget(_p->levelsOutWidget[0]);
    vLayout->addWidget(_p->levelsOutWidget[1]);
    formLayout->addRow(
        qApp->translate("djvViewDisplayProfileWidget", "Output:"),
        vLayout);
    formLayout->addRow(
        qApp->translate("djvViewDisplayProfileWidget", "Soft clip:"),
        _p->softClipWidget);

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
        SIGNAL(valueChanged(float)),
        SLOT(brightnessCallback(float)));
    connect(
        _p->contrastWidget,
        SIGNAL(valueChanged(float)),
        SLOT(contrastCallback(float)));
    connect(
        _p->saturationWidget,
        SIGNAL(valueChanged(float)),
        SLOT(saturationCallback(float)));

    // Setup the levels callbacks.
    connect(
        _p->levelsInWidget[0],
        SIGNAL(valueChanged(float)),
        SLOT(levelsIn0Callback(float)));
    connect(
        _p->levelsInWidget[1],
        SIGNAL(valueChanged(float)),
        SLOT(levelsIn1Callback(float)));
    connect(
        _p->gammaWidget,
        SIGNAL(valueChanged(float)),
        SLOT(gammaCallback(float)));
    connect(
        _p->levelsOutWidget[0],
        SIGNAL(valueChanged(float)),
        SLOT(levelsOut0Callback(float)));
    connect(
        _p->levelsOutWidget[1],
        SIGNAL(valueChanged(float)),
        SLOT(levelsOut1Callback(float)));
    connect(
        _p->softClipWidget,
        SIGNAL(valueChanged(float)),
        SLOT(softClipCallback(float)));

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
{}

const djvViewDisplayProfile & djvViewDisplayProfileWidget::displayProfile() const
{
    return _p->displayProfile;
}

void djvViewDisplayProfileWidget::setDisplayProfile(const djvViewDisplayProfile & displayProfile)
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
    try
    {
        djvViewUtil::loadLut(
            _p->displayProfile.lutFile,
            _p->displayProfile.lut,
            _p->context);
    }
    catch (const djvError & error)
    {
        _p->context->printError(error);
    }
    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::brightnessCallback(float value)
{
    //DJV_DEBUG("djvViewDisplayProfileWidget::brightnessCallback");
    //DJV_DEBUG_PRINT("value = " << value);
    _p->displayProfile.color.brightness = value;
    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::contrastCallback(float in)
{
    _p->displayProfile.color.contrast = in;
    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::saturationCallback(float in)
{
    _p->displayProfile.color.saturation = in;
    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::levelsIn0Callback(float in)
{
    _p->displayProfile.levels.inLow = in;
    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::levelsIn1Callback(float in)
{
    _p->displayProfile.levels.inHigh = in;
    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::gammaCallback(float in)
{
    _p->displayProfile.levels.gamma = in;
    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::levelsOut0Callback(float in)
{
    _p->displayProfile.levels.outLow = in;
    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::levelsOut1Callback(float in)
{
    _p->displayProfile.levels.outHigh = in;
    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::softClipCallback(float in)
{
    _p->displayProfile.softClip = in;
    Q_EMIT displayProfileChanged(_p->displayProfile);
}

void djvViewDisplayProfileWidget::addCallback()
{
    djvInputDialog dialog(
        qApp->translate("djvViewDisplayProfileWidget", "Name this display profile:"));
    if (QDialog::Accepted == dialog.exec())
    {
        QVector<djvViewDisplayProfile> displayProfiles =
            _p->context->imagePrefs()->displayProfiles();
        djvViewDisplayProfile displayProfile(_p->displayProfile);
        displayProfile.name = dialog.text();
        displayProfiles += displayProfile;
        _p->context->imagePrefs()->setDisplayProfiles(displayProfiles);
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
