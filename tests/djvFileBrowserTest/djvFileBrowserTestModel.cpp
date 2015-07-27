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

//! \file djvFileBrowserTestModel.cpp

#include <djvFileBrowserTestModel.h>

#include <djvDebug.h>

#include <QToolButton>

djvFileBrowserTestModel::djvFileBrowserTestModel(djvGuiContext * context, QObject * parent) :
    QAbstractItemModel(parent),
    _context (context),
    _sequence(static_cast<djvSequence::COMPRESS>(0)),
    _dir     (new djvFileBrowserTestDir)
{
    _dir->moveToThread(&_thread);

    connect(
        _dir,
        SIGNAL(pathChanged(const QString &, const djvFileInfoList &)),
        SLOT(pathCallback(const QString &, const djvFileInfoList &)));
   
    _dir->connect(
        this,
        SIGNAL(requestPath(const QString &, djvSequence::COMPRESS)),
        SLOT(setPath(const QString &, djvSequence::COMPRESS)));
    
    _thread.start();
}

djvFileBrowserTestModel::~djvFileBrowserTestModel()
{
    _thread.quit();
    _thread.wait();
}

const QString & djvFileBrowserTestModel::path() const
{
    return _path;
}

djvSequence::COMPRESS djvFileBrowserTestModel::sequence() const
{
    return _sequence;
}

int djvFileBrowserTestModel::columnCount(const QModelIndex & parent) const
{
    return 1;
}

QVariant djvFileBrowserTestModel::data(const QModelIndex & index, int role) const
{
    if (! index.isValid())
        return QVariant();

    if (role != Qt::DecorationRole &&
        role != Qt::DisplayRole    &&
        role != Qt::EditRole       &&
        role != Qt::SizeHintRole)
        return QVariant();

    const int row    = index.row();
    const int column = index.column();

    if (row    < 0 || row    >= _list.count() ||
        column < 0 || column >= 1)
        return QVariant();

    switch (role)
    {
        case Qt::DisplayRole: return _displayRoles[row];
    }
    
    return QVariant();
}

Qt::ItemFlags djvFileBrowserTestModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return defaultFlags;
}

QVariant djvFileBrowserTestModel::headerData(
    int             section,
    Qt::Orientation orientation,
    int             role) const
{
    return QVariant();
}

QModelIndex	djvFileBrowserTestModel::index(
    int                 row,
    int                 column,
    const QModelIndex & parent) const
{
    if (! hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, (void *)&_list[row]);
}

QModelIndex	djvFileBrowserTestModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int djvFileBrowserTestModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : _list.count();
}

void djvFileBrowserTestModel::setPath(const QString & path)
{
    //DJV_DEBUG("djvFileBrowserTestModel::setPath");
    //DJV_DEBUG_PRINT("path = " << path);
    
    Q_EMIT requestPath(path, _sequence);
}

void djvFileBrowserTestModel::setSequence(djvSequence::COMPRESS sequence)
{
    DJV_DEBUG("djvFileBrowserTestModel::setSequence");
    DJV_DEBUG_PRINT("sequence = " << sequence);
    
    Q_EMIT requestPath(_path, sequence);
}

void djvFileBrowserTestModel::pathCallback(const QString & path, const djvFileInfoList & list)
{
    //DJV_DEBUG("djvFileBrowserTestModel::pathCallback");
    //DJV_DEBUG_PRINT("path = " << path);
    //DJV_DEBUG_PRINT("list = " << list);
    
    beginResetModel();
    
    _path = path;
    
    _list = list;
    
    _displayRoles.clear();
    
    for (int i = 0; i < _list.count(); ++i)
    {
        _displayRoles.append(_list[i].fileName());
    }

    endResetModel();
    
    Q_EMIT pathChanged(_path);
}

