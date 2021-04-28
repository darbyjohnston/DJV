// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAudio/Data.h>

#include <cstring>

namespace djv
{
    namespace Audio
    {
        void Data::_init(const Info& info, size_t sampleCount)
        {
            _info = info;
            _sampleCount = sampleCount;
            _data.resize(getByteCount());
        }

        Data::Data()
        {}

        std::shared_ptr<Data> Data::create(const Info& info, size_t sampleCount)
        {
            auto out = std::shared_ptr<Data>(new Data);
            out->_init(info, sampleCount);
            return out;
        }
        
        void Data::zero()
        {
            memset(_data.data(), 0, getByteCount());
        }

        bool Data::operator == (const Data& other) const
        {
            return
                _info == other._info &&
                _sampleCount == other._sampleCount &&
                0 == memcmp(getData(), other.getData(), getByteCount());
        }

        bool Data::operator != (const Data& other) const
        {
            return !(*this == other);
        }

#define _VOLUME(t) \
    { \
        const t##_T* inP = reinterpret_cast<const t##_T*>(in); \
        t##_T* outP = reinterpret_cast<t##_T*>(out); \
        t##_T* const endP = outP + sampleCount * channelCount; \
        for (; outP < endP; ++inP, ++outP) \
        { \
            *outP = *inP * volume; \
        } \
    }

        void volume(const uint8_t* in, uint8_t* out, float volume, size_t sampleCount, uint8_t channelCount, Type type)
        {
            switch (type)
            {
            case Type::S8:  _VOLUME(S8);  break;
            case Type::S16: _VOLUME(S16); break;
            case Type::S32: _VOLUME(S32); break;
            case Type::F32: _VOLUME(F32); break;
            case Type::F64: _VOLUME(F64); break;
            default: break;
            }
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

        std::shared_ptr<Data> convert(const std::shared_ptr<Data>& data, Type type)
        {
            const Type dataType = data->getType();
            const size_t sampleCount = data->getSampleCount();
            const size_t channelCount = static_cast<size_t>(data->getChannelCount());
            auto out = Data::create(Info(channelCount, type, data->getSampleRate()), sampleCount);
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
            void _planarInterleave(const U* value, U* out, uint8_t channelCount, size_t size)
            {
                const size_t planeSize = size;
                for (uint8_t c = 0; c < channelCount; ++c)
                {
                    const U* inP = value + c * planeSize;
                    const U* endP = inP + planeSize;
                    U* outP = out + c;
                    for (; inP < endP; ++inP, outP += channelCount)
                    {
                        *outP = *inP;
                    }
                }
            }
        }

        std::shared_ptr<Data> planarInterleave(const std::shared_ptr<Data>& data)
        {
            const size_t sampleCount = data->getSampleCount();
            const size_t channelCount = data->getChannelCount();
            auto out = Data::create(data->getInfo(), sampleCount);
            switch (data->getType())
            {
            case Type::S8:
                _planarInterleave(reinterpret_cast<const S8_T*> (data->getData()), reinterpret_cast<S8_T*> (out->getData()), channelCount, sampleCount);
                break;
            case Type::S16:
                _planarInterleave(reinterpret_cast<const S16_T*>(data->getData()), reinterpret_cast<S16_T*>(out->getData()), channelCount, sampleCount);
                break;
            case Type::S32:
                _planarInterleave(reinterpret_cast<const S32_T*>(data->getData()), reinterpret_cast<S32_T*>(out->getData()), channelCount, sampleCount);
                break;
            case Type::F32:
                _planarInterleave(reinterpret_cast<const F32_T*>(data->getData()), reinterpret_cast<F32_T*>(out->getData()), channelCount, sampleCount);
                break;
            case Type::F64:
                _planarInterleave(reinterpret_cast<const F64_T*>(data->getData()), reinterpret_cast<F64_T*>(out->getData()), channelCount, sampleCount);
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
                const size_t planeSize = size;
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

        std::shared_ptr<Data> planarDeinterleave(const std::shared_ptr<Data>& data)
        {
            const uint8_t channelCount = data->getChannelCount();
            const size_t sampleCount = data->getSampleCount();
            auto out = Data::create(data->getInfo(), sampleCount);
            switch (data->getType())
            {
            case Type::S8:
                _planarDeinterleave(reinterpret_cast<const S8_T*> (data->getData()), reinterpret_cast<S8_T*>(out->getData()), channelCount, sampleCount);
                break;
            case Type::S16:
                _planarDeinterleave(reinterpret_cast<const S16_T*>(data->getData()), reinterpret_cast<S16_T*>(out->getData()), channelCount, sampleCount);
                break;
            case Type::S32:
                _planarDeinterleave(reinterpret_cast<const S32_T*>(data->getData()), reinterpret_cast<S32_T*>(out->getData()), channelCount, sampleCount);
                break;
            case Type::F32:
                _planarDeinterleave(reinterpret_cast<const F32_T*>(data->getData()), reinterpret_cast<F32_T*>(out->getData()), channelCount, sampleCount);
                break;
            case Type::F64:
                _planarDeinterleave(reinterpret_cast<const F64_T*>(data->getData()), reinterpret_cast<F64_T*>(out->getData()), channelCount, sampleCount);
                break;
            default: break;
            }
            return out;
        }

    } // namespace Audio
} // namespace djv

