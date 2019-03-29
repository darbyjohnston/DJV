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

#pragma once

#include <djvViewLib/Enum.h>
#include <djvViewLib/MousePrefs.h>

#include <QAbstractTableModel>
#include <QStyledItemDelegate>

#include <memory>

namespace djv
{
    namespace ViewLib
    {
        //! This class provides a mouse button action model.
        class MouseButtonActionsModel : public QAbstractTableModel
        {
            Q_OBJECT
            Q_PROPERTY(
                QVector<MouseButtonAction> actions
                READ                       actions
                WRITE                      setActions
                NOTIFY                     actionsChanged)

        public:
            MouseButtonActionsModel(QObject * parent = nullptr);
            ~MouseButtonActionsModel();

            const QVector<djv::ViewLib::MouseButtonAction> & actions() const;

            QModelIndex	parent(const QModelIndex & = QModelIndex()) const override;
            QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
            QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
            bool setData(const QModelIndex &, const QVariant &, int role = Qt::EditRole) override;
            int	columnCount(const QModelIndex & parent = QModelIndex()) const override;
            int	rowCount(const QModelIndex & parent = QModelIndex()) const override;
            Qt::ItemFlags flags(const QModelIndex &) const override;

        public Q_SLOTS:
            void setActions(const QVector<djv::ViewLib::MouseButtonAction> &);

        Q_SIGNALS:
            void actionsChanged(const QVector<djv::ViewLib::MouseButtonAction> &);

        private:
            DJV_PRIVATE_COPY(MouseButtonActionsModel);

            QVector<MouseButtonAction> _actions;
        };

        //! This class provides a mouse wheel action model.
        class MouseWheelActionsModel : public QAbstractTableModel
        {
            Q_OBJECT
            Q_PROPERTY(
                QVector<MouseWheelAction> actions
                READ                      actions
                WRITE                     setActions
                NOTIFY                    actionsChanged)

        public:
            MouseWheelActionsModel(QObject * parent = nullptr);
            ~MouseWheelActionsModel();

            const QVector<djv::ViewLib::MouseWheelAction> & actions() const;

            QModelIndex	parent(const QModelIndex & = QModelIndex()) const override;
            QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
            QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
            bool setData(const QModelIndex &, const QVariant &, int role = Qt::EditRole) override;
            int	columnCount(const QModelIndex & parent = QModelIndex()) const override;
            int	rowCount(const QModelIndex & parent = QModelIndex()) const override;
            Qt::ItemFlags flags(const QModelIndex &) const override;

        public Q_SLOTS:
            void setActions(const QVector<djv::ViewLib::MouseWheelAction> &);

        Q_SIGNALS:
            void actionsChanged(const QVector<djv::ViewLib::MouseWheelAction> &);

        private:
            DJV_PRIVATE_COPY(MouseWheelActionsModel);

            QVector<MouseWheelAction> _actions;
        };

        //! This class provides a delegate for a mouse button action.
        class MouseButtonActionDelegate : public QStyledItemDelegate
        {
            Q_OBJECT

        public:
            MouseButtonActionDelegate(QObject * parent = nullptr);

            QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem &, const QModelIndex &) const override;
            void setEditorData(QWidget *, const QModelIndex &) const override;
            void setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &) const override;
            void updateEditorGeometry(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const override;
        };

        //! This class provides a delegate for a mouse wheel action.
        class MouseWheelActionDelegate : public QStyledItemDelegate
        {
            Q_OBJECT

        public:
            MouseWheelActionDelegate(QObject * parent = nullptr);

            QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem &, const QModelIndex &) const override;
            void setEditorData(QWidget *, const QModelIndex &) const override;
            void setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &) const override;
            void updateEditorGeometry(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const override;
        };

    } // namespace ViewLib
} // namespace djv
