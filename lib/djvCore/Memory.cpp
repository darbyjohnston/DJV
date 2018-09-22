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

#include <djvCore/Memory.h>

#include <djvCore/Assert.h>

#include <QCoreApplication>

#include <string.h>

namespace djv
{
    namespace Core
    {
        Memory::~Memory()
        {}

        const quint64 Memory::kilobyte = 1024;
        const quint64 Memory::megabyte = kilobyte * 1024;
        const quint64 Memory::gigabyte = megabyte * 1024;
        const quint64 Memory::terabyte = gigabyte * 1024;

        QString Memory::sizeLabel(quint64 in)
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Core::Memory", "%1TB") <<
                qApp->translate("djv::Core::Memory", "%1GB") <<
                qApp->translate("djv::Core::Memory", "%1MB") <<
                qApp->translate("djv::Core::Memory", "%1KB");
            if (in >= Memory::terabyte)
            {
                return data[0].arg(in / static_cast<float>(Memory::terabyte), 0, 'f', 2);
            }
            else if (in >= Memory::gigabyte)
            {
                return data[1].arg(in / static_cast<float>(Memory::gigabyte), 0, 'f', 2);
            }
            else if (in >= Memory::megabyte)
            {
                return data[2].arg(in / static_cast<float>(Memory::megabyte), 0, 'f', 2);
            }
            else
            {
                return data[3].arg(in / static_cast<float>(Memory::kilobyte), 0, 'f', 2);
            }
        }

        const QStringList & Memory::endianLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Core::Memory", "MSB") <<
                qApp->translate("djv::Core::Memory", "LSB");
            DJV_ASSERT(data.count() == ENDIAN_COUNT);
            return data;
        }

        Memory::ENDIAN Memory::endian()
        {
            static const int tmp = 1;
            static const quint8 * const p = reinterpret_cast<const quint8 *>(&tmp);
            return *p ? LSB : MSB;
        }

    } // namespace Core

    _DJV_STRING_OPERATOR_LABEL(Core::Memory::ENDIAN, Core::Memory::endianLabels())

} // namespace djv
