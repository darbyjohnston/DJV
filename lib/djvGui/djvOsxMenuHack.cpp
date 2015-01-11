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

//! \file djvOsxMenuHack.cpp

#include <djvOsxMenuHack.h>

#include <QAction>
#include <QMenu>
#include <QShortcut>
#include <QSignalMapper>

//------------------------------------------------------------------------------
// djvOsxMenuHack::P
//------------------------------------------------------------------------------

struct djvOsxMenuHack::P
{
    P() :
        mapper(0)
    {}
    
    QList<QShortcut *> shortcuts;
    QSignalMapper *    mapper;
};

//------------------------------------------------------------------------------
// djvOsxMenuHack
//------------------------------------------------------------------------------

djvOsxMenuHack::djvOsxMenuHack(QObject * parent) :
    QObject(parent),
    _p(new P)
{}

djvOsxMenuHack::~djvOsxMenuHack()
{
    Q_FOREACH(QShortcut * shortcut, _p->shortcuts)
    {
        delete shortcut;
    }
    
    delete _p;
}

namespace
{

bool fixKey(int key)
{
    return ! (
        (key & Qt::MetaModifier) ||
        (key & Qt::ControlModifier) ||
        (key & Qt::AltModifier));
}

} // namespace

void djvOsxMenuHack::fix(const QList<QAction *> & actions)
{
#if defined(DJV_OSX) && QT_VERSION >= 0x050100

    delete _p->mapper;

    _p->mapper = new QSignalMapper(this);

    connect(
        _p->mapper,
        SIGNAL(mapped(QObject *)),
        SLOT(actionCallback(QObject *)));
    
    Q_FOREACH(QAction * action, actions)
    {
        QKeySequence key = action->shortcut();
        
        bool fix = false;
        
        for (int i = 0; i < key.count(); ++i)
        {
            fix |= fixKey(key[i]);
        }
        
        if (fix)
        {
            QShortcut * shortcut = new QShortcut(key, action->parentWidget());
            
            _p->mapper->setMapping(shortcut, action);

            _p->mapper->connect(
                shortcut,
                SIGNAL(activated()),
                SLOT(map()));
            
            //action->setShortcut(QKeySequence());
            
            _p->shortcuts += shortcut;
        }
    }
    
#endif // DJV_OSX
}

void djvOsxMenuHack::actionCallback(QObject * object)
{
    QAction * action = qobject_cast<QAction *>(object);
    
    if (action && action->isEnabled())
    {
        action->trigger();
    }
}
