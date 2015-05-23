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

//! \file djvError.h

#ifndef DJV_ERROR_H
#define DJV_ERROR_H

#include <djvUtil.h>

#include <QString>

struct djvErrorPrivate;

//! \addtogroup djvCoreMisc
//@{

//------------------------------------------------------------------------------
//! \class djvError
//!
//! This class provides the base exception class.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvError
{
public:

    //! This struct provides an error message.
    
    struct Message
    {
        Message(
            const QString & prefix = QString(),
            const QString & string = QString());

        QString prefix;
        QString string;
    };

    //! Constructor.

    djvError();

    //! Constructor.

    djvError(const QString & string);

    //! Constructor.

    djvError(const QString & prefix, const QString & string);

    //! Constructor.

    djvError(const djvError &);

    //! Destructor.

    virtual ~djvError();

    //! Get the message list.

    const QList<Message> & messages() const;
    
    //! Get the number of messages.
    
    int count() const;
    
    //! Add a message to the list.
    
    void add(const QString & string);
    
    //! Add a message to the list.
    
    void add(const QString & prefix, const QString & string);

    djvError & operator = (const djvError &);

private:

    void init();

    djvErrorPrivate * _p;
};

//@} // djvCoreMisc

#endif // DJV_ERROR_H

