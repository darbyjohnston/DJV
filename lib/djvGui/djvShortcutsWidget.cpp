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

//! \file djvShortcutsWidget.cpp

#include <djvShortcutsWidget.h>

#include <djvIconLibrary.h>
#include <djvSearchBox.h>
#include <djvShortcutsModel.h>
#include <djvSignalBlocker.h>
#include <djvStyle.h>

#include <djvDebug.h>
#include <djvListUtil.h>

#include <QAbstractItemModel>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMouseEvent>
#include <QPainter>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QTreeView>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// djvShortcutDelegate
//------------------------------------------------------------------------------

namespace
{

class ShortcutDelegateEdit : public QWidget
{
public:

    ShortcutDelegateEdit(QWidget * parent);
    
    const djvShortcut & shortcut() const;
    
    void setShortcut(const djvShortcut &);

    virtual QSize sizeHint() const;

protected:

    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void keyPressEvent(QKeyEvent *);

private:

    djvShortcut _shortcut;
};

ShortcutDelegateEdit::ShortcutDelegateEdit(QWidget * parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

const djvShortcut & ShortcutDelegateEdit::shortcut() const
{
    return _shortcut;
}

void ShortcutDelegateEdit::setShortcut(const djvShortcut & shortcut)
{
    if (shortcut == _shortcut)
        return;

    _shortcut = shortcut;

    updateGeometry();
    update();
}

QSize ShortcutDelegateEdit::sizeHint() const
{
    const int margin = djvStyle::global()->sizeMetric().margin;
    
    const QSize size =
        fontMetrics().size(Qt::TextSingleLine, _shortcut.value.toString());
    
    return QSize(size.width() + margin * 2, size.height());
}

void ShortcutDelegateEdit::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    
    const int w      = width ();
    const int h      = height();
    const int margin = djvStyle::global()->sizeMetric().margin;
    
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
        (event->modifiers() & Qt::ShiftModifier   ? Qt::SHIFT : 0) |
        (event->modifiers() & Qt::ControlModifier ? Qt::CTRL  : 0) |
        (event->modifiers() & Qt::MetaModifier    ? Qt::META  : 0) |
        (event->modifiers() & Qt::AltModifier     ? Qt::ALT   : 0));
    
    djvShortcut shortcut = _shortcut;
    shortcut.value = QKeySequence(event->key() | mod);
        
    setShortcut(shortcut);
}

class ShortcutDelegate : public QStyledItemDelegate
{
public:
    
    explicit ShortcutDelegate(QObject * parent = 0);

    virtual QWidget * createEditor(
        QWidget *                    parent,
        const QStyleOptionViewItem & option,
        const QModelIndex &          index) const;

    virtual void setEditorData(
        QWidget *           editor,
        const QModelIndex & index) const;

    virtual void setModelData(
        QWidget *            editor,
        QAbstractItemModel * model,
        const QModelIndex &  index) const;

    virtual void updateEditorGeometry(
        QWidget *                    editor,
        const QStyleOptionViewItem & option,
        const QModelIndex &          index) const;

    virtual QSize sizeHint(
        const QStyleOptionViewItem & option,
        const QModelIndex &          index) const;
    
    virtual void paint(
        QPainter *                   painter,
        const QStyleOptionViewItem & option,
        const QModelIndex &          index) const;
    
    virtual bool editorEvent(
        QEvent *                     event,
        QAbstractItemModel *         model,
        const QStyleOptionViewItem & option,
        const QModelIndex &          index);

private:

    QPixmap _clearPixmap;
};

ShortcutDelegate::ShortcutDelegate(QObject * parent) :
    QStyledItemDelegate(parent)
{
    _clearPixmap = djvIconLibrary::global()->pixmap("djvResetIcon.png");
}

QWidget * ShortcutDelegate::createEditor(
    QWidget *                    parent,
    const QStyleOptionViewItem & option,
    const QModelIndex &          index) const
{
    return new ShortcutDelegateEdit(parent);
}

void ShortcutDelegate::setEditorData(
    QWidget *           editor,
    const QModelIndex & index) const
{
    ShortcutDelegateEdit * e = static_cast<ShortcutDelegateEdit *>(editor);

    e->setShortcut(djvShortcut(
        index.model()->data(index, Qt::DisplayRole).toString(),
        index.model()->data(index, Qt::EditRole).toString()));
}

void ShortcutDelegate::setModelData(
    QWidget *            editor,
    QAbstractItemModel * model,
    const QModelIndex &  index) const
{
    ShortcutDelegateEdit * e = static_cast<ShortcutDelegateEdit *>(editor);

    const djvShortcut shortcut = e->shortcut();
    
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
            const int spacing = djvStyle::global()->sizeMetric().spacing;
    
            size = QSize(
                size.width() + spacing + _clearPixmap.width(),
                djvMath::max(size.height(), _clearPixmap.height()));
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
            painter->drawPixmap(
                r.x() + r.width() - _clearPixmap.width(),
                r.y(),
                _clearPixmap);
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
                    
                    QRect r(
                        option.rect.x() + option.rect.width() - _clearPixmap.width(),
                        option.rect.y(),
                        _clearPixmap.width(),
                        _clearPixmap.height());
                    
                    if (r.contains(e->pos()))
                    {
                        model->setData(index, djvShortcut().value, Qt::EditRole);

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

//------------------------------------------------------------------------------
// djvShortcutsWidget::P
//------------------------------------------------------------------------------

struct djvShortcutsWidget::P
{
    P() :
        model     (0),
        proxyModel(0),
        browser   (0),
        searchBox (0)
    {}

    djvShortcutsModel *     model;
    QSortFilterProxyModel * proxyModel;
    QTreeView *             browser;
    djvSearchBox *          searchBox;
};

//------------------------------------------------------------------------------
// djvShortcutsWidget
//------------------------------------------------------------------------------

djvShortcutsWidget::djvShortcutsWidget(QWidget * parent) :
    QWidget(parent),
    _p     (new P)
{
    _p->model = new djvShortcutsModel(this);

    _p->proxyModel = new QSortFilterProxyModel(this);
    _p->proxyModel->setSourceModel(_p->model);
    _p->proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    _p->proxyModel->sort(0, Qt::AscendingOrder);

    _p->browser = new QTreeView;
    _p->browser->setSortingEnabled(true);
    _p->browser->setRootIsDecorated(false);
    _p->browser->setItemDelegate(new ShortcutDelegate(this));
    _p->browser->setModel(_p->proxyModel);
    _p->browser->setMinimumHeight(200);

    _p->browser->header()->setSortIndicator(0, Qt::AscendingOrder);

    _p->searchBox = new djvSearchBox;

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
        SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
        SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)));
}

djvShortcutsWidget::~djvShortcutsWidget()
{
    delete _p;
}

const QVector<djvShortcut> & djvShortcutsWidget::shortcuts() const
{
    return _p->model->shortcuts();
}

void djvShortcutsWidget::setShortcuts(const QVector<djvShortcut> & shortcuts)
{
    _p->model->setShortcuts(shortcuts);
}
