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

namespace djv
{
    namespace AV
    {
        namespace Audio
        {
            constexpr DataInfo::DataInfo()
            {}

            constexpr DataInfo::DataInfo(uint8_t channelCount, Type type, size_t sampleRate) :
                channelCount(channelCount),
                type(type),
                sampleRate(sampleRate)
            {}

            constexpr bool DataInfo::isValid() const
            {
                return type != Type::None;
            }

            inline const DataInfo & Data::getInfo() const
            {
                return _info;
            }

            inline Data::Data()
            {}

            inline uint8_t Data::getChannelCount() const
            {
                return _info.channelCount;
            }

            inline Type Data::getType() const
            {
                return _info.type;
            }

            inline size_t Data::getSampleRate() const
            {
                return _info.sampleRate;
            }

            inline size_t Data::getSampleCount() const
            {
                return _sampleCount;
            }

            inline bool Data::isValid() const
            {
                return _info.isValid();
            }

            inline size_t Data::getByteCount() const
            {
                return _sampleCount * Audio::getByteCount(_info.type);
            }

            inline uint8_t * Data::getData()
            {
                return _data.data();
            }

            inline const uint8_t * Data::getData() const
            {
                return _data.data();
            }

            inline uint8_t * Data::getData(size_t offset)
            {
                return _data.data();
            }

            inline const uint8_t * Data::getData(size_t offset) const
            {
                return _data.data();
            }

            template<typename T>
            inline void Data::extract(const T* value, T* out, size_t sampleCount, uint8_t inChannelCount, uint8_t outChannelCount)
            {
                const T* inP = value;
                T* outP = out;
                T* const endP = outP + sampleCount;
                switch (outChannelCount)
                {
                case 1:
                    for (; outP < endP; inP += inChannelCount, outP += 1)
                    {
                        outP[0] = inP[0];
                    }
                    break;
                case 2:
                    for (; outP < endP; inP += inChannelCount, outP += 2)
                    {
                        outP[0] = inP[0];
                        outP[1] = inP[1];
                    }
                    break;
                default:
                    for (; outP < endP; inP += inChannelCount, outP += outChannelCount)
                    {
                        for (size_t i = 0; i < outChannelCount; ++i)
                        {
                            outP[i] = inP[i];
                        }
                    }
                    break;
                }
            }

            template<typename T>
            inline void Data::planarInterleave(const T** value, T* out, size_t sampleCount, uint8_t channelCount)
            {
                const size_t planeSize = sampleCount / channelCount;
                switch (channelCount)
                {
                case 1:
                    memcpy(out, value[0], sampleCount * sizeof(T));
                    break;
                case 2:
                {
                    const T* inP0 = value[0];
                    const T* inP1 = value[1];
                    T* outP = out;
                    T* const endP = out + sampleCount;
                    for (; outP < endP; outP += 2, ++inP0, ++inP1)
                    {
                        outP[0] = inP0[0];
                        outP[1] = inP1[0];
                    }
                    break;
                }
                default:
                    for (uint8_t c = 0; c < channelCount; ++c)
                    {
                        const T* inP = value[c];
                        const T* endP = inP + planeSize;
                        T* outP = out + c;
                        for (; inP < endP; ++inP, outP += channelCount)
                        {
                            *outP = *inP;
                        }
                    }
                    break;
                }
            }

        } // namespace Audio
    } // namespace AV
} // namespace djv
