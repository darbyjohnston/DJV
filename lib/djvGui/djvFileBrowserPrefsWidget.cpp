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

#include <djvFileBrowserPrefsWidget.h>

#include <djvFileBrowserPrefs.h>
#include <djvGuiContext.h>
#include <djvIconLibrary.h>
#include <djvIntEdit.h>
#include <djvInputDialog.h>
#include <djvPrefsGroupBox.h>
#include <djvShortcutsWidget.h>
#include <djvStyle.h>
#include <djvToolButton.h>

#include <djvMemory.h>
#include <djvSignalBlocker.h>

#include <QApplication>
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
// djvFileBrowserPrefsWidget::Private
//------------------------------------------------------------------------------

struct djvFileBrowserPrefsWidget::Private
{
    QComboBox *          seqWidget             = nullptr;
    QCheckBox *          showHiddenWidget      = nullptr;
    QComboBox *          sortWidget            = nullptr;
    QCheckBox *          reverseSortWidget     = nullptr;
    QCheckBox *          sortDirsFirstWidget   = nullptr;
    QComboBox *          thumbnailsWidget      = nullptr;
    QComboBox *          thumbnailsSizeWidget  = nullptr;
    djvIntEdit *         thumbnailsCacheWidget = nullptr;
    QListWidget *        bookmarksWidget       = nullptr;
    djvShortcutsWidget * shortcutsWidget       = nullptr;
};

//------------------------------------------------------------------------------
// djvFileBrowserPrefsWidget
//------------------------------------------------------------------------------

djvFileBrowserPrefsWidget::djvFileBrowserPrefsWidget(
    djvGuiContext * context,
    QWidget *       parent) :
    djvAbstractPrefsWidget(
        qApp->translate("djvFileBrowserPrefsWidget", "File Browser"), context, parent),
    _p(new Private)
{
    // Create the widgets.
    _p->seqWidget = new QComboBox;
    _p->seqWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->seqWidget->addItems(djvSequence::compressLabels());

    _p->showHiddenWidget = new QCheckBox(
        qApp->translate("djvFileBrowserPrefsWidget", "Show hidden files"));

    _p->sortWidget = new QComboBox;
    _p->sortWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _p->sortWidget->addItems(djvFileBrowserModel::columnsLabels());

    _p->reverseSortWidget = new QCheckBox(
        qApp->translate("djvFileBrowserPrefsWidget", "Reverse the sort direction"));

    _p->sortDirsFirstWidget = new QCheckBox(
        qApp->translate("djvFileBrowserPrefsWidget", "Sort directories first"));

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
        context->iconLibrary()->icon("djvAddIcon.png"));
    addBookmarkButton->setToolTip(
        qApp->translate("djvFileBrowserPrefsWidget", "Add a new bookmark"));
    
    djvToolButton * removeBookmarkButton = new djvToolButton(
        context->iconLibrary()->icon("djvRemoveIcon.png"));
    removeBookmarkButton->setAutoRepeat(true);
    removeBookmarkButton->setToolTip(
        qApp->translate("djvFileBrowserPrefsWidget", "Remove the selected bookmark"));
    
    djvToolButton * moveBookmarkUpButton = new djvToolButton(
        context->iconLibrary()->icon("djvUpIcon.png"));
    moveBookmarkUpButton->setAutoRepeat(true);
    moveBookmarkUpButton->setToolTip(
        qApp->translate("djvFileBrowserPrefsWidget", "Move the selected bookmark up"));
    
    djvToolButton * moveBookmarkDownButton = new djvToolButton(
        context->iconLibrary()->icon("djvDownIcon.png"));
    moveBookmarkDownButton->setAutoRepeat(true);
    moveBookmarkDownButton->setToolTip(
        qApp->translate("djvFileBrowserPrefsWidget", "Move the selected bookmark down"));

    _p->shortcutsWidget = new djvShortcutsWidget(context);

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setSpacing(context->style()->sizeMetric().largeSpacing);

    djvPrefsGroupBox * prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvFileBrowserPrefsWidget", "Files"), context);
    QFormLayout * formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvFileBrowserPrefsWidget", "File sequencing:"),
        _p->seqWidget);
    formLayout->addRow(_p->showHiddenWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvFileBrowserPrefsWidget", "Sorting"), context);
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvFileBrowserPrefsWidget", "Sort by:"),
        _p->sortWidget);
    formLayout->addRow(_p->reverseSortWidget);
    formLayout->addRow(_p->sortDirsFirstWidget);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvFileBrowserPrefsWidget", "Thumbnails"), context);
    formLayout = prefsGroupBox->createLayout();
    formLayout->addRow(
        qApp->translate("djvFileBrowserPrefsWidget", "Thumbnails:"),
        _p->thumbnailsWidget);
    formLayout->addRow(
        qApp->translate("djvFileBrowserPrefsWidget", "Size:"),
        _p->thumbnailsSizeWidget);
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->addWidget(_p->thumbnailsCacheWidget);
    hLayout->addWidget(
        new QLabel(qApp->translate("djvFileBrowserPrefsWidget", "(MB)")));
    formLayout->addRow(
        qApp->translate("djvFileBrowserPrefsWidget", "Cache size:"),
        hLayout);
    layout->addWidget(prefsGroupBox);

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvFileBrowserPrefsWidget", "Bookmarks"), context);
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

    prefsGroupBox = new djvPrefsGroupBox(
        qApp->translate("djvFileBrowserPrefsWidget", "Keyboard Shortcuts"), context);
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
        context->fileBrowserPrefs(),
        SLOT(setShowHidden(bool)));
    connect(
        _p->sortWidget,
        SIGNAL(activated(int)),
        SLOT(sortCallback(int)));
    connect(
        _p->reverseSortWidget,
        SIGNAL(toggled(bool)),
        context->fileBrowserPrefs(),
        SLOT(setReverseSort(bool)));
    connect(
        _p->sortDirsFirstWidget,
        SIGNAL(toggled(bool)),
        context->fileBrowserPrefs(),
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
        context->fileBrowserPrefs(),
        SIGNAL(prefChanged()),
        SLOT(widgetUpdate()));
}

djvFileBrowserPrefsWidget::~djvFileBrowserPrefsWidget()
{}

void djvFileBrowserPrefsWidget::resetPreferences()
{
    context()->fileBrowserPrefs()->setSequence(
        djvFileBrowserPrefs::sequenceDefault());
    context()->fileBrowserPrefs()->setShowHidden(
        djvFileBrowserPrefs::showHiddenDefault());
    context()->fileBrowserPrefs()->setSort(
        djvFileBrowserPrefs::sortDefault());
    context()->fileBrowserPrefs()->setReverseSort(
        djvFileBrowserPrefs::reverseSortDefault());
    context()->fileBrowserPrefs()->setSortDirsFirst(
        djvFileBrowserPrefs::sortDirsFirstDefault());
    context()->fileBrowserPrefs()->setThumbnails(
        djvFileBrowserPrefs::thumbnailsDefault());
    context()->fileBrowserPrefs()->setThumbnailsSize(
        djvFileBrowserPrefs::thumbnailsSizeDefault());
    context()->fileBrowserPrefs()->setThumbnailsCache(
        djvFileBrowserPrefs::thumbnailsCacheDefault());
    context()->fileBrowserPrefs()->setShortcuts(
        djvFileBrowserPrefs::shortcutsDefault());
    widgetUpdate();
}

void djvFileBrowserPrefsWidget::seqCallback(int index)
{
    context()->fileBrowserPrefs()->setSequence(
        static_cast<djvSequence::COMPRESS>(index));
}

void djvFileBrowserPrefsWidget::sortCallback(int index)
{
    context()->fileBrowserPrefs()->setSort(
        static_cast<djvFileBrowserModel::COLUMNS>(index));
}

void djvFileBrowserPrefsWidget::thumbnailsCallback(int index)
{
    context()->fileBrowserPrefs()->setThumbnails(
        static_cast<djvFileBrowserModel::THUMBNAILS>(index));
}

void djvFileBrowserPrefsWidget::thumbnailsSizeCallback(int index)
{
    context()->fileBrowserPrefs()->setThumbnailsSize(
        static_cast<djvFileBrowserModel::THUMBNAILS_SIZE>(index));
}

void djvFileBrowserPrefsWidget::thumbnailsCacheCallback(int value)
{
    context()->fileBrowserPrefs()->setThumbnailsCache(
        value * djvMemory::megabyte);
}

void djvFileBrowserPrefsWidget::bookmarkCallback(QListWidgetItem * item)
{
    QStringList bookmarks = context()->fileBrowserPrefs()->bookmarks();
    bookmarks[_p->bookmarksWidget->row(item)] =
        item->data(Qt::EditRole).toString();
    context()->fileBrowserPrefs()->setBookmarks(bookmarks);
}

void djvFileBrowserPrefsWidget::addBookmarkCallback()
{
    djvInputDialog dialog(
        qApp->translate("djvFileBrowserPrefsWidget", "Add bookmark:"));
    if (QDialog::Accepted == dialog.exec())
    {
        context()->fileBrowserPrefs()->addBookmark(dialog.text());
    }
}

void djvFileBrowserPrefsWidget::removeBookmarkCallback()
{
    int index = _p->bookmarksWidget->currentRow();
    QStringList bookmarks = context()->fileBrowserPrefs()->bookmarks();
    if (-1 == index)
        index = bookmarks.count() - 1;
    if (index != -1)
    {
        bookmarks.removeAt(index);
        context()->fileBrowserPrefs()->setBookmarks(bookmarks);
        _p->bookmarksWidget->setCurrentRow(
            index >= bookmarks.count() ? (index - 1) : index);
    }
}

void djvFileBrowserPrefsWidget::moveBookmarkUpCallback()
{
    int index = _p->bookmarksWidget->currentRow();
    if (index != -1)
    {
        QStringList bookmarks = context()->fileBrowserPrefs()->bookmarks();
        QString bookmark = bookmarks[index];
        bookmarks.removeAt(index);
        if (index > 0)
            --index;
        bookmarks.insert(index, bookmark);
        context()->fileBrowserPrefs()->setBookmarks(bookmarks);
        _p->bookmarksWidget->setCurrentRow(index);
    }
}

void djvFileBrowserPrefsWidget::moveBookmarkDownCallback()
{
    int index = _p->bookmarksWidget->currentRow();
    if (index != -1)
    {
        QStringList bookmarks = context()->fileBrowserPrefs()->bookmarks();
        QString bookmark = bookmarks[index];
        bookmarks.removeAt(index);
        if (index < bookmarks.count())
            ++index;
        bookmarks.insert(index, bookmark);
        context()->fileBrowserPrefs()->setBookmarks(bookmarks);
        _p->bookmarksWidget->setCurrentRow(index);
    }
}

void djvFileBrowserPrefsWidget::shortcutsCallback(const QVector<djvShortcut> & in)
{
    context()->fileBrowserPrefs()->setShortcuts(in);
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
        context()->fileBrowserPrefs()->sequence());
    _p->showHiddenWidget->setChecked(
        context()->fileBrowserPrefs()->hasShowHidden());
    _p->sortWidget->setCurrentIndex(
        context()->fileBrowserPrefs()->sort());
    _p->reverseSortWidget->setChecked(
        context()->fileBrowserPrefs()->hasReverseSort());
    _p->sortDirsFirstWidget->setChecked(
        context()->fileBrowserPrefs()->hasSortDirsFirst());
    _p->thumbnailsWidget->setCurrentIndex(
        context()->fileBrowserPrefs()->thumbnails());
    _p->thumbnailsSizeWidget->setCurrentIndex(
        context()->fileBrowserPrefs()->thumbnailsSize());
    _p->thumbnailsCacheWidget->setValue(
        context()->fileBrowserPrefs()->thumbnailsCache() / djvMemory::megabyte);
    _p->bookmarksWidget->clear();
    const QStringList & bookmarks = context()->fileBrowserPrefs()->bookmarks();
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
        context()->fileBrowserPrefs()->shortcuts());
}
