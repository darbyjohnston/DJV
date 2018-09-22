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

#include <djvUI/ShortcutsWidget.h>

#include <djvUI/UIContext.h>
#include <djvUI/IconLibrary.h>
#include <djvUI/SearchBox.h>
#include <djvUI/ShortcutsModel.h>
#include <djvCore/SignalBlocker.h>

#include <djvCore/Debug.h>
#include <djvCore/ListUtil.h>
#include <djvCore/Math.h>

#include <QAbstractItemModel>
#include <QApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMouseEvent>
#include <QPainter>
#include <QSortFilterProxyModel>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QTreeView>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        namespace
        {
            class ShortcutDelegateEdit : public QWidget
            {
            public:
                ShortcutDelegateEdit(UIContext *, QWidget * parent);

                const Shortcut & shortcut() const;

                void setShortcut(const Shortcut &);

                QSize sizeHint() const override;

            protected:
                void paintEvent(QPaintEvent *) override;
                void mousePressEvent(QMouseEvent *) override;
                void keyPressEvent(QKeyEvent *) override;

            private:
                Shortcut _shortcut;
                UIContext * _context = nullptr;
            };

            ShortcutDelegateEdit::ShortcutDelegateEdit(UIContext * context, QWidget * parent) :
                QWidget(parent),
                _context(context)
            {
                setAttribute(Qt::WA_OpaquePaintEvent);
                setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            }

            const Shortcut & ShortcutDelegateEdit::shortcut() const
            {
                return _shortcut;
            }

            void ShortcutDelegateEdit::setShortcut(const Shortcut & shortcut)
            {
                if (shortcut == _shortcut)
                    return;
                _shortcut = shortcut;
                updateGeometry();
                update();
            }

            QSize ShortcutDelegateEdit::sizeHint() const
            {
                const int margin = qApp->style()->pixelMetric(QStyle::PM_ButtonMargin);
                const QSize size = fontMetrics().size(Qt::TextSingleLine, _shortcut.value.toString());
                return QSize(size.width() + margin * 2, size.height());
            }

            void ShortcutDelegateEdit::paintEvent(QPaintEvent * event)
            {
                QPainter painter(this);
                const int w = width();
                const int h = height();
                const int margin = qApp->style()->pixelMetric(QStyle::PM_ButtonMargin);
                painter.fillRect(0, 0, w, h, palette().color(QPalette::Base));
                painter.setPen(palette().color(QPalette::Text));
                QFont font(this->font());
                font.setItalic(true);
                painter.setFont(font);
                painter.drawText(
                    QRect(margin, 0, w - margin, h),
                    _shortcut.value.toString());
            }

            void ShortcutDelegateEdit::mousePressEvent(QMouseEvent * event)
            {
                event->accept();
            }

            void ShortcutDelegateEdit::keyPressEvent(QKeyEvent * event)
            {
                switch (event->key())
                {
                case Qt::Key_Shift:
                case Qt::Key_Control:
                case Qt::Key_Meta:
                case Qt::Key_Alt:
                    return;
                }
                const Qt::Modifier mod = Qt::Modifier(
                    (event->modifiers() & Qt::ShiftModifier ? Qt::SHIFT : 0) |
                    (event->modifiers() & Qt::ControlModifier ? Qt::CTRL : 0) |
                    (event->modifiers() & Qt::MetaModifier ? Qt::META : 0) |
                    (event->modifiers() & Qt::AltModifier ? Qt::ALT : 0));
                Shortcut shortcut = _shortcut;
                shortcut.value = QKeySequence(event->key() | mod);
                setShortcut(shortcut);
            }

            class ShortcutDelegate : public QStyledItemDelegate
            {
            public:
                explicit ShortcutDelegate(UIContext *, QObject * parent = nullptr);

                QWidget * createEditor(
                    QWidget *                    parent,
                    const QStyleOptionViewItem & option,
                    const QModelIndex &          index) const override;
                void setEditorData(
                    QWidget *           editor,
                    const QModelIndex & index) const override;
                void setModelData(
                    QWidget *            editor,
                    QAbstractItemModel * model,
                    const QModelIndex &  index) const override;
                void updateEditorGeometry(
                    QWidget *                    editor,
                    const QStyleOptionViewItem & option,
                    const QModelIndex &          index) const override;
                QSize sizeHint(
                    const QStyleOptionViewItem & option,
                    const QModelIndex &          index) const override;
                void paint(
                    QPainter *                   painter,
                    const QStyleOptionViewItem & option,
                    const QModelIndex &          index) const override;
                bool editorEvent(
                    QEvent *                     event,
                    QAbstractItemModel *         model,
                    const QStyleOptionViewItem & option,
                    const QModelIndex &          index) override;

            private:
                UIContext * _context;
            };

            ShortcutDelegate::ShortcutDelegate(UIContext * context, QObject * parent) :
                QStyledItemDelegate(parent),
                _context(context)
            {}

            QWidget * ShortcutDelegate::createEditor(
                QWidget *                    parent,
                const QStyleOptionViewItem & option,
                const QModelIndex &          index) const
            {
                return new ShortcutDelegateEdit(_context, parent);
            }

            void ShortcutDelegate::setEditorData(
                QWidget *           editor,
                const QModelIndex & index) const
            {
                ShortcutDelegateEdit * e = static_cast<ShortcutDelegateEdit *>(editor);
                e->setShortcut(Shortcut(
                    index.model()->data(index, Qt::DisplayRole).toString(),
                    index.model()->data(index, Qt::EditRole).toString()));
            }

            void ShortcutDelegate::setModelData(
                QWidget *            editor,
                QAbstractItemModel * model,
                const QModelIndex &  index) const
            {
                ShortcutDelegateEdit * e = static_cast<ShortcutDelegateEdit *>(editor);
                const Shortcut shortcut = e->shortcut();
                model->setData(index, shortcut.value, Qt::EditRole);
            }

            void ShortcutDelegate::updateEditorGeometry(
                QWidget *                    editor,
                const QStyleOptionViewItem & option,
                const QModelIndex &          index) const
            {
                editor->setGeometry(option.rect);
            }

            QSize ShortcutDelegate::sizeHint(
                const QStyleOptionViewItem & option,
                const QModelIndex &          index) const
            {
                QSize size = QStyledItemDelegate::sizeHint(option, index);
                switch (index.column())
                {
                case 1:
                {
                    const int iconSize = qApp->style()->pixelMetric(QStyle::PM_ToolBarIconSize);
                    const int spacing = qApp->style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
                    size = QSize(
                        size.width() + spacing + iconSize,
                        Core::Math::max(size.height(), iconSize));
                }
                break;
                }
                return size;
            }

            void ShortcutDelegate::paint(
                QPainter *                   painter,
                const QStyleOptionViewItem & option,
                const QModelIndex &          index) const
            {
                QStyledItemDelegate::paint(painter, option, index);
                const QRect & r = option.rect;
                switch (index.column())
                {
                case 1:
                {
                    const int iconSize = qApp->style()->pixelMetric(QStyle::PM_ToolBarIconSize);
                    painter->drawPixmap(
                        r.x() + r.width() - iconSize,
                        r.y(),
                        _context->iconLibrary()->pixmap("djv/UI/ResetIcon"));
                }
                break;
                }
            }

            bool ShortcutDelegate::editorEvent(
                QEvent *                     event,
                QAbstractItemModel *         model,
                const QStyleOptionViewItem & option,
                const QModelIndex &          index)
            {
                switch (index.column())
                {
                case 1:
                    switch (event->type())
                    {
                    case QEvent::MouseButtonPress:
                    {
                        QMouseEvent * e = static_cast<QMouseEvent *>(event);
                        const int iconSize = qApp->style()->pixelMetric(QStyle::PM_ToolBarIconSize);
                        QRect r(
                            option.rect.x() + option.rect.width() - iconSize,
                            option.rect.y(),
                            iconSize,
                            iconSize);
                        if (r.contains(e->pos()))
                        {
                            model->setData(index, Shortcut().value, Qt::EditRole);
                            return true;
                        }
                    }
                    break;
                    default: break;
                    }
                    break;
                }
                return QStyledItemDelegate::editorEvent(event, model, option, index);
            }

        } // namespace

        struct ShortcutsWidget::Private
        {
            ShortcutsModel * model = nullptr;
            QSortFilterProxyModel * proxyModel = nullptr;
            QTreeView * browser = nullptr;
            SearchBox * searchBox = nullptr;
        };

        ShortcutsWidget::ShortcutsWidget(UIContext * context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->model = new ShortcutsModel(this);

            _p->proxyModel = new QSortFilterProxyModel(this);
            _p->proxyModel->setSourceModel(_p->model);
            _p->proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
            _p->proxyModel->sort(0, Qt::AscendingOrder);

            _p->browser = new QTreeView;
            _p->browser->setSortingEnabled(true);
            _p->browser->setRootIsDecorated(false);
            _p->browser->setItemDelegate(new ShortcutDelegate(context, this));
            _p->browser->setModel(_p->proxyModel);
            _p->browser->setMinimumHeight(200);

            _p->browser->header()->setSortIndicator(0, Qt::AscendingOrder);

            _p->searchBox = new SearchBox(context);

            QVBoxLayout * layout = new QVBoxLayout(this);
            layout->setMargin(0);

            QHBoxLayout * hLayout = new QHBoxLayout;
            hLayout->addStretch();
            hLayout->addWidget(_p->searchBox);
            layout->addLayout(hLayout);

            layout->addWidget(_p->browser);

            connect(
                _p->searchBox,
                SIGNAL(textChanged(const QString &)),
                _p->proxyModel,
                SLOT(setFilterFixedString(const QString &)));
            connect(
                _p->model,
                SIGNAL(shortcutsChanged(const QVector<djv::UI::Shortcut> &)),
                SIGNAL(shortcutsChanged(const QVector<djv::UI::Shortcut> &)));
        }

        ShortcutsWidget::~ShortcutsWidget()
        {}
        
        const QVector<Shortcut> & ShortcutsWidget::shortcuts() const
        {
            return _p->model->shortcuts();
        }

        void ShortcutsWidget::setShortcuts(const QVector<Shortcut> & shortcuts)
        {
            _p->model->setShortcuts(shortcuts);
        }

    } // namespace UI
} // namespace djv
