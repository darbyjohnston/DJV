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

//! \file djvFileBrowser.cpp

#include <djvFileBrowser.h>

#include <djvFileBrowserModel.h>
#include <djvFileBrowserPrefs.h>
#include <djvIndexShortcut.h>
#include <djvMultiChoiceDialog.h>

#include <djvOsxMenuHack.h>
#include <djvPrefs.h>
#include <djvQuestionDialog.h>
#include <djvSearchBox.h>
#include <djvStyle.h>
#include <djvToolButton.h>

#include <djvDebug.h>
#include <djvFileInfoUtil.h>
#include <djvIconLibrary.h>
#include <djvListUtil.h>
#include <djvSignalBlocker.h>
#include <djvSystem.h>

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QPointer>
#include <QPushButton>
#include <QResizeEvent>
#include <QShortcut>
#include <QTreeView>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvFileBrowser::P
//------------------------------------------------------------------------------

struct djvFileBrowser::P
{
    struct Menus
    {
        enum MENU
        {
            RECENT,
            DRIVES,
            THUMBNAILS,
            SEQ,
            SORT,
            BOOKMARKS
        };
        
        Menus() :
            menuBar(0)
        {}
    
        QMenuBar *         menuBar;
        QMap<int, QMenu *> menus;
    };
    
    struct Actions
    {
        enum ACTION
        {
            UP,
            PREV,
            CURRENT,
            HOME,
            DESKTOP,
            RELOAD,
            SHOW_HIDDEN,
            REVERSE_SORT,
            SORT_DIRS_FIRST
        };
        
        enum ACTION_GROUP
        {
            RECENT_GROUP,
            DRIVES_GROUP,
            THUMBNAILS_GROUP,
            SEQ_GROUP,
            SORT_GROUP,
            BOOKMARKS_GROUP
        };
        
        QMap<int, QAction *>      actions;
        QMap<int, QActionGroup *> groups;
    };
        
    struct Widgets
    {
        Widgets() :
            file   (0),
            up     (0),
            prev   (0),
            reload (0),
            seq    (0),
            search (0),
            browser(0),
            pinned (0)
        {}
        
        QLineEdit *     file;
        djvToolButton * up;
        djvToolButton * prev;
        djvToolButton * reload;
        QComboBox *     seq;
        djvSearchBox *  search;
        QTreeView *     browser;
        QCheckBox *     pinned;
    };
    
    P() :
        pinnable   (false),
        pinned     (false),
        shown      (false),
        model      (0),
        osxMenuHack(0)
    {}
    
    bool                  pinnable;
    bool                  pinned;
    bool                  shown;
    djvFileInfo           fileInfo;
    QString               prev;
    djvFileBrowserModel * model;
    Menus                 menus;
    Actions               actions;
    Widgets               widgets;
    djvOsxMenuHack *      osxMenuHack;
};

//------------------------------------------------------------------------------
// djvFileBrowser
//------------------------------------------------------------------------------

djvFileBrowser::djvFileBrowser(QWidget * parent) :
    QDialog(parent),
    _p(new P)
{
    //DJV_DEBUG("djvFileBrowser::djvFileBrowser");
    
    _p->osxMenuHack = new djvOsxMenuHack(this);
    
    // Create the menus.

    _p->menus.menuBar = new QMenuBar(this);
    _p->menus.menuBar->setNativeMenuBar(false);
    
    QMenu * directoryMenu = _p->menus.menuBar->addMenu(tr("&Directory"));
    
    _p->actions.actions[P::Actions::UP] = directoryMenu->addAction(
        tr("&Up"), this, SLOT(upCallback()));
    _p->actions.actions[P::Actions::UP]->setIcon(
        djvIconLibrary::global()->icon("djvDirUpIcon.png"));

    _p->actions.actions[P::Actions::PREV] = directoryMenu->addAction(
        tr("&Prev"), this, SLOT(prevCallback()));
    _p->actions.actions[P::Actions::PREV]->setIcon(
        djvIconLibrary::global()->icon("djvDirPrevIcon.png"));

    _p->menus.menus[P::Menus::RECENT] = directoryMenu->addMenu(tr("&Recent"));
    _p->actions.groups[P::Actions::RECENT_GROUP] = new QActionGroup(this);
    
    directoryMenu->addSeparator();
    
    _p->actions.actions[P::Actions::CURRENT] = directoryMenu->addAction(
        tr("&Current"), this, SLOT(currentCallback()));

    _p->actions.actions[P::Actions::HOME] = directoryMenu->addAction(
        tr("&Home"), this, SLOT(homeCallback()));

    _p->actions.actions[P::Actions::DESKTOP] = directoryMenu->addAction(
        tr("&Desktop"), this, SLOT(desktopCallback()));

    _p->menus.menus[P::Menus::DRIVES] = directoryMenu->addMenu(tr("&Drives"));
    _p->actions.groups[P::Actions::DRIVES_GROUP] = new QActionGroup(this);
    
    directoryMenu->addSeparator();
    
    _p->actions.actions[P::Actions::RELOAD] = directoryMenu->addAction(
        tr("Re&load"), this, SLOT(reloadCallback()));
    _p->actions.actions[P::Actions::RELOAD]->setIcon(
        djvIconLibrary::global()->icon("djvDirReloadIcon.png"));
    
    QMenu * optionsMenu = _p->menus.menuBar->addMenu(tr("&Options"));
    
    _p->menus.menus[P::Menus::THUMBNAILS] =
        optionsMenu->addMenu(tr("Thu&mbnails"));
    _p->actions.groups[P::Actions::THUMBNAILS_GROUP] = new QActionGroup(this);
    _p->actions.groups[P::Actions::THUMBNAILS_GROUP]->setExclusive(true);
    
    _p->menus.menus[P::Menus::SEQ] = optionsMenu->addMenu(tr("Se&quence"));
    _p->actions.groups[P::Actions::SEQ_GROUP] = new QActionGroup(this);
    
    _p->actions.actions[P::Actions::SHOW_HIDDEN] = optionsMenu->addAction(
        tr("Show &Hidden"), this, SLOT(showHiddenCallback(bool)));
    _p->actions.actions[P::Actions::SHOW_HIDDEN]->setCheckable(true);

    optionsMenu->addSeparator();
    
    _p->menus.menus[P::Menus::SORT] = optionsMenu->addMenu(tr("&Sort"));
    _p->actions.groups[P::Actions::SORT_GROUP] = new QActionGroup(this);
    
    _p->actions.actions[P::Actions::REVERSE_SORT] = optionsMenu->addAction(
        tr("&Reverse Sort"), this, SLOT(reverseSortCallback(bool)));
    _p->actions.actions[P::Actions::REVERSE_SORT]->setCheckable(true);
    
    _p->actions.actions[P::Actions::SORT_DIRS_FIRST] = optionsMenu->addAction(
        tr("Sort &Directories First"), this, SLOT(sortDirsFirstCallback(bool)));
    _p->actions.actions[P::Actions::SORT_DIRS_FIRST]->setCheckable(true);
    
    _p->menus.menus[P::Menus::BOOKMARKS] =
        _p->menus.menuBar->addMenu(tr("&Bookmarks"));
    
    _p->actions.groups[P::Actions::BOOKMARKS_GROUP] = new QActionGroup(this);
    
    // Create the widgets.

    _p->widgets.file = new QLineEdit;
    
    _p->widgets.up = new djvToolButton;
    _p->widgets.up->setDefaultAction(_p->actions.actions[P::Actions::UP]);
    
    _p->widgets.prev = new djvToolButton;
    _p->widgets.prev->setDefaultAction(_p->actions.actions[P::Actions::PREV]);
    
    _p->widgets.reload = new djvToolButton;
    _p->widgets.reload->setDefaultAction(_p->actions.actions[P::Actions::RELOAD]);

    _p->widgets.seq = new QComboBox;
    _p->widgets.seq->addItems(djvSequence::compressLabels());
    QLabel * seqLabel = new QLabel(tr("Sequence:"));

    _p->widgets.search = new djvSearchBox;
    _p->widgets.search->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    _p->widgets.browser = new QTreeView;
    _p->widgets.browser->setSortingEnabled(true);
    _p->widgets.browser->sortByColumn(1, Qt::AscendingOrder);
    _p->widgets.browser->setRootIsDecorated(false);
    _p->widgets.browser->setItemsExpandable(false);
    _p->widgets.browser->setVerticalScrollMode(
        QAbstractItemView::ScrollPerPixel);
    _p->widgets.browser->setAlternatingRowColors(true);
    _p->widgets.browser->setDragDropMode(QAbstractItemView::DragOnly);
    _p->widgets.browser->setDragEnabled(true);

    _p->widgets.pinned = new QCheckBox(tr("Pin"));

    QPushButton * okButton = new QPushButton(tr("Ok"));

    QPushButton * closeButton = new QPushButton(tr("Close"));
    closeButton->setAutoDefault(false);
    
    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setMenuBar(_p->menus.menuBar);
    
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->setSpacing(djvStyle::global()->sizeMetric().largeSpacing);
    
    QHBoxLayout * hLayout2 = new QHBoxLayout;
    hLayout2->setMargin(0);
    hLayout2->setSpacing(0);
    hLayout2->addWidget(_p->widgets.up);
    hLayout2->addWidget(_p->widgets.prev);
    hLayout2->addWidget(_p->widgets.reload);
    hLayout->addLayout(hLayout2);

    hLayout2 = new QHBoxLayout;
    hLayout2->setMargin(0);
    hLayout2->setSpacing(5);
    hLayout2->addWidget(seqLabel);
    hLayout2->addWidget(_p->widgets.seq);
    hLayout->addLayout(hLayout2);
    
    hLayout->addStretch();
    hLayout->addWidget(_p->widgets.search);
    
    layout->addLayout(hLayout);

    layout->addWidget(_p->widgets.browser);
    layout->addWidget(_p->widgets.file);
    
    hLayout = new QHBoxLayout;
    hLayout->addWidget(_p->widgets.pinned);
    hLayout->addStretch();
    hLayout->addWidget(okButton);
    hLayout->addWidget(closeButton);
    layout->addLayout(hLayout);
    
    // Restore the last used path.

    _p->fileInfo = djvFileInfoUtil::fixPath(djvFileBrowserPrefs::pathDefault());
    _p->prev = _p->fileInfo;
    
    //DJV_DEBUG_PRINT("file = " << _p->fileInfo);
    
    // Load the preferences.

    _p->model = new djvFileBrowserModel(this);
    _p->model->setSequence(djvFileBrowserPrefs::global()->sequence());
    _p->model->setShowHidden(djvFileBrowserPrefs::global()->hasShowHidden());
    _p->model->setSort(djvFileBrowserPrefs::global()->sort());
    _p->model->setReverseSort(djvFileBrowserPrefs::global()->hasReverseSort());
    _p->model->setSortDirsFirst(djvFileBrowserPrefs::global()->hasSortDirsFirst());
    _p->model->setThumbnails(djvFileBrowserPrefs::global()->thumbnails());
    _p->model->setPath(_p->fileInfo);
    _p->model->setView(_p->widgets.browser);
    
    // Initialize.
    
    setWindowTitle(tr("File Browser"));

    _p->widgets.browser->setModel(_p->model);

    const QVector<int> sizes = columnSizes();
    
    for (int i = 0; i < sizes.count(); ++i)
    {
        _p->widgets.browser->header()->resizeSection(i, sizes[i]);
    }

    widgetUpdate();
    menuUpdate();
    toolTipUpdate();

    resize(700, 600);

    // Setup the callbacks.

    connect(
        _p->model,
        SIGNAL(optionChanged()),
        SLOT(widgetUpdate()));

    connect(
        _p->model,
        SIGNAL(optionChanged()),
        SLOT(menuUpdate()));

    connect(
        _p->model,
        SIGNAL(optionChanged()),
        SLOT(widgetUpdate()));

    connect(
        _p->model,
        SIGNAL(optionChanged()),
        SLOT(menuUpdate()));

    _p->model->connect(
        djvFileBrowserPrefs::global(),
        SIGNAL(sequenceChanged(djvSequence::COMPRESS)),
        SLOT(setSequence(djvSequence::COMPRESS)));

    _p->model->connect(
        djvFileBrowserPrefs::global(),
        SIGNAL(showHiddenChanged(bool)),
        SLOT(setShowHidden(bool)));

    _p->model->connect(
        djvFileBrowserPrefs::global(),
        SIGNAL(sortChanged(djvFileBrowserModel::COLUMNS)),
        SLOT(setSort(djvFileBrowserModel::COLUMNS)));

    _p->model->connect(
        djvFileBrowserPrefs::global(),
        SIGNAL(reverseSortChanged(bool)),
        SLOT(setReverseSort(bool)));

    _p->model->connect(
        djvFileBrowserPrefs::global(),
        SIGNAL(sortDirsFirstChanged(bool)),
        SLOT(setSortDirsFirst(bool)));

    connect(
        _p->actions.groups[P::Actions::RECENT_GROUP],
        SIGNAL(triggered(QAction *)),
        SLOT(recentCallback(QAction *)));

    connect(
        _p->actions.groups[P::Actions::DRIVES_GROUP],
        SIGNAL(triggered(QAction *)),
        SLOT(drivesCallback(QAction *)));

    connect(
        _p->actions.groups[P::Actions::THUMBNAILS_GROUP],
        SIGNAL(triggered(QAction *)),
        SLOT(thumbnailsCallback(QAction *)));

    connect(
        _p->actions.groups[P::Actions::SEQ_GROUP],
        SIGNAL(triggered(QAction *)),
        SLOT(seqCallback(QAction *)));

    connect(
        _p->actions.groups[P::Actions::SORT_GROUP],
        SIGNAL(triggered(QAction *)),
        SLOT(sortCallback(QAction *)));

    connect(
        _p->actions.groups[P::Actions::BOOKMARKS_GROUP],
        SIGNAL(triggered(QAction *)),
        SLOT(bookmarksCallback(QAction *)));
    
    connect(
        _p->widgets.file,
        SIGNAL(editingFinished()),
        SLOT(fileCallback()));

    connect(
        _p->widgets.browser,
        SIGNAL(activated(const QModelIndex &)),
        SLOT(browserCallback(const QModelIndex &)));

    connect(
        _p->widgets.browser->selectionModel(),
        SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
        SLOT(browserCurrentCallback(const QModelIndex &, const QModelIndex &)));

    connect(
        _p->widgets.browser->header(),
        SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
        SLOT(sortCallback(int, Qt::SortOrder)));

    connect(
        _p->widgets.seq,
        SIGNAL(activated(int)),
        SLOT(seqCallback(int)));

    connect(
        _p->widgets.search,
        SIGNAL(textChanged(const QString &)),
        SLOT(searchCallback(const QString &)));

    connect(
        _p->widgets.pinned,
        SIGNAL(toggled(bool)),
        SLOT(pinnedCallback(bool)));

    connect(okButton, SIGNAL(clicked()), SLOT(acceptedCallback()));

    connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));

    connect(
        djvFileBrowserPrefs::global(),
        SIGNAL(thumbnailsSizeChanged(djvFileBrowserModel::THUMBNAILS_SIZE)),
        SLOT(modelUpdate()));

    connect(
        djvFileBrowserPrefs::global(),
        SIGNAL(recentChanged(const QStringList &)),
        SLOT(menuUpdate()));

    connect(
        djvFileBrowserPrefs::global(),
        SIGNAL(bookmarksChanged(const QStringList &)),
        SLOT(menuUpdate()));

    connect(
        djvFileBrowserPrefs::global(),
        SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
        SLOT(menuUpdate()));

    connect(
        djvFileBrowserPrefs::global(),
        SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
        SLOT(toolTipUpdate()));

    connect(
        djvStyle::global(),
        SIGNAL(sizeMetricsChanged()),
        SLOT(modelUpdate()));
}

djvFileBrowser::~djvFileBrowser()
{
    // Remember the last used path.

    djvFileBrowserPrefs::setPathDefault(_p->model->path());

    // Save the preferences.

    djvFileBrowserPrefs::global()->setSequence(_p->model->sequence());
    djvFileBrowserPrefs::global()->setShowHidden(_p->model->hasShowHidden());
    djvFileBrowserPrefs::global()->setSort(_p->model->sort());
    djvFileBrowserPrefs::global()->setReverseSort(_p->model->hasReverseSort());
    djvFileBrowserPrefs::global()->setSortDirsFirst(_p->model->hasSortDirsFirst());
    djvFileBrowserPrefs::global()->setThumbnails(_p->model->thumbnails());

    delete _p;
}

const djvFileInfo & djvFileBrowser::fileInfo() const
{
    return _p->fileInfo;
}
    
bool djvFileBrowser::isPinnable() const
{
    return _p->pinnable;
}

bool djvFileBrowser::isPinned() const
{
    return _p->pinned;
}

djvFileBrowser * djvFileBrowser::global(const QString & title)
{
    static QPointer<djvFileBrowser> fileBrowser;
    
    if (! fileBrowser)
    {
        fileBrowser = QPointer<djvFileBrowser>(new djvFileBrowser);
    }
    
    fileBrowser->close();

    fileBrowser->setWindowTitle(! title.isEmpty() ? title : tr("File Browser"));
    
    fileBrowser->setPinnable(false);
    
    fileBrowser->disconnect(SIGNAL(fileInfoChanged(const djvFileInfo &)));

    return fileBrowser;
}

void djvFileBrowser::setFileInfo(const djvFileInfo & fileInfo)
{
    djvFileInfo tmp = djvFileInfoUtil::fixPath(fileInfo);
    tmp.setType(fileInfo.type());
    
    if (tmp == _p->fileInfo)
        return;

    //DJV_DEBUG("djvFileBrowser::setFileInfo");
    //DJV_DEBUG_PRINT("fileInfo = " << tmp);

    const QString oldDir = djvFileInfoUtil::fixPath(
        _p->fileInfo.type() == djvFileInfo::DIRECTORY ?
        _p->fileInfo.fileName() :
        _p->fileInfo.path());
    
    //DJV_DEBUG_PRINT("oldDir = " << oldDir);

    _p->fileInfo = tmp;
    
    //DJV_DEBUG_PRINT("file = " << _p->fileInfo);

    const QString newDir = djvFileInfoUtil::fixPath(
        _p->fileInfo.type() == djvFileInfo::DIRECTORY ?
        _p->fileInfo.fileName() :
        _p->fileInfo.path());

    //DJV_DEBUG_PRINT("newDir = " << newDir);

    if (newDir != oldDir)
    {
        _p->prev = oldDir;
        
        modelUpdate();
    }

    widgetUpdate();
}
    
void djvFileBrowser::setPinnable(bool pinnable)
{
    if (pinnable == _p->pinnable)
        return;
    
    _p->pinnable = pinnable;
    
    widgetUpdate();
    
    Q_EMIT pinnableChanged(_p->pinnable);
}

void djvFileBrowser::setPinned(bool pinned)
{
    if (pinned == _p->pinned)
        return;
    
    _p->pinned = pinned;
    
    Q_EMIT pinnedChanged(_p->pinned);
}

void djvFileBrowser::showEvent(QShowEvent *)
{
    //DJV_DEBUG("djvFileBrowser::showEvent");

    if (! _p->shown)
    {
        modelUpdate();
        widgetUpdate();
        
        _p->shown = true;
    }

    _p->widgets.file->setFocus(Qt::OtherFocusReason);
}

void djvFileBrowser::fileCallback()
{
    //DJV_DEBUG("djvFileBrowser::fileCallback");

    const QString fileName = djvFileInfoUtil::fixPath(
        _p->widgets.file->text());

    //DJV_DEBUG_PRINT("fileName = " << fileName);

    djvFileInfo fileInfo(fileName);

    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);

    if (fileInfo.type() == djvFileInfo::DIRECTORY)
    {
        //DJV_DEBUG_PRINT("valid");

        setFileInfo(fileInfo);

        _p->widgets.file->setText(QDir::toNativeSeparators(_p->fileInfo));
    }
}

void djvFileBrowser::browserCallback(const QModelIndex & index)
{
    //DJV_DEBUG("djvFileBrowser::browserCallback");

    const djvFileInfo fileInfo = _p->model->fileInfo(index);

    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo << " " << fileInfo.type());

    _p->widgets.file->setText(QDir::toNativeSeparators(fileInfo));

    acceptedCallback();
}

void djvFileBrowser::browserCurrentCallback(
    const QModelIndex & index,
    const QModelIndex & previous)
{
    //DJV_DEBUG("djvFileBrowser::browserCurrentCallback");

    const djvFileInfo fileInfo = _p->model->fileInfo(index);

    //DJV_DEBUG_PRINT("file = " << fileInfo);

    _p->widgets.file->setText(QDir::toNativeSeparators(fileInfo));
}

void djvFileBrowser::recentCallback(QAction * action)
{
    setFileInfo(djvFileInfoUtil::fixPath(action->data().toString()));
}

void djvFileBrowser::upCallback()
{
    QDir tmp(_p->fileInfo.path());
    
    tmp.cdUp();
    
    setFileInfo(djvFileInfoUtil::fixPath(tmp.path()));
}

void djvFileBrowser::prevCallback()
{
    setFileInfo(_p->prev);
}

void djvFileBrowser::drivesCallback(QAction * action)
{
    setFileInfo(djvFileInfoUtil::fixPath(action->data().toString()));
}

void djvFileBrowser::currentCallback()
{
    setFileInfo(djvFileInfoUtil::fixPath(QDir::currentPath()));
}

void djvFileBrowser::homeCallback()
{
    setFileInfo(djvFileInfoUtil::fixPath(QDir::homePath()));
}

void djvFileBrowser::desktopCallback()
{
    setFileInfo(djvFileInfoUtil::fixPath(QDir::homePath() + "/Desktop"));
}

void djvFileBrowser::reloadCallback()
{
    _p->model->reload();
}

void djvFileBrowser::thumbnailsCallback(QAction * action)
{
    _p->model->setThumbnails(
        static_cast<djvFileBrowserModel::THUMBNAILS>(action->data().toInt()));
}

void djvFileBrowser::showHiddenCallback(bool value)
{
    _p->model->setShowHidden(value);
}

void djvFileBrowser::showHiddenCallback()
{
    _p->model->setShowHidden(! _p->model->hasShowHidden());
}

void djvFileBrowser::seqCallback(QAction * action)
{
    _p->model->setSequence(
        static_cast<djvSequence::COMPRESS>(action->data().toInt()));
}

void djvFileBrowser::seqCallback(int in)
{
    _p->model->setSequence(static_cast<djvSequence::COMPRESS>(in));
}

void djvFileBrowser::searchCallback(const QString & text)
{
    _p->model->setFilterText(text);
}

void djvFileBrowser::sortCallback(int section, Qt::SortOrder order)
{
    _p->model->setSort(static_cast<djvFileBrowserModel::COLUMNS>(section));
    _p->model->setReverseSort(order);
}

void djvFileBrowser::sortCallback(QAction * action)
{
    const djvFileBrowserModel::COLUMNS sort =
        static_cast<djvFileBrowserModel::COLUMNS>(action->data().toInt());

    if (sort != _p->model->sort())
    {
        _p->model->setSort(sort);
    }
    else
    {
        _p->model->setReverseSort(! _p->model->hasReverseSort());
    }
}

void djvFileBrowser::sortCallback()
{
    if (djvIndexShortcut * shortcut = qobject_cast<djvIndexShortcut *>(sender()))
    {
        const int index = shortcut->index();
        
        bool reverseSort =
            _p->widgets.browser->header()->sortIndicatorOrder();
        
        if (index == _p->widgets.browser->header()->sortIndicatorSection())
        {
            reverseSort = ! reverseSort;
        }
        
        _p->model->setSort(static_cast<djvFileBrowserModel::COLUMNS>(index));
        _p->model->setReverseSort(reverseSort);
    }
}

void djvFileBrowser::reverseSortCallback(bool value)
{
    _p->model->setShowHidden(value);
}

void djvFileBrowser::reverseSortCallback()
{
    _p->model->setShowHidden(! _p->model->hasShowHidden());
}

void djvFileBrowser::sortDirsFirstCallback(bool value)
{
    _p->model->setSortDirsFirst(value);
}

void djvFileBrowser::sortDirsFirstCallback()
{
    _p->model->setSortDirsFirst(! _p->model->hasSortDirsFirst());
}

void djvFileBrowser::pinnedCallback(bool pinned)
{
    _p->pinned = pinned;
}

void djvFileBrowser::addBookmarkCallback()
{
    djvFileBrowserPrefs::global()->addBookmark(_p->fileInfo.path());
}

void djvFileBrowser::deleteBookmarkCallback()
{
    const QStringList & bookmarks = djvFileBrowserPrefs::global()->bookmarks();

    if (! bookmarks.count())
        return;

    //DJV_DEBUG("djvFileBrowser::deleteBookmarkCallback");
    //DJV_DEBUG_PRINT("bookmarks = " << bookmarks);
    
    djvMultiChoiceDialog dialog(tr("Delete bookmarks:"), bookmarks);
    
    if (QDialog::Accepted == dialog.exec())
    {
        QStringList tmp;
        
        const QVector<bool> & values = dialog.values();
        
        //DJV_DEBUG_PRINT("values = " << values);
        
        for (int i = 0; i < values.count(); ++i)
        {
            if (! values[i])
            {
                tmp += bookmarks[i];
            }
        }
        
        //DJV_DEBUG_PRINT("bookmarks = " << tmp);
        
        djvFileBrowserPrefs::global()->setBookmarks(tmp);
    }
}

void djvFileBrowser::deleteAllBookmarksCallback()
{
    const QStringList & bookmarks = djvFileBrowserPrefs::global()->bookmarks();

    if (! bookmarks.count())
        return;

    djvQuestionDialog dialog(tr("Delete all bookmarks?"));
    
    if (QDialog::Accepted == dialog.exec())
    {
        djvFileBrowserPrefs::global()->setBookmarks(QStringList());
    }
}

void djvFileBrowser::bookmarksCallback(QAction * action)
{
    setFileInfo(djvFileInfoUtil::fixPath(action->data().toString()));
}

void djvFileBrowser::bookmarksCallback()
{
    if (djvIndexShortcut * shortcut = qobject_cast<djvIndexShortcut *>(sender()))
    {
        const QStringList & bookmarks = djvFileBrowserPrefs::global()->bookmarks();
    
        setFileInfo(bookmarks[shortcut->index()]);
    }
}

void djvFileBrowser::bookmarkDelCallback(const QVector<int> & list)
{
    const QStringList & bookmarks = djvFileBrowserPrefs::global()->bookmarks();

    QStringList tmp;

    for (int i = 0; i < bookmarks.count(); ++i)
    {
        int j = 0;

        for (; j < list.count(); ++j)
        {
            if (list[j] == i)
                break;
        }

        if (j < list.count())
            continue;

        tmp += bookmarks[i];
    }

    djvFileBrowserPrefs::global()->setBookmarks(tmp);

    menuUpdate();
}

void djvFileBrowser::bookmarkDelAllCallback()
{
    djvFileBrowserPrefs::global()->setBookmarks(QStringList());

    menuUpdate();
}

void djvFileBrowser::acceptedCallback()
{
    //DJV_DEBUG("djvFileBrowser::acceptedCallback");

    const QString fileName = djvFileInfoUtil::fixPath(
        _p->widgets.file->text());

    //DJV_DEBUG_PRINT("file name = " << fileName);

    djvFileInfo fileInfo(fileName);
    
    if (_p->model->sequence() != djvSequence::COMPRESS_OFF &&
        fileInfo.isSequenceValid())
    {
        fileInfo.setType(djvFileInfo::SEQUENCE);
    }

    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo << " " << fileInfo.type());

    setFileInfo(fileInfo);

    _p->widgets.file->setText(QDir::toNativeSeparators(_p->fileInfo));

    if (fileInfo.type() != djvFileInfo::DIRECTORY)
    {
        djvFileBrowserPrefs::global()->addRecent(_p->fileInfo.path());

        if (! _p->pinnable || (_p->pinnable && ! _p->pinned))
        {
            accept();
        }

        Q_EMIT fileInfoChanged(_p->fileInfo);
    }
}

void djvFileBrowser::modelUpdate()
{
    //DJV_DEBUG("djvFileBrowser::modelUpdate");
    //DJV_DEBUG_PRINT("path = " << _p->fileInfo.path());

    setCursor(Qt::WaitCursor);

    _p->model->setThumbnailsSize(djvFileBrowserPrefs::global()->thumbnailsSize());
    _p->model->setPath(_p->fileInfo.path());

    setCursor(QCursor());
}

void djvFileBrowser::widgetUpdate()
{
    //DJV_DEBUG("djvFileBrowser::widgetUpdate");
    //DJV_DEBUG_PRINT("file = " << _p->fileInfo);
    //DJV_DEBUG_PRINT("file seq = " << _p->fileInfo.seq());

    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->widgets.file <<
        _p->widgets.seq <<
        _p->widgets.browser->header() <<
        _p->widgets.pinned);

    _p->widgets.file->setText(QDir::toNativeSeparators(_p->fileInfo));

    _p->widgets.seq->setCurrentIndex(_p->model->sequence());

    _p->widgets.browser->header()->setSortIndicator(
        _p->model->sort(),
        static_cast<Qt::SortOrder>(_p->model->hasReverseSort()));
    
    _p->widgets.pinned->setVisible(_p->pinnable);
    _p->widgets.pinned->setChecked(_p->pinned);
}

void djvFileBrowser::menuUpdate()
{
    //DJV_DEBUG("djvFileBrowser::menuUpdate");

    const QVector<djvShortcut> & shortcuts = djvFileBrowserPrefs::global()->shortcuts();

    _p->actions.actions[P::Actions::UP]->setShortcut(
        shortcuts[djvFileBrowserPrefs::UP].value);

    _p->actions.actions[P::Actions::PREV]->setShortcut(
        shortcuts[djvFileBrowserPrefs::PREV].value);

    _p->menus.menus[P::Menus::RECENT]->clear();
    
    const QStringList & recent = djvFileBrowserPrefs::global()->recent();
    
    for (int i = 0; i < recent.count(); ++i)
    {
        QAction * action = _p->menus.menus[P::Menus::RECENT]->addAction(
            QDir::toNativeSeparators(recent[i]));
        action->setData(recent[i]);
        _p->actions.groups[P::Actions::RECENT_GROUP]->addAction(action);
    }

    _p->actions.actions[P::Actions::CURRENT]->setShortcut(
        shortcuts[djvFileBrowserPrefs::CURRENT].value);

    _p->actions.actions[P::Actions::HOME]->setShortcut(
        shortcuts[djvFileBrowserPrefs::HOME].value);

    _p->actions.actions[P::Actions::DESKTOP]->setShortcut(
        shortcuts[djvFileBrowserPrefs::DESKTOP].value);

    _p->menus.menus[P::Menus::DRIVES]->clear();
    
    const QStringList drives = djvSystem::drives();
    
    for (int i = 0; i < drives.count(); ++i)
    {
        QAction * action = _p->menus.menus[P::Menus::DRIVES]->addAction(
            QDir::toNativeSeparators(drives[i]));
        action->setData(drives[i]);
        _p->actions.groups[P::Actions::DRIVES_GROUP]->addAction(action);
    }

    _p->actions.actions[P::Actions::RELOAD]->setShortcut(
        shortcuts[djvFileBrowserPrefs::RELOAD].value);

    _p->menus.menus[P::Menus::THUMBNAILS]->clear();
    
    const QStringList & thumbnails = djvFileBrowserModel::thumbnailsLabels();
    
    for (int i = 0; i < thumbnails.count(); ++i)
    {
        QAction * action = _p->menus.menus[P::Menus::THUMBNAILS]->addAction(thumbnails[i]);
        action->setCheckable(true);
        action->setChecked(_p->model ->thumbnails() == i);
        action->setData(i);
        _p->actions.groups[P::Actions::THUMBNAILS_GROUP]->addAction(action);
    }

    _p->menus.menus[P::Menus::SEQ]->clear();
    
    const QStringList & seq = djvSequence::compressLabels();
    
    for (int i = 0; i < seq.count(); ++i)
    {
        QAction * action = _p->menus.menus[P::Menus::SEQ]->addAction(seq[i]);
        action->setCheckable(true);
        action->setChecked(_p->model->sequence() == i);
        action->setData(i);
        _p->actions.groups[P::Actions::SEQ_GROUP]->addAction(action);
    }

    _p->actions.actions[P::Actions::SHOW_HIDDEN]->setChecked(_p->model->hasShowHidden());
    _p->actions.actions[P::Actions::SHOW_HIDDEN]->setShortcut(
        shortcuts[djvFileBrowserPrefs::SHOW_HIDDEN].value);

    QVector<djvShortcut> sortShortcuts = QVector<djvShortcut>() <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::SORT_NAME] <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::SORT_SIZE] <<
#if ! defined(DJV_WINDOWS)
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::SORT_USER] <<
#endif
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::SORT_PERMISSIONS] <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::SORT_TIME];

    _p->menus.menus[P::Menus::SORT]->clear();

    const QStringList & sort = djvFileBrowserModel::columnsLabels();

    for (int i = 0; i < sort.count(); ++i)
    {
        QAction * action = _p->menus.menus[P::Menus::SORT]->addAction(sort[i]);
        action->setCheckable(true);
        action->setChecked(
            _p->widgets.browser->header()->sortIndicatorSection() == i);
        action->setData(i);
        action->setShortcut(sortShortcuts[i].value);
        _p->actions.groups[P::Actions::SORT_GROUP]->addAction(action);
    }

    _p->actions.actions[P::Actions::REVERSE_SORT]->setChecked(
        _p->widgets.browser->header()->sortIndicatorOrder());
    _p->actions.actions[P::Actions::REVERSE_SORT]->setShortcut(
        shortcuts[djvFileBrowserPrefs::REVERSE_SORT].value);

    _p->actions.actions[P::Actions::SORT_DIRS_FIRST]->setChecked(
        _p->model->hasSortDirsFirst());
    _p->actions.actions[P::Actions::SORT_DIRS_FIRST]->setShortcut(
        shortcuts[djvFileBrowserPrefs::SORT_DIRS_FIRST].value);

    _p->menus.menus[P::Menus::BOOKMARKS]->clear();

    QAction * action = _p->menus.menus[P::Menus::BOOKMARKS]->addAction(
        tr("&Add"), this, SLOT(addBookmarkCallback()));
    action->setShortcut(
        shortcuts[djvFileBrowserPrefs::ADD_BOOKMARK].value);

    _p->menus.menus[P::Menus::BOOKMARKS]->addAction(
        tr("&Delete"), this, SLOT(deleteBookmarkCallback()));

    _p->menus.menus[P::Menus::BOOKMARKS]->addAction(
        tr("D&elete All"), this, SLOT(deleteAllBookmarksCallback()));

    _p->menus.menus[P::Menus::BOOKMARKS]->addSeparator();

    QVector<djvShortcut> bookmarkShortcuts = QVector<djvShortcut>() <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::BOOKMARK_1] <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::BOOKMARK_2] <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::BOOKMARK_3] <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::BOOKMARK_4] <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::BOOKMARK_5] <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::BOOKMARK_6] <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::BOOKMARK_7] <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::BOOKMARK_8] <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::BOOKMARK_9] <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::BOOKMARK_10] <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::BOOKMARK_11] <<
        djvFileBrowserPrefs::global()->shortcuts()[djvFileBrowserPrefs::BOOKMARK_12];

    const QStringList & bookmarks = djvFileBrowserPrefs::global()->bookmarks();

    for (int i = 0; i < bookmarks.count(); ++i)
    {
        QAction * action = _p->menus.menus[P::Menus::BOOKMARKS]->addAction(bookmarks[i]);
        action->setData(bookmarks[i]);
        if (i < bookmarkShortcuts.count())
            action->setShortcut(bookmarkShortcuts[i].value);
        _p->actions.groups[P::Actions::BOOKMARKS_GROUP]->addAction(action);
    }
    
    // Fix up the menus.
    
    QList<QAction *> actions = _p->actions.actions.values();
    
    Q_FOREACH(QActionGroup * group, _p->actions.groups)
    {
        actions += group->actions();
    }
    
    _p->osxMenuHack->fix(actions);
}

void djvFileBrowser::toolTipUpdate()
{
    //DJV_DEBUG("djvFileBrowser::toolTipUpdate");

    const QVector<djvShortcut> & shortcuts = djvFileBrowserPrefs::global()->shortcuts();

    _p->widgets.file->setToolTip(tr("File name"));

    _p->widgets.up->setToolTip(QString(
        tr("Go up a directory\n\nShortcut: %1")).
        arg(shortcuts[djvFileBrowserPrefs::UP].value.toString()));

    _p->widgets.prev->setToolTip(QString(
        tr("Go to the previous directory\n\nShortcut: %1")).
        arg(shortcuts[djvFileBrowserPrefs::PREV].value.toString()));

    _p->widgets.reload->setToolTip(QString(
        tr("Reload the current directory\n\nShortcut: %1")).
        arg(shortcuts[djvFileBrowserPrefs::RELOAD].value.toString()));
}

QVector<int> djvFileBrowser::columnSizes() const
{
    const QFontMetrics fontMetrics(this->fontMetrics());
    
    static const int border = 20;
    
    return QVector<int>() <<
        300 <<
        (fontMetrics.width("000.0000") + border) <<
#if ! defined(DJV_WINDOWS)
        (fontMetrics.width("000000") + border) <<
#endif
        (fontMetrics.averageCharWidth() * 4 + border)<<
        fontMetrics.width("000 000 00 00:00:00 0000");
}
