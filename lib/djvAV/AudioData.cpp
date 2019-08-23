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

#include <djvAV/AudioData.h>

#include <string.h>

namespace djv
{
    namespace AV
    {
        namespace Audio
        {
            bool DataInfo::operator == (const DataInfo & other) const
            {
                return
                    channelCount == other.channelCount &&
                    type == other.type &&
                    sampleRate == other.sampleRate &&
                    sampleCount == other.sampleCount;
            }

            bool DataInfo::operator != (const DataInfo & other) const
            {
                return !(*this == other);
            }

            void Data::_init(const DataInfo & info)
            {
                _info = info;
                _data.resize(getByteCount());
            }

            std::shared_ptr<Data> Data::create(const DataInfo & info)
            {
                auto out = std::shared_ptr<Data>(new Data);
                out->_init(info);
                return out;
            }

#define _CONVERT(a, b) \
    { \
        const a##_T * inP = reinterpret_cast<const a##_T *>(data->getData()); \
        b##_T * outP = reinterpret_cast<b##_T *>(out->getData()); \
        for (size_t i = 0; i < sampleCount * channelCount; ++i, ++inP, ++outP) \
        { \
            a##To##b(*inP, *outP); \
        } \
    }

            std::shared_ptr<Data> Data::convert(const std::shared_ptr<Data> & data, Type type)
            {
                const Type dataType = data->getType();
                const size_t sampleCount = data->getSampleCount();
                const size_t channelCount = static_cast<size_t>(data->getChannelCount());
                auto out = Data::create(DataInfo(channelCount, type, data->getSampleRate(), sampleCount));
                if (dataType == type)
                {
                    memcpy(out->getData(), data->getData(), sampleCount * channelCount * Audio::getByteCount(type));
                }
                else
                {
                    switch (dataType)
                    {
                        case Type::S8:
                            switch (type)
                            {
                            case Type::S16: _CONVERT(S8, S16); break;
                            case Type::S32: _CONVERT(S8, S32); break;
                            case Type::F32: _CONVERT(S8, F32); break;
                            case Type::F64: _CONVERT(S8, F64); break;
                            default: break;
                            }
                            break;
                        case Type::S16:
                            switch (type)
                            {
                            case Type::S8:  _CONVERT(S16, S8);  break;
                            case Type::S32: _CONVERT(S16, S32); break;
                            case Type::F32: _CONVERT(S16, F32); break;
                            case Type::F64: _CONVERT(S16, F64); break;
                            default: break;
                            }
                            break;
                        case Type::S32:
                            switch (type)
                            {
                            case Type::S8:  _CONVERT(S32, S8);  break;
                            case Type::S16: _CONVERT(S32, S16); break;
                            case Type::F32: _CONVERT(S32, F32); break;
                            case Type::F64: _CONVERT(S32, F64); break;
                            default: break;
                            }
                            break;
                        case Type::F32:
                            switch (type)
                            {
                            case Type::S8:  _CONVERT(F32, S8);  break;
                            case Type::S16: _CONVERT(F32, S16); break;
                            case Type::S32: _CONVERT(F32, S32); break;
                            case Type::F64: _CONVERT(F32, F64); break;
                            default: break;
                            }
                            break;
                        case Type::F64:
                            switch (type)
                            {
                            case Type::S8:  _CONVERT(F64, S8);  break;
                            case Type::S16: _CONVERT(F64, S16); break;
                            case Type::S32: _CONVERT(F64, S32); break;
                            case Type::F32: _CONVERT(F64, F32); break;
                            default: break;
                            }
                            break;
                        default: break;
                    }
                }
                return out;
            }

            namespace
            {
                template<typename U>
                void _planarInterleave(const U * value, U * out, size_t channelCount, size_t size)
                {
                    const size_t planeSize = size / channelCount;
                    for (size_t c = 0; c < channelCount; ++c)
                    {
                        const U * inP = value + c * planeSize;
                        const U * endP = inP + planeSize;
                        U * outP = out + c;
                        for (; inP < endP; ++inP, outP += channelCount)
                        {
                            *outP = *inP;
                        }
                    }
                }
            }

            std::shared_ptr<Data> Data::planarInterleave(const std::shared_ptr<Data> & data)
            {
                auto out = Data::create(data->getInfo());
                const size_t channelCount = data->getChannelCount();
                const size_t sampleCount = data->getSampleCount();
                switch (data->getType())
                {
                case Type::S8:
                    _planarInterleave(reinterpret_cast<const S8_T *> (data->getData()), reinterpret_cast<S8_T *> (out->getData()), channelCount, sampleCount);
                    break;
                case Type::S16:
                    _planarInterleave(reinterpret_cast<const S16_T *>(data->getData()), reinterpret_cast<S16_T *>(out->getData()), channelCount, sampleCount);
                    break;
                case Type::S32:
                    _planarInterleave(reinterpret_cast<const S32_T *>(data->getData()), reinterpret_cast<S32_T *>(out->getData()), channelCount, sampleCount);
                    break;
                case Type::F32:
                    _planarInterleave(reinterpret_cast<const F32_T *>(data->getData()), reinterpret_cast<F32_T *>(out->getData()), channelCount, sampleCount);
                    break;
                case Type::F64:
                    _planarInterleave(reinterpret_cast<const F64_T *>(data->getData()), reinterpret_cast<F64_T *>(out->getData()), channelCount, sampleCount);
                    break;
                default: break;
                }
                return out;
            }

            namespace
            {
                template<typename T>
                void _planarDeinterleave(const T* value, T* out, uint8_t channelCount, size_t size)
                {
                    const size_t planeSize = size / channelCount;
                    for (uint8_t c = 0; c < channelCount; ++c)
                    {
                        const T* inP = value + c;
                        T* outP = out + c * planeSize;
                        for (size_t i = 0; i < planeSize; ++i, inP += channelCount, ++outP)
                        {
                            *outP = *inP;
                        }
                    }
                }
            }

            std::shared_ptr<Data> Data::planarDeinterleave(const std::shared_ptr<Data> & data)
            {
                auto out = Data::create(data->getInfo());
                const uint8_t channelCount = data->getChannelCount();
                const size_t sampleCount = data->getSampleCount();
                switch (data->getType())
                {
                case Type::S8:
                    _planarDeinterleave(reinterpret_cast<const S8_T *> (data->getData()), reinterpret_cast<S8_T *> (out->getData()), channelCount, sampleCount);
                    break;
                case Type::S16:
                    _planarDeinterleave(reinterpret_cast<const S16_T *>(data->getData()), reinterpret_cast<S16_T *>(out->getData()), channelCount, sampleCount);
                    break;
                case Type::S32:
                    _planarDeinterleave(reinterpret_cast<const S32_T *>(data->getData()), reinterpret_cast<S32_T *>(out->getData()), channelCount, sampleCount);
                    break;
                case Type::F32:
                    _planarDeinterleave(reinterpret_cast<const F32_T *>(data->getData()), reinterpret_cast<F32_T *>(out->getData()), channelCount, sampleCount);
                    break;
                case Type::F64:
                    _planarDeinterleave(reinterpret_cast<const F64_T *>(data->getData()), reinterpret_cast<F64_T *>(out->getData()), channelCount, sampleCount);
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

