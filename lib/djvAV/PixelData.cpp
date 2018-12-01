//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/PixelData.h>

#include <djvAV/Context.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            void PixelData::_init(const PixelDataInfo & info)
            {
                _info = info;
                _pixelByteCount = info.getPixelByteCount();
                _scanlineByteCount = info.getScanlineByteCount();
                _dataByteCount = info.getDataByteCount();
                if (_dataByteCount)
                {
                    _data = new uint8_t[_dataByteCount];
                    _p = _data;
                }
            }

            PixelData::PixelData()
            {}

            PixelData::~PixelData()
            {
                delete[] _data;
            }

            std::shared_ptr<PixelData> PixelData::create(const PixelDataInfo& info)
            {
                auto out = std::shared_ptr<PixelData>(new PixelData);
                out->_init(info);
                return out;
            }

            uint8_t * PixelData::getData()
            {
                return _data;
            }

            uint8_t* PixelData::getData(int y)
            {
                return _data + y * _scanlineByteCount;
            }

            uint8_t * PixelData::getData(int x, int y)
            {
                return _data + y * _scanlineByteCount + x * _pixelByteCount;
            }

            void PixelData::zero()
            {
                memset(_data, 0, _dataByteCount);
            }

            std::shared_ptr<PixelData> PixelData::convert(const Pixel & pixel, const std::shared_ptr<Context> & context) const
            {
                auto out = PixelData::create(PixelDataInfo(_info.getSize(), pixel));
                //! \todo
                return out;
            }

            bool PixelData::operator == (const PixelData & other) const
            {
                if (other._info == _info)
                {
                    if (GL_UNSIGNED_INT_10_10_10_2 == _info.getType())
                    {
                        const auto & size = _info.getSize();
                        for (int y = 0; y < size.y; ++y)
                        {
                            const U10_S * p = reinterpret_cast<const U10_S*>(getData(y));
                            const U10_S * otherP = reinterpret_cast<const U10_S*>(other.getData(y));
                            for (int x = 0; x < size.x; ++x, ++p, ++otherP)
                            {
                                if (*p != *otherP)
                                {
                                    return false;
                                }
                            }
                        }
                        return true;
                    }
                    else
                    {
                        return 0 == memcmp(other._p, _p, _dataByteCount);
                    }
                }
                return false;
            }

        } // namespace Image
    } // namespace AV
} // namespace djv

