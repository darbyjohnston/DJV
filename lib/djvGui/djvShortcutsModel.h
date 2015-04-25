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

//! \file djvShortcutsModel.h

#ifndef DJV_SHORTCUTS_MODEL_H
#define DJV_SHORTCUTS_MODEL_H

#include <djvShortcut.h>

#include <djvUtil.h>

#include <QAbstractItemModel>

struct djvShortcutsModelPrivate;

//! \addtogroup djvGuiMisc
//@{

//------------------------------------------------------------------------------
//! \class djvShortcutsModel
//!
//! This class provides a model for keyboard shorcuts.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvShortcutsModel : public QAbstractItemModel
{
    Q_OBJECT
    
    //! This property holds the list of shortcuts.
    
    Q_PROPERTY(
        QVector<djvShortcut> shortcuts
        READ                 shortcuts
        WRITE                setShortcuts
        NOTIFY               shortcutsChanged)
    
public:

    //! Constructor.

    explicit djvShortcutsModel(QObject * parent = 0);

    //! Destructor.

    virtual ~djvShortcutsModel();

    //! Get the list of shortcuts.

    const QVector<djvShortcut> & shortcuts() const;

    virtual QModelIndex	index(
        int                 row,
        int                 column,
        const QModelIndex & parent = QModelIndex()) const;

    virtual QModelIndex	parent(
        const QModelIndex & = QModelIndex()) const;

    virtual Qt::ItemFlags flags(const QModelIndex &) const;

    virtual QVariant data(
        const QModelIndex & index,
        int                 role = Qt::DisplayRole) const;

    virtual bool setData(
        const QModelIndex & index,
        const QVariant &    value,
        int                 role);

    virtual QVariant headerData(
        int             section,
        Qt::Orientation orientation,
        int             role = Qt::DisplayRole) const;

    virtual int rowCount(
        const QModelIndex & parent = QModelIndex()) const;

    virtual int columnCount(
        const QModelIndex & parent = QModelIndex()) const;

public Q_SLOTS:

    //! Set the list of shortcuts.

    void setShortcuts(const QVector<djvShortcut> &);

Q_SIGNALS:

    //! This signal is emitted when the list of shortcuts are changed.

    void shortcutsChanged(const QVector<djvShortcut> &);

private:

    void collisionsUpdate();

    DJV_PRIVATE_COPY(djvShortcutsModel);
    
    djvShortcutsModelPrivate * _p;
};

//@} // djvGuiMisc

#endif // DJV_SHORTCUTS_MODEL_H

