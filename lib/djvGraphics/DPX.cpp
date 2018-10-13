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

#include <djvGraphics/DPX.h>

#include <djvCore/Assert.h>
#include <djvCore/Error.h>
#include <djvCore/Memory.h>

#include <QCoreApplication>

namespace djv
{
    namespace Graphics
    {
        const QString DPX::staticName = "DPX";

        const QStringList & DPX::versionLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::DPX", "1.0") <<
                qApp->translate("djv::Graphics::DPX", "2.0");
            DJV_ASSERT(data.count() == VERSION_COUNT);
            return data;
        }

        const QStringList & DPX::typeLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::DPX", "Auto") <<
                qApp->translate("djv::Graphics::DPX", "U10");
            DJV_ASSERT(data.count() == TYPE_COUNT);
            return data;
        }

        const QStringList & DPX::endianLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::DPX", "Auto") <<
                Core::Memory::endianLabels();
            DJV_ASSERT(data.count() == ENDIAN_COUNT);
            return data;
        }

        const QStringList & DPX::tagLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::DPX", "Source Offset") <<
                qApp->translate("djv::Graphics::DPX", "Source Center") <<
                qApp->translate("djv::Graphics::DPX", "Source Size") <<
                qApp->translate("djv::Graphics::DPX", "Source File") <<
                qApp->translate("djv::Graphics::DPX", "Source Time") <<
                qApp->translate("djv::Graphics::DPX", "Source Input Device") <<
                qApp->translate("djv::Graphics::DPX", "Source Input Serial") <<
                qApp->translate("djv::Graphics::DPX", "Source Border") <<
                qApp->translate("djv::Graphics::DPX", "Source Pixel Aspect") <<
                qApp->translate("djv::Graphics::DPX", "Source ScanSize") <<
                qApp->translate("djv::Graphics::DPX", "Film Format") <<
                qApp->translate("djv::Graphics::DPX", "Film Frame") <<
                qApp->translate("djv::Graphics::DPX", "Film Sequence") <<
                qApp->translate("djv::Graphics::DPX", "Film Hold") <<
                qApp->translate("djv::Graphics::DPX", "Film Frame Rate") <<
                qApp->translate("djv::Graphics::DPX", "Film Shutter") <<
                qApp->translate("djv::Graphics::DPX", "Film Frame ID") <<
                qApp->translate("djv::Graphics::DPX", "Film Slate") <<
                qApp->translate("djv::Graphics::DPX", "TV Interlace") <<
                qApp->translate("djv::Graphics::DPX", "TV Field") <<
                qApp->translate("djv::Graphics::DPX", "TV Video Signal") <<
                qApp->translate("djv::Graphics::DPX", "TV Sample Rate") <<
                qApp->translate("djv::Graphics::DPX", "TV Frame Rate") <<
                qApp->translate("djv::Graphics::DPX", "TV Time Offset") <<
                qApp->translate("djv::Graphics::DPX", "TV Gamma") <<
                qApp->translate("djv::Graphics::DPX", "TV Black Level") <<
                qApp->translate("djv::Graphics::DPX", "TV Black Gain") <<
                qApp->translate("djv::Graphics::DPX", "TV Break Point") <<
                qApp->translate("djv::Graphics::DPX", "TV White Level") <<
                qApp->translate("djv::Graphics::DPX", "TV Integration Times");
            DJV_ASSERT(data.count() == TAG_COUNT);
            return data;
        }

        const QStringList & DPX::optionsLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::DPX", "Input Color Profile") <<
                qApp->translate("djv::Graphics::DPX", "Input Film Print") <<
                qApp->translate("djv::Graphics::DPX", "Output Color Profile") <<
                qApp->translate("djv::Graphics::DPX", "Output Film Print") <<
                qApp->translate("djv::Graphics::DPX", "Version") <<
                qApp->translate("djv::Graphics::DPX", "Type") <<
                qApp->translate("djv::Graphics::DPX", "Endian");
            DJV_ASSERT(data.count() == OPTIONS_COUNT);
            return data;
        }

    } // namespace Graphics

    _DJV_STRING_OPERATOR_LABEL(Graphics::DPX::VERSION, Graphics::DPX::versionLabels());
    _DJV_STRING_OPERATOR_LABEL(Graphics::DPX::TYPE, Graphics::DPX::typeLabels());
    _DJV_STRING_OPERATOR_LABEL(Graphics::DPX::ENDIAN, Graphics::DPX::endianLabels());

} // namespace djv
