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

//! \file djvShortcutsModel.cpp

#include <djvShortcutsModel.h>

#include <QBrush>
#include <QColor>

//------------------------------------------------------------------------------
// djvShortcutsModel::P
//------------------------------------------------------------------------------

struct djvShortcutsModel::P
{
    QVector<djvShortcut> shortcuts;
    QVector<bool>        collisions;
};

//------------------------------------------------------------------------------
// djvShortcutsModel
//------------------------------------------------------------------------------

djvShortcutsModel::djvShortcutsModel(QObject * parent) :
    QAbstractItemModel(parent),
    _p(new P)
{
    collisionsUpdate();
}

djvShortcutsModel::~djvShortcutsModel()
{
    delete _p;
}

const QVector<djvShortcut> & djvShortcutsModel::shortcuts() const
{
    return _p->shortcuts;
}

void djvShortcutsModel::setShortcuts(const QVector<djvShortcut> & shortcuts)
{
    if (shortcuts == _p->shortcuts)
        return;

    beginResetModel();

    _p->shortcuts = shortcuts;

    collisionsUpdate();

    endResetModel();

    Q_EMIT shortcutsChanged(_p->shortcuts);
}

QModelIndex	djvShortcutsModel::index(
    int                 row,
    int                 column,
    const QModelIndex & parent) const
{
    if (! hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, &_p->shortcuts[row]);
}

QModelIndex	djvShortcutsModel::parent(const QModelIndex & index) const
{
    return QModelIndex();
}

Qt::ItemFlags djvShortcutsModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    if (index.isValid() && 1 == index.column())
    {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

QVariant djvShortcutsModel::data(
    const QModelIndex & index,
    int                 role) const
{
    if (! index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole    &&
        role != Qt::EditRole       &&
        role != Qt::BackgroundRole)
        return QVariant();

    const int row    = index.row();
    const int column = index.column();

    if (row    < 0 || row >= _p->shortcuts.count() ||
        column < 0 || column >= 2)
        return QVariant();

    const djvShortcut & shortcut = _p->shortcuts[row];

    switch (role)
    {
        case Qt::DisplayRole:

            switch (column)
            {
                case 0: return shortcut.name;
                case 1: return shortcut.value.toString();

                default: break;
            }

            break;

        case Qt::EditRole:

            switch (column)
            {
                case 1: return shortcut.value;

                default: break;
            }

            break;

        case Qt::BackgroundRole:

            if (_p->collisions[row])
            {
                return QVariant::fromValue(QColor(190, 60, 60));
            }

            break;

        default: break;
    }

    return QVariant();
}

bool djvShortcutsModel::setData(
    const QModelIndex & index,
    const QVariant &    value,
    int                 role)
{
    if (! index.isValid())
        return false;

    const int row    = index.row();
    const int column = index.column();

    if (row  >= 0                     &&
        row  <  _p->shortcuts.count() &&
        1    == column                &&
        role == Qt::EditRole)
    {
        _p->shortcuts[index.row()].value = value.toString();

        collisionsUpdate();

        Q_EMIT dataChanged(index, index);
        Q_EMIT shortcutsChanged(_p->shortcuts);

        return true;
    }

    return false;
}

QVariant djvShortcutsModel::headerData(
    int             section,
    Qt::Orientation orientation,
    int             role) const
{
    static const QStringList data = QStringList() <<
        tr("Name") <<
        tr("Shortcut");

    switch (role)
    {
        case Qt::DisplayRole:

            switch (section)
            {
                case 0: return data[0];
                case 1: return data[1];

                default: break;
            }

            break;

        default: break;
    }
    
    return QVariant();
}

int djvShortcutsModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : _p->shortcuts.count();
}

int djvShortcutsModel::columnCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : 2;
}

void djvShortcutsModel::collisionsUpdate()
{
    _p->collisions.resize(_p->shortcuts.count());

    for (int i = 0; i < _p->shortcuts.count(); ++i)
    {
        _p->collisions[i] =
            ! _p->shortcuts[i].value.isEmpty() &&
            _p->shortcuts.count(_p->shortcuts[i]) > 1;
    }
}
