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

#include <djvUI/FileBrowserPrefs.h>

#include <djvUI/FileBrowserCache.h>
#include <djvUI/UIContext.h>
#include <djvUI/Prefs.h>

#include <djvCore/Assert.h>
#include <djvCore/FileInfoUtil.h>
#include <djvCore/Memory.h>

#include <QApplication>

namespace djv
{
    namespace UI
    {
        struct FileBrowserPrefs::Private
        {
            Private(UIContext * context) :
                context(context)
            {}

            djvSequence::COMPRESS sequence = FileBrowserPrefs::sequenceDefault();
            bool showHidden = FileBrowserPrefs::showHiddenDefault();
            FileBrowserModel::COLUMNS columnsSort = FileBrowserPrefs::columnsSortDefault();
            bool reverseSort = FileBrowserPrefs::reverseSortDefault();
            bool sortDirsFirst = FileBrowserPrefs::sortDirsFirstDefault();
            FileBrowserModel::THUMBNAILS thumbnails = FileBrowserPrefs::thumbnailsDefault();
            FileBrowserModel::THUMBNAILS_SIZE thumbnailsSize = FileBrowserPrefs::thumbnailsSizeDefault();
            qint64 thumbnailsCache = FileBrowserPrefs::thumbnailsCacheDefault();
            QStringList recent;
            QStringList bookmarks;
            QVector<Shortcut> shortcuts = FileBrowserPrefs::shortcutsDefault();
            UIContext * context = nullptr;
        };

        namespace
        {
            QString _pathDefault = ".";

        } // namespace

        FileBrowserPrefs::FileBrowserPrefs(UIContext * context, QObject * parent) :
            QObject(parent),
            _p(new Private(context))
        {
            //DJV_DEBUG("FileBrowserPrefs::FileBrowserPrefs");
            Prefs prefs("djv::UI::FileBrowserPrefs");
            prefs.get("sequence", _p->sequence);
            prefs.get("showHidden", _p->showHidden);
            prefs.get("columnsSort", _p->columnsSort);
            prefs.get("reverseSort", _p->reverseSort);
            prefs.get("sortDirsFirst", _p->sortDirsFirst);
            prefs.get("thumbnails", _p->thumbnails);
            prefs.get("thumbnailsSize", _p->thumbnailsSize);
            prefs.get("thumbnailsCache", _p->thumbnailsCache);
            prefs.get("recent", _p->recent);
            prefs.get("bookmarks", _p->bookmarks);
            if (_p->recent.count() > djvFileInfoUtil::recentMax)
                _p->recent = _p->recent.mid(0, djvFileInfoUtil::recentMax);
            Prefs shortcutsPrefs("djv::UI::FileBrowserPrefs/Shortcuts");
            for (int i = 0; i < _p->shortcuts.count(); ++i)
            {
                QStringList s;
                if (shortcutsPrefs.get(_p->shortcuts[i].name, s))
                {
                    Shortcut::serialize(s, _p->shortcuts[i].value);
                }
            }
        }

        FileBrowserPrefs::~FileBrowserPrefs()
        {
            //DJV_DEBUG("FileBrowserPrefs::~FileBrowserPrefs");
            Prefs prefs("djv::UI::FileBrowserPrefs");
            prefs.set("sequence", _p->sequence);
            prefs.set("showHidden", _p->showHidden);
            prefs.set("columnsSort", _p->columnsSort);
            prefs.set("reverseSort", _p->reverseSort);
            prefs.set("sortDirsFirst", _p->sortDirsFirst);
            prefs.set("thumbnails", _p->thumbnails);
            prefs.set("thumbnailsSize", _p->thumbnailsSize);
            prefs.set("thumbnailsCache", _p->thumbnailsCache);
            prefs.set("recent", _p->recent);
            prefs.set("bookmarks", _p->bookmarks);
            Prefs shortcutsPrefs("djv::UI::FileBrowserPrefs/Shortcuts");
            for (int i = 0; i < _p->shortcuts.count(); ++i)
            {
                shortcutsPrefs.set(
                    _p->shortcuts[i].name,
                    Shortcut::serialize(_p->shortcuts[i].value));
            }
        }

        const QString & FileBrowserPrefs::pathDefault()
        {
            return _pathDefault;
        }

        djvSequence::COMPRESS FileBrowserPrefs::sequenceDefault()
        {
            return djvSequence::COMPRESS_RANGE;
        }

        djvSequence::COMPRESS FileBrowserPrefs::sequence() const
        {
            return _p->sequence;
        }

        bool FileBrowserPrefs::showHiddenDefault()
        {
            return false;
        }

        bool FileBrowserPrefs::hasShowHidden() const
        {
            return _p->showHidden;
        }

        FileBrowserModel::COLUMNS FileBrowserPrefs::columnsSortDefault()
        {
            return FileBrowserModel::NAME;
        }

        FileBrowserModel::COLUMNS FileBrowserPrefs::columnsSort() const
        {
            return _p->columnsSort;
        }

        bool FileBrowserPrefs::reverseSortDefault()
        {
            return false;
        }

        bool FileBrowserPrefs::hasReverseSort() const
        {
            return _p->reverseSort;
        }

        bool FileBrowserPrefs::sortDirsFirstDefault()
        {
            return true;
        }

        bool FileBrowserPrefs::hasSortDirsFirst() const
        {
            return _p->sortDirsFirst;
        }

        FileBrowserModel::THUMBNAILS FileBrowserPrefs::thumbnailsDefault()
        {
            return FileBrowserModel::THUMBNAILS_HIGH;
        }

        FileBrowserModel::THUMBNAILS FileBrowserPrefs::thumbnails() const
        {
            return _p->thumbnails;
        }

        FileBrowserModel::THUMBNAILS_SIZE FileBrowserPrefs::thumbnailsSizeDefault()
        {
            return FileBrowserModel::THUMBNAILS_MEDIUM;
        }

        FileBrowserModel::THUMBNAILS_SIZE FileBrowserPrefs::thumbnailsSize() const
        {
            return _p->thumbnailsSize;
        }

        qint64 FileBrowserPrefs::thumbnailsCacheDefault()
        {
            return 128 * djvMemory::megabyte;
        }

        qint64 FileBrowserPrefs::thumbnailsCache() const
        {
            return _p->thumbnailsCache;
        }

        const QStringList & FileBrowserPrefs::recent() const
        {
            return _p->recent;
        }

        const QStringList & FileBrowserPrefs::bookmarks() const
        {
            return _p->bookmarks;
        }

        const QStringList & FileBrowserPrefs::shortcutLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Up") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Previous") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Current") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Home") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Desktop") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Root") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Reload") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Sequence Off") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Sequence Sparse") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Sequence Range") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Show Hidden") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Sort Name") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Sort Size") <<
#if ! defined(DJV_WINDOWS)
                qApp->translate("djv::UI::FileBrowserPrefs", "SortUser") <<
#endif
                qApp->translate("djv::UI::FileBrowserPrefs", "Sort Time") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Sort Permissions") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Reverse Sort") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Sort Dirs First") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Add Bookmark") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Bookmark 1") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Bookmark 2") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Bookmark 3") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Bookmark 4") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Bookmark 5") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Bookmark 6") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Bookmark 7") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Bookmark 8") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Bookmark 9") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Bookmark 10") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Bookmark 11") <<
                qApp->translate("djv::UI::FileBrowserPrefs", "Bookmark 12");
            DJV_ASSERT(data.count() == SHORTCUT_COUNT);
            return data;
        }

        const QVector<Shortcut> & FileBrowserPrefs::shortcutsDefault()
        {
            static const QVector<Shortcut> data = QVector<Shortcut>() <<
                Shortcut(shortcutLabels()[UP], QKeySequence("Ctrl+\\")) <<
                Shortcut(shortcutLabels()[PREV], QKeySequence(Qt::Key_Backspace)) <<
                Shortcut(shortcutLabels()[CURRENT], QKeySequence("Ctrl+.")) <<
                Shortcut(shortcutLabels()[HOME], QKeySequence("Ctrl+,")) <<
                Shortcut(shortcutLabels()[DESKTOP], QKeySequence("Ctrl+k")) <<
                Shortcut(shortcutLabels()[ROOT], QKeySequence("Ctrl+/")) <<
                Shortcut(shortcutLabels()[RELOAD], QKeySequence("Ctrl+r")) <<
                Shortcut(shortcutLabels()[SEQ_OFF], QKeySequence("Ctrl+q")) <<
                Shortcut(shortcutLabels()[SEQ_SPARSE], QKeySequence("Ctrl+a")) <<
                Shortcut(shortcutLabels()[SEQ_RANGE], QKeySequence("Ctrl+z")) <<
                Shortcut(shortcutLabels()[SHOW_HIDDEN], QKeySequence("Ctrl+n")) <<
                Shortcut(shortcutLabels()[SORT_NAME], QKeySequence("Ctrl+1")) <<
                Shortcut(shortcutLabels()[SORT_SIZE], QKeySequence("Ctrl+2")) <<
#if ! defined(DJV_WINDOWS)
                Shortcut(shortcutLabels()[SORT_USER], QKeySequence("Ctrl+3")) <<
                Shortcut(shortcutLabels()[SORT_TIME], QKeySequence("Ctrl+4")) <<
                Shortcut(shortcutLabels()[SORT_PERMISSIONS], QKeySequence("Ctrl+5")) <<
#else
                Shortcut(shortcutLabels()[SORT_TIME], QKeySequence("Ctrl+3")) <<
                Shortcut(shortcutLabels()[SORT_PERMISSIONS], QKeySequence("Ctrl+4")) <<
#endif
                Shortcut(shortcutLabels()[REVERSE_SORT], QKeySequence("Ctrl+`")) <<
                Shortcut(shortcutLabels()[SORT_DIRS_FIRST], QKeySequence("Ctrl+d")) <<
                Shortcut(shortcutLabels()[ADD_BOOKMARK], QKeySequence("Ctrl+b")) <<
                Shortcut(shortcutLabels()[BOOKMARK_1], QKeySequence(Qt::Key_F1)) <<
                Shortcut(shortcutLabels()[BOOKMARK_2], QKeySequence(Qt::Key_F2)) <<
                Shortcut(shortcutLabels()[BOOKMARK_3], QKeySequence(Qt::Key_F3)) <<
                Shortcut(shortcutLabels()[BOOKMARK_4], QKeySequence(Qt::Key_F4)) <<
                Shortcut(shortcutLabels()[BOOKMARK_5], QKeySequence(Qt::Key_F5)) <<
                Shortcut(shortcutLabels()[BOOKMARK_6], QKeySequence(Qt::Key_F6)) <<
                Shortcut(shortcutLabels()[BOOKMARK_7], QKeySequence(Qt::Key_F7)) <<
                Shortcut(shortcutLabels()[BOOKMARK_8], QKeySequence(Qt::Key_F8)) <<
                Shortcut(shortcutLabels()[BOOKMARK_9], QKeySequence(Qt::Key_F9)) <<
                Shortcut(shortcutLabels()[BOOKMARK_10], QKeySequence(Qt::Key_F10)) <<
                Shortcut(shortcutLabels()[BOOKMARK_11], QKeySequence(Qt::Key_F11)) <<
                Shortcut(shortcutLabels()[BOOKMARK_12], QKeySequence(Qt::Key_F12));
            return data;
        }

        const QVector<Shortcut> & FileBrowserPrefs::shortcuts() const
        {
            return _p->shortcuts;
        }

        void FileBrowserPrefs::setPathDefault(const QString & in)
        {
            _pathDefault = in;
        }

        void FileBrowserPrefs::setSequence(djvSequence::COMPRESS in)
        {
            if (in == _p->sequence)
                return;
            _p->sequence = in;
            Q_EMIT sequenceChanged(_p->sequence);
            Q_EMIT prefChanged();
        }

        void FileBrowserPrefs::setShowHidden(bool show)
        {
            if (show == _p->showHidden)
                return;
            _p->showHidden = show;
            Q_EMIT showHiddenChanged(_p->showHidden);
            Q_EMIT prefChanged();
        }

        void FileBrowserPrefs::setColumnsSort(FileBrowserModel::COLUMNS value)
        {
            if (value == _p->columnsSort)
                return;
            _p->columnsSort = value;
            Q_EMIT columnsSortChanged(_p->columnsSort);
            Q_EMIT prefChanged();
        }

        void FileBrowserPrefs::setReverseSort(bool value)
        {
            if (value == _p->reverseSort)
                return;
            _p->reverseSort = value;
            Q_EMIT reverseSortChanged(_p->reverseSort);
            Q_EMIT prefChanged();
        }

        void FileBrowserPrefs::setSortDirsFirst(bool sort)
        {
            if (sort == _p->sortDirsFirst)
                return;
            _p->sortDirsFirst = sort;
            Q_EMIT sortDirsFirstChanged(_p->sortDirsFirst);
            Q_EMIT prefChanged();
        }

        void FileBrowserPrefs::setThumbnails(FileBrowserModel::THUMBNAILS thumbnails)
        {
            if (thumbnails == _p->thumbnails)
                return;
            _p->thumbnails = thumbnails;
            _p->context->fileBrowserCache()->clear();
            Q_EMIT thumbnailsChanged(_p->thumbnails);
            Q_EMIT prefChanged();
        }

        void FileBrowserPrefs::setThumbnailsSize(FileBrowserModel::THUMBNAILS_SIZE size)
        {
            if (size == _p->thumbnailsSize)
                return;
            _p->thumbnailsSize = size;
            _p->context->fileBrowserCache()->clear();
            Q_EMIT thumbnailsSizeChanged(_p->thumbnailsSize);
            Q_EMIT prefChanged();
        }

        void FileBrowserPrefs::setThumbnailsCache(qint64 size)
        {
            if (size == _p->thumbnailsCache)
                return;
            _p->thumbnailsCache = size;
            _p->context->fileBrowserCache()->setMaxCost(_p->thumbnailsCache);
            Q_EMIT thumbnailsCacheChanged(_p->thumbnailsCache);
            Q_EMIT prefChanged();
        }

        void FileBrowserPrefs::setRecent(const QStringList & in)
        {
            if (in == _p->recent)
                return;
            _p->recent = in;
            Q_EMIT recentChanged(_p->recent);
            Q_EMIT prefChanged();
        }

        void FileBrowserPrefs::addRecent(const QString & in)
        {
            const QStringList tmp = _p->recent;
            djvFileInfoUtil::recent(in, _p->recent);
            if (_p->recent != tmp)
            {
                Q_EMIT recentChanged(_p->recent);
                Q_EMIT prefChanged();
            }
        }

        void FileBrowserPrefs::clearRecent()
        {
            if (_p->recent.count())
            {
                _p->recent.clear();
                Q_EMIT recentChanged(_p->recent);
                Q_EMIT prefChanged();
            }
        }

        void FileBrowserPrefs::setBookmarks(const QStringList & in)
        {
            if (in == _p->bookmarks)
                return;
            _p->bookmarks = in;
            Q_EMIT bookmarksChanged(_p->bookmarks);
            Q_EMIT prefChanged();
        }

        void FileBrowserPrefs::addBookmark(const QString & in)
        {
            _p->bookmarks += in;
            Q_EMIT bookmarksChanged(_p->bookmarks);
            Q_EMIT prefChanged();
        }

        void FileBrowserPrefs::setShortcuts(const QVector<Shortcut> & shortcuts)
        {
            if (shortcuts == _p->shortcuts)
                return;
            _p->shortcuts = shortcuts;
            Q_EMIT shortcutsChanged(_p->shortcuts);
            Q_EMIT prefChanged();
        }

    } // namespace UI
} // namespace djv
