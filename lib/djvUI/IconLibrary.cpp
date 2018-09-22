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

#include <djvUI/IconLibrary.h>

#include <djvCore/Debug.h>

#include <QApplication>
#include <QDirIterator>
#include <QIcon>
#include <QMap>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QPixmap>

#include <iostream>

namespace djv
{
    namespace UI
    {
        struct IconLibrary::Private
        {
            QColor color = Qt::black;
            QMap<QString, QIcon> icons;
            QMap<QString, QPixmap> pixmaps;
        };

        IconLibrary::IconLibrary(QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            //DJV_DEBUG("IconLibrary::IconLibrary");
        }

        IconLibrary::~IconLibrary()
        {
            //DJV_DEBUG("IconLibrary::~IconLibrary");
        }

        void IconLibrary::setColor(const QColor & value)
        {
            if (value == _p->color)
                return;
            _p->color = value;
            _p->icons.clear();
            _p->pixmaps.clear();
        }
        
        namespace
        {
            QStringList getFileNames(const QString& name)
            {
                QStringList out;
                QFileInfo fileInfo(name);
                QDirIterator it(":" + fileInfo.path());
                while (it.hasNext())
                {
                    const QString s = it.fileInfo().baseName().mid(0, fileInfo.baseName().size());
                    if (fileInfo.baseName() == s && 0 == it.fileInfo().suffix().compare("png", Qt::CaseInsensitive))
                    {
                        out.append(it.fileInfo().filePath());
                    }
                    it.next();
                }
                return out;
            }
            
            QPixmap fillPixmap(const QPixmap& pixmap, const QColor& color)
            {
                QImage out;
                if (!pixmap.isNull())
                {
                    out = QImage(pixmap.width(), pixmap.height(), QImage::Format_ARGB32_Premultiplied);
                    out.fill(Qt::transparent);
                    QPainter imagePainter(&out);
                    imagePainter.drawPixmap(0, 0, pixmap);
                    imagePainter.setCompositionMode(QPainter::CompositionMode::CompositionMode_SourceIn);
                    imagePainter.fillRect(0, 0, pixmap.width(), pixmap.height(), color);
                }
                return QPixmap::fromImage(out);
            }
        
        } // namespace
        
        const QIcon & IconLibrary::icon(const QString & name) const
        {
            //DJV_DEBUG("IconLibrary::icon");
            //DJV_DEBUG_PRINT("name = " << name);
            if (!_p->icons.contains(name))
            {
                QIcon icon;
                Q_FOREACH(const QString& i, getFileNames(name))
                {
                    icon.addPixmap(fillPixmap(QPixmap(i), _p->color), QIcon::Normal, QIcon::Off);                    
                }
                //Q_FOREACH(const QSize& size, icon.availableSizes())
                //{
                //    DJV_DEBUG_PRINT("size = " << size.width() << " " << size.height());
                //}
                const_cast<Private *>(_p.get())->icons.insert(name, icon);
            }
            return _p->icons[name];
        }

        QIcon IconLibrary::icon(const QString & off, const QString & on) const
        {
            const QString key = off + on;
            if (!_p->icons.contains(key))
            {
                QIcon icon;
                Q_FOREACH(const QString& i, getFileNames(off))
                {
                    icon.addPixmap(fillPixmap(QPixmap(i), _p->color), QIcon::Normal, QIcon::Off);                    
                }
                Q_FOREACH(const QString& i, getFileNames(on))
                {
                    icon.addPixmap(fillPixmap(QPixmap(i), _p->color), QIcon::Normal, QIcon::On);
                }
                const_cast<Private *>(_p.get())->icons.insert(key, icon);
            }
            return _p->icons[key];
        }

        const QPixmap & IconLibrary::pixmap(const QString & name) const
        {
            //DJV_DEBUG("IconLibrary::pixmap");
            //DJV_DEBUG_PRINT("name = " << name);
            if (!_p->pixmaps.contains(name))
            {
                const_cast<Private *>(_p.get())->pixmaps.insert(name, fillPixmap(QPixmap(":" + name), _p->color));
            }
            return _p->pixmaps[name];
        }

        QStringList IconLibrary::names() const
        {
            QStringList names;
            QDirIterator it(":");
            while (it.hasNext())
            {
                if (0 == it.fileInfo().suffix().compare("png", Qt::CaseInsensitive))
                {
                    names += it.fileInfo().fileName();
                }
                it.next();
            }
            return names;
        }

    } // namespace UI
} // namespace djv
