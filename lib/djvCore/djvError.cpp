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

//! \file djvError.cpp

#include <djvError.h>

#include <QList>
#include <QString>

//------------------------------------------------------------------------------
// djvError::Message
//------------------------------------------------------------------------------

djvError::Message::Message(const QString & prefix, const QString & string) :
    prefix(prefix),
    string(string)
{}

//------------------------------------------------------------------------------
// djvError::P
//------------------------------------------------------------------------------

struct djvError::P
{
    QList<Message> messages;
};

//------------------------------------------------------------------------------
// djvError
//------------------------------------------------------------------------------

djvError::djvError()
{
    init();
}

djvError::djvError(const QString & string)
{
    init();

    _p->messages += Message(QString(), string);
}

djvError::djvError(const QString & prefix, const QString & string)
{
    init();

    _p->messages += Message(prefix, string);
}

djvError::djvError(const djvError & other)
{
    init();

    _p->messages = other._p->messages;
}

djvError::~djvError()
{
    delete _p;
}

const QList<djvError::Message> & djvError::messages() const
{
    return _p->messages;
}
    
void djvError::add(const QString & string)
{
    _p->messages.append(Message(QString(), string));
}

void djvError::add(const QString & prefix, const QString & string)
{
    _p->messages.append(Message(prefix, string));
}

djvError & djvError::operator = (const djvError & other)
{
    if (this != &other)
    {
        _p->messages = other._p->messages;
    }

    return *this;
}

void djvError::init()
{
    _p = new P;
}

