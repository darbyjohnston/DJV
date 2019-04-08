//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvViewLib/AnnotateModel.h>

#include <djvViewLib/AnnotateData.h>

#include <QApplication>
#include <QPointer>

namespace djv
{
    namespace ViewLib
    {
        struct AnnotateModel::Private
        {
            QList<QPointer<Annotate::Data> > annotations;
        };

        AnnotateModel::AnnotateModel(QObject * parent) :
            QAbstractItemModel(parent),
            _p(new Private)
        {}
        
        AnnotateModel::~AnnotateModel()
        {}

        QModelIndex	AnnotateModel::index(
            int                 row,
            int                 column,
            const QModelIndex & parent) const
        {
            if (!hasIndex(row, column, parent))
                return QModelIndex();
            if (_p->annotations.isEmpty())
                return QModelIndex();
            return createIndex(row, column, (void *)_p->annotations[row].data());
        }

        QModelIndex	AnnotateModel::parent(const QModelIndex & index) const
        {
            return QModelIndex();
        }
        
        Qt::ItemFlags AnnotateModel::flags(const QModelIndex & index) const
        {
            Qt::ItemFlags flags = QAbstractItemModel::flags(index);
            if (index.isValid() && 1 == index.column())
            {
                flags |= Qt::ItemIsEditable;
            }
            return flags;
        }

        QVariant AnnotateModel::data(
            const QModelIndex & index,
            int                 role) const
        {
            if (!index.isValid())
                return QVariant();
            if (role != Qt::DisplayRole)
                return QVariant();
            if (_p->annotations.isEmpty())
                return QVariant();
            const int row = index.row();
            const int column = index.column();
            if (row < 0 || row >= (_p->annotations.count()) ||
                column < 0 || column >= 2)
                return QVariant();
            switch (role)
            {
            case Qt::DisplayRole:
                switch (column)
                {
                case 0:
                    return _p->annotations[row] ? _p->annotations[row]->frame() : 0;
                case 1:
                {
                    QStringList split;
                    if (_p->annotations[row])
                    {
                        split = _p->annotations[row]->text().split(QRegExp("\n"), QString::SkipEmptyParts);
                    }
                    if (split.size())
                    {
                        return split[0];
                    }
                    break;
                }
                default: break;
                }
                break;
            default: break;
            }
            return QVariant();
        }

        QVariant AnnotateModel::headerData(
            int             section,
            Qt::Orientation orientation,
            int             role) const
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::AnnotateModel", "Frame") <<
                qApp->translate("djv::ViewLib::AnnotateModel", "Text");
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

        int AnnotateModel::rowCount(const QModelIndex & parent) const
        {
            return parent.isValid() ? 0 : _p->annotations.count();
        }

        int AnnotateModel::columnCount(const QModelIndex & parent) const
        {
            return parent.isValid() ? 0 : 2;
        }

        void AnnotateModel::setAnnotations(const QList<QPointer<Annotate::Data> > & value)
        {
            beginResetModel();
            _p->annotations.clear();
            Q_FOREACH(auto i, value)
            {
                _p->annotations.push_back(i);
            }
            endResetModel();
        }

    } // namespace ViewLib
} // namespace djv
