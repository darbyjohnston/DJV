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

namespace djv
{
    namespace UI
    {
        struct IconLibrary::Private
        {
            QMap<QString, QIcon>   icons;
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

        namespace
        {
            QString getFullName(const QString& name, int DPI)
            {
                return QString(":%1%2DPI.png").arg(name).arg(DPI);
            }

        } // namespace

        const QIcon & IconLibrary::icon(const QString & name, int DPI) const
        {
            //DJV_DEBUG("IconLibrary::icon");
            const QString fullName = getFullName(name, DPI);
            //DJV_DEBUG_PRINT("full name = " << fullName);
            if (!_p->icons.contains(fullName))
            {
                QIcon icon;
                icon.addPixmap(fullName, QIcon::Normal, QIcon::Off);
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
                const_cast<Private *>(_p.get())->icons.insert(fullName, icon);
            }
            return _p->icons[fullName];
        }

        QIcon IconLibrary::icon(const QString & off, const QString & on, int DPI) const
        {
            const QString offFullName = getFullName(off, DPI);
            const QString onFullName = getFullName(on, DPI);
            const QString key = offFullName + onFullName;
            if (!_p->icons.contains(key))
            {
                QIcon icon;
                icon.addPixmap(offFullName, QIcon::Normal, QIcon::Off);
                icon.addPixmap(onFullName, QIcon::Normal, QIcon::On);
                const_cast<Private *>(_p.get())->icons.insert(key, icon);
            }
            return _p->icons[key];
        }

        const QPixmap & IconLibrary::pixmap(const QString & name, int DPI) const
        {
            const QString fullName = getFullName(name, DPI);
            if (!_p->pixmaps.contains(fullName))
            {
                const_cast<Private *>(_p.get())->pixmaps.insert(fullName, QPixmap(fullName));
            }
            return _p->pixmaps[fullName];
        }

        QStringList IconLibrary::names() const
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

    } // namespace UI
} // namespace djv
