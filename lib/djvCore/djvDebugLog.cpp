//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

#include <djvCoreApplication.h>

#include <djvDebug.h>

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
    P() :
        print(false)
    {}
    
    QVector<QString> messages;
    bool             print;
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

bool djvDebugLog::hasPrint() const
{
    return _p->print;
}

djvDebugLog * djvDebugLog::global()
{
    static djvDebugLog * object = 0;
    
    if (! object)
    {
        object = new djvDebugLog(qApp);
    }
    
    return object;
}

void djvDebugLog::addMessage(const QString & context, const QString & message)
{
    Q_FOREACH(const QString & line, message.split(QRegExp("[\n\r]")))
    {
        QString s = QString("[%1] %2").arg(context).arg(line);
        
        _p->messages.append(s);
        
        if (_p->messages.count() > max)
        {
            _p->messages.pop_front();
        }
        
        if (_p->print)
        {
            print(s);
        }
        
        Q_EMIT this->message(s);
    }
}

void djvDebugLog::setPrint(bool print)
{
    if (print == _p->print)
        return;

    _p->print = print;
    
    if (_p->print)
    {
        while (_p->messages.count())
        {
            this->print(_p->messages.first());
            
            _p->messages.pop_front();
        }
    }
    
    Q_EMIT printChanged(_p->print);
}

void djvDebugLog::print(const QString & string)
{
    DJV_CORE_APP->printMessage(string);
}

