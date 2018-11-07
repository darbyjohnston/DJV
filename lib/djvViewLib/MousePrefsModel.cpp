//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvViewLib/MousePrefsModel.h>

#include <djvViewLib/ViewContext.h>

#include <QComboBox>

namespace djv
{
    namespace ViewLib
    {
        MouseButtonActionsModel::MouseButtonActionsModel(QObject * parent) :
            QAbstractTableModel(parent)
        {}

        MouseButtonActionsModel::~MouseButtonActionsModel()
        {}

        const QVector<djv::ViewLib::MouseButtonAction> & MouseButtonActionsModel::actions() const
        {
            return _actions;
        }

        QModelIndex	MouseButtonActionsModel::parent(const QModelIndex &) const
        {
            return QModelIndex();
        }

        QVariant MouseButtonActionsModel::headerData(int section, Qt::Orientation orientation, int role) const
        {
            if (orientation != Qt::Horizontal)
                return QVariant();
            switch (role)
            {
            case Qt::DisplayRole: return MouseButtonAction::labels()[section];
            default: break;
            }
            return QVariant();
        }

        QVariant MouseButtonActionsModel::data(const QModelIndex & index, int role) const
        {
            if (!index.isValid())
                return QVariant();
            if (!(Qt::DisplayRole == role || Qt::EditRole == role))
                return QVariant();
            const int row = index.row();
            const int column = index.column();
            if (row < 0 || row >= _actions.count() || column < 0 || column >= 3)
                return QVariant();
            switch (role)
            {
            case Qt::DisplayRole:
                switch (column)
                {
                case 0: return Enum::mouseButtonLabels()[_actions[row].button];
                case 1: return Enum::keyboardModifierLabels()[_actions[row].modifier];
                case 2: return Enum::mouseButtonActionLabels()[_actions[row].action];
                }
                break;
            case Qt::EditRole:
                switch (column)
                {
                case 0: return _actions[row].button;
                case 1: return _actions[row].modifier;
                case 2: return _actions[row].action;
                }
                break;
            default: break;
            }
            return QVariant();
        }

        bool MouseButtonActionsModel::setData(const QModelIndex & index, const QVariant & value, int role)
        {
            if (index.isValid() && Qt::EditRole == role)
            {
                switch (index.column())
                {
                case 0: _actions[index.row()].button = value.value<Enum::MOUSE_BUTTON>(); break;
                case 1: _actions[index.row()].modifier = value.value<Enum::KEYBOARD_MODIFIER>(); break;
                case 2: _actions[index.row()].action = value.value<Enum::MOUSE_BUTTON_ACTION>(); break;
                }
                Q_EMIT dataChanged(index, index, { role });
                Q_EMIT actionsChanged(_actions);
                return true;
            }
            return false;
        }

        int	MouseButtonActionsModel::columnCount(const QModelIndex & parent) const
        {
            return parent.isValid() ? 0 : 3;
        }

        int	MouseButtonActionsModel::rowCount(const QModelIndex & parent) const
        {
            return parent.isValid() ? 0 : _actions.count();
        }

        Qt::ItemFlags MouseButtonActionsModel::flags(const QModelIndex & index) const
        {
            return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
        }

        void MouseButtonActionsModel::setActions(const QVector<djv::ViewLib::MouseButtonAction> & value)
        {
            beginResetModel();
            _actions = value;
            endResetModel();
            Q_EMIT actionsChanged(_actions);
        }

        MouseWheelActionsModel::MouseWheelActionsModel(QObject * parent) :
            QAbstractTableModel(parent)
        {}

        MouseWheelActionsModel::~MouseWheelActionsModel()
        {}

        const QVector<djv::ViewLib::MouseWheelAction> & MouseWheelActionsModel::actions() const
        {
            return _actions;
        }

        QModelIndex	MouseWheelActionsModel::parent(const QModelIndex &) const
        {
            return QModelIndex();
        }

        QVariant MouseWheelActionsModel::headerData(int section, Qt::Orientation orientation, int role) const
        {
            if (orientation != Qt::Horizontal)
                return QVariant();
            switch (role)
            {
            case Qt::DisplayRole: return MouseWheelAction::labels()[section];
            default: break;
            }
            return QVariant();
        }

        QVariant MouseWheelActionsModel::data(const QModelIndex & index, int role) const
        {
            if (!index.isValid())
                return QVariant();
            if (!(Qt::DisplayRole == role || Qt::EditRole == role))
                return QVariant();
            const int row = index.row();
            const int column = index.column();
            if (row < 0 || row >= _actions.count() || column < 0 || column >= 2)
                return QVariant();
            switch (role)
            {
            case Qt::DisplayRole:
                switch (column)
                {
                case 0: return Enum::keyboardModifierLabels()[_actions[row].modifier];
                case 1: return Enum::mouseWheelActionLabels()[_actions[row].action];
                }
                break;
            case Qt::EditRole:
                switch (column)
                {
                case 0: return _actions[row].modifier;
                case 1: return _actions[row].action;
                }
                break;
            default: break;
            }
            return QVariant();
        }

        bool MouseWheelActionsModel::setData(const QModelIndex & index, const QVariant & value, int role)
        {
            if (index.isValid() && Qt::EditRole == role)
            {
                switch (index.column())
                {
                case 0: _actions[index.row()].modifier = value.value<Enum::KEYBOARD_MODIFIER>(); break;
                case 1: _actions[index.row()].action = value.value<Enum::MOUSE_WHEEL_ACTION>(); break;
                }
                Q_EMIT dataChanged(index, index, { role });
                Q_EMIT actionsChanged(_actions);
                return true;
            }
            return false;
        }

        int	MouseWheelActionsModel::columnCount(const QModelIndex & parent) const
        {
            return parent.isValid() ? 0 : 2;
        }

        int	MouseWheelActionsModel::rowCount(const QModelIndex & parent) const
        {
            return parent.isValid() ? 0 : _actions.count();
        }

        Qt::ItemFlags MouseWheelActionsModel::flags(const QModelIndex & index) const
        {
            return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
        }

        void MouseWheelActionsModel::setActions(const QVector<djv::ViewLib::MouseWheelAction> & value)
        {
            beginResetModel();
            _actions = value;
            endResetModel();
            Q_EMIT actionsChanged(_actions);
        }

        MouseButtonActionDelegate::MouseButtonActionDelegate(QObject * parent) :
            QStyledItemDelegate(parent)
        {}

        QWidget * MouseButtonActionDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
        {
            auto out = new QComboBox(parent);
            switch (index.column())
            {
            case 0:
                for (int i = 0; i < Enum::MOUSE_BUTTON_COUNT; ++i)
                {
                    out->addItem(Enum::mouseButtonLabels()[i], static_cast<Enum::MOUSE_BUTTON>(i));
                }
                break;
            case 1:
                for (int i = 0; i < Enum::KEYBOARD_MODIFIER_COUNT; ++i)
                {
                    out->addItem(Enum::keyboardModifierLabels()[i], static_cast<Enum::KEYBOARD_MODIFIER>(i));
                }
                break;
            case 2:
                for (int i = 0; i < Enum::MOUSE_BUTTON_ACTION_COUNT; ++i)
                {
                    out->addItem(Enum::mouseButtonActionLabels()[i], static_cast<Enum::MOUSE_BUTTON_ACTION>(i));
                }
                break;
            }
            return out;
        }

        void MouseButtonActionDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
        {
            const auto value = index.model()->data(index, Qt::EditRole);
            auto comboBox = static_cast<QComboBox *>(editor);
            comboBox->setCurrentIndex(comboBox->findData(value));
        }

        void MouseButtonActionDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
        {
            auto comboBox = static_cast<QComboBox *>(editor);
            model->setData(index, comboBox->currentData(), Qt::EditRole);
        }

        void MouseButtonActionDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const
        {
            //! \todo Hard-coded margin values.
            editor->setGeometry(option.rect.adjusted(1, 1, -1, -1));
        }

        MouseWheelActionDelegate::MouseWheelActionDelegate(QObject * parent) :
            QStyledItemDelegate(parent)
        {}

        QWidget * MouseWheelActionDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
        {
            auto out = new QComboBox(parent);
            switch (index.column())
            {
            case 0:
                for (int i = 0; i < Enum::KEYBOARD_MODIFIER_COUNT; ++i)
                {
                    out->addItem(Enum::keyboardModifierLabels()[i], static_cast<Enum::KEYBOARD_MODIFIER>(i));
                }
                break;
            case 1:
                for (int i = 0; i < Enum::MOUSE_WHEEL_ACTION_COUNT; ++i)
                {
                    out->addItem(Enum::mouseWheelActionLabels()[i], static_cast<Enum::MOUSE_WHEEL_ACTION>(i));
                }
                break;
            }
            return out;
        }

        void MouseWheelActionDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
        {
            const auto value = index.model()->data(index, Qt::EditRole);
            auto comboBox = static_cast<QComboBox *>(editor);
            comboBox->setCurrentIndex(comboBox->findData(value));
        }

        void MouseWheelActionDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
        {
            auto comboBox = static_cast<QComboBox *>(editor);
            model->setData(index, comboBox->currentData(), Qt::EditRole);
        }

        void MouseWheelActionDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const
        {
            //! \todo Hard-coded margin values.
            editor->setGeometry(option.rect.adjusted(1, 1, -1, -1));
        }

    } // namespace ViewLib
} // namespace djv
