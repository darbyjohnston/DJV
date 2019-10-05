//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAV/ImageData.h>

#include <djvCore/FileIO.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            void Data::_init(const Info & info, const std::shared_ptr<Core::FileSystem::FileIO>& fileIO)
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

            std::shared_ptr<Data> Data::create(const Info& info)
            {
                auto out = std::shared_ptr<Data>(new Data);
                out->_init(info, nullptr);
                return out;
            }

#if defined(DJV_MMAP)
            std::shared_ptr<Data> Data::create(const Info& info, const std::shared_ptr<Core::FileSystem::FileIO>& fileIO)
            {
                auto out = std::shared_ptr<Data>(new Data);
                out->_init(info, fileIO);
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
                detach();
                memset(_data, 0, _dataByteCount);
            }

            void Data::detach()
            {
#if defined(DJV_MMAP)
                if (_fileIO)
                {
                    _data = new uint8_t[_dataByteCount];
                    memcpy(_data, _fileIO->mmapP(), std::min(_fileIO->getSize() - _fileIO->getPos(), _dataByteCount));
                    _p = _data;
                    _fileIO.reset();
                }
#endif // DJV_MMAP
            }

            bool Data::operator == (const Data & other) const
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
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
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

