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

//! \file djvSignalBlocker.cpp

#include <djvSignalBlocker.h>

//------------------------------------------------------------------------------
// djvSignalBlocker
//------------------------------------------------------------------------------

namespace
{

QObjectList allObjects;

} // namespace

djvSignalBlocker::djvSignalBlocker(QObject * object)
{
    add(object);
}

djvSignalBlocker::djvSignalBlocker(const QObjectList & objects)
{
    add(objects);
}

djvSignalBlocker::djvSignalBlocker(const QVector<QObject *> & objects)
{
    add(objects);
}

djvSignalBlocker::~djvSignalBlocker()
{
    Q_FOREACH (QObject * object, _objects)
    {
        allObjects.removeOne(object);
        
        if (allObjects.indexOf(object) == -1)
        {
            object->blockSignals(false);
        }
    }
}

void djvSignalBlocker::add(QObject * object)
{
    object->blockSignals(true);
        
    _objects += object;
    
    allObjects += object;
}

void djvSignalBlocker::add(const QObjectList & objects)
{
    Q_FOREACH (QObject * object, objects)
    {
        object->blockSignals(true);
        
        _objects += object;
        
        allObjects += object;
    }
}

void djvSignalBlocker::add(const QVector<QObject *> & objects)
{
    for (int i = 0; i < objects.count(); ++i)
    {
        objects[i]->blockSignals(true);
        
        _objects += objects[i];
        
        allObjects += objects[i];
    }
}
