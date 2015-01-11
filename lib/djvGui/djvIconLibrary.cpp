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

//! \file djvIconLibrary.cpp

#include <djvIconLibrary.h>

#include <QApplication>
#include <QDirIterator>
#include <QIcon>
#include <QMap>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QPixmap>

//------------------------------------------------------------------------------
// djvIconLibrary::P
//------------------------------------------------------------------------------

struct djvIconLibrary::P
{
    QMap<QString, QIcon>   icons;
    QMap<QString, QPixmap> pixmaps;
};

//------------------------------------------------------------------------------
// djvIconLibrary
//------------------------------------------------------------------------------

djvIconLibrary::djvIconLibrary(QObject * parent) :
    QObject(parent),
    _p(new P)
{}

djvIconLibrary::~djvIconLibrary()
{
    delete _p;
}
    
const QIcon & djvIconLibrary::icon(const QString & name) const
{
    if (! _p->icons.contains(name))
    {
        QPixmap pixmap(QString(":%1").arg(name));
        
        QIcon icon;
        icon.addPixmap(pixmap, QIcon::Normal, QIcon::Off);
        
        /*{
            QPixmap tmp(pixmap);
            QPainter painter(&tmp);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(
                QRect(QPoint(), tmp.size()),
                QColor(255, 255, 255, 50));
            
            icon.addPixmap(tmp, QIcon::Disabled, QIcon::Off);
        }*/
        
        /*{
            QPixmap tmp(pixmap);
            QPainter painter(&tmp);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(
                QRect(QPoint(), tmp.size()),
                qApp->palette().color(QPalette::Highlight));
            
            icon.addPixmap(tmp, QIcon::Normal, QIcon::On);
        }*/
        
        /*{
            QPixmap tmp(pixmap);
            QPainter painter(&tmp);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(
                QRect(QPoint(), tmp.size()),
                qApp->palette().color(QPalette::Highlight));
            painter.fillRect(
                QRect(QPoint(), tmp.size()),
                QColor(255, 255, 255, 50));

            icon.addPixmap(tmp, QIcon::Disabled, QIcon::On);
        }*/
        
        const_cast<djvIconLibrary::P *>(_p)->icons.insert(name, icon);
    }
    
    return _p->icons[name];
}

QIcon djvIconLibrary::icon(const QString & off, const QString & on) const
{
    QIcon icon;
    
    icon.addPixmap(pixmap(off), QIcon::Normal, QIcon::Off);
    icon.addPixmap(pixmap(on), QIcon::Normal, QIcon::On);
    
    return icon;
}
    
const QPixmap & djvIconLibrary::pixmap(const QString & name) const
{
    if (! _p->pixmaps.contains(name))
    {
        QPixmap pixmap(QString(":%1").arg(name));
        
        const_cast<djvIconLibrary::P *>(_p)->pixmaps.insert(name, pixmap);
    }
    
    return _p->pixmaps[name];
}

QStringList djvIconLibrary::names() const
{
    QStringList names;

    QDirIterator it(":");

    while (it.hasNext())
    {
        QFileInfo fileInfo(it.fileInfo());

        if (0 == fileInfo.suffix().compare("png", Qt::CaseInsensitive))
        {
            names += fileInfo.fileName();
        }

        it.next();
    }

    return names;
}

QSize djvIconLibrary::defaultSize() const
{
    return QSize(25, 25);
}
    
djvIconLibrary * djvIconLibrary::global()
{
    static djvIconLibrary * iconLibrary = 0;
    
    if (! iconLibrary)
    {
        iconLibrary = new djvIconLibrary(qApp);
    }
    
    return iconLibrary;
}
