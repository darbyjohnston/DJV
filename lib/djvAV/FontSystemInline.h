// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
