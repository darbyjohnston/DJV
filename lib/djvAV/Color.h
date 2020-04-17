// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Pixel.h>

#include <djvCore/PicoJSON.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            //! This class provides a color.
            class Color
            {
            public:
                Color();
                explicit Color(Type);
                Color(int r, int g, int b, int a = U8Range.max);
                Color(F32_T r, F32_T g, F32_T b, F32_T a = F32Range.max);

                Type getType() const;
                bool isValid() const;

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

                const uint8_t * getData() const;
                uint8_t * getData();
                void setData(const uint8_t*);

                void zero();

                Color convert(Type) const;

                static Color RGB_U8(int, int, int);
                static Color RGB_F32(float, float, float);

                static void rgbToHSV(const float[3], float[3]);
                static void hsvToRGB(const float[3], float[3]);

                static std::string getLabel(const Color&, int precision = 2, bool pad = true);

                bool operator == (const Color &) const;
                bool operator != (const Color &) const;

            private:
                Type _type = Type::None;
                std::vector<uint8_t> _data;
            };

        } // namespace Image
    } // namespace AV

    picojson::value toJSON(const AV::Image::Color&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, AV::Image::Color&);

    std::ostream & operator << (std::ostream &, const AV::Image::Color &);
    std::istream & operator >> (std::istream &, AV::Image::Color &);

} // namespace djv

#include <djvAV/ColorInline.h>
