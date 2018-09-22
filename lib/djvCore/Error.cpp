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

#include <djvCore/Error.h>

#include <QList>
#include <QString>

namespace djv
{
    namespace Core
    {
        Error::Message::Message(const QString & prefix, const QString & string) :
            prefix(prefix),
            string(string)
        {}

        struct Error::Private
        {
            QList<Error::Message> messages;
        };

        Error::Error() :
            _p(new Private)
        {}

        Error::Error(const QString & string) :
            _p(new Private)
        {
            _p->messages += Message(QString(), string);
        }

        Error::Error(const QString & prefix, const QString & string) :
            _p(new Private)
        {
            _p->messages += Message(prefix, string);
        }

        Error::Error(const Error & other) :
            _p(new Private)
        {
            _p->messages = other._p->messages;
        }

        Error::~Error()
        {}

        const QList<Error::Message> & Error::messages() const
        {
            return _p->messages;
        }

        int Error::count() const
        {
            return _p->messages.count();
        }

        void Error::add(const QString & string)
        {
            _p->messages.append(Message(QString(), string));
        }

        void Error::add(const QString & prefix, const QString & string)
        {
            _p->messages.append(Message(prefix, string));
        }

        Error & Error::operator = (const Error & other)
        {
            if (this != &other)
            {
                _p->messages = other._p->messages;
            }
            return *this;
        }

    } // namespace Core
} // namespace djv
