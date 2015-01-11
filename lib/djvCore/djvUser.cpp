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

//! \file djvUser.cpp

#include <djvUser.h>

#include <djvStringUtil.h>

#if defined(DJV_WINDOWS)
#include <windows.h>
#else
#include <pwd.h>
#include <unistd.h>
#endif

//------------------------------------------------------------------------------
// djvUser
//------------------------------------------------------------------------------

djvUser::~djvUser()
{}

QString djvUser::current()
{
    QString out;

#if defined(DJV_WINDOWS)

    TCHAR tmp[djvStringUtil::cStringLength] = { 0 };
    DWORD size = djvStringUtil::cStringLength;
    ::GetUserName(tmp, &size);
    out = QString(tmp);

#else // DJV_WINDOWS

    out = uidToString(::getuid());

#endif // DJV_WINDOWS

    return out;
}

QString djvUser::uidToString(uid_t value)
{
    QString out;

#if ! defined(DJV_WINDOWS)

    struct passwd * buf = ::getpwuid(value);

    if (buf)
    {
        out = buf->pw_name;
    }

#endif // ! DJV_WINDOWS

    return out;
}

