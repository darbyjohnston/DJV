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

//! \file djvFileBrowserPrefsWidget.cpp

#include <djvFileBrowserPrefsWidget.h>

#include <djvFileBrowserPrefs.h>
#include <djvIconLibrary.h>
#include <djvIntEdit.h>
#include <djvInputDialog.h>
#include <djvPrefsGroupBox.h>
#include <djvShortcutsWidget.h>
#include <djvStyle.h>
#include <djvToolButton.h>

#include <djvMemory.h>
#include <djvSignalBlocker.h>

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
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
// djvFileBrowserPrefsWidget::P
//------------------------------------------------------------------------------

struct djvFileBrowserPrefsWidget::P
{
    P() :
        seqWidget            (0),
        showHiddenWidget     (0),
        sortWidget           (0),
        reverseSortWidget    (0),
        sortDirsFirstWidget  (0),
        thumbnailsWidget     (0),
        thumbnailsSizeWidget (0),
        thumbnailsCacheWidget(0),
        bookmarksWidget      (0),
        shortcutsWidget      (0)
    {}
    
    QComboBox *          seqWidget;
    QCheckBox *          showHiddenWidget;
    QComboBox *          sortWidget;
    QCheckBox *          reverseSortWidget;
    QCheckBox *          sortDirsFirstWidget;
    QComboBox *          thumbnailsWidget;
    QComboBox *          thumbnailsSizeWidget;
    djvIntEdit *         thumbnailsCacheWidget;
    QListWidget *        bookmarksWidget;
    djvShortcutsWidget * shortcutsWidget;
};

//------------------------------------------------------------------------------
// djvFileBrowserPrefsWidget
//------------------------------------------------------------------------------

djvFileBrowserPrefsWidget::djvFileBrowserPrefsWidget() :
    djvAbstractPrefsWidget(tr("File Browser")),
    _p(new P)
{
    // Create the widgets.

    _p->seqWidget = new QComboBox;
    _p->seqWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->seqWidget->addItems(djvSequence::compressLabels());

    _p->showHiddenWidget = new QCheckBox(tr("Show hidden files"));

    _p->sortWidget = new QComboBox;
    _p->sortWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->sortWidget->addItems(djvFileBrowserModel::columnsLabels());

    _p->reverseSortWidget = new QCheckBox(tr("Reverse the sort direction"));

    _p->sortDirsFirstWidget = new QCheckBox(tr("Sort directories first"));

    _p->thumbnailsWidget = new QComboBox;
    _p->thumbnailsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->thumbnailsWidget->addItems(djvFileBrowserModel::thumbnailsLabels());

    _p->thumbnailsSizeWidget = new QComboBox;
    _p->thumbnailsSizeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->thumbnailsSizeWidget->addItems(djvFileBrowserModel::thumbnailsSizeLabels());

    _p->thumbnailsCacheWidget = new djvIntEdit;
    _p->thumbnailsCacheWidget->setRange(0, 4096);
    _p->thumbnailsCacheWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _p->bookmarksWidget = new SmallListWidget;
    
    djvToolButton * addBookmarkButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvAddIcon.png"));
    addBookmarkButton->setToolTip(tr("Add a new bookmark"));
    
    djvToolButton * removeBookmarkButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvRemoveIcon.png"));
    removeBookmarkButton->setAutoRepeat(true);
    removeBookmarkButton->setToolTip(tr("Remove the selected bookmark"));
    
    djvToolButton * moveBookmarkUpButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvUpIcon.png"));
    moveBookmarkUpButton->setAutoRepeat(true);
    moveBookmarkUpButton->setToolTip(tr("Move the selected bookmark up"));
    
    djvToolButton * moveBookmarkDownButton = new djvToolButton(
        djvIconLibrary::global()->icon("djvDownIcon.png"));
    moveBookmarkDownButton->setAutoRepeat(true);
    moveBookmarkDownButton->setToolTip(tr("Move the selected bookmark down"));

    _p->shortcutsWidget = new djvShortcutsWidget;

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(tr("General"));
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(tr("File sequencing:"), _p->seqWidget);
    formLayout->addRow(_p->showHiddenWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Sorting"));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(tr("Sort by:"), _p->sortWidget);
    formLayout->addRow(_p->reverseSortWidget);
    formLayout->addRow(_p->sortDirsFirstWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Thumbnails"));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->thumbnailsWidget);
    formLayout->addRow(tr("Size:"), _p->thumbnailsSizeWidget);
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->addWidget(_p->thumbnailsCacheWidget);
    hLayout->addWidget(new QLabel(tr("(MB)")));
    formLayout->addRow(tr("Cache size:"), hLayout);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Bookmarks"));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->bookmarksWidget);
    hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(addBookmarkButton);
    hLayout->addWidget(removeBookmarkButton);
    hLayout->addWidget(moveBookmarkUpButton);
    hLayout->addWidget(moveBookmarkDownButton);
    formLayout->addRow(hLayout);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(tr("Keyboard Shortcuts"));
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(_p->shortcutsWidget);
    layout->addWidget(prefsGroupBox);
    
    // Initialize.
    
    widgetUpdate();

    // Setup the callbacks.

    connect(
        _p->seqWidget,
        SIGNAL(activated(int)),
        SLOT(seqCallback(int)));

    connect(
        _p->showHiddenWidget,
        SIGNAL(toggled(bool)),
        djvFileBrowserPrefs::global(),
        SLOT(setShowHidden(bool)));

    connect(
        _p->sortWidget,
        SIGNAL(activated(int)),
        SLOT(sortCallback(int)));

    connect(
        _p->reverseSortWidget,
        SIGNAL(toggled(bool)),
        djvFileBrowserPrefs::global(),
        SLOT(setReverseSort(bool)));

    connect(
        _p->sortDirsFirstWidget,
        SIGNAL(toggled(bool)),
        djvFileBrowserPrefs::global(),
        SLOT(setSortDirsFirst(bool)));

    connect(
        _p->thumbnailsWidget,
        SIGNAL(activated(int)),
        SLOT(thumbnailsCallback(int)));

    connect(
        _p->thumbnailsSizeWidget,
        SIGNAL(activated(int)),
        SLOT(thumbnailsSizeCallback(int)));

    connect(
        _p->thumbnailsCacheWidget,
        SIGNAL(valueChanged(int)),
        SLOT(thumbnailsCacheCallback(int)));

    connect(
        _p->bookmarksWidget,
        SIGNAL(itemChanged(QListWidgetItem *)),
        SLOT(bookmarkCallback(QListWidgetItem *)));

    connect(
        addBookmarkButton,
        SIGNAL(clicked()),
        SLOT(addBookmarkCallback()));

    connect(
        removeBookmarkButton,
        SIGNAL(clicked()),
        SLOT(removeBookmarkCallback()));

    connect(
        moveBookmarkUpButton,
        SIGNAL(clicked()),
        SLOT(moveBookmarkUpCallback()));

    connect(
        moveBookmarkDownButton,
        SIGNAL(clicked()),
        SLOT(moveBookmarkDownCallback()));

    connect(
        _p->shortcutsWidget,
        SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
        SLOT(shortcutsCallback(const QVector<djvShortcut> &)));

    connect(
        djvFileBrowserPrefs::global(),
        SIGNAL(prefChanged()),
        SLOT(widgetUpdate()));
}

djvFileBrowserPrefsWidget::~djvFileBrowserPrefsWidget()
{
    delete _p;
}

void djvFileBrowserPrefsWidget::resetPreferences()
{
    djvFileBrowserPrefs::global()->setSequence(
        djvFileBrowserPrefs::sequenceDefault());

    djvFileBrowserPrefs::global()->setShowHidden(
        djvFileBrowserPrefs::showHiddenDefault());

    djvFileBrowserPrefs::global()->setSort(
        djvFileBrowserPrefs::sortDefault());

    djvFileBrowserPrefs::global()->setReverseSort(
        djvFileBrowserPrefs::reverseSortDefault());

    djvFileBrowserPrefs::global()->setSortDirsFirst(
        djvFileBrowserPrefs::sortDirsFirstDefault());

    djvFileBrowserPrefs::global()->setThumbnails(
        djvFileBrowserPrefs::thumbnailsDefault());

    djvFileBrowserPrefs::global()->setThumbnailsSize(
        djvFileBrowserPrefs::thumbnailsSizeDefault());

    djvFileBrowserPrefs::global()->setThumbnailsCache(
        djvFileBrowserPrefs::thumbnailsCacheDefault());

    djvFileBrowserPrefs::global()->setShortcuts(
        djvFileBrowserPrefs::shortcutsDefault());
    
    widgetUpdate();
}

void djvFileBrowserPrefsWidget::seqCallback(int index)
{
    djvFileBrowserPrefs::global()->setSequence(
        static_cast<djvSequence::COMPRESS>(index));
}

void djvFileBrowserPrefsWidget::sortCallback(int index)
{
    djvFileBrowserPrefs::global()->setSort(
        static_cast<djvFileBrowserModel::COLUMNS>(index));
}

void djvFileBrowserPrefsWidget::thumbnailsCallback(int index)
{
    djvFileBrowserPrefs::global()->setThumbnails(
        static_cast<djvFileBrowserModel::THUMBNAILS>(index));
}

void djvFileBrowserPrefsWidget::thumbnailsSizeCallback(int index)
{
    djvFileBrowserPrefs::global()->setThumbnailsSize(
        static_cast<djvFileBrowserModel::THUMBNAILS_SIZE>(index));
}

void djvFileBrowserPrefsWidget::thumbnailsCacheCallback(int value)
{
    djvFileBrowserPrefs::global()->setThumbnailsCache(
        value * djvMemory::megabyte);
}

void djvFileBrowserPrefsWidget::bookmarkCallback(QListWidgetItem * item)
{
    QStringList bookmarks = djvFileBrowserPrefs::global()->bookmarks();

    bookmarks[_p->bookmarksWidget->row(item)] =
        item->data(Qt::EditRole).toString();

    djvFileBrowserPrefs::global()->setBookmarks(bookmarks);
}

void djvFileBrowserPrefsWidget::addBookmarkCallback()
{
    djvInputDialog dialog(tr("Add bookmark:"));

    if (QDialog::Accepted == dialog.exec())
    {
        djvFileBrowserPrefs::global()->addBookmark(dialog.text());
    }
}

void djvFileBrowserPrefsWidget::removeBookmarkCallback()
{
    int index = _p->bookmarksWidget->currentRow();

    QStringList bookmarks = djvFileBrowserPrefs::global()->bookmarks();

    if (-1 == index)
        index = bookmarks.count() - 1;

    if (index != -1)
    {
        bookmarks.removeAt(index);

        djvFileBrowserPrefs::global()->setBookmarks(bookmarks);

        _p->bookmarksWidget->setCurrentRow(
            index >= bookmarks.count() ? (index - 1) : index);
    }
}

void djvFileBrowserPrefsWidget::moveBookmarkUpCallback()
{
    int index = _p->bookmarksWidget->currentRow();

    if (index != -1)
    {
        QStringList bookmarks = djvFileBrowserPrefs::global()->bookmarks();

        QString bookmark = bookmarks[index];

        bookmarks.removeAt(index);

        if (index > 0)
            --index;
        
        bookmarks.insert(index, bookmark);

        djvFileBrowserPrefs::global()->setBookmarks(bookmarks);

        _p->bookmarksWidget->setCurrentRow(index);
    }
}

void djvFileBrowserPrefsWidget::moveBookmarkDownCallback()
{
    int index = _p->bookmarksWidget->currentRow();

    if (index != -1)
    {
        QStringList bookmarks = djvFileBrowserPrefs::global()->bookmarks();

        QString bookmark = bookmarks[index];

        bookmarks.removeAt(index);

        if (index < bookmarks.count())
            ++index;

        bookmarks.insert(index, bookmark);

        djvFileBrowserPrefs::global()->setBookmarks(bookmarks);

        _p->bookmarksWidget->setCurrentRow(index);
    }
}

void djvFileBrowserPrefsWidget::shortcutsCallback(const QVector<djvShortcut> & in)
{
    djvFileBrowserPrefs::global()->setShortcuts(in);
}

void djvFileBrowserPrefsWidget::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->seqWidget <<
        _p->showHiddenWidget <<
        _p->sortWidget <<
        _p->reverseSortWidget <<
        _p->sortDirsFirstWidget <<
        _p->thumbnailsWidget <<
        _p->thumbnailsSizeWidget <<
        _p->thumbnailsCacheWidget <<
        _p->bookmarksWidget <<
        _p->shortcutsWidget);

    _p->seqWidget->setCurrentIndex(
        djvFileBrowserPrefs::global()->sequence());

    _p->showHiddenWidget->setChecked(
        djvFileBrowserPrefs::global()->hasShowHidden());

    _p->sortWidget->setCurrentIndex(
        djvFileBrowserPrefs::global()->sort());

    _p->reverseSortWidget->setChecked(
        djvFileBrowserPrefs::global()->hasReverseSort());

    _p->sortDirsFirstWidget->setChecked(
        djvFileBrowserPrefs::global()->hasSortDirsFirst());

    _p->thumbnailsWidget->setCurrentIndex(
        djvFileBrowserPrefs::global()->thumbnails());

    _p->thumbnailsSizeWidget->setCurrentIndex(
        djvFileBrowserPrefs::global()->thumbnailsSize());

    _p->thumbnailsCacheWidget->setValue(
        djvFileBrowserPrefs::global()->thumbnailsCache() / djvMemory::megabyte);

    _p->bookmarksWidget->clear();
    const QStringList & bookmarks = djvFileBrowserPrefs::global()->bookmarks();
    for (int i = 0; i < bookmarks.count(); ++i)
    {
        QListWidgetItem * item = new QListWidgetItem(_p->bookmarksWidget);
        item->setFlags(
            Qt::ItemIsSelectable |
            Qt::ItemIsEditable |
            Qt::ItemIsEnabled);
        item->setData(Qt::DisplayRole, bookmarks[i]);
        item->setData(Qt::EditRole, bookmarks[i]);
    }

    _p->shortcutsWidget->setShortcuts(
        djvFileBrowserPrefs::global()->shortcuts());
}
