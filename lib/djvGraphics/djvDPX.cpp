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

#include <djvDPX.h>

#include <djvAssert.h>
#include <djvError.h>
#include <djvMemory.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvDPX::Options
//------------------------------------------------------------------------------

djvDPX::Options::Options() :
    inputColorProfile (djvCineon::COLOR_PROFILE_AUTO),
    outputColorProfile(djvCineon::COLOR_PROFILE_FILM_PRINT),
    version           (djvDPX::VERSION_2_0),
    type              (djvDPX::TYPE_U10),
    endian            (djvDPX::ENDIAN_MSB)
{}

//------------------------------------------------------------------------------
// djvDPX
//------------------------------------------------------------------------------

const QString djvDPX::staticName = "DPX";

const QStringList & djvDPX::versionLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvDPX", "1.0") <<
        qApp->translate("djvDPX", "2.0");
    DJV_ASSERT(data.count() == VERSION_COUNT);
    return data;
}

const QStringList & djvDPX::typeLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvDPX", "Auto") <<
        qApp->translate("djvDPX", "U10");
    DJV_ASSERT(data.count() == TYPE_COUNT);
    return data;
}

const QStringList & djvDPX::endianLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvDPX", "Auto") <<
        djvMemory::endianLabels();
    DJV_ASSERT(data.count() == ENDIAN_COUNT);
    return data;
}

const QStringList & djvDPX::tagLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvDPX", "Source Offset") <<
        qApp->translate("djvDPX", "Source Center") <<
        qApp->translate("djvDPX", "Source Size") <<
        qApp->translate("djvDPX", "Source File") <<
        qApp->translate("djvDPX", "Source Time") <<
        qApp->translate("djvDPX", "Source Input Device") <<
        qApp->translate("djvDPX", "Source Input Serial") <<
        qApp->translate("djvDPX", "Source Border") <<
        qApp->translate("djvDPX", "Source Pixel Aspect") <<
        qApp->translate("djvDPX", "Source ScanSize") <<
        qApp->translate("djvDPX", "Film Format") <<
        qApp->translate("djvDPX", "Film Frame") <<
        qApp->translate("djvDPX", "Film Sequence") <<
        qApp->translate("djvDPX", "Film Hold") <<
        qApp->translate("djvDPX", "Film Frame Rate") <<
        qApp->translate("djvDPX", "Film Shutter") <<
        qApp->translate("djvDPX", "Film Frame ID") <<
        qApp->translate("djvDPX", "Film Slate") <<
        qApp->translate("djvDPX", "TV Interlace") <<
        qApp->translate("djvDPX", "TV Field") <<
        qApp->translate("djvDPX", "TV Video Signal") <<
        qApp->translate("djvDPX", "TV Sample Rate") <<
        qApp->translate("djvDPX", "TV Frame Rate") <<
        qApp->translate("djvDPX", "TV Time Offset") <<
        qApp->translate("djvDPX", "TV Gamma") <<
        qApp->translate("djvDPX", "TV Black Level") <<
        qApp->translate("djvDPX", "TV Black Gain") <<
        qApp->translate("djvDPX", "TV Break Point") <<
        qApp->translate("djvDPX", "TV White Level") <<
        qApp->translate("djvDPX", "TV Integration Times");
    DJV_ASSERT(data.count() == TAG_COUNT);
    return data;
}

const QStringList & djvDPX::optionsLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvDPX", "Input Color Profile") <<
        qApp->translate("djvDPX", "Input Film Print") <<
        qApp->translate("djvDPX", "Output Color Profile") <<
        qApp->translate("djvDPX", "Output Film Print") <<
        qApp->translate("djvDPX", "Version") <<
        qApp->translate("djvDPX", "Type") <<
        qApp->translate("djvDPX", "Endian");
    DJV_ASSERT(data.count() == OPTIONS_COUNT);
    return data;
}

_DJV_STRING_OPERATOR_LABEL(djvDPX::VERSION, djvDPX::versionLabels())
_DJV_STRING_OPERATOR_LABEL(djvDPX::TYPE, djvDPX::typeLabels())
_DJV_STRING_OPERATOR_LABEL(djvDPX::ENDIAN, djvDPX::endianLabels())
