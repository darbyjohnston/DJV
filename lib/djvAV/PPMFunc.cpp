// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAV/PPMFunc.h>

#include <djvSystem/FileIOFunc.h>

#include <djvCore/StringFunc.h>

#include <array>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace PPM
            {
                size_t getScanlineByteCount(
                    int    width,
                    size_t channelCount,
                    size_t bitDepth)
                {
                    size_t chars = 0;
                    switch (bitDepth)
                    {
                    case  8: chars = 3; break;
                    case 16: chars = 5; break;
                    default: break;
                    }
                    return (chars + 1) * width * channelCount + 1;
                }

                namespace
                {
                    template<typename T>
                    void _readASCII(const std::shared_ptr<System::File::IO>& io, uint8_t * out, size_t size)
                    {
                        char tmp[String::cStringLength] = "";
                        T* outP = reinterpret_cast<T*>(out);
                        for (int i = 0; i < size; ++i)
                        {
                            System::File::readWord(io, tmp, String::cStringLength);
                            int value = 0;
                            String::fromString(tmp, String::cStringLength, value);
                            outP[i] = value;
                        }
                    }

                } // namespace

                void readASCII(const std::shared_ptr<System::File::IO>& io, uint8_t* out, size_t size, size_t bitDepth)
                {
                    switch (bitDepth)
                    {
                    case  8: _readASCII<uint8_t> (io, out, size); break;
                    case 16: _readASCII<uint16_t>(io, out, size); break;
                    default: break;
                    }
                }

                namespace
                {
                    template<typename T>
                    size_t _writeASCII(const uint8_t * in, char * out, size_t size)
                    {
                        char * outP = out;
                        const T* inP = reinterpret_cast<const T*>(in);
                        for (size_t i = 0; i < size; ++i)
                        {
                            const std::string s = std::to_string(static_cast<unsigned int>(inP[i]));
                            const char * c = s.c_str();
                            for (size_t j = 0; j < s.size(); ++j)
                            {
                                *outP++ = c[j];
                            }
                            *outP++ = ' ';
                        }
                        *outP++ = '\n';
                        return outP - out;
                    }

                } // namespace

                size_t writeASCII(const uint8_t * in, char * out, size_t size, size_t bitDepth)
                {
                    switch (bitDepth)
                    {
                    case  8: return _writeASCII<uint8_t> (in, out, size);
                    case 16: return _writeASCII<uint16_t>(in, out, size);
                    default: break;
                    }
                    return 0;
                }

                DJV_ENUM_HELPERS_IMPLEMENTATION(Data);

            } // namespace PPM
        } // namespace IO
    } // namespace AV
    
    rapidjson::Value toJSON(const AV::IO::PPM::Options& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            std::stringstream ss;
            ss << value.data;
            const std::string& s = ss.str();
            out.AddMember("Data", rapidjson::Value(s.c_str(), s.size(), allocator), allocator);
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, AV::IO::PPM::Options& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("Data", i.name.GetString()) && i.value.IsString())
                {
                    std::stringstream ss(i.value.GetString());
                    ss >> out.data;
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::PPM,
        Data,
        DJV_TEXT("ppm_type_ascii"),
        DJV_TEXT("ppm_type_binary"));

} // namespace djv

