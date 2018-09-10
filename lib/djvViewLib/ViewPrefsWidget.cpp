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

#include <djvViewLib/ViewPrefsWidget.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/ViewPrefs.h>

#include <djvUI/ColorSwatch.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/PrefsGroupBox.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QListWidget>
#include <QVBoxLayout>

namespace
{
class SmallListWidget : public QListWidget
{
public:

    QSize sizeHint() const
    {
        const QSize size = QListWidget::sizeHint();
        return QSize(size.width(), size.height() / 2);
    }
};

} // namespace

//------------------------------------------------------------------------------
// djvViewViewPrefsWidget::Private
//------------------------------------------------------------------------------

struct djvViewViewPrefsWidget::Private
{
    Private() :
        zoomFactorWidget        (0),
        backgroundColorWidget   (0),
        gridWidget              (0),
        gridColorWidget         (0),
        hudEnabledWidget        (0),
        hudInfoWidget           (0),
        hudColorWidget          (0),
        hudBackgroundWidget     (0),
        hudBackgroundColorWidget(0)
    {}

    QComboBox *      zoomFactorWidget;
    djvColorSwatch * backgroundColorWidget;
    QComboBox *      gridWidget;
    djvColorSwatch * gridColorWidget;
    QCheckBox *      hudEnabledWidget;
    QListWidget *    hudInfoWidget;
    djvColorSwatch * hudColorWidget;
    QComboBox *      hudBackgroundWidget;
    djvColorSwatch * hudBackgroundColorWidget;
};

//------------------------------------------------------------------------------
// djvViewViewPrefsWidget
//------------------------------------------------------------------------------

djvViewViewPrefsWidget::djvViewViewPrefsWidget(djvViewContext * context) :
    djvViewAbstractPrefsWidget(
        qApp->translate("djvViewViewPrefsWidget", "Views"), context),
    _p(new Private)
{
    // Create the general widgets.
    _p->zoomFactorWidget = new QComboBox;
    _p->zoomFactorWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->zoomFactorWidget->addItems(djvViewUtil::zoomFactorLabels());
    
    _p->backgroundColorWidget = new djvColorSwatch(context);
    _p->backgroundColorWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->backgroundColorWidget->setColorDialogEnabled(true);

    // Create the grid widgets.
    _p->gridWidget = new QComboBox;
    _p->gridWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->gridWidget->addItems(djvViewUtil::gridLabels());

    _p->gridColorWidget = new djvColorSwatch(context);
    _p->gridColorWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->gridColorWidget->setColorDialogEnabled(true);

    // Create the HUD widgets.
    _p->hudEnabledWidget = new QCheckBox(
        qApp->translate("djvViewViewPrefsWidget", "Enable the HUD"));

    _p->hudInfoWidget = new SmallListWidget;
    
    for (int i = 0; i < djvViewUtil::HUD_COUNT; ++i)
    {
        QListWidgetItem * item = new QListWidgetItem(_p->hudInfoWidget);
        item->setText(djvViewUtil::hudInfoLabels()[i]);
        item->setFlags(
            Qt::ItemIsSelectable    |
            Qt::ItemIsUserCheckable |
            Qt::ItemIsEnabled);
    }

    _p->hudColorWidget = new djvColorSwatch(context);
    _p->hudColorWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->hudColorWidget->setColorDialogEnabled(true);

    _p->hudBackgroundWidget = new QComboBox;
    _p->hudBackgroundWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->hudBackgroundWidget->addItems(djvViewUtil::hudBackgroundLabels());

    _p->hudBackgroundColorWidget = new djvColorSwatch(context);
    _p->hudBackgroundColorWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->hudBackgroundColorWidget->setColorDialogEnabled(true);

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvViewViewPrefsWidget", "Views"), context);
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvViewViewPrefsWidget", "Mouse wheel zoom factor:"),
        _p->zoomFactorWidget);
    formLayout->addRow(
        qApp->translate("djvViewViewPrefsWidget", "Background color:"),
        _p->backgroundColorWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvViewViewPrefsWidget", "Grid"), context);
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvViewViewPrefsWidget", "Size:"),
        _p->gridWidget);
    formLayout->addRow(
        qApp->translate("djvViewViewPrefsWidget", "Color:"),
        _p->gridColorWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvViewViewPrefsWidget", "HUD (Heads Up Display)"), context);
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->hudEnabledWidget);
    formLayout->addRow(_p->hudInfoWidget);
    formLayout->addRow(
        qApp->translate("djvViewViewPrefsWidget", "Foreground color:"),
        _p->hudColorWidget);
    formLayout->addRow(
        qApp->translate("djvViewViewPrefsWidget", "Background style:"),
        _p->hudBackgroundWidget);
    formLayout->addRow(
        qApp->translate("djvViewViewPrefsWidget", "Background color:"),
        _p->hudBackgroundColorWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.
    widgetUpdate();
    
    // Setup the callbacks.
    connect(
        _p->zoomFactorWidget,
        SIGNAL(currentIndexChanged(int)),
        SLOT(zoomFactorCallback(int)));
    connect(
        _p->backgroundColorWidget,
        SIGNAL(colorChanged(const djvColor &)),
        SLOT(backgroundCallback(const djvColor &)));
    connect(
        _p->gridWidget,
        SIGNAL(currentIndexChanged(int)),
        SLOT(gridCallback(int)));
    connect(
        _p->gridColorWidget,
        SIGNAL(colorChanged(const djvColor &)),
        SLOT(gridColorCallback(const djvColor &)));
    connect(
        _p->hudEnabledWidget,
        SIGNAL(toggled(bool)),
        SLOT(hudEnabledCallback(bool)));
    connect(
        _p->hudInfoWidget,
        SIGNAL(itemChanged(QListWidgetItem *)),
        SLOT(hudInfoCallback(QListWidgetItem *)));
    connect(
        _p->hudColorWidget,
        SIGNAL(colorChanged(const djvColor &)),
        SLOT(hudColorCallback(const djvColor &)));
    connect(
        _p->hudBackgroundWidget,
        SIGNAL(currentIndexChanged(int)),
        SLOT(hudBackgroundCallback(int)));
    connect(
        _p->hudBackgroundColorWidget,
        SIGNAL(colorChanged(const djvColor &)),
        SLOT(hudBackgroundColorCallback(const djvColor &)));
}

djvViewViewPrefsWidget::~djvViewViewPrefsWidget()
{}

void djvViewViewPrefsWidget::resetPreferences()
{
    context()->viewPrefs()->setZoomFactor(djvViewViewPrefs::zoomFactorDefault());
    context()->viewPrefs()->setBackground(djvViewViewPrefs::backgroundDefault());
    context()->viewPrefs()->setGrid(djvViewViewPrefs::gridDefault());
    context()->viewPrefs()->setGridColor(djvViewViewPrefs::gridColorDefault());
    context()->viewPrefs()->setHudEnabled(djvViewViewPrefs::hudEnabledDefault());
    context()->viewPrefs()->setHudInfo(djvViewViewPrefs::hudInfoDefault());
    context()->viewPrefs()->setHudColor(djvViewViewPrefs::hudColorDefault());
    context()->viewPrefs()->setHudBackground(djvViewViewPrefs::hudBackgroundDefault());
    context()->viewPrefs()->setHudBackgroundColor(djvViewViewPrefs::hudBackgroundColorDefault());
    widgetUpdate();
}

void djvViewViewPrefsWidget::zoomFactorCallback(int in)
{
    context()->viewPrefs()->setZoomFactor(static_cast<djvViewUtil::ZOOM_FACTOR>(in));
}

void djvViewViewPrefsWidget::backgroundCallback(const djvColor & in)
{
    _p->backgroundColorWidget->setColor(in);
    context()->viewPrefs()->setBackground(in);
}

void djvViewViewPrefsWidget::gridCallback(int in)
{
    context()->viewPrefs()->setGrid(static_cast<djvViewUtil::GRID>(in));
}

void djvViewViewPrefsWidget::gridColorCallback(const djvColor & in)
{
    _p->gridColorWidget->setColor(in);
    context()->viewPrefs()->setGridColor(in);
}

void djvViewViewPrefsWidget::hudEnabledCallback(bool in)
{
    context()->viewPrefs()->setHudEnabled(in);
}

void djvViewViewPrefsWidget::hudInfoCallback(QListWidgetItem * item)
{
    const int row = _p->hudInfoWidget->row(item);
    QVector<bool> info = context()->viewPrefs()->hudInfo();
    info[row] = ! info[row];
    context()->viewPrefs()->setHudInfo(info);
}

void djvViewViewPrefsWidget::hudColorCallback(const djvColor & in)
{
    _p->hudColorWidget->setColor(in);
    context()->viewPrefs()->setHudColor(in);
}

void djvViewViewPrefsWidget::hudBackgroundCallback(int in)
{
    context()->viewPrefs()->setHudBackground(static_cast<djvViewUtil::HUD_BACKGROUND>(in));
}

void djvViewViewPrefsWidget::hudBackgroundColorCallback(const djvColor & in)
{
    _p->hudBackgroundColorWidget->setColor(in);
    context()->viewPrefs()->setHudBackgroundColor(in);
}

void djvViewViewPrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->zoomFactorWidget <<
        _p->backgroundColorWidget <<
        _p->gridWidget <<
        _p->gridColorWidget <<
        _p->hudEnabledWidget <<
        _p->hudInfoWidget <<
        _p->hudColorWidget <<
        _p->hudBackgroundWidget <<
        _p->hudBackgroundColorWidget);
    _p->zoomFactorWidget->setCurrentIndex(context()->viewPrefs()->zoomFactor());
    _p->backgroundColorWidget->setColor(context()->viewPrefs()->background());
    _p->gridWidget->setCurrentIndex(context()->viewPrefs()->grid());
    _p->gridColorWidget->setColor(context()->viewPrefs()->gridColor());
    _p->hudEnabledWidget->setChecked(context()->viewPrefs()->isHudEnabled());
    QVector<bool> hudInfo = context()->viewPrefs()->hudInfo();
    for (int i = 0; i < djvViewUtil::HUD_COUNT; ++i)
    {
        QListWidgetItem * item = _p->hudInfoWidget->item(i);
        item->setCheckState(hudInfo[i] ? Qt::Checked : Qt::Unchecked);
    }
    _p->hudColorWidget->setColor(context()->viewPrefs()->hudColor());
    _p->hudBackgroundWidget->setCurrentIndex(context()->viewPrefs()->hudBackground());
    _p->hudBackgroundColorWidget->setColor(context()->viewPrefs()->hudBackgroundColor());
}

