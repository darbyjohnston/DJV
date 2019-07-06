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

#include <djvAV/PPM.h>

#include <djvCore/FileIO.h>
#include <djvCore/String.h>

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
                    void _readASCII(FileSystem::FileIO & io, uint8_t * out, size_t size)
                    {
                        char tmp[String::cStringLength] = "";
                        T* outP = reinterpret_cast<T*>(out);
                        for (int i = 0; i < size; ++i)
                        {
                            FileSystem::FileIO::readWord(io, tmp, String::cStringLength);
                            int value = 0;
                            String::fromString(tmp, String::cStringLength, value);
                            outP[i] = value;
                        }
                    }

                } // namespace

                void readASCII(FileSystem::FileIO & io, uint8_t * out, size_t size, size_t bitDepth)
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

                struct Plugin::Private
                {
                    Options options;
                };

                Plugin::Plugin() :
                    _p(new Private)
                {}

                std::shared_ptr<Plugin> Plugin::create(
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(
                        pluginName,
                        DJV_TEXT("This plugin provides NetPBM image I/O."),
                        fileExtensions,
                        resourceSystem,
                        logSystem);
                    return out;
                }

                picojson::value Plugin::getOptions() const
                {
                    return toJSON(_p->options);
                }

                void Plugin::setOptions(const picojson::value & value)
                {
                    fromJSON(value, _p->options);
                }

                std::shared_ptr<IRead> Plugin::read(const std::string & fileName, const ReadOptions& options) const
                {
                    return Read::create(fileName, options, _resourceSystem, _logSystem);
                }

                std::shared_ptr<IWrite> Plugin::write(const std::string & fileName, const Info & info, const WriteOptions& options) const
                {
                    return Write::create(fileName, info, options, _p->options, _resourceSystem, _logSystem);
                }

            } // namespace PPM
        } // namespace IO
    } // namespace AV
    
    picojson::value toJSON(const AV::IO::PPM::Options & value)
    {
        picojson::value out(picojson::object_type, true);
        {
            std::stringstream ss;
            ss << value.data;
            out.get<picojson::object>()["Data"] = picojson::value(ss.str());
        }
        return out;
    }

    void fromJSON(const picojson::value & value, AV::IO::PPM::Options & out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto & i : value.get<picojson::object>())
            {
                if ("Data" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.data;
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::PPM,
        Data,
        DJV_TEXT("ASCII"),
        DJV_TEXT("Binary"));

} // namespace djv

