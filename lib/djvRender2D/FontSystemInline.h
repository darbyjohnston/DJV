// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Memory.h>

namespace djv
{
    namespace Render2D
    {
        namespace Font
        {
            inline FontInfo::FontInfo() noexcept
            {}

            inline FontInfo::FontInfo(FamilyID family, FaceID face, uint16_t size, uint16_t dpi) :
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

            inline FamilyID FontInfo::getFamily() const noexcept
            {
                return _family;
            }
            
            inline FaceID   FontInfo::getFace() const noexcept
            {
                return _face;
            }
            
            inline uint16_t FontInfo::getSize() const noexcept
            {
                return _size;
            }
            
            inline uint16_t FontInfo::getDPI() const noexcept
            {
                return _dpi;
            }

            inline bool FontInfo::operator == (const FontInfo& other) const noexcept
            {
                return _hash == other._hash;
            }

            inline bool FontInfo::operator < (const FontInfo& other) const noexcept
            {
                return _hash < other._hash;
            }

            inline Metrics::Metrics() noexcept
            {}

            inline GlyphInfo::GlyphInfo() noexcept
            {}

            inline GlyphInfo::GlyphInfo(uint32_t code, const FontInfo& fontInfo) noexcept :
                code(code),
                fontInfo(fontInfo)
            {}

            inline bool GlyphInfo::operator == (const GlyphInfo& other) const noexcept
            {
                return
                    code == other.code &&
                    fontInfo == other.fontInfo;
            }

            inline bool GlyphInfo::operator < (const GlyphInfo& other) const
            {
                return std::tie(code, fontInfo) < std::tie(other.code, other.fontInfo);
            }

            inline Glyph::Glyph()
            {}

            inline TextLine::TextLine()
            {}

            inline TextLine::TextLine(const std::string& text, const glm::vec2& size, const std::vector<std::shared_ptr<Glyph> >& glyphs) :
                text(text),
                size(size),
                glyphs(glyphs)
            {}

        } // namespace Font
    } // namespace Render2D
} // namespace djv
