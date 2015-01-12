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

//! \file djvFileBrowserPrefs.cpp

#include <djvFileBrowserPrefs.h>

#include <djvFileBrowserCache.h>
#include <djvPrefs.h>

#include <djvAssert.h>
#include <djvFileInfoUtil.h>
#include <djvMemory.h>

#include <QApplication>

//------------------------------------------------------------------------------
// djvFileBrowserPrefs::P
//------------------------------------------------------------------------------

struct djvFileBrowserPrefs::P
{
    P() :
        sequence       (sequenceDefault()),
        showHidden     (showHiddenDefault()),
        sort           (sortDefault()),
        reverseSort    (reverseSortDefault()),
        sortDirsFirst  (sortDirsFirstDefault()),
        thumbnails     (thumbnailsDefault()),
        thumbnailsSize (thumbnailsSizeDefault()),
        thumbnailsCache(thumbnailsCacheDefault()),
        shortcuts      (shortcutsDefault())
    {}
    
    djvSequence::COMPRESS                sequence;
    bool                                 showHidden;
    djvFileBrowserModel::COLUMNS         sort;
    bool                                 reverseSort;
    bool                                 sortDirsFirst;
    djvFileBrowserModel::THUMBNAILS      thumbnails;
    djvFileBrowserModel::THUMBNAILS_SIZE thumbnailsSize;
    qint64                               thumbnailsCache;
    QStringList                          recent;
    QStringList                          bookmarks;
    QVector<djvShortcut>                 shortcuts;
};

namespace
{

QString _pathDefault = ".";

} // namespace

//------------------------------------------------------------------------------
// djvFileBrowserPrefs
//------------------------------------------------------------------------------

djvFileBrowserPrefs::djvFileBrowserPrefs(QObject * parent) :
    QObject(parent),
    _p(new P)
{
    //DJV_DEBUG("djvFileBrowserPrefs::djvFileBrowserPrefs");

    djvPrefs prefs("djvFileBrowserPrefs");

    prefs.get("sequence", _p->sequence);
    prefs.get("showHidden", _p->showHidden);
    prefs.get("sort", _p->sort);
    prefs.get("reverseSort", _p->reverseSort);
    prefs.get("sortDirsFirst", _p->sortDirsFirst);
    prefs.get("thumbnails", _p->thumbnails);
    prefs.get("thumbnailsSize", _p->thumbnailsSize);
    prefs.get("thumbnailsCache", _p->thumbnailsCache);
    prefs.get("recent", _p->recent);
    prefs.get("bookmarks", _p->bookmarks);

    if (_p->recent.count() > djvFileInfoUtil::recentMax)
        _p->recent = _p->recent.mid(0, djvFileInfoUtil::recentMax);

    djvPrefs shortcutsPrefs("djvFileBrowserPrefs/Shortcuts");

    for (int i = 0; i < _p->shortcuts.count(); ++i)
    {
        QStringList s;

        if (shortcutsPrefs.get(_p->shortcuts[i].name, s))
        {
            djvShortcut::serialize(s, _p->shortcuts[i].value);
        }
    }
}

djvFileBrowserPrefs::~djvFileBrowserPrefs()
{
    //DJV_DEBUG("djvFileBrowserPrefs::~djvFileBrowserPrefs");

    djvPrefs prefs("djvFileBrowserPrefs");

    prefs.set("sequence", _p->sequence);
    prefs.set("showHidden", _p->showHidden);
    prefs.set("sort", _p->sort);
    prefs.set("reverseSort", _p->reverseSort);
    prefs.set("sortDirsFirst", _p->sortDirsFirst);
    prefs.set("thumbnails", _p->thumbnails);
    prefs.set("thumbnailsSize", _p->thumbnailsSize);
    prefs.set("thumbnailsCache", _p->thumbnailsCache);
    prefs.set("recent", _p->recent);
    prefs.set("bookmarks", _p->bookmarks);

    djvPrefs shortcutsPrefs("djvFileBrowserPrefs/Shortcuts");

    for (int i = 0; i < _p->shortcuts.count(); ++i)
    {
        shortcutsPrefs.set(
            _p->shortcuts[i].name,
            djvShortcut::serialize(_p->shortcuts[i].value));
    }

    delete _p;
}

const QString & djvFileBrowserPrefs::pathDefault()
{
    return _pathDefault;
}

djvSequence::COMPRESS djvFileBrowserPrefs::sequenceDefault()
{
    return djvSequence::COMPRESS_RANGE;
}

djvSequence::COMPRESS djvFileBrowserPrefs::sequence() const
{
    return _p->sequence;
}

bool djvFileBrowserPrefs::showHiddenDefault()
{
    return false;
}

bool djvFileBrowserPrefs::hasShowHidden() const
{
    return _p->showHidden;
}

djvFileBrowserModel::COLUMNS djvFileBrowserPrefs::sortDefault()
{
    return djvFileBrowserModel::NAME;
}

djvFileBrowserModel::COLUMNS djvFileBrowserPrefs::sort() const
{
    return _p->sort;
}

bool djvFileBrowserPrefs::reverseSortDefault()
{
    return false;
}

bool djvFileBrowserPrefs::hasReverseSort() const
{
    return _p->reverseSort;
}

bool djvFileBrowserPrefs::sortDirsFirstDefault()
{
    return true;
}

bool djvFileBrowserPrefs::hasSortDirsFirst() const
{
    return _p->sortDirsFirst;
}

djvFileBrowserModel::THUMBNAILS djvFileBrowserPrefs::thumbnailsDefault()
{
    return djvFileBrowserModel::THUMBNAILS_HIGH;
}

djvFileBrowserModel::THUMBNAILS djvFileBrowserPrefs::thumbnails() const
{
    return _p->thumbnails;
}

djvFileBrowserModel::THUMBNAILS_SIZE djvFileBrowserPrefs::thumbnailsSizeDefault()
{
    return djvFileBrowserModel::THUMBNAILS_MEDIUM;
}

djvFileBrowserModel::THUMBNAILS_SIZE djvFileBrowserPrefs::thumbnailsSize() const
{
    return _p->thumbnailsSize;
}

qint64 djvFileBrowserPrefs::thumbnailsCacheDefault()
{
    return 128 * djvMemory::megabyte;
}

qint64 djvFileBrowserPrefs::thumbnailsCache() const
{
    return _p->thumbnailsCache;
}

const QStringList & djvFileBrowserPrefs::recent() const
{
    return _p->recent;
}

const QStringList & djvFileBrowserPrefs::bookmarks() const
{
    return _p->bookmarks;
}

djvFileBrowserPrefs * djvFileBrowserPrefs::global()
{
    static djvFileBrowserPrefs * global = 0;
    
    if (! global)
    {
        global = new djvFileBrowserPrefs(qApp);
    }
    
    return global;
}

const QStringList & djvFileBrowserPrefs::shortcutLabels()
{
    static const QStringList data = QStringList() <<
        tr("Up") <<
        tr("Previous") <<
        tr("Current") <<
        tr("Home") <<
        tr("Desktop") <<
        tr("Root") <<
        tr("Reload") <<
        tr("Sequence Off") <<
        tr("Sequence Sparse") <<
        tr("Sequence Range") <<
        tr("Show Hidden") <<
        tr("Sort Name") <<
        tr("Sort Size") <<
#if ! defined(DJV_WINDOWS)
        tr("SortUser") <<
#endif
        tr("Sort Time") <<
        tr("Sort Permissions") <<
        tr("Reverse Sort") <<
        tr("Sort Dirs First") <<
        tr("Add Bookmark") <<
        tr("Bookmark 1") <<
        tr("Bookmark 2") <<
        tr("Bookmark 3") <<
        tr("Bookmark 4") <<
        tr("Bookmark 5") <<
        tr("Bookmark 6") <<
        tr("Bookmark 7") <<
        tr("Bookmark 8") <<
        tr("Bookmark 9") <<
        tr("Bookmark 10") <<
        tr("Bookmark 11") <<
        tr("Bookmark 12");

    DJV_ASSERT(data.count() == SHORTCUT_COUNT);

    return data;
}

const QVector<djvShortcut> & djvFileBrowserPrefs::shortcutsDefault()
{
    static const QVector<djvShortcut> data = QVector<djvShortcut>() <<
        djvShortcut(shortcutLabels()[UP], QKeySequence("Ctrl+\\")) <<
        djvShortcut(shortcutLabels()[PREV], QKeySequence(Qt::Key_Backspace)) <<
        djvShortcut(shortcutLabels()[CURRENT], QKeySequence("Ctrl+.")) <<
        djvShortcut(shortcutLabels()[HOME], QKeySequence("Ctrl+,")) <<
        djvShortcut(shortcutLabels()[DESKTOP], QKeySequence("Ctrl+k")) <<
        djvShortcut(shortcutLabels()[ROOT], QKeySequence("Ctrl+/")) <<
        djvShortcut(shortcutLabels()[RELOAD], QKeySequence("Ctrl+r")) <<
        djvShortcut(shortcutLabels()[SEQ_OFF], QKeySequence("Ctrl+q")) <<
        djvShortcut(shortcutLabels()[SEQ_SPARSE], QKeySequence("Ctrl+a")) <<
        djvShortcut(shortcutLabels()[SEQ_RANGE], QKeySequence("Ctrl+z")) <<
        djvShortcut(shortcutLabels()[SHOW_HIDDEN], QKeySequence("Ctrl+n")) <<
        djvShortcut(shortcutLabels()[SORT_NAME], QKeySequence("Ctrl+1")) <<
        djvShortcut(shortcutLabels()[SORT_SIZE], QKeySequence("Ctrl+2")) <<
#if ! defined(DJV_WINDOWS)
        djvShortcut(shortcutLabels()[SORT_USER], QKeySequence("Ctrl+3")) <<
        djvShortcut(shortcutLabels()[SORT_TIME], QKeySequence("Ctrl+4")) <<
        djvShortcut(shortcutLabels()[SORT_PERMISSIONS], QKeySequence("Ctrl+5")) <<
#else
        djvShortcut(shortcutLabels()[SORT_TIME], QKeySequence("Ctrl+3")) <<
        djvShortcut(shortcutLabels()[SORT_PERMISSIONS], QKeySequence("Ctrl+4")) <<
#endif
        djvShortcut(shortcutLabels()[REVERSE_SORT], QKeySequence("Ctrl+`")) <<
        djvShortcut(shortcutLabels()[SORT_DIRS_FIRST], QKeySequence("Ctrl+d")) <<
        djvShortcut(shortcutLabels()[ADD_BOOKMARK], QKeySequence("Ctrl+b")) <<
        djvShortcut(shortcutLabels()[BOOKMARK_1], QKeySequence(Qt::Key_F1)) <<
        djvShortcut(shortcutLabels()[BOOKMARK_2], QKeySequence(Qt::Key_F2)) <<
        djvShortcut(shortcutLabels()[BOOKMARK_3], QKeySequence(Qt::Key_F3)) <<
        djvShortcut(shortcutLabels()[BOOKMARK_4], QKeySequence(Qt::Key_F4)) <<
        djvShortcut(shortcutLabels()[BOOKMARK_5], QKeySequence(Qt::Key_F5)) <<
        djvShortcut(shortcutLabels()[BOOKMARK_6], QKeySequence(Qt::Key_F6)) <<
        djvShortcut(shortcutLabels()[BOOKMARK_7], QKeySequence(Qt::Key_F7)) <<
        djvShortcut(shortcutLabels()[BOOKMARK_8], QKeySequence(Qt::Key_F8)) <<
        djvShortcut(shortcutLabels()[BOOKMARK_9], QKeySequence(Qt::Key_F9)) <<
        djvShortcut(shortcutLabels()[BOOKMARK_10], QKeySequence(Qt::Key_F10)) <<
        djvShortcut(shortcutLabels()[BOOKMARK_11], QKeySequence(Qt::Key_F11)) <<
        djvShortcut(shortcutLabels()[BOOKMARK_12], QKeySequence(Qt::Key_F12));

    return data;
}

const QVector<djvShortcut> & djvFileBrowserPrefs::shortcuts() const
{
    return _p->shortcuts;
}

void djvFileBrowserPrefs::setPathDefault(const QString & in)
{
    _pathDefault = in;
}

void djvFileBrowserPrefs::setSequence(djvSequence::COMPRESS in)
{
    if (in == _p->sequence)
        return;

    _p->sequence = in;

    Q_EMIT sequenceChanged(_p->sequence);
    Q_EMIT prefChanged();
}

void djvFileBrowserPrefs::setShowHidden(bool show)
{
    if (show == _p->showHidden)
        return;

    _p->showHidden = show;

    Q_EMIT showHiddenChanged(_p->showHidden);
    Q_EMIT prefChanged();
}

void djvFileBrowserPrefs::setSort(djvFileBrowserModel::COLUMNS sort)
{
    if (sort == _p->sort)
        return;

    _p->sort = sort;

    Q_EMIT sortChanged(_p->sort);
    Q_EMIT prefChanged();
}

void djvFileBrowserPrefs::setReverseSort(bool value)
{
    if (value == _p->reverseSort)
        return;

    _p->reverseSort = value;

    Q_EMIT reverseSortChanged(_p->reverseSort);
    Q_EMIT prefChanged();
}

void djvFileBrowserPrefs::setSortDirsFirst(bool sort)
{
    if (sort == _p->sortDirsFirst)
        return;

    _p->sortDirsFirst = sort;

    Q_EMIT sortDirsFirstChanged(_p->sortDirsFirst);
    Q_EMIT prefChanged();
}

void djvFileBrowserPrefs::setThumbnails(djvFileBrowserModel::THUMBNAILS thumbnails)
{
    if (thumbnails == _p->thumbnails)
        return;

    _p->thumbnails = thumbnails;

    djvFileBrowserCache::global()->clear();
    
    Q_EMIT thumbnailsChanged(_p->thumbnails);
    Q_EMIT prefChanged();
}

void djvFileBrowserPrefs::setThumbnailsSize(djvFileBrowserModel::THUMBNAILS_SIZE size)
{
    if (size == _p->thumbnailsSize)
        return;

    _p->thumbnailsSize = size;

    djvFileBrowserCache::global()->clear();
    
    Q_EMIT thumbnailsSizeChanged(_p->thumbnailsSize);
    Q_EMIT prefChanged();
}

void djvFileBrowserPrefs::setThumbnailsCache(qint64 size)
{
    if (size == _p->thumbnailsCache)
        return;

    _p->thumbnailsCache = size;
    
    djvFileBrowserCache::global()->setMaxCost(_p->thumbnailsCache);

    Q_EMIT thumbnailsCacheChanged(_p->thumbnailsCache);
    Q_EMIT prefChanged();
}

void djvFileBrowserPrefs::setRecent(const QStringList & in)
{
    if (in == _p->recent)
        return;

    _p->recent = in;

    Q_EMIT recentChanged(_p->recent);
    Q_EMIT prefChanged();
}

void djvFileBrowserPrefs::addRecent(const QString & in)
{
    const QStringList tmp = _p->recent;

    djvFileInfoUtil::recent(in, _p->recent);

    if (_p->recent != tmp)
    {
        Q_EMIT recentChanged(_p->recent);
        Q_EMIT prefChanged();
    }
}

void djvFileBrowserPrefs::clearRecent()
{
    if (_p->recent.count())
    {
        _p->recent.clear();

        Q_EMIT recentChanged(_p->recent);
        Q_EMIT prefChanged();
    }
}

void djvFileBrowserPrefs::setBookmarks(const QStringList & in)
{
    if (in == _p->bookmarks)
        return;

    _p->bookmarks = in;

    Q_EMIT bookmarksChanged(_p->bookmarks);
    Q_EMIT prefChanged();
}

void djvFileBrowserPrefs::addBookmark(const QString & in)
{
    _p->bookmarks += in;

    Q_EMIT bookmarksChanged(_p->bookmarks);
    Q_EMIT prefChanged();
}

void djvFileBrowserPrefs::setShortcuts(const QVector<djvShortcut> & shortcuts)
{
    if (shortcuts == _p->shortcuts)
        return;

    _p->shortcuts = shortcuts;
    
    Q_EMIT shortcutsChanged(_p->shortcuts);
    Q_EMIT prefChanged();
}
