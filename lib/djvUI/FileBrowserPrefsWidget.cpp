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

#include <djvUI/FileBrowserPrefsWidget.h>

#include <djvUI/FileBrowserPrefs.h>
#include <djvUI/UIContext.h>
#include <djvUI/IconLibrary.h>
#include <djvUI/IntEdit.h>
#include <djvUI/InputDialog.h>
#include <djvUI/PrefsGroupBox.h>
#include <djvUI/ShortcutsWidget.h>
#include <djvUI/Style.h>
#include <djvUI/ToolButton.h>

#include <djvCore/Memory.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
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

        struct FileBrowserPrefsWidget::Private
        {
            QComboBox * seqWidget = nullptr;
            QCheckBox * showHiddenWidget = nullptr;
            QComboBox * sortWidget = nullptr;
            QCheckBox * reverseSortWidget = nullptr;
            QCheckBox * sortDirsFirstWidget = nullptr;
            QComboBox * thumbnailsWidget = nullptr;
            QComboBox * thumbnailsSizeWidget = nullptr;
            IntEdit * thumbnailsCacheWidget = nullptr;
            QListWidget * bookmarksWidget = nullptr;
            ShortcutsWidget * shortcutsWidget = nullptr;
        };

        FileBrowserPrefsWidget::FileBrowserPrefsWidget(
            UIContext * context,
            QWidget * parent) :
            AbstractPrefsWidget(qApp->translate("djv::UI::FileBrowserPrefsWidget", "File Browser"), context, parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->seqWidget = new QComboBox;
            _p->seqWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->seqWidget->addItems(Core::Sequence::compressLabels());

            _p->showHiddenWidget = new QCheckBox(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Show hidden files"));

            _p->sortWidget = new QComboBox;
            _p->sortWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->sortWidget->addItems(FileBrowserModel::columnsLabels());

            _p->reverseSortWidget = new QCheckBox(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Reverse the sort direction"));

            _p->sortDirsFirstWidget = new QCheckBox(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Sort directories first"));

            _p->thumbnailsWidget = new QComboBox;
            _p->thumbnailsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->thumbnailsWidget->addItems(FileBrowserModel::thumbnailsLabels());

            _p->thumbnailsSizeWidget = new QComboBox;
            _p->thumbnailsSizeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->thumbnailsSizeWidget->addItems(FileBrowserModel::thumbnailsSizeLabels());

            _p->thumbnailsCacheWidget = new IntEdit;
            _p->thumbnailsCacheWidget->setRange(0, 4096);
            _p->thumbnailsCacheWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            _p->bookmarksWidget = new SmallListWidget;

            ToolButton * addBookmarkButton = new ToolButton(
                context->iconLibrary()->icon("djvAddIcon.png"));
            addBookmarkButton->setToolTip(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Add a new bookmark"));

            ToolButton * removeBookmarkButton = new ToolButton(
                context->iconLibrary()->icon("djvRemoveIcon.png"));
            removeBookmarkButton->setAutoRepeat(true);
            removeBookmarkButton->setToolTip(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Remove the selected bookmark"));

            ToolButton * moveBookmarkUpButton = new ToolButton(
                context->iconLibrary()->icon("djvUpIcon.png"));
            moveBookmarkUpButton->setAutoRepeat(true);
            moveBookmarkUpButton->setToolTip(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Move the selected bookmark up"));

            ToolButton * moveBookmarkDownButton = new ToolButton(
                context->iconLibrary()->icon("djvDownIcon.png"));
            moveBookmarkDownButton->setAutoRepeat(true);
            moveBookmarkDownButton->setToolTip(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Move the selected bookmark down"));

            _p->shortcutsWidget = new ShortcutsWidget(context);

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);
            layout->setSpacing(context->style()->sizeMetric().largeSpacing);

            PrefsGroupBox * prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Files"), context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "File sequencing:"),
                _p->seqWidget);
            formLayout->addRow(_p->showHiddenWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Sorting"), context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Sort by:"),
                _p->sortWidget);
            formLayout->addRow(_p->reverseSortWidget);
            formLayout->addRow(_p->sortDirsFirstWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Thumbnails"), context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Thumbnails:"),
                _p->thumbnailsWidget);
            formLayout->addRow(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Size:"),
                _p->thumbnailsSizeWidget);
            QHBoxLayout * hLayout = new QHBoxLayout;
            hLayout->addWidget(_p->thumbnailsCacheWidget);
            hLayout->addWidget(
                new QLabel(qApp->translate("djv::UI::FileBrowserPrefsWidget", "(MB)")));
            formLayout->addRow(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Cache size:"),
                hLayout);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Bookmarks"), context);
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

            prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::FileBrowserPrefsWidget", "Keyboard Shortcuts"), context);
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
                SIGNAL(shortcutsChanged(const QVector<Shortcut> &)),
                SLOT(shortcutsCallback(const QVector<Shortcut> &)));
            connect(
                context->fileBrowserPrefs(),
                SIGNAL(prefChanged()),
                SLOT(widgetUpdate()));
        }

        FileBrowserPrefsWidget::~FileBrowserPrefsWidget()
        {}

        void FileBrowserPrefsWidget::resetPreferences()
        {
            context()->fileBrowserPrefs()->setSequence(FileBrowserPrefs::sequenceDefault());
            context()->fileBrowserPrefs()->setShowHidden(FileBrowserPrefs::showHiddenDefault());
            context()->fileBrowserPrefs()->setColumnsSort(FileBrowserPrefs::columnsSortDefault());
            context()->fileBrowserPrefs()->setReverseSort(FileBrowserPrefs::reverseSortDefault());
            context()->fileBrowserPrefs()->setSortDirsFirst(FileBrowserPrefs::sortDirsFirstDefault());
            context()->fileBrowserPrefs()->setThumbnails(FileBrowserPrefs::thumbnailsDefault());
            context()->fileBrowserPrefs()->setThumbnailsSize(FileBrowserPrefs::thumbnailsSizeDefault());
            context()->fileBrowserPrefs()->setThumbnailsCache(FileBrowserPrefs::thumbnailsCacheDefault());
            context()->fileBrowserPrefs()->setShortcuts(FileBrowserPrefs::shortcutsDefault());
            widgetUpdate();
        }

        void FileBrowserPrefsWidget::seqCallback(int index)
        {
            context()->fileBrowserPrefs()->setSequence(static_cast<Core::Sequence::COMPRESS>(index));
        }

        void FileBrowserPrefsWidget::sortCallback(int index)
        {
            context()->fileBrowserPrefs()->setColumnsSort(static_cast<FileBrowserModel::COLUMNS>(index));
        }

        void FileBrowserPrefsWidget::thumbnailsCallback(int index)
        {
            context()->fileBrowserPrefs()->setThumbnails(static_cast<FileBrowserModel::THUMBNAILS>(index));
        }

        void FileBrowserPrefsWidget::thumbnailsSizeCallback(int index)
        {
            context()->fileBrowserPrefs()->setThumbnailsSize(static_cast<FileBrowserModel::THUMBNAILS_SIZE>(index));
        }

        void FileBrowserPrefsWidget::thumbnailsCacheCallback(int value)
        {
            context()->fileBrowserPrefs()->setThumbnailsCache(value * Core::Memory::megabyte);
        }

        void FileBrowserPrefsWidget::bookmarkCallback(QListWidgetItem * item)
        {
            QStringList bookmarks = context()->fileBrowserPrefs()->bookmarks();
            bookmarks[_p->bookmarksWidget->row(item)] = item->data(Qt::EditRole).toString();
            context()->fileBrowserPrefs()->setBookmarks(bookmarks);
        }

        void FileBrowserPrefsWidget::addBookmarkCallback()
        {
            InputDialog dialog(qApp->translate("djv::UI::FileBrowserPrefsWidget", "Add bookmark:"));
            if (QDialog::Accepted == dialog.exec())
            {
                context()->fileBrowserPrefs()->addBookmark(dialog.text());
            }
        }

        void FileBrowserPrefsWidget::removeBookmarkCallback()
        {
            int index = _p->bookmarksWidget->currentRow();
            QStringList bookmarks = context()->fileBrowserPrefs()->bookmarks();
            if (-1 == index)
                index = bookmarks.count() - 1;
            if (index != -1)
            {
                bookmarks.removeAt(index);
                context()->fileBrowserPrefs()->setBookmarks(bookmarks);
                _p->bookmarksWidget->setCurrentRow(index >= bookmarks.count() ? (index - 1) : index);
            }
        }

        void FileBrowserPrefsWidget::moveBookmarkUpCallback()
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

        void FileBrowserPrefsWidget::moveBookmarkDownCallback()
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

        void FileBrowserPrefsWidget::shortcutsCallback(const QVector<Shortcut> & in)
        {
            context()->fileBrowserPrefs()->setShortcuts(in);
        }

        void FileBrowserPrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
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
            _p->seqWidget->setCurrentIndex(context()->fileBrowserPrefs()->sequence());
            _p->showHiddenWidget->setChecked(context()->fileBrowserPrefs()->hasShowHidden());
            _p->sortWidget->setCurrentIndex(context()->fileBrowserPrefs()->columnsSort());
            _p->reverseSortWidget->setChecked(context()->fileBrowserPrefs()->hasReverseSort());
            _p->sortDirsFirstWidget->setChecked(context()->fileBrowserPrefs()->hasSortDirsFirst());
            _p->thumbnailsWidget->setCurrentIndex(context()->fileBrowserPrefs()->thumbnails());
            _p->thumbnailsSizeWidget->setCurrentIndex(context()->fileBrowserPrefs()->thumbnailsSize());
            _p->thumbnailsCacheWidget->setValue(context()->fileBrowserPrefs()->thumbnailsCache() / Core::Memory::megabyte);
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
            _p->shortcutsWidget->setShortcuts(context()->fileBrowserPrefs()->shortcuts());
        }

    } // namespace UI
} // namespace djv
