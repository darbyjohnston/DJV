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

#include <djvAV/ColorProfile.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        ColorProfile::Exposure::Exposure()
        {}

        ColorProfile::Exposure::Exposure(
            float value,
            float defog,
            float kneeLow,
            float kneeHigh) :
            value(value),
            defog(defog),
            kneeLow(kneeLow),
            kneeHigh(kneeHigh)
        {}

        ColorProfile::ColorProfile()
        {}

        const QStringList & ColorProfile::profileLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::ColorProfile", "Raw") <<
                qApp->translate("djv::AV::ColorProfile", "Gamma") <<
                qApp->translate("djv::AV::ColorProfile", "LUT") <<
                qApp->translate("djv::AV::ColorProfile", "Exposure");
            DJV_ASSERT(data.count() == PROFILE_COUNT);
            return data;
        }

    } // namespace AV

    bool operator == (
        const AV::ColorProfile::Exposure & a,
        const AV::ColorProfile::Exposure & b)
    {
        return
            Core::Math::fuzzyCompare(a.value, b.value) &&
            Core::Math::fuzzyCompare(a.defog, b.defog) &&
            Core::Math::fuzzyCompare(a.kneeLow, b.kneeLow) &&
            Core::Math::fuzzyCompare(a.kneeHigh, b.kneeHigh);
    }

    bool operator != (
        const AV::ColorProfile::Exposure & a,
        const AV::ColorProfile::Exposure & b)
    {
        return !(a == b);
    }

    bool operator == (const AV::ColorProfile & a, const AV::ColorProfile & b)
    {
        return
            a.type == b.type &&
            Core::Math::fuzzyCompare(a.gamma, b.gamma) &&
            a.lut == b.lut &&
            a.exposure == b.exposure;
    }

    bool operator != (const AV::ColorProfile & a, const AV::ColorProfile & b)
    {
        return !(a == b);
    }

    QStringList & operator >> (QStringList & in, AV::ColorProfile::Exposure & out)
    {
        return in >> out.value >> out.defog >> out.kneeLow >> out.kneeHigh;
    }

    QStringList & operator << (QStringList & out, const AV::ColorProfile::Exposure & in)
    {
        return out << in.value << in.defog << in.kneeLow << in.kneeHigh;
    }

    _DJV_STRING_OPERATOR_LABEL(
        AV::ColorProfile::PROFILE,
        AV::ColorProfile::profileLabels());

    Core::Debug & operator << (Core::Debug & debug, const AV::ColorProfile::Exposure & in)
    {
        return debug << in.value << " " << in.defog << " " << in.kneeLow << " " << in.kneeHigh;
    }

    Core::Debug & operator << (Core::Debug & debug, const AV::ColorProfile::PROFILE & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

    Core::Debug & operator << (Core::Debug & debug, const AV::ColorProfile & in)
    {
        return debug <<
            in.type << ", " <<
            "gamma = " << in.gamma << ", " <<
            "lut = " << in.lut << ", " <<
            "exposure = " << in.exposure;
    }

} // namespace djv
