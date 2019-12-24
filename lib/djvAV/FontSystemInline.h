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

#include <djvCore/Memory.h>

namespace djv
{
    namespace AV
    {
        namespace Font
        {
            inline Info::Info()
            {}

            inline Info::Info(FamilyID family, FaceID face, uint16_t size, uint16_t dpi) :
                _family(family),
                _face(face),
                _size(size),
                _dpi(dpi)
            {
                Core::Memory::hashCombine(_hash, _family);
                Core::Memory::hashCombine(_hash, _face);
                Core::Memory::hashCombine(_hash, _size);
                Core::Memory::hashCombine(_hash, _dpi);
            }

            inline FamilyID Info::getFamily() const
            {
                return _family;
            }
            
            inline FaceID   Info::getFace() const
            {
                return _face;
            }
            
            inline uint16_t Info::getSize() const
            {
                return _size;
            }
            
            inline uint16_t Info::getDPI() const
            {
                return _dpi;
            }

            inline bool Info::operator == (const Info & other) const
            {
                return _hash == other._hash;
            }

            inline bool Info::operator < (const Info& other) const
            {
                return _hash < other._hash;
            }

            inline Metrics::Metrics()
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

            inline TextLine::TextLine()
            {}

            inline TextLine::TextLine(const std::string & text, const glm::vec2 & size, const std::vector<std::shared_ptr<Glyph> > & glyphs) :
                text(text),
                size(size),
                glyphs(glyphs)
            {}

        } // namespace Font
    } // namespace AV
} // namespace djv
