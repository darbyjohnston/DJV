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

#include <utility>

namespace djv
{
    namespace AV
    {
        namespace Font
        {
            inline Info::Info()
            {}

            constexpr Info::Info(FamilyID family, FaceID face, float size, int dpi) :
                family(family),
                face(face),
                size(size),
                dpi(dpi)
            {}

            inline bool Info::operator == (const Info & other) const
            {
                return
                    dpi    == other.dpi    &&
                    size   == other.size   &&
                    face   == other.face   &&
                    family == other.family;
            }

            inline bool Info::operator < (const Info& other) const
            {
                return std::tie(dpi, size, face, family) < std::tie(other.dpi, other.size, other.face, other.family);
            }

            inline TextLine::TextLine()
            {}

            inline TextLine::TextLine(size_t offset, size_t length, const glm::vec2& size) :
                offset(offset),
                length(length),
                size(size)
            {}

            inline GlyphInfo::GlyphInfo()
            {}

            inline GlyphInfo::GlyphInfo(uint32_t code, const Info& info) :
                code(code),
                info(info)
            {}

            inline bool GlyphInfo::operator == (const GlyphInfo & other) const
            {
                return
                    code == other.code &&
                    info == other.info;
            }

            inline bool GlyphInfo::operator < (const GlyphInfo & other) const
            {
                return std::tie(code, info) < std::tie(other.code, other.info);
            }

            inline Glyph::Glyph()
            {}

        } // namespace Font
    } // namespace AV
} // namespace djv
