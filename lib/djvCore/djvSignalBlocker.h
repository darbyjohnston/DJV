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

//! \file djvSignalBlocker.h

#ifndef DJV_SIGNAL_BLOCKER_H
#define DJV_SIGNAL_BLOCKER_H

#include <djvUtil.h>

#include <QObject>
#include <QVector>

//! \addtogroup djvCoreMisc
//@{

//------------------------------------------------------------------------------
//! \class djvSignalBlocker
//!
//! This class provides functionality to temporarily block QObject signals.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvSignalBlocker
{
public:

    //! Constructor.
    
    djvSignalBlocker(QObject *);

    //! Constructor.
    
    djvSignalBlocker(const QObjectList &);

    //! Constructor.
    
    djvSignalBlocker(const QVector<QObject *> &);

    //! Destructor.
    
    ~djvSignalBlocker();
    
    //! Add an object.
    
    void add(QObject *);
    
    //! Add a list of objects.

    void add(const QObjectList &);
    
    //! Add a list of objects.

    void add(const QVector<QObject *> &);
    
private:

    DJV_PRIVATE_COPY(djvSignalBlocker)
    
    QObjectList _objects;
};

//@} // djvCoreMisc

#endif // DJV_SIGNAL_BLOCKER_H
