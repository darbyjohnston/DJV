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

//! \file djvDebugLog.cpp

#include <djvDebugLog.h>

#include <djvDebug.h>

#include <QCoreApplication>
#include <QPointer>
#include <QRegExp>
#include <QStringList>
#include <QVector>

namespace
{

const int max = 10000;

} // namespace

//------------------------------------------------------------------------------
// djvDebugLog::P
//------------------------------------------------------------------------------

struct djvDebugLog::P
{
    QVector<QString> messages;
};

//------------------------------------------------------------------------------
// djvDebugLog
//------------------------------------------------------------------------------

djvDebugLog::djvDebugLog(QObject * parent) :
    QObject(parent),
    _p(new P)
{}

djvDebugLog::~djvDebugLog()
{
    delete _p;
}

const QVector<QString> & djvDebugLog::messages() const
{
    return _p->messages;
}

djvDebugLog * djvDebugLog::global()
{
    static QPointer<djvDebugLog> global;
    
    if (! global)
    {
        global = QPointer<djvDebugLog>(new djvDebugLog(qApp));
    }
    
    return global.data();
}

void djvDebugLog::addMessage(const QString & context, const QString & message)
{
    Q_FOREACH(const QString & line, message.split(QRegExp("[\n\r]")))
    {
        QString s = QString("[%1] %2").arg(context, -30).arg(line);
        
        _p->messages.append(s);
        
        if (_p->messages.count() > max)
        {
            _p->messages.pop_front();
        }
        
        Q_EMIT this->message(s);
    }
}

