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

//! \file djvMemory.cpp

#include <djvMemory.h>

#include <djvAssert.h>

#include <QCoreApplication>

#include <string.h>

//------------------------------------------------------------------------------
// djvMemory
//------------------------------------------------------------------------------

djvMemory::~djvMemory()
{}

const quint64 djvMemory::kilobyte = 1024;
const quint64 djvMemory::megabyte = kilobyte * 1024;
const quint64 djvMemory::gigabyte = megabyte * 1024;
const quint64 djvMemory::terabyte = gigabyte * 1024;

QString djvMemory::sizeLabel(quint64 in)
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvMemory", "%1TB") <<
        qApp->translate("djvMemory", "%1GB") <<
        qApp->translate("djvMemory", "%1MB") <<
        qApp->translate("djvMemory", "%1KB");

    if (in >= djvMemory::terabyte)
    {
        return data[0].arg(in /
            static_cast<double>(djvMemory::terabyte), 0, 'f', 2);
    }
    else if (in >= djvMemory::gigabyte)
    {
        return data[1].arg(in /
            static_cast<double>(djvMemory::gigabyte), 0, 'f', 2);
    }
    else if (in >= djvMemory::megabyte)
    {
        return data[2].arg(in /
            static_cast<double>(djvMemory::megabyte), 0, 'f', 2);
    }
    else
    {
        return data[3].arg(in /
            static_cast<double>(djvMemory::kilobyte), 0, 'f', 2);
    }
}

const QStringList & djvMemory::endianLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvMemory", "MSB") <<
        qApp->translate("djvMemory", "LSB");

    DJV_ASSERT(data.count() == ENDIAN_COUNT);

    return data;
}

void djvMemory::copy(const void * in, void * out, quint64 size)
{
    ::memcpy(out, in, size);
}

void djvMemory::zero(void * out, quint64 size)
{
    ::memset(out, 0, size);
}

int djvMemory::compare(const void * a, const void * b, quint64 size)
{
    return ::memcmp(a, b, size);
}

djvMemory::ENDIAN djvMemory::endian()
{
    static const int tmp = 1;

    static const quint8 * const p = reinterpret_cast<const quint8 *>(&tmp);

    return *p ? LSB : MSB;
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvMemory::ENDIAN, djvMemory::endianLabels())

