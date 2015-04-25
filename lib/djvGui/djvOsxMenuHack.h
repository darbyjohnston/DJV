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

//! \file djvOsxMenuHack.h

#ifndef DJV_OSX_MENU_HACK_H
#define DJV_OSX_MENU_HACK_H

#include <djvGuiExport.h>

#include <djvUtil.h>

#include <QObject>

struct djvOsxMenuHackPrivate;

class QAction;
class QMenu;

//! \addtogroup djvGuiMisc
//@{

//------------------------------------------------------------------------------
//! \class djvOsxMenuHack
//!
//! This class provides a hack to fix keyboard shortcuts without modifiers on
//! OSX.
//!
//! https://bugreports.qt-project.org/browse/QTBUG-38256
//! http://thebreakfastpost.com/2014/06/03/single-key-menu-shortcuts-with-qt5-on-osx/
//!
//! \todo Remove this class when QTBUG-38256 is resolved.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvOsxMenuHack : public QObject
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvOsxMenuHack(QObject * parent = 0);

    //! Destructor.

    virtual ~djvOsxMenuHack();
    
    //! Fix the list of actions.
    
    void fix(const QList<QAction *> &);

private Q_SLOTS:

    void actionCallback(QObject *);

private:

    DJV_PRIVATE_COPY(djvOsxMenuHack);
    
    djvOsxMenuHackPrivate * _p;
};

//@} // djvGuiMisc

#endif // DJV_OSX_MENU_HACK_H

