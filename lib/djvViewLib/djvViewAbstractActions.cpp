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

//! \file djvViewAbstractActions.cpp

#include <djvViewAbstractActions.h>

#include <djvOsxMenuHack.h>

#include <QAction>
#include <QActionGroup>

//------------------------------------------------------------------------------
// djvViewAbstractActions::P
//------------------------------------------------------------------------------

struct djvViewAbstractActions::P
{
    P() :
        osxMenuHack(0)
    {}
    
    djvOsxMenuHack * osxMenuHack;
};

//------------------------------------------------------------------------------
// djvViewAbstractActions
//------------------------------------------------------------------------------

djvViewAbstractActions::djvViewAbstractActions(QObject * parent) :
	QObject(parent),
    _p(new P)
{
    _p->osxMenuHack = new djvOsxMenuHack(this);
}

djvViewAbstractActions::~djvViewAbstractActions()
{
    delete _p;
}

const QList<QAction *> djvViewAbstractActions::actions() const
{
    return _actions.values();
}

QAction * djvViewAbstractActions::action(int index) const
{
    return _actions.contains(index) ? _actions[index] : 0;
}

const QList<QActionGroup *> djvViewAbstractActions::groups() const
{
    return _groups.values();
}

QActionGroup * djvViewAbstractActions::group(int index) const
{
    return _groups.contains(index) ? _groups[index] : 0;
}

void djvViewAbstractActions::osxMenuHack()
{
    QList<QAction *> actions = _actions.values();
    
    Q_FOREACH(QActionGroup * group, _groups)
    {
        actions += group->actions();
    }
    
    _p->osxMenuHack->fix(actions);
}
