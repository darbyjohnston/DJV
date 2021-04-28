// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImage/Data.h>

#include <djvImage/Color.h>

#include <djvCore/UID.h>

namespace djv
{
    namespace Image
    {
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

        void Data::setPluginName(const std::string& value)
        {
            _pluginName = value;
        }

        void Data::setTags(const Tags& value)
        {
            _tags = value;
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

        namespace
        {
            template<typename T, typename T2>
            void getAverageColor(const uint8_t* data, uint16_t width, uint16_t height, uint8_t channels, uint8_t* out)
            {
                std::vector<T2> average(channels, T2(0));
                const T* p = reinterpret_cast<const T*>(data);
                for (uint16_t y = 0; y < height; ++y)
                {
                    for (uint16_t x = 0; x < width; ++x)
                    {
                        for (uint8_t c = 0; c < channels; ++c)
                        {
                            average[c] += *p++;
                        }
                    }
                }
                T* outP = reinterpret_cast<T*>(out);
                for (uint8_t c = 0; c < channels; ++c)
                {
                    outP[c] = average[c] / static_cast<float>(width * height);
                }
            }

            void getAverageColorU10(const uint8_t* data, uint16_t width, uint16_t height, uint8_t* out)
            {
                uint64_t average[3] = { 0, 0, 0 };
                const U10_S_LSB* p = reinterpret_cast<const U10_S_LSB*>(data);
                for (uint16_t y = 0; y < height; ++y)
                {
                    for (uint16_t x = 0; x < width; ++x)
                    {
                        average[0] += p->r;
                        average[1] += p->g;
                        average[2] += p->b;
                        ++p;
                    }
                }
                U10_S_LSB* outP = reinterpret_cast<U10_S_LSB*>(out);
                outP->r = static_cast<uint32_t>(static_cast<float>(average[0]) / static_cast<float>(width * height));
                outP->g = static_cast<uint32_t>(static_cast<float>(average[1]) / static_cast<float>(width * height));
                outP->b = static_cast<uint32_t>(static_cast<float>(average[2]) / static_cast<float>(width * height));
            }

        } // namespace

        Color getAverageColor(const std::shared_ptr<Data>& data)
        {
            Color out;
            if (data && data->isValid())
            {
                const uint16_t w = data->getWidth();
                const uint16_t h = data->getHeight();
                const Image::Type type = data->getType();
                const uint8_t c = getChannelCount(type);
                const uint8_t* p = data->getData();
                out = Color(type);
                switch (getDataType(type))
                {
                case DataType::U8:  getAverageColor<U8_T, uint64_t>(p, w, h, c, out.getData()); break;
                case DataType::U16: getAverageColor<U16_T, uint64_t>(p, w, h, c, out.getData()); break;
                case DataType::U10: getAverageColorU10(p, w, h, out.getData()); break;
                case DataType::U32: getAverageColor<U32_T, uint64_t>(p, w, h, c, out.getData()); break;
                case DataType::F16: getAverageColor<F16_T, double>(p, w, h, c, out.getData()); break;
                case DataType::F32: getAverageColor<F32_T, double>(p, w, h, c, out.getData()); break;
                default: break;
                }
            }
            return out;
        }
        
    } // namespace Image
} // namespace djv

