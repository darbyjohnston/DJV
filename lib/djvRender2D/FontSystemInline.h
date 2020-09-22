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

        } // namespace Font
    } // namespace Render2D
} // namespace djv
