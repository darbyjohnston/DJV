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

#include <djvFileBrowserTestContactSheetDelegate.h>

#include <djvFileBrowserTestModel.h>

#include <djvStyle.h>
#include <djvUIContext.h>

#include <djvDebug.h>

#include <QPainter>
#include <QPixmap>
#include <QPixmapCache>
#include <QSvgRenderer>

//------------------------------------------------------------------------------
// djvFileBrowserTestContactSheetDelegate::Private
//------------------------------------------------------------------------------

struct djvFileBrowserTestContactSheetDelegate::Private
{
    Private(djvUIContext * context) :
        context      (context),
        thumbnailSize(0)
    {}
        
    djvUIContext * context;
    int            thumbnailSize;
};

//------------------------------------------------------------------------------
// djvFileBrowserTestContactSheetDelegate
//------------------------------------------------------------------------------

djvFileBrowserTestContactSheetDelegate::djvFileBrowserTestContactSheetDelegate(
    djvUIContext *  context,
    QObject *       parent) :
    QAbstractItemDelegate(parent),
    _p(new Private(context))
{}

djvFileBrowserTestContactSheetDelegate::~djvFileBrowserTestContactSheetDelegate()
{}

int djvFileBrowserTestContactSheetDelegate::thumbnailSize() const
{
    return _p->thumbnailSize;
}

QSize djvFileBrowserTestContactSheetDelegate::sizeHint(
    const QStyleOptionViewItem & option,
    const QModelIndex &          index) const
{
    return QSize(_p->thumbnailSize, _p->thumbnailSize);
}

void djvFileBrowserTestContactSheetDelegate::paint(
    QPainter *                   painter,
    const QStyleOptionViewItem & option,
    const QModelIndex &          index) const
{
    //DJV_DEBUG("djvFileBrowserTestContactSheetDelegate::paint");
    
    painter->save();
    painter->setClipRect(option.rect);
    
    //painter->fillRect(option.rect, Qt::red);
    
    const int margin = _p->context->style()->sizeMetric().margin;
    
    const QString & text =
        index.model()->data(index, Qt::DisplayRole).toString();
    const QPixmap & pixmap =
        index.model()->data(index, Qt::DecorationRole).value<QPixmap>();
    const QString & svg =
        index.model()->data(index, djvFileBrowserTestModel::SvgRole).toString();
    
    //DJV_DEBUG_PRINT("text = " << text);
    //DJV_DEBUG_PRINT("svg = " << svg);
        
    QStringList lines = text.split('\n');
    
    for (int i = 0; i < lines.count(); ++i)
    {
        lines[i] = option.fontMetrics.elidedText(
            lines[i], 
            Qt::ElideRight,
            option.rect.width() - margin * 2);
    }

    int lineHeight = option.fontMetrics.height();
    int textHeight = lines.count() * lineHeight;
    QPointF p;    
    if (! svg.isNull())
    {
        const QString key = QString("%1%2").arg(svg).arg(_p->thumbnailSize);
        QPixmap tmp;
        if (! QPixmapCache::find(key, &tmp))
        {
            const int size = _p->thumbnailSize * 0.4;
            tmp = QPixmap(size, size);
            tmp.fill(Qt::transparent);
            QPainter painterTmp(&tmp);
            QSvgRenderer(svg).render(&painterTmp, QRectF(p, QSizeF(size, size)));
            QPixmapCache::insert(key, tmp);
        }
        p.setX(option.rect.x() + option.rect.width () / 2 - tmp.width () / 2);
        p.setY(option.rect.y() + option.rect.height() / 2 - tmp.height() / 2);
        painter->drawPixmap(p, tmp);
    }
    else if (! pixmap.isNull())
    {
        p.setX(option.rect.x() + option.rect.width () / 2 - pixmap.width () / 2);
        p.setY(option.rect.y() + option.rect.height() / 2 - pixmap.height() / 2);
        painter->drawPixmap(p, pixmap);
    }
    
    p.setX(option.rect.x());
    p.setY(option.rect.y() + option.rect.height() - textHeight - margin * 2);
    if (option.state & QStyle::State_Selected)
    {
        QColor color = option.palette.color(QPalette::Highlight);
        color.setAlpha(127);
        painter->fillRect(option.rect, color);
    }
    
    painter->fillRect(
        p.x(),
        p.y(),
        option.rect.width(),
        textHeight + margin * 2,
        QColor(0, 0, 0, 127));
    p.setX(p.x() + margin);
    p.setY(p.y() + margin);
    for (int i = 0; i < lines.count(); ++i)
    {
        painter->drawText(
            p.x(),
            p.y() + i * lineHeight + option.fontMetrics.ascent(),
            lines[i]);
    }
    
    painter->restore();
}

void djvFileBrowserTestContactSheetDelegate::setThumbnailSize(int size)
{
    if (size == _p->thumbnailSize)
        return;
    _p->thumbnailSize = size;
}

