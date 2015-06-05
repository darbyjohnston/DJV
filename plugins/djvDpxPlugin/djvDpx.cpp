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

//! \file djvDpx.cpp

#include <djvDpx.h>

#include <djvAssert.h>
#include <djvError.h>
#include <djvMemory.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvDpx::Options
//------------------------------------------------------------------------------

djvDpx::Options::Options() :
    inputColorProfile (djvCineon::COLOR_PROFILE_AUTO),
    outputColorProfile(djvCineon::COLOR_PROFILE_FILM_PRINT),
    version           (djvDpx::VERSION_2_0),
    type              (djvDpx::TYPE_U10),
    endian            (djvDpx::ENDIAN_MSB)
{}

//------------------------------------------------------------------------------
// djvDpx
//------------------------------------------------------------------------------

const QString djvDpx::staticName = "DPX";

const QStringList & djvDpx::versionLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvDpx", "1.0") <<
        qApp->translate("djvDpx", "2.0");

    DJV_ASSERT(data.count() == VERSION_COUNT);

    return data;
}

const QStringList & djvDpx::typeLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvDpx", "Auto") <<
        qApp->translate("djvDpx", "U10");

    DJV_ASSERT(data.count() == TYPE_COUNT);

    return data;
}

const QStringList & djvDpx::endianLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvDpx", "Auto") <<
        djvMemory::endianLabels();

    DJV_ASSERT(data.count() == ENDIAN_COUNT);

    return data;
}

const QStringList & djvDpx::tagLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvDpx", "Source Offset") <<
        qApp->translate("djvDpx", "Source Center") <<
        qApp->translate("djvDpx", "Source Size") <<
        qApp->translate("djvDpx", "Source File") <<
        qApp->translate("djvDpx", "Source Time") <<
        qApp->translate("djvDpx", "Source Input Device") <<
        qApp->translate("djvDpx", "Source Input Serial") <<
        qApp->translate("djvDpx", "Source Border") <<
        qApp->translate("djvDpx", "Source Pixel Aspect") <<
        qApp->translate("djvDpx", "Source ScanSize") <<
        qApp->translate("djvDpx", "Film Format") <<
        qApp->translate("djvDpx", "Film Frame") <<
        qApp->translate("djvDpx", "Film Sequence") <<
        qApp->translate("djvDpx", "Film Hold") <<
        qApp->translate("djvDpx", "Film Frame Rate") <<
        qApp->translate("djvDpx", "Film Shutter") <<
        qApp->translate("djvDpx", "Film Frame ID") <<
        qApp->translate("djvDpx", "Film Slate") <<
        qApp->translate("djvDpx", "TV Interlace") <<
        qApp->translate("djvDpx", "TV Field") <<
        qApp->translate("djvDpx", "TV Video Signal") <<
        qApp->translate("djvDpx", "TV Sample Rate") <<
        qApp->translate("djvDpx", "TV Frame Rate") <<
        qApp->translate("djvDpx", "TV Time Offset") <<
        qApp->translate("djvDpx", "TV Gamma") <<
        qApp->translate("djvDpx", "TV Black Level") <<
        qApp->translate("djvDpx", "TV Black Gain") <<
        qApp->translate("djvDpx", "TV Break Point") <<
        qApp->translate("djvDpx", "TV White Level") <<
        qApp->translate("djvDpx", "TV Integration Times");

    DJV_ASSERT(data.count() == TAG_COUNT);

    return data;
}

const QStringList & djvDpx::optionsLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvDpx", "Input Color Profile") <<
        qApp->translate("djvDpx", "Input Film Print") <<
        qApp->translate("djvDpx", "Output Color Profile") <<
        qApp->translate("djvDpx", "Output Film Print") <<
        qApp->translate("djvDpx", "Version") <<
        qApp->translate("djvDpx", "Type") <<
        qApp->translate("djvDpx", "Endian");

    DJV_ASSERT(data.count() == OPTIONS_COUNT);

    return data;
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvDpx::VERSION, djvDpx::versionLabels())
_DJV_STRING_OPERATOR_LABEL(djvDpx::TYPE, djvDpx::typeLabels())
_DJV_STRING_OPERATOR_LABEL(djvDpx::ENDIAN, djvDpx::endianLabels())
