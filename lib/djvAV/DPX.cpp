//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAV/DPX.h>

#include <djvCore/Assert.h>
#include <djvCore/Error.h>
#include <djvCore/Memory.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        const QString DPX::staticName = "DPX";

        const QStringList & DPX::versionLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::DPX", "1.0") <<
                qApp->translate("djv::AV::DPX", "2.0");
            DJV_ASSERT(data.count() == VERSION_COUNT);
            return data;
        }

        const QStringList & DPX::typeLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::DPX", "Auto") <<
                qApp->translate("djv::AV::DPX", "U10");
            DJV_ASSERT(data.count() == TYPE_COUNT);
            return data;
        }

        const QStringList & DPX::endianLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::DPX", "Auto") <<
                Core::Memory::endianLabels();
            DJV_ASSERT(data.count() == ENDIAN_COUNT);
            return data;
        }

        const QStringList & DPX::tagLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::DPX", "Source Offset") <<
                qApp->translate("djv::AV::DPX", "Source Center") <<
                qApp->translate("djv::AV::DPX", "Source Size") <<
                qApp->translate("djv::AV::DPX", "Source File") <<
                qApp->translate("djv::AV::DPX", "Source Time") <<
                qApp->translate("djv::AV::DPX", "Source Input Device") <<
                qApp->translate("djv::AV::DPX", "Source Input Serial") <<
                qApp->translate("djv::AV::DPX", "Source Border") <<
                qApp->translate("djv::AV::DPX", "Source Pixel Aspect") <<
                qApp->translate("djv::AV::DPX", "Source ScanSize") <<
                qApp->translate("djv::AV::DPX", "Film Format") <<
                qApp->translate("djv::AV::DPX", "Film Frame") <<
                qApp->translate("djv::AV::DPX", "Film Sequence") <<
                qApp->translate("djv::AV::DPX", "Film Hold") <<
                qApp->translate("djv::AV::DPX", "Film Frame Rate") <<
                qApp->translate("djv::AV::DPX", "Film Shutter") <<
                qApp->translate("djv::AV::DPX", "Film Frame ID") <<
                qApp->translate("djv::AV::DPX", "Film Slate") <<
                qApp->translate("djv::AV::DPX", "TV Interlace") <<
                qApp->translate("djv::AV::DPX", "TV Field") <<
                qApp->translate("djv::AV::DPX", "TV Video Signal") <<
                qApp->translate("djv::AV::DPX", "TV Sample Rate") <<
                qApp->translate("djv::AV::DPX", "TV Frame Rate") <<
                qApp->translate("djv::AV::DPX", "TV Time Offset") <<
                qApp->translate("djv::AV::DPX", "TV Gamma") <<
                qApp->translate("djv::AV::DPX", "TV Black Level") <<
                qApp->translate("djv::AV::DPX", "TV Black Gain") <<
                qApp->translate("djv::AV::DPX", "TV Break Point") <<
                qApp->translate("djv::AV::DPX", "TV White Level") <<
                qApp->translate("djv::AV::DPX", "TV Integration Times");
            DJV_ASSERT(data.count() == TAG_COUNT);
            return data;
        }

        const QStringList & DPX::optionsLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::DPX", "Input Color Profile") <<
                qApp->translate("djv::AV::DPX", "Input Film Print") <<
                qApp->translate("djv::AV::DPX", "Output Color Profile") <<
                qApp->translate("djv::AV::DPX", "Output Film Print") <<
                qApp->translate("djv::AV::DPX", "Version") <<
                qApp->translate("djv::AV::DPX", "Type") <<
                qApp->translate("djv::AV::DPX", "Endian");
            DJV_ASSERT(data.count() == OPTIONS_COUNT);
            return data;
        }

    } // namespace AV

    _DJV_STRING_OPERATOR_LABEL(AV::DPX::VERSION, AV::DPX::versionLabels());
    _DJV_STRING_OPERATOR_LABEL(AV::DPX::TYPE, AV::DPX::typeLabels());
    _DJV_STRING_OPERATOR_LABEL(AV::DPX::ENDIAN, AV::DPX::endianLabels());

} // namespace djv
