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

#include <djvAV/AudioData.h>

#include <string.h>

namespace djv
{
    namespace AV
    {
        namespace Audio
        {
            DataInfo::DataInfo()
            {}

            DataInfo::DataInfo(size_t channels, Type type, size_t sampleRate) :
                _channels(channels),
                _type(type),
                _sampleRate(sampleRate)
            {}

            bool DataInfo::operator == (const DataInfo & other) const
            {
                return
                    _channels == other._channels &&
                    _type == other._type &&
                    _sampleRate == other._sampleRate;
            }

            bool DataInfo::operator != (const DataInfo & other) const
            {
                return !(*this == other);
            }

            void Data::_init(const DataInfo & info, size_t sampleCount)
            {
                _info = info;
                _sampleCount = sampleCount;
                _data.resize(getByteCount());
            }

            Data::Data()
            {}

            Data::~Data()
            {}

            std::shared_ptr<Data> Data::create(const DataInfo & info, size_t sampleCount)
            {
                auto out = std::shared_ptr<Data>(new Data);
                out->_init(info, sampleCount);
                return out;
            }

#define _CONVERT(a, b) \
    { \
        const a##_T * inP = reinterpret_cast<const a##_T *>(data->getData()); \
        b##_T * outP = reinterpret_cast<b##_T *>(out->getData()); \
        for (size_t i = 0; i < sampleCount; ++i, ++inP, ++outP) \
        { \
            a##To##b(*inP, *outP); \
        } \
    }

            std::shared_ptr<Data> Data::convert(const std::shared_ptr<Data> & data, Type type)
            {
                const size_t sampleCount = data->getSampleCount();
                auto out = Data::create(DataInfo(data->getChannels(), type, data->getSampleRate()), sampleCount);
                switch (data->getType())
                {
                case Type::U8:
                    switch (type)
                    {
                    case Type::U8:  memcpy(out->getData(), data->getData(), sampleCount * Audio::getByteCount(type)); break;
                    case Type::S16: _CONVERT(U8, S16); break;
                    case Type::S32: _CONVERT(U8, S32); break;
                    case Type::F32: _CONVERT(U8, F32); break;
                    default: break;
                    }
                    break;
                case Type::S16:
                    switch (type)
                    {
                    case Type::U8:  _CONVERT(S16, U8); break;
                    case Type::S16: memcpy(out->getData(), data->getData(), data->getSampleCount() * Audio::getByteCount(type)); break;
                    case Type::S32: _CONVERT(S16, S32); break;
                    case Type::F32: _CONVERT(S16, F32); break;
                    default: break;
                    }
                    break;
                case Type::S32:
                    switch (type)
                    {
                    case Type::U8:  _CONVERT(S32, U8); break;
                    case Type::S16: _CONVERT(S32, S16); break;
                    case Type::S32: memcpy(out->getData(), data->getData(), data->getSampleCount() * Audio::getByteCount(type)); break;
                    case Type::F32: _CONVERT(S32, F32); break;
                    default: break;
                    }
                    break;
                case Type::F32:
                    switch (type)
                    {
                    case Type::U8:  _CONVERT(F32, U8); break;
                    case Type::S16: _CONVERT(F32, S16); break;
                    case Type::S32: _CONVERT(F32, S32); break;
                    case Type::F32: memcpy(out->getData(), data->getData(), data->getSampleCount() * Audio::getByteCount(type)); break;
                    default: break;
                    }
                    break;
                default: break;
                }
                return out;
            }

            namespace
            {
                template<typename U>
                void _planarInterleave(const U * value, U * out, size_t channels, size_t size)
                {
                    const size_t planeSize = size / channels;
                    for (size_t c = 0; c < channels; ++c)
                    {
                        const U * inP = value + c * planeSize;
                        U * outP = out + c;
                        for (size_t i = 0; i < planeSize; ++i, ++inP, outP += channels)
                        {
                            *outP = *inP;
                        }
                    }
                }
            }

            std::shared_ptr<Data> Data::planarInterleave(const std::shared_ptr<Data> & data)
            {
                auto out = Data::create(data->getInfo(), data->getSampleCount());
                const size_t channels = data->getChannels();
                const size_t sampleCount = data->getSampleCount();
                switch (data->getType())
                {
                case Type::U8:
                    _planarInterleave(reinterpret_cast<const U8_T *> (data->getData()), reinterpret_cast<U8_T *> (out->getData()), channels, sampleCount);
                    break;
                case Type::S16:
                    _planarInterleave(reinterpret_cast<const S16_T *>(data->getData()), reinterpret_cast<S16_T *>(out->getData()), channels, sampleCount);
                    break;
                case Type::S32:
                    _planarInterleave(reinterpret_cast<const S32_T *>(data->getData()), reinterpret_cast<S32_T *>(out->getData()), channels, sampleCount);
                    break;
                case Type::F32:
                    _planarInterleave(reinterpret_cast<const F32_T *>(data->getData()), reinterpret_cast<F32_T *>(out->getData()), channels, sampleCount);
                    break;
                default: break;
                }
                return out;
            }

            namespace
            {
                template<typename U>
                void _planarDeinterleave(const U * value, U * out, size_t channels, size_t size)
                {
                    const size_t planeSize = size / channels;
                    for (size_t c = 0; c < channels; ++c)
                    {
                        const U * inP = value + c;
                        U * outP = out + c * planeSize;
                        for (size_t i = 0; i < planeSize; ++i, inP += channels, ++outP)
                        {
                            *outP = *inP;
                        }
                    }
                }
            }

            std::shared_ptr<Data> Data::planarDeinterleave(const std::shared_ptr<Data> & data)
            {
                auto out = Data::create(data->getInfo(), data->getSampleCount());
                const size_t channels = data->getChannels();
                const size_t sampleCount = data->getSampleCount();
                switch (data->getType())
                {
                case Type::U8:
                    _planarDeinterleave(reinterpret_cast<const U8_T *> (data->getData()), reinterpret_cast<U8_T *> (out->getData()), channels, sampleCount);
                    break;
                case Type::S16:
                    _planarDeinterleave(reinterpret_cast<const S16_T *>(data->getData()), reinterpret_cast<S16_T *>(out->getData()), channels, sampleCount);
                    break;
                case Type::S32:
                    _planarDeinterleave(reinterpret_cast<const S32_T *>(data->getData()), reinterpret_cast<S32_T *>(out->getData()), channels, sampleCount);
                    break;
                case Type::F32:
                    _planarDeinterleave(reinterpret_cast<const F32_T *>(data->getData()), reinterpret_cast<F32_T *>(out->getData()), channels, sampleCount);
                    break;
                default: break;
                }
                return out;
            }

            bool Data::operator == (const Data & other) const
            {
                return
                    _info == other._info &&
                    0 == memcmp(getData(), other.getData(), getByteCount());
            }

            bool Data::operator != (const Data & other) const
            {
                return !(*this == other);
            }

        } // namespace Audio
    } // namespace AV
} // namespace djv

