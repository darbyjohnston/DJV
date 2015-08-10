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

//! \file djvFileBrowserTestModel.h

#ifndef DJV_FILE_BROWSER_TEST_MODEL_H
#define DJV_FILE_BROWSER_TEST_MODEL_H

#include <djvFileBrowserTestDir.h>
#include <djvFileBrowserTestInfo.h>
#include <djvFileBrowserTestThumbnail.h>
#include <djvFileBrowserTestUtil.h>

#include <djvImageIo.h>

#include <QAbstractItemModel>
#include <QDir>

struct djvFileBrowserTestModelPrivate;

class djvGuiContext;

//------------------------------------------------------------------------------
//! \class djvFileBrowserTestModel
//!
//! This class provides the data model for a file browser.
//------------------------------------------------------------------------------

class djvFileBrowserTestModel : public QAbstractItemModel
{
    Q_OBJECT
    
    //! This property holds the current directory.
    
    Q_PROPERTY(
        QDir   dir
        READ   dir
        WRITE  setDir
        NOTIFY dirChanged)
    
    //! This property holds the current directory's path.
    
    Q_PROPERTY(
        QString path
        READ    path
        WRITE   setPath
        NOTIFY  pathChanged)
    
    //! This property holds the file sequencing.
    
    Q_PROPERTY(
        djvSequence::COMPRESS sequence
        READ                  sequence
        WRITE                 setSequence
        NOTIFY                sequenceChanged)
    
    //! This property holds the filter text.
    
    Q_PROPERTY(
        QString filterText
        READ    filterText
        WRITE   setFilterText
        NOTIFY  filterTextChanged)
    
    //! This property holds whether hidden files are shown.
    
    Q_PROPERTY(
        bool   showHidden
        READ   hasShowHidden
        WRITE  setShowHidden
        NOTIFY showHiddenChanged)
    
    //! This property holds the sorting.
    
    Q_PROPERTY(
        djvFileBrowserTestUtil::COLUMNS sort
        READ                            sort
        WRITE                           setSort
        NOTIFY                          sortChanged)
    
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
        djvFileBrowserTestUtil::THUMBNAILS thumbnails
        READ                               thumbnails
        WRITE                              setThumbnails
        NOTIFY                             thumbnailsChanged)
    
    //! This property holds the image thumbnail size.
    
    Q_PROPERTY(
        int    thumbnailSize
        READ   thumbnailSize
        WRITE  setThumbnailSize
        NOTIFY thumbnailSizeChanged)
    
public:

    //! This constant provides a role for SVG file names.
    
    static const int SvgRole = Qt::UserRole + 1;
    
    //! Constructor.

    explicit djvFileBrowserTestModel(djvGuiContext *, QObject * parent = 0);
    
    //! Destructor.
    
    virtual ~djvFileBrowserTestModel();
    
    //! Get the current directory.
    
    const QDir & dir() const;
    
    //! Get the current directory's path.
    
    QString path() const;
    
    //! Get the file sequencing.
    
    djvSequence::COMPRESS sequence() const;
    
    //! Get the filter text.
    
    const QString & filterText() const;

    //! Get whether hidden files are shown.

    bool hasShowHidden() const;

    //! Get the sorting.

    djvFileBrowserTestUtil::COLUMNS sort() const;

    //! Get whether sorting is reversed.

    bool hasReverseSort() const;

    //! Get whether directories are sorted first.

    bool hasSortDirsFirst() const;

    //! Get the image thumbnail mode.

    djvFileBrowserTestUtil::THUMBNAILS thumbnails() const;

    //! Get the image thumbnail size.

    int thumbnailSize() const;

    virtual int columnCount(
        const QModelIndex & parent = QModelIndex()) const;

    virtual QVariant data(
        const QModelIndex & index,
        int                 role = Qt::DisplayRole) const;

    virtual Qt::ItemFlags flags(const QModelIndex &) const;

    virtual QVariant headerData(
        int             section,
        Qt::Orientation orientation,
        int             role = Qt::DisplayRole) const;

    virtual QModelIndex	index(
        int                 row,
        int                 column,
        const QModelIndex & parent = QModelIndex()) const;
    
    virtual QModelIndex	parent(
        const QModelIndex & = QModelIndex()) const;

    virtual QHash<int, QByteArray> roleNames() const;
    
    virtual int rowCount(
        const QModelIndex & parent = QModelIndex()) const;

public Q_SLOTS:

    //! Set the current directory.

    void setDir(const QDir &);
    
    //! Set the current directory's path.
    
    void setPath(const QString &);
    
    //! Set the file sequencing.
    
    void setSequence(djvSequence::COMPRESS);
    
    //! Go up a directory.
    
    void up();
    
    //! Go to the previous directory.
    
    void back();
    
    //! Reload the current directory.
    
    void reload();
    
    //! Set the filter text.
    
    void setFilterText(const QString &);

    //! Set whether hidden files are shown.

    void setShowHidden(bool);

    //! Set the sorting.

    void setSort(djvFileBrowserTestUtil::COLUMNS);

    //! Set whether sorting is reversed.

    void setReverseSort(bool);

    //! Set whether directories are sorted first.

    void setSortDirsFirst(bool);

    //! Set the image thumbnail mode.

    void setThumbnails(djvFileBrowserTestUtil::THUMBNAILS);

    //! Set the image thumbnail size.

    void setThumbnailSize(int);

Q_SIGNALS:

    //! This signal is emitted when the current directory is changed.
    
    void dirChanged(const QDir &);

    //! This signal is emitted when the current directory's path is changed.
    
    void pathChanged(const QString &);

    //! This signal is emitted when the file sequencing is changed.
    
    void sequenceChanged(djvSequence::COMPRESS);
    
    //! This signal is emitted when the filter text is changed.
    
    void filterTextChanged(const QString &);

    //! This signal is emitted when the hidden files are changed.
    
    void showHiddenChanged(bool);
    
    //! This signal is emitted when the sorting is changed.
    
    void sortChanged(djvFileBrowserTestUtil::COLUMNS);
    
    //! This signal is emitted when reverse sorting is changed.
    
    void reverseSortChanged(bool);
    
    //! This signal is emitted when sort directories first is changed.
    
    void sortDirsFirstChanged(bool);

    //! This signal is emitted when the thumbnail mode is changed.

    void thumbnailsChanged(djvFileBrowserTestUtil::THUMBNAILS);

    //! This signal is emitted when the thumbnail size is changed.

    void thumbnailSizeChanged(int);
    
    //! This signal is emitted to request a directory's contents.
    
    void requestDir(const djvFileBrowserTestDirRequest &);
    
    //! This signal is emitted when the request for a directory's contents is
    //! completed.
    
    void requestDirComplete();
    
protected:

    virtual void timerEvent(QTimerEvent *);
    
private Q_SLOTS:

    void dirCallback(const djvFileBrowserTestDirResult &);
    void infoCallback(const djvFileBrowserTestInfoResult &);
    void thumbnailCallback(const djvFileBrowserTestThumbnailResult &);

private:

    void nextId();

    djvFileBrowserTestDirRequest dirRequest() const;

    djvFileBrowserTestInfoRequest infoRequest(
        const djvFileInfo & fileInfo,
        int                 row) const;
    
    djvFileBrowserTestThumbnailRequest thumbnailRequest(
        const djvFileInfo & fileInfo,
        int                 row) const;
    
    DJV_PRIVATE_COPY(djvFileBrowserTestModel);
    
    djvFileBrowserTestModelPrivate * _p;
};

#endif // DJV_FILE_BROWSER_TEST_MODEL_H

