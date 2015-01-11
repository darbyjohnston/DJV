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

//! \file djvViewImagePrefsWidget.cpp

#include <djvViewImagePrefsWidget.h>

#include <djvViewImagePrefs.h>

#include <djvIconLibrary.h>
#include <djvInputDialog.h>
#include <djvPrefsGroupBox.h>
#include <djvToolButton.h>

#include <djvImage.h>
#include <djvSignalBlocker.h>

#include <QComboBox>
#include <QCheckBox>
#include <QListWidget>
#include <QFormLayout>
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
// djvViewImagePrefsWidget::P
//------------------------------------------------------------------------------

struct djvViewImagePrefsWidget::P
{
    P() :
        frameStoreFileReloadWidget(0),
        mirrorHWidget             (0),
        mirrorVWidget             (0),
        scaleWidget               (0),
        rotateWidget              (0),
        colorProfileWidget        (0),
        displayProfileWidget      (0),
        displayProfileListWidget  (0),
        channelWidget             (0)
    {}
    
    QCheckBox *    frameStoreFileReloadWidget;
    QCheckBox *    mirrorHWidget;
    QCheckBox *    mirrorVWidget;
    QComboBox *    scaleWidget;
    QComboBox *    rotateWidget;
    QCheckBox *    colorProfileWidget;
    QComboBox *    displayProfileWidget;
    QListWidget *  displayProfileListWidget;
    QComboBox *    channelWidget;
};

//------------------------------------------------------------------------------
// djvViewImagePrefsWidget
//------------------------------------------------------------------------------

djvViewImagePrefsWidget::djvViewImagePrefsWidget() :
    djvViewAbstractPrefsWidget(tr("Images")),
    _p(new P)
{
    // Create the widgets.

    _p->frameStoreFileReloadWidget = new QCheckBox(
        tr("Automatically store the current frame when files are reloaded"));

    _p->mirrorHWidget = new QCheckBox(tr("Mirror horizontal"));

    _p->mirrorVWidget = new QCheckBox(tr("Mirror vertical"));

    _p->scaleWidget = new QComboBox;
    _p->scaleWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->scaleWidget->addItems(djvViewUtil::imageScaleLabels());

    _p->rotateWidget = new QComboBox;
    _p->rotateWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->rotateWidget->addItems(djvViewUtil::imageRotateLabels());

    _p->colorProfileWidget = new QCheckBox(tr("Enable"));

    _p->displayProfileWidget = new QComboBox;
    _p->displayProfileWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _p->displayProfileListWidget = new SmallListWidget;

    djvToolButton * addDisplayProfileButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvAddIcon.png"));
    addDisplayProfileButton->setToolTip(tr("Add a new display profile"));

    djvToolButton * removeDisplayProfileButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvRemoveIcon.png"));
    removeDisplayProfileButton->setAutoRepeat(true);
    removeDisplayProfileButton->setToolTip(
        tr("Remove the selected display profile"));

    djvToolButton * moveDisplayProfileUpButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvUpIcon.png"));
    moveDisplayProfileUpButton->setAutoRepeat(true);
    moveDisplayProfileUpButton->setToolTip(
        tr("Move the selected display profile up"));

    djvToolButton * moveDisplayProfileDownButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvDownIcon.png"));
    moveDisplayProfileDownButton->setAutoRepeat(true);
    moveDisplayProfileDownButton->setToolTip(
        tr("Move the selected display profile down"));

    _p->channelWidget = new QComboBox;
    _p->channelWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->channelWidget->addItems(djvOpenGlImageOptions::channelLabels());

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(tr("General"));
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->frameStoreFileReloadWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Transforms"));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->mirrorHWidget);
    formLayout->addRow(_p->mirrorVWidget);
    formLayout->addRow(tr("Scale:"), _p->scaleWidget);
    formLayout->addRow(tr("Rotate:"), _p->rotateWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Color Profile"));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->colorProfileWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Display Profile"));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->displayProfileWidget);
    formLayout->addRow(_p->displayProfileListWidget);
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(addDisplayProfileButton);
    hLayout->addWidget(removeDisplayProfileButton);
    hLayout->addWidget(moveDisplayProfileUpButton);
    hLayout->addWidget(moveDisplayProfileDownButton);
    formLayout->addRow(hLayout);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Channels"));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->channelWidget);
    layout->addWidget(prefsGroupBox);

    layout->addStretch();

    // Initialize.

    widgetUpdate();

    // Setup the callbacks.

    connect(
        _p->frameStoreFileReloadWidget,
        SIGNAL(toggled(bool)),
        SLOT(frameStoreFileReloadCallback(bool)));

    connect(
        _p->mirrorHWidget,
        SIGNAL(toggled(bool)),
        SLOT(mirrorHCallback(bool)));

    connect(
        _p->mirrorVWidget,
        SIGNAL(toggled(bool)),
        SLOT(mirrorVCallback(bool)));

    connect(
        _p->scaleWidget,
        SIGNAL(activated(int)),
        SLOT(scaleCallback(int)));

    connect(
        _p->rotateWidget,
        SIGNAL(activated(int)),
        SLOT(rotateCallback(int)));

    connect(
        _p->colorProfileWidget,
        SIGNAL(toggled(bool)),
        SLOT(colorProfileCallback(bool)));

    connect(
        _p->displayProfileWidget,
        SIGNAL(activated(int)),
        SLOT(displayProfileCallback(int)));

    connect(
        _p->displayProfileListWidget,
        SIGNAL(itemChanged(QListWidgetItem *)),
        SLOT(displayProfileCallback(QListWidgetItem *)));

    connect(
        addDisplayProfileButton,
        SIGNAL(clicked()),
        SLOT(addDisplayProfileCallback()));

    connect(
        removeDisplayProfileButton,
        SIGNAL(clicked()),
        SLOT(removeDisplayProfileCallback()));

    connect(
        moveDisplayProfileUpButton,
        SIGNAL(clicked()),
        SLOT(moveDisplayProfileUpCallback()));

    connect(
        moveDisplayProfileDownButton,
        SIGNAL(clicked()),
        SLOT(moveDisplayProfileDownCallback()));

    connect(
        _p->channelWidget,
        SIGNAL(activated(int)),
        SLOT(channelCallback(int)));

    connect(
        djvViewImagePrefs::global(),
        SIGNAL(prefChanged()),
        SLOT(widgetUpdate()));
}

djvViewImagePrefsWidget::~djvViewImagePrefsWidget()
{
    delete _p;
}

void djvViewImagePrefsWidget::resetPreferences()
{
    djvViewImagePrefs::global()->setFrameStoreFileReload(
        djvViewImagePrefs::frameStoreFileReloadDefault());
    djvViewImagePrefs::global()->setMirror(
        djvViewImagePrefs::mirrorDefault());
    djvViewImagePrefs::global()->setScale(
        djvViewImagePrefs::scaleDefault());
    djvViewImagePrefs::global()->setRotate(
        djvViewImagePrefs::rotateDefault());
    djvViewImagePrefs::global()->setColorProfile(
        djvViewImagePrefs::colorProfileDefault());
    djvViewImagePrefs::global()->setDisplayProfileIndex(
        djvViewImagePrefs::displayProfileIndexDefault());
    djvViewImagePrefs::global()->setChannel(
        djvViewImagePrefs::channelDefault());
    
    widgetUpdate();
}

void djvViewImagePrefsWidget::frameStoreFileReloadCallback(bool in)
{
    djvViewImagePrefs::global()->setFrameStoreFileReload(in);
}

void djvViewImagePrefsWidget::mirrorHCallback(bool in)
{
    djvPixelDataInfo::Mirror mirror = djvViewImagePrefs::global()->mirror();
    mirror.x = in;
    djvViewImagePrefs::global()->setMirror(mirror);
}

void djvViewImagePrefsWidget::mirrorVCallback(bool in)
{
    djvPixelDataInfo::Mirror mirror = djvViewImagePrefs::global()->mirror();
    mirror.y = in;
    djvViewImagePrefs::global()->setMirror(mirror);
}

void djvViewImagePrefsWidget::scaleCallback(int in)
{
    djvViewImagePrefs::global()->setScale(
        static_cast<djvViewUtil::IMAGE_SCALE>(in));
}

void djvViewImagePrefsWidget::rotateCallback(int in)
{
    djvViewImagePrefs::global()->setRotate(
        static_cast<djvViewUtil::IMAGE_ROTATE>(in));
}

void djvViewImagePrefsWidget::colorProfileCallback(bool in)
{
    djvViewImagePrefs::global()->setColorProfile(in);
}

void djvViewImagePrefsWidget::displayProfileCallback(int in)
{
    djvViewImagePrefs::global()->setDisplayProfileIndex(in - 1);
}

void djvViewImagePrefsWidget::displayProfileCallback(QListWidgetItem * item)
{
    QVector<djvViewDisplayProfile> displayProfiles =
        djvViewImagePrefs::global()->displayProfiles();

    const int index = _p->displayProfileListWidget->row(item);

    const QString name = item->data(Qt::EditRole).toString();

    displayProfiles[index].name = name;

    djvViewImagePrefs::global()->setDisplayProfiles(displayProfiles);
}

void djvViewImagePrefsWidget::addDisplayProfileCallback()
{
    djvInputDialog dialog(tr("Add display profile:"));

    if (QDialog::Accepted == dialog.exec())
    {
        QVector<djvViewDisplayProfile> displayProfiles =
            djvViewImagePrefs::global()->displayProfiles();

        djvViewDisplayProfile displayProfile;
        displayProfile.name = dialog.text();
        displayProfiles += displayProfile;

        djvViewImagePrefs::global()->setDisplayProfiles(displayProfiles);
    }
}

void djvViewImagePrefsWidget::removeDisplayProfileCallback()
{
    int index = _p->displayProfileListWidget->currentRow();

    QVector<djvViewDisplayProfile> displayProfiles =
        djvViewImagePrefs::global()->displayProfiles();

    if (-1 == index)
        index = displayProfiles.count() - 1;

    if (index != -1)
    {
        displayProfiles.remove(index);

        djvViewImagePrefs::global()->setDisplayProfiles(displayProfiles);

        _p->displayProfileListWidget->setCurrentRow(
            index >= displayProfiles.count() ? (index - 1) : index);
    }
}

void djvViewImagePrefsWidget::moveDisplayProfileUpCallback()
{
    int index = _p->displayProfileListWidget->currentRow();

    if (index != -1)
    {
        QVector<djvViewDisplayProfile> displayProfiles =
            djvViewImagePrefs::global()->displayProfiles();

        djvViewDisplayProfile displayProfile = displayProfiles[index];

        displayProfiles.remove(index);

        if (index > 0)
            --index;

        displayProfiles.insert(index, displayProfile);

        djvViewImagePrefs::global()->setDisplayProfiles(displayProfiles);

        _p->displayProfileListWidget->setCurrentRow(index);
    }
}

void djvViewImagePrefsWidget::moveDisplayProfileDownCallback()
{
    int index = _p->displayProfileListWidget->currentRow();

    if (index != -1)
    {
        QVector<djvViewDisplayProfile> displayProfiles =
            djvViewImagePrefs::global()->displayProfiles();

        djvViewDisplayProfile displayProfile = displayProfiles[index];

        displayProfiles.remove(index);

        if (index < displayProfiles.count())
            ++index;

        displayProfiles.insert(index, displayProfile);

        djvViewImagePrefs::global()->setDisplayProfiles(displayProfiles);

        _p->displayProfileListWidget->setCurrentRow(index);
    }
}

void djvViewImagePrefsWidget::channelCallback(int in)
{
    djvViewImagePrefs::global()->setChannel(
        static_cast<djvOpenGlImageOptions::CHANNEL>(in));
}

void djvViewImagePrefsWidget::widgetUpdate()
{
    //DJV_DEBUG("djvViewImagePrefsWidget::widgetUpdate");
    
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->frameStoreFileReloadWidget <<
        _p->mirrorHWidget <<
        _p->mirrorVWidget <<
        _p->scaleWidget <<
        _p->rotateWidget <<
        _p->colorProfileWidget <<
        _p->displayProfileWidget <<
        _p->displayProfileListWidget <<
        _p->channelWidget);
    
    _p->frameStoreFileReloadWidget->setChecked(
        djvViewImagePrefs::global()->hasFrameStoreFileReload());
    
    _p->mirrorHWidget->setChecked(
        djvViewImagePrefs::global()->mirror().x);

    _p->mirrorVWidget->setChecked(
        djvViewImagePrefs::global()->mirror().y);

    _p->scaleWidget->setCurrentIndex(
        djvViewImagePrefs::global()->scale());
    
    _p->rotateWidget->setCurrentIndex(
        djvViewImagePrefs::global()->rotate());
    
    _p->colorProfileWidget->setChecked(
        djvViewImagePrefs::global()->hasColorProfile());
    
    _p->displayProfileWidget->setCurrentIndex(
        djvViewImagePrefs::global()->displayProfileIndex() + 1);

    _p->displayProfileWidget->clear();
    _p->displayProfileWidget->addItems(QStringList() <<
        tr("None") <<
        djvViewImagePrefs::global()->displayProfileNames());
    _p->displayProfileWidget->setCurrentIndex(
        djvViewImagePrefs::global()->displayProfileIndex() + 1);

    _p->displayProfileListWidget->clear();
    const QVector<djvViewDisplayProfile> & displayProfiles =
        djvViewImagePrefs::global()->displayProfiles();
    for (int i = 0; i < displayProfiles.count(); ++i)
    {
        QListWidgetItem * item = new QListWidgetItem(_p->displayProfileListWidget);
        item->setFlags(
            Qt::ItemIsSelectable |
            Qt::ItemIsEditable |
            Qt::ItemIsEnabled);
        item->setData(Qt::DisplayRole, displayProfiles[i].name);
        item->setData(Qt::EditRole, displayProfiles[i].name);
    }

    _p->channelWidget->setCurrentIndex(
        djvViewImagePrefs::global()->channel());
}
