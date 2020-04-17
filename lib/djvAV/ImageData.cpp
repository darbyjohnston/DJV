// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/ImageData.h>

#include <djvCore/FileIO.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            void Data::_init(const Info& info, const std::shared_ptr<Core::FileSystem::FileIO>& fileIO)
            {
                _uid = Core::createUID();
                _info = info;
                _pixelByteCount = info.getPixelByteCount();
                _scanlineByteCount = info.getScanlineByteCount();
                _dataByteCount = info.getDataByteCount();
#if defined(DJV_MMAP)
                _fileIO = fileIO;
                if (_fileIO)
                {
                    _p = _fileIO->mmapP();
                }
                else if (_dataByteCount)
                {
                    _data = new uint8_t[_dataByteCount];
                    _p = _data;
                }
#else // DJV_MMAP
                if (_dataByteCount)
                {
                    _data = new uint8_t[_dataByteCount];
                    _p = _data;
                }
#endif // DJV_MMAP
            }

            Data::~Data()
            {
                delete[] _data;
            }

#if defined(DJV_MMAP)
            std::shared_ptr<Data> Data::create(const Info& info, const std::shared_ptr<Core::FileSystem::FileIO>& fileIO)
            {
                auto out = std::shared_ptr<Data>(new Data);
                out->_init(info, fileIO);
                return out;
            }
#else // DJV_MMAP
            std::shared_ptr<Data> Data::create(const Info& info)
            {
                auto out = std::shared_ptr<Data>(new Data);
                out->_init(info, nullptr);
                return out;
            }
#endif // DJV_MMAP

            size_t Data::getDataByteCount() const
            {
#if defined(DJV_MMAP)
                return _fileIO ? (_fileIO->getSize() - _fileIO->getPos()) : _dataByteCount;
#else
                return _dataByteCount;
#endif // DJV_MMAP
            }

            void Data::zero()
            {
#if defined(DJV_MMAP)
                detach();
#endif // DJV_MMAP
                memset(_data, 0, _dataByteCount);
            }

#if defined(DJV_MMAP)
            void Data::detach()
            {
                if (_fileIO)
                {
                    _data = new uint8_t[_dataByteCount];
                    memcpy(_data, _fileIO->mmapP(), std::min(_fileIO->getSize() - _fileIO->getPos(), _dataByteCount));
                    _p = _data;
                    _fileIO.reset();
                }
            }
#endif // DJV_MMAP

            bool Data::operator == (const Data& other) const
            {
                if (other._info == _info)
                {
#if defined(DJV_OPENGL_ES2)
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
    } // namespace AV

    picojson::value toJSON(const AV::Image::Size& value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    void fromJSON(const picojson::value& value, AV::Image::Size& out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    std::ostream& operator << (std::ostream& s, const AV::Image::Size& value)
    {
        s << value.w << " ";
        s << value.h;
        return s;
    }

    std::istream& operator >> (std::istream& s, AV::Image::Size& value)
    {
        s.exceptions(std::istream::failbit | std::istream::badbit);
        s >> value.w;
        s >> value.h;
        return s;
    }

} // namespace djv

