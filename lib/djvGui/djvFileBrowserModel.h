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

//! \file djvFileBrowserModel.h

#ifndef DJV_FILE_BROWSER_MODEL_H
#define DJV_FILE_BROWSER_MODEL_H

#include <djvGuiExport.h>

#include <djvUtil.h>
#include <djvFileInfo.h>

#include <QAbstractItemModel>

class  djvFileBrowserItem;
struct djvFileBrowserModelPrivate;
class  djvGuiContext;

//------------------------------------------------------------------------------
//! \class djvFileBrowserModel
//!
//! This class provides a file browser model.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvFileBrowserModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_ENUMS(COLUMNS)
    Q_ENUMS(THUMBNAILS)
    Q_ENUMS(THUMBNAILS_SIZE)
    
    //! This property holds the path.
    
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

public:

    //! This enumeration provides the columns.
    
    enum COLUMNS
    {
        NAME,
        SIZE,
#if ! defined(DJV_WINDOWS)
        USER,
#endif
        PERMISSIONS,
        TIME,
        
        COLUMNS_COUNT
    };

    //! Get the column labels.

    static const QStringList & columnsLabels();
    
    //! Constructor.
    
    explicit djvFileBrowserModel(djvGuiContext *, QObject * parent = 0);

    //! Destructor.
    
    virtual ~djvFileBrowserModel();

    //! Get the path.

    const QString & path() const;
    
    //! Get the list of files.
    
    const djvFileInfoList & contents() const;
    
    //! Convert a model index to file information.
    
    djvFileInfo fileInfo(const QModelIndex &) const;
    
    //! Get the file sequencing.

    djvSequence::COMPRESS sequence() const;
    
    //! Get the filter text.
    
    const QString & filterText() const;

    //! Get whether hidden files are shown.

    bool hasShowHidden() const;

    //! Get the sorting.

    COLUMNS sort() const;

    //! Get whether sorting is reversed.

    bool hasReverseSort() const;

    //! Get whether directories are sorted first.

    bool hasSortDirsFirst() const;

    //! This enumeration provides the image thumbnail mode.

    enum THUMBNAILS
    {
        THUMBNAILS_OFF,
        THUMBNAILS_LOW,
        THUMBNAILS_HIGH,

        THUMBNAILS_COUNT
    };

    //! Get the image thumbnail mode labels.

    static const QStringList & thumbnailsLabels();

    //! Get the image thumbnail mode.

    THUMBNAILS thumbnails() const;

    //! This enumeration provides the image thumbnail size.

    enum THUMBNAILS_SIZE
    {
        THUMBNAILS_SMALL,
        THUMBNAILS_MEDIUM,
        THUMBNAILS_LARGE,

        THUMBNAILS_SIZE_COUNT
    };

    //! Get the image thumbnail size labels.

    static const QStringList & thumbnailsSizeLabels();

    //! Get the image thumbnail size value.

    static int thumbnailsSizeValue(THUMBNAILS_SIZE);

    //! Get the image thumbnail size.

    THUMBNAILS_SIZE thumbnailsSize() const;

    virtual QModelIndex	index(
        int                 row,
        int                 column,
        const QModelIndex & parent = QModelIndex()) const;
    
    virtual QModelIndex	parent(
        const QModelIndex & = QModelIndex()) const;
    
    virtual QVariant headerData(
        int             section,
        Qt::Orientation orientation,
        int             role = Qt::DisplayRole) const;
    
    virtual QVariant data(
        const QModelIndex & index,
        int                 role = Qt::DisplayRole) const;

    virtual Qt::ItemFlags flags(const QModelIndex &) const;
    
    virtual int rowCount(
        const QModelIndex & parent = QModelIndex()) const;

    virtual int columnCount(
        const QModelIndex & parent = QModelIndex()) const;

    virtual QStringList mimeTypes() const;

    virtual QMimeData * mimeData(const QModelIndexList &) const;

public Q_SLOTS:

    //! Set the path.

    void setPath(const QString &);
    
    //! Reload the files.
    
    void reload();

    //! Set the file sequencing.

    void setSequence(djvSequence::COMPRESS);
    
    //! Set the filter text.
    
    void setFilterText(const QString &);

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

Q_SIGNALS:

    //! This signal is emitted when the path is changed.
    
    void pathChanged(const QString &);

    //! This signal is emitted when the file sequencing is changed.
    
    void sequenceChanged(djvSequence::COMPRESS);
    
    //! This signal is emitted when the filter text is changed.
    
    void filterTextChanged(const QString &);

    //! This signal is emitted when the hidden files are changed.
    
    void showHiddenChanged(bool);
    
    //! This signal is emitted when the sorting is changed.
    
    void sortChanged(djvFileBrowserModel::COLUMNS);
    
    //! This signal is emitted when reverse sorting is changed.
    
    void reverseSortChanged(bool);
    
    //! This signal is emitted when sort directories first is changed.
    
    void sortDirsFirstChanged(bool);

    //! This signal is emitted when the thumbnail mode is changed.

    void thumbnailsChanged(djvFileBrowserModel::THUMBNAILS);

    //! This signal is emitted when the thumbnail size is changed.

    void thumbnailsSizeChanged(djvFileBrowserModel::THUMBNAILS_SIZE);
    
    //! This signal is emitted when an option is changed.
    
    void optionChanged();
   
private Q_SLOTS:

    void imageInfoCallback();
    void thumbnailCallback();

private:

    void dirUpdate();
    void modelUpdate();

    DJV_PRIVATE_COPY(djvFileBrowserModel);
    
    djvFileBrowserModelPrivate * _p;
};

//------------------------------------------------------------------------------

DJV_STRING_OPERATOR(DJV_GUI_EXPORT, djvFileBrowserModel::COLUMNS);
DJV_STRING_OPERATOR(DJV_GUI_EXPORT, djvFileBrowserModel::THUMBNAILS);
DJV_STRING_OPERATOR(DJV_GUI_EXPORT, djvFileBrowserModel::THUMBNAILS_SIZE);

#endif // DJV_FILE_BROWSER_MODEL_H

