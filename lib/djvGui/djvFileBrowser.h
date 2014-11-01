//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvFileBrowser.h

#ifndef DJV_FILE_BROWSER_H
#define DJV_FILE_BROWSER_H

#include <djvGuiExport.h>

#include <djvFileInfo.h>
#include <djvUtil.h>

#include <QDialog>

class djvFileBrowserModel;

class QModelIndex;

//! \addtogroup djvGuiDialog
//@{

//------------------------------------------------------------------------------
//! \class djvFileBrowser
//!
//! This class provides a file browser dialog. The file browser dialog is used
//! to load or save files.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvFileBrowser : public QDialog
{
    Q_OBJECT
    
    //! This property holds the file information.
    
    Q_PROPERTY(
        djvFileInfo fileInfo
        READ        fileInfo
        WRITE       setFileInfo
        NOTIFY      fileInfoChanged)
    
public:

    //! Constructor.

    explicit djvFileBrowser(QWidget * parent = 0);
    
    //! Destructor.
    
    virtual ~djvFileBrowser();

    //! Get the file information.

    const djvFileInfo & fileInfo() const;
    
    //! Get the global file browser.
    
    static djvFileBrowser * global(const QString & title = QString());
    
public Q_SLOTS:

    //! Set the file information.

    void setFileInfo(const djvFileInfo &);

Q_SIGNALS:

    //! This signal is emitted when the file information is changed.

    void fileInfoChanged(const djvFileInfo &);

protected:

    virtual void showEvent(QShowEvent *);

private Q_SLOTS:

    void fileCallback();
    void browserCallback(const QModelIndex &);
    void browserCurrentCallback(const QModelIndex &, const QModelIndex &);
    void upCallback();
    void prevCallback();
    void currentCallback();
    void recentCallback(QAction *);
    void drivesCallback(QAction  *);
    void homeCallback();
    void desktopCallback();
    void reloadCallback();
    void thumbnailsCallback(QAction *);
    void showHiddenCallback(bool);
    void showHiddenCallback();
    void seqCallback(QAction *);
    void seqCallback(int);
    void searchCallback(const QString &);
    void sortCallback(int, Qt::SortOrder);
    void sortCallback(QAction *);
    void sortCallback();
    void reverseSortCallback(bool);
    void reverseSortCallback();
    void sortDirsFirstCallback(bool);
    void sortDirsFirstCallback();
    void addBookmarkCallback();
    void deleteBookmarkCallback();
    void deleteAllBookmarksCallback();
    void bookmarksCallback(QAction *);
    void bookmarksCallback();
    void bookmarkDelCallback(const QVector<int> &);
    void bookmarkDelAllCallback();
    void acceptedCallback();

    void modelUpdate();
    void widgetUpdate();
    void menuUpdate();
    void toolTipUpdate();
    
private:

    QVector<int> columnSizes() const;

    DJV_PRIVATE_COPY(djvFileBrowser);
    DJV_PRIVATE_IMPLEMENTATION();
};

//@} // djvGuiDialog

#endif // DJV_FILE_BROWSER_H

