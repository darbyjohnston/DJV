// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Pixel.h>

namespace djv
{
    namespace Image
    {
        //! This class provides a color.
        class Color
        {
        public:
            Color();
            explicit Color(Type);
            explicit Color(int v);
            Color(int r, int g, int b, int a = U8Range.getMax());
            Color(F32_T r, F32_T g, F32_T b, F32_T a = F32Range.getMax());
            explicit Color(F32_T v);

            //! \name Information
            ///@{

            Type getType() const noexcept;
            bool isValid() const noexcept;

            ///@}

            //! \name Data
            ///@{

            U8_T  getU8(size_t channel) const;
            U10_T getU10(size_t channel) const;
            U16_T getU16(size_t channel) const;
            U32_T getU32(size_t channel) const;
            F16_T getF16(size_t channel) const;
            F32_T getF32(size_t channel) const;

            void setU8(U8_T, size_t channel);
            void setU10(U10_T, size_t channel);
            void setU16(U16_T, size_t channel);
            void setU32(U32_T, size_t channel);
            void setF16(F16_T, size_t channel);
            void setF32(F32_T, size_t channel);

            const uint8_t * getData() const noexcept;
            uint8_t * getData() noexcept;

            void setData(const uint8_t*);

            ///@}

            //! \name Utility
            ///@{

            void zero();

            //! \name Conversion
            ///@{

            Color convert(Type) const;

            static Color RGB_U8(int, int, int);
            static Color RGB_F32(float, float, float);

            ///@}

            bool operator == (const Color&) const;
            bool operator != (const Color&) const;

        private:
            Type _type = Type::None;
            std::vector<uint8_t> _data;
        };

    } // namespace Image
} // namespace djv

#include <djvImage/ColorInline.h>
