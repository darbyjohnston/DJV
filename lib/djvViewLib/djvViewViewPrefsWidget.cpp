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

//! \file djvViewViewPrefsWidget.cpp

#include <djvViewViewPrefsWidget.h>

#include <djvViewViewPrefs.h>

#include <djvColorSwatch.h>
#include <djvPrefsGroupBox.h>

#include <djvSignalBlocker.h>

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
// djvViewViewPrefsWidget::P
//------------------------------------------------------------------------------

struct djvViewViewPrefsWidget::P
{
    P() :
        backgroundColorWidget   (0),
        gridWidget              (0),
        gridColorWidget         (0),
        hudEnabledWidget        (0),
        hudInfoWidget           (0),
        hudColorWidget          (0),
        hudBackgroundWidget     (0),
        hudBackgroundColorWidget(0)
    {}

    djvColorSwatch *        backgroundColorWidget;
    QComboBox *             gridWidget;
    djvColorSwatch *        gridColorWidget;
    QCheckBox *             hudEnabledWidget;
    QListWidget *           hudInfoWidget;
    djvColorSwatch *        hudColorWidget;
    QComboBox *             hudBackgroundWidget;
    djvColorSwatch *        hudBackgroundColorWidget;
};

//------------------------------------------------------------------------------
// djvViewViewPrefsWidget
//------------------------------------------------------------------------------

djvViewViewPrefsWidget::djvViewViewPrefsWidget() :
    djvViewAbstractPrefsWidget(tr("Views")),
    _p(new P)
{
    // Create the options widgets.

    _p->backgroundColorWidget = new djvColorSwatch;
    _p->backgroundColorWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->backgroundColorWidget->setColorDialogEnabled(true);

    // Create the grid widgets.

    _p->gridWidget = new QComboBox;
    _p->gridWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->gridWidget->addItems(djvViewUtil::gridLabels());

    _p->gridColorWidget = new djvColorSwatch();
    _p->gridColorWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->gridColorWidget->setColorDialogEnabled(true);

    // Create the HUD widgets.

    _p->hudEnabledWidget = new QCheckBox(tr("Enable"));

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

    _p->hudColorWidget = new djvColorSwatch;
    _p->hudColorWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->hudColorWidget->setColorDialogEnabled(true);

    _p->hudBackgroundWidget = new QComboBox;
    _p->hudBackgroundWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->hudBackgroundWidget->addItems(djvViewUtil::hudBackgroundLabels());

    _p->hudBackgroundColorWidget = new djvColorSwatch;
    _p->hudBackgroundColorWidget->setSwatchSize(djvColorSwatch::SWATCH_SMALL);
    _p->hudBackgroundColorWidget->setColorDialogEnabled(true);

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(tr("General"));
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(tr("Background color:"), _p->backgroundColorWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Grid"));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->gridWidget);
    formLayout->addRow(tr("Color:"), _p->gridColorWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("HUD (Heads Up Display)"));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->hudEnabledWidget);
    formLayout->addRow(tr("Information:"), _p->hudInfoWidget);
    formLayout->addRow(tr("Foreground color:"), _p->hudColorWidget);
    formLayout->addRow(tr("Background style:"), _p->hudBackgroundWidget);
    formLayout->addRow(tr("Background color:"), _p->hudBackgroundColorWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.
    
    widgetUpdate();

    // Setup the callbacks.

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
{
    delete _p;
}

void djvViewViewPrefsWidget::resetPreferences()
{
    djvViewViewPrefs::global()->setBackground(
        djvViewViewPrefs::backgroundDefault());
    djvViewViewPrefs::global()->setGrid(
        djvViewViewPrefs::gridDefault());
    djvViewViewPrefs::global()->setGridColor(
        djvViewViewPrefs::gridColorDefault());
    djvViewViewPrefs::global()->setHudEnabled(
        djvViewViewPrefs::hudEnabledDefault());
    djvViewViewPrefs::global()->setHudInfo(
        djvViewViewPrefs::hudInfoDefault());
    djvViewViewPrefs::global()->setHudColor(
        djvViewViewPrefs::hudColorDefault());
    djvViewViewPrefs::global()->setHudBackground(
        djvViewViewPrefs::hudBackgroundDefault());
    djvViewViewPrefs::global()->setHudBackgroundColor(
        djvViewViewPrefs::hudBackgroundColorDefault());

    widgetUpdate();
}

void djvViewViewPrefsWidget::backgroundCallback(const djvColor & in)
{
    _p->backgroundColorWidget->setColor(in);

    djvViewViewPrefs::global()->setBackground(in);
}

void djvViewViewPrefsWidget::gridCallback(int in)
{
    djvViewViewPrefs::global()->setGrid(
        static_cast<djvViewUtil::GRID>(in));
}

void djvViewViewPrefsWidget::gridColorCallback(const djvColor & in)
{
    _p->gridColorWidget->setColor(in);

    djvViewViewPrefs::global()->setGridColor(in);
}

void djvViewViewPrefsWidget::hudEnabledCallback(bool in)
{
    djvViewViewPrefs::global()->setHudEnabled(in);
}

void djvViewViewPrefsWidget::hudInfoCallback(QListWidgetItem * item)
{
    const int row = _p->hudInfoWidget->row(item);

    QVector<bool> info = djvViewViewPrefs::global()->hudInfo();

    info[row] = ! info[row];

    djvViewViewPrefs::global()->setHudInfo(info);
}

void djvViewViewPrefsWidget::hudColorCallback(const djvColor & in)
{
    _p->hudColorWidget->setColor(in);

    djvViewViewPrefs::global()->setHudColor(in);
}

void djvViewViewPrefsWidget::hudBackgroundCallback(int in)
{
    djvViewViewPrefs::global()->setHudBackground(
        static_cast<djvViewUtil::HUD_BACKGROUND>(in));
}

void djvViewViewPrefsWidget::hudBackgroundColorCallback(const djvColor & in)
{
    _p->hudBackgroundColorWidget->setColor(in);

    djvViewViewPrefs::global()->setHudBackgroundColor(in);
}

void djvViewViewPrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->backgroundColorWidget <<
        _p->gridWidget <<
        _p->gridColorWidget <<
        _p->hudEnabledWidget <<
        _p->hudInfoWidget <<
        _p->hudColorWidget <<
        _p->hudBackgroundWidget <<
        _p->hudBackgroundColorWidget);

    _p->backgroundColorWidget->setColor(
        djvViewViewPrefs::global()->background());

    _p->gridWidget->setCurrentIndex(
        djvViewViewPrefs::global()->grid());

    _p->gridColorWidget->setColor(
        djvViewViewPrefs::global()->gridColor());

    _p->hudEnabledWidget->setChecked(
        djvViewViewPrefs::global()->isHudEnabled());

    QVector<bool> hudInfo = djvViewViewPrefs::global()->hudInfo();

    for (int i = 0; i < djvViewUtil::HUD_COUNT; ++i)
    {
        QListWidgetItem * item = _p->hudInfoWidget->item(i);

        item->setCheckState(hudInfo[i] ? Qt::Checked : Qt::Unchecked);
    }

    _p->hudColorWidget->setColor(
        djvViewViewPrefs::global()->hudColor());

    _p->hudBackgroundWidget->setCurrentIndex(
        djvViewViewPrefs::global()->hudBackground());

    _p->hudBackgroundColorWidget->setColor(
        djvViewViewPrefs::global()->hudBackgroundColor());
}

