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

//! \file djvColorProfile.cpp

#include <djvColorProfile.h>

#include <djvAssert.h>
#include <djvDebug.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvColorProfile::Exposure
//------------------------------------------------------------------------------

djvColorProfile::Exposure::Exposure(
    double value,
    double defog,
    double kneeLow,
    double kneeHigh) :
    value   (value),
    defog   (defog),
    kneeLow (kneeLow),
    kneeHigh(kneeHigh)
{}

//------------------------------------------------------------------------------
// djvColorProfile
//------------------------------------------------------------------------------

djvColorProfile::djvColorProfile() :
    type (RAW),
    gamma(2.2)
{}

const QStringList & djvColorProfile::profileLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvColorProfile", "Raw") <<
        qApp->translate("djvColorProfile", "Gamma") <<
        qApp->translate("djvColorProfile", "LUT") <<
        qApp->translate("djvColorProfile", "Exposure");

    DJV_ASSERT(data.count() == PROFILE_COUNT);

    return data;
}

//------------------------------------------------------------------------------

bool operator == (
    const djvColorProfile::Exposure & a,
    const djvColorProfile::Exposure & b)
{
    return
        djvMath::fuzzyCompare(a.value, b.value) &&
        djvMath::fuzzyCompare(a.defog, b.defog) &&
        djvMath::fuzzyCompare(a.kneeLow, b.kneeLow) &&
        djvMath::fuzzyCompare(a.kneeHigh, b.kneeHigh);
}

bool operator != (
    const djvColorProfile::Exposure & a,
    const djvColorProfile::Exposure & b)
{
    return ! (a == b);
}

bool operator == (const djvColorProfile & a, const djvColorProfile & b)
{
    return
        a.type     == b.type     &&
        djvMath::fuzzyCompare(a.gamma, b.gamma) &&
        a.lut      == b.lut      &&
        a.exposure == b.exposure;
}

bool operator != (const djvColorProfile & a, const djvColorProfile & b)
{
    return ! (a == b);
}

QStringList & operator >> (QStringList & in, djvColorProfile::Exposure & out)
    throw (QString)
{
    return in >> out.value >> out.defog >> out.kneeLow >> out.kneeHigh;
}

QStringList & operator << (QStringList & out, const djvColorProfile::Exposure & in)
{
    return out << in.value << in.defog << in.kneeLow << in.kneeHigh;
}

_DJV_STRING_OPERATOR_LABEL(
    djvColorProfile::PROFILE,
    djvColorProfile::profileLabels())

djvDebug & operator << (djvDebug & debug, const djvColorProfile::Exposure & in)
{
    return debug << in.value << " " << in.defog << " " << in.kneeLow << " " <<
        in.kneeHigh;
}

djvDebug & operator << (djvDebug & debug, const djvColorProfile::PROFILE & in)
{
    return debug << djvStringUtil::label(in);
}

djvDebug & operator << (djvDebug & debug, const djvColorProfile & in)
{
    return debug <<
        in.type << ", " <<
        "gamma = " << in.gamma << ", " <<
        "lut = " << in.lut << ", " <<
        "exposure = " << in.exposure;
}

