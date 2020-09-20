// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImage/ImageData.h>

namespace djv
{
    namespace Image
    {
        Layout::Layout() noexcept
        {}

        Size::Size(uint16_t w, uint16_t h) noexcept :
            w(w),
            h(h)
        {}

        Info::Info()
        {}

        Info::Info(const Size & size, Type type, const Layout & layout) :
            size(size),
            type(type),
            layout(layout)
        {}

        Info::Info(uint16_t width, uint16_t height, Type type, const Layout & layout) :
            size(width, height),
            type(type),
            layout(layout)
        {}

        void Data::_init(const Info& info)
        {
            _uid = Core::createUID();
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

        Data::Data()
        {}

        Data::~Data()
        {
            delete[] _data;
        }

        std::shared_ptr<Data> Data::create(const Info& info)
        {
            auto out = std::shared_ptr<Data>(new Data);
            out->_init(info);
            return out;
        }

        void Data::zero()
        {
            memset(_data, 0, _dataByteCount);
        }

        bool Data::operator == (const Data& other) const
        {
            if (other._info == _info)
            {
#if defined(DJV_GL_ES2)
                return 0 == memcmp(other._p, _p, _dataByteCount);
#else
                if (GL_UNSIGNED_INT_10_10_10_2 == _info.getGLType())
                {
                    for (uint16_t y = 0; y < _info.size.h; ++y)
                    {
                        const U10_S * p = reinterpret_cast<const U10_S*>(getData(y));
                        const U10_S * otherP = reinterpret_cast<const U10_S*>(other.getData(y));
                        for (uint16_t x = 0; x < _info.size.w; ++x, ++p, ++otherP)
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
#endif
            }
            return false;
        }

        bool Data::operator != (const Data& other) const
        {
            return !(*this == other);
        }
        
    } // namespace Image
} // namespace djv

