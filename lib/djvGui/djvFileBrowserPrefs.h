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

//! \file djvFileBrowserPrefs.h

#ifndef DJV_FILE_BROWSER_PREFS_H
#define DJV_FILE_BROWSER_PREFS_H

#include <djvShortcut.h>

#include <djvFileBrowserModel.h>

#include <QObject>

struct djvFileBrowserPrefsPrivate;
class  djvGuiContext;

//! \addtogroup djvGuiDialog
//@{

//------------------------------------------------------------------------------
//! \class djvFileBrowserPrefs
//!
//! This class provides file browser preferences.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvFileBrowserPrefs : public QObject
{
    Q_OBJECT
    Q_ENUMS(SHORTCUT)
    
    //! This property holds the file sequencing.
    
    Q_PROPERTY(
        djvSequence::COMPRESS sequence
        READ                  sequence
        WRITE                 setSequence
        NOTIFY                sequenceChanged)
    
    //! This property holds whether hidden files are shown.
    
    Q_PROPERTY(
        bool   showHidden
        READ   hasShowHidden
        WRITE  setShowHidden
        NOTIFY showHiddenChanged)
    
    //! This property holds the sorting.
    
    Q_PROPERTY(
        djvFileBrowserModel::COLUMNS sort
        READ                         sort
        WRITE                        setSort
        NOTIFY                       sortChanged)
    
    //! This property holds whether sorting is reversed.
    
    Q_PROPERTY(
        bool   reverseSort
        READ   hasReverseSort
        WRITE  setReverseSort
        NOTIFY reverseSortChanged)
    
    //! This property holds whether directories are sorted first.
    
    Q_PROPERTY(
        bool   sortDirsFirst
        READ   hasSortDirsFirst
        WRITE  setSortDirsFirst
        NOTIFY sortDirsFirstChanged)
    
    //! This property holds the image thumbnail mode.
    
    Q_PROPERTY(
        djvFileBrowserModel::THUMBNAILS thumbnails
        READ                            thumbnails
        WRITE                           setThumbnails
        NOTIFY                          thumbnailsChanged)
    
    //! This property holds the image thumbnail size.
    
    Q_PROPERTY(
        djvFileBrowserModel::THUMBNAILS_SIZE thumbnailsSize
        READ                                 thumbnailsSize
        WRITE                                setThumbnailsSize
        NOTIFY                               thumbnailsSizeChanged)

    //! This property holds the image thumbnail cache size.
    
    Q_PROPERTY(
        qint64 thumbnailsCache
        READ   thumbnailsCache
        WRITE  setThumbnailsCache
        NOTIFY thumbnailsCacheChanged)
    
    //! This property holds the list of recent directories.
    
    Q_PROPERTY(
        QStringList recent
        READ        recent
        NOTIFY      recentChanged);

    //! This property holds the list of bookmarks.
    
    Q_PROPERTY(
        QStringList bookmarks
        READ        bookmarks
        WRITE       setBookmarks
        NOTIFY      bookmarksChanged);
    
    //! This property holds the list of shortcuts.
    
    Q_PROPERTY(
        QVector<djvShortcut> shortcuts
        READ                 shortcuts
        WRITE                setShortcuts
        NOTIFY               shortcutsChanged)
    
public:

    //! Constructor.

    explicit djvFileBrowserPrefs(djvGuiContext *, QObject * parent = 0);

    //! Destructor.

    ~djvFileBrowserPrefs();

    //! Get the file system path default.

    static const QString & pathDefault();

    //! Get the file sequencing default.

    static djvSequence::COMPRESS sequenceDefault();

    //! Get the file sequencing.

    djvSequence::COMPRESS sequence() const;

    //! Get the hidden files shown default.

    static bool showHiddenDefault();

    //! Get whether hidden files are shown.

    bool hasShowHidden() const;

    //! Get the sorting default.

    static djvFileBrowserModel::COLUMNS sortDefault();

    //! Get the sorting.

    djvFileBrowserModel::COLUMNS sort() const;

    //! Get the the sorting reversed default.

    static bool reverseSortDefault();

    //! Get whether sorting is reversed.

    bool hasReverseSort() const;

    //! Get the directories sorted first default.

    static bool sortDirsFirstDefault();

    //! Get whether directories are sorted first.

    bool hasSortDirsFirst() const;

    //! Get the image thumbnail mode default.

    static djvFileBrowserModel::THUMBNAILS thumbnailsDefault();

    //! Get the image thumbnail mode.

    djvFileBrowserModel::THUMBNAILS thumbnails() const;

    //! Get the image thumbnail size default.

    static djvFileBrowserModel::THUMBNAILS_SIZE thumbnailsSizeDefault();

    //! Get the image thumbnail size.

    djvFileBrowserModel::THUMBNAILS_SIZE thumbnailsSize() const;

    //! Get the image thumbnail cache size default.
    
    static qint64 thumbnailsCacheDefault();

    //! Get the image thumbnail cache size.
    
    qint64 thumbnailsCache() const;

    //! Get the list of recent directories.

    const QStringList & recent() const;

    //! Get the list of bookmarks.

    const QStringList & bookmarks() const;

    //! This enumeration provides the keyboard shortcuts.

    enum SHORTCUT
    {
        UP,
        PREV,
        CURRENT,
        HOME,
        DESKTOP,
        ROOT,
        RELOAD,
        SEQ_OFF,
        SEQ_SPARSE,
        SEQ_RANGE,
        SHOW_HIDDEN,
        SORT_NAME,
        SORT_SIZE,
#if ! defined(DJV_WINDOWS)
        SORT_USER,
#endif
        SORT_PERMISSIONS,
        SORT_TIME,
        REVERSE_SORT,
        SORT_DIRS_FIRST,
        ADD_BOOKMARK,
        BOOKMARK_1,
        BOOKMARK_2,
        BOOKMARK_3,
        BOOKMARK_4,
        BOOKMARK_5,
        BOOKMARK_6,
        BOOKMARK_7,
        BOOKMARK_8,
        BOOKMARK_9,
        BOOKMARK_10,
        BOOKMARK_11,
        BOOKMARK_12,

        SHORTCUT_COUNT
    };

    //! Get the shortcut labels.

    static const QStringList & shortcutLabels();
    
    //! Get the shortcut defaults.
    
    static const QVector<djvShortcut> & shortcutsDefault();

    //! Get the shortcuts.

    const QVector<djvShortcut> & shortcuts() const;

public Q_SLOTS:

    //! Set the file system path default.

    static void setPathDefault(const QString &);

    //! Set the file sequencing.

    void setSequence(djvSequence::COMPRESS);
    
    //! Set whether hidden files are shown.

    void setShowHidden(bool);

    //! Set the sorting.

    void setSort(djvFileBrowserModel::COLUMNS);

    //! Set whether sorting is reversed.

    void setReverseSort(bool);

    //! Set whether directories are sorted first.

    void setSortDirsFirst(bool);

    //! Set the image thumbnail mode.

    void setThumbnails(djvFileBrowserModel::THUMBNAILS);

    //! Set the image thumbnail size.

    void setThumbnailsSize(djvFileBrowserModel::THUMBNAILS_SIZE);

    //! Set the image thumbnail cache size.
    
    void setThumbnailsCache(qint64);

    //! Set the list of recent directories.

    void setRecent(const QStringList &);

    //! Add a recent directory.

    void addRecent(const QString &);

    //! Clear the list of recent directories.

    void clearRecent();
    
    //! Set the list of bookmarks.

    void setBookmarks(const QStringList &);

    //! Add a bookmark.

    void addBookmark(const QString &);

    //! Set the list of shortcuts.

    void setShortcuts(const QVector<djvShortcut> &);

Q_SIGNALS:

    //! This signal is emitted when the file sequencing is changed.

    void sequenceChanged(djvSequence::COMPRESS);

    //! This signal is emitted when the hidden files are changed.

    void showHiddenChanged(bool);

    //! This signal is emitted when the sorting is changed.

    void sortChanged(djvFileBrowserModel::COLUMNS);

    //! This signal is emitted when reverse sorting is changed.

    void reverseSortChanged(bool);

    //! This signal is emitted when sort directories first is changed.

    void sortDirsFirstChanged(bool);

    //! This signal is emitted when the image thumbnail mode is changed.

    void thumbnailsChanged(djvFileBrowserModel::THUMBNAILS);

    //! This signal is emitted when the image thumbnail size is changed.

    void thumbnailsSizeChanged(djvFileBrowserModel::THUMBNAILS_SIZE);

    //! This signal is emitted when the image thumbnail cache size is changed.
    
    void thumbnailsCacheChanged(qint64);

    //! This signal is emitted when the recent directories are changed.
    
    void recentChanged(const QStringList &);

    //! This signal is emitted when the bookmarks are changed.
    
    void bookmarksChanged(const QStringList &);

    //! This signal is emitted when the list of shortcuts are changed.

    void shortcutsChanged(const QVector<djvShortcut> &);

    //! This signal is emitted when a preference is changed.

    void prefChanged();

private:

    DJV_PRIVATE_COPY(djvFileBrowserPrefs);
    
    djvFileBrowserPrefsPrivate * _p;
};

//@} // djvGuiDialog

#endif // DJV_FILE_BROWSER_PREFS_H

