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

//! \file djvErrorUtil.cpp

#include <djvErrorUtil.h>

#include <djvError.h>

#include <QString>

#if defined(DJV_WINDOWS)
#include <windows.h>
#endif // DJV_WINDOWS

#include <stdio.h>

//------------------------------------------------------------------------------
// djvErrorUtil
//------------------------------------------------------------------------------

QString djvErrorUtil::format(const djvError & error)
{
    return error.prefix().isEmpty() ?
        QString("[ERROR] %1").arg(error.string()) :
        QString("[ERROR %1] %2").arg(error.prefix()).arg(error.string());
}

void djvErrorUtil::print(const djvError & in)
{
    ::printf("%s\n", format(in).toLatin1().data());
}

QString djvErrorUtil::lastError()
{
    QString out;

#if defined(DJV_WINDOWS)

    struct Buffer
    {
        Buffer() :
            p(0)
        {}
        
        ~Buffer()
        {
            LocalFree(p);
        }
        
        LPTSTR * p;
        
    } buffer;

    ::FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        0,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&buffer.p,
        0,
        0);

    out = QString((LPCTSTR)buffer.p);

#endif // DJV_WINDOWS

    return out;
}
