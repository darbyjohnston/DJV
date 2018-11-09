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

#include <djvAV/ColorUtil.h>

#include <djvCore/VectorUtil.h>

namespace djv
{
    namespace AV
    {
        inline const PixelDataInfo & PixelData::info() const
        {
            return _info;
        }

        inline const glm::ivec2 & PixelData::size() const
        {
            return _info.size;
        }

        inline PixelDataInfo::PROXY PixelData::proxy() const
        {
            return _info.proxy;
        }

        inline int PixelData::w() const
        {
            return _info.size.x;
        }

        inline int PixelData::h() const
        {
            return _info.size.y;
        }

        inline Pixel::PIXEL PixelData::pixel() const
        {
            return _info.pixel;
        }

        inline int PixelData::channels() const
        {
            return _channels;
        }

        bool PixelData::isValid() const
        {
            return Core::VectorUtil::isSizeValid(_info.size);
        }

        inline quint8 * PixelData::data()
        {
            detach();

            return _data.data();
        }

        inline const quint8 * PixelData::data() const
        {
            return _p;
        }

        inline quint8 * PixelData::data(int x, int y)
        {
            detach();
            return _data.data() + (y * _info.size.x + x) * _pixelByteCount;
        }

        inline const quint8 * PixelData::data(int x, int y) const
        {
            return _p + (y * _info.size.x + x) * _pixelByteCount;
        }

        inline quint64 PixelData::pixelByteCount() const
        {
            return _pixelByteCount;
        }

        inline quint64 PixelData::scanlineByteCount() const
        {
            return _scanlineByteCount;
        }

        inline quint64 PixelData::dataByteCount() const
        {
            return _dataByteCount;
        }

    } // namespace AV
} // namespace djv
