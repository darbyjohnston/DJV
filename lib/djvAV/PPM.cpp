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

#include <djvAV/PPM.h>

#include <djvCore/Context.h>
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
                    int width,
                    size_t channelCount,
                    size_t bitDepth)
                {
                    size_t chars = 0;
                    switch (bitDepth)
                    {
                    case 8: chars = 3; break;
                    case 16: chars = 5; break;
                    default: break;
                    }
                    return (chars + 1) * width * channelCount + 1;
                }

                namespace
                {
                    template<typename T>
                    void _readASCII(FileSystem::FileIO& io, uint8_t* out, size_t size)
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

                void readASCII(FileSystem::FileIO& io, uint8_t* out, size_t size, size_t bitDepth)
                {
                    switch (bitDepth)
                    {
                    case 8: _readASCII<uint8_t>(io, out, size); break;
                    case 16: _readASCII<uint16_t>(io, out, size); break;
                    default: break;
                    }
                }

                namespace
                {
                    template<typename T>
                    size_t _writeASCII(const uint8_t* in, char* out, size_t size)
                    {
                        char* outP = out;
                        const T* inP = reinterpret_cast<const T*>(in);
                        for (size_t i = 0; i < size; ++i)
                        {
                            const std::string s = std::to_string(static_cast<unsigned int>(inP[i]));
                            const char* c = s.c_str();
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

                size_t writeASCII(const uint8_t* in, char* out, size_t size, size_t bitDepth)
                {
                    switch (bitDepth)
                    {
                    case 8: return _writeASCII<uint8_t>(in, out, size);
                    case 16: return _writeASCII<uint16_t>(in, out, size);
                    default: break;
                    }
                    return 0;
                }

                struct Plugin::Private
                {
                    Data data = Data::Binary;
                };

                Plugin::Plugin() :
                    _p(new Private)
                {}

                std::shared_ptr<Plugin> Plugin::create(Context * context)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(
                        pluginName,
                        DJV_TEXT("djv::AV::IO::PPM", "This plugin provides NetPBM image I/O."),
                        fileExtensions,
                        context);
                    return out;
                }

                picojson::value Plugin::getOptions() const
                {
                    picojson::value out(picojson::object_type, true);
                    std::stringstream ss;
                    ss << _p->data;
                    out.get<picojson::object>()["Data"] = picojson::value(ss.str());
                    return out;
                }

                void Plugin::setOptions(const picojson::value& value)
                {
                    if (value.is<picojson::object>())
                    {
                        for (const auto& i : value.get<picojson::object>())
                        {
                            if ("Data" == i.first)
                            {
                                std::stringstream ss(i.second.get<std::string>());
                                ss >> _p->data;
                            }
                        }
                    }
                    else
                    {
                        throw std::invalid_argument("Cannot parse value.");
                    }
                }

                std::shared_ptr<IRead> Plugin::read(
                    const std::string & fileName,
                    const std::shared_ptr<Queue> & queue) const
                {
                    return Read::create(fileName, queue, _context);
                }

                std::shared_ptr<IWrite> Plugin::write(
                    const std::string & fileName,
                    const Info & info,
                    const std::shared_ptr<Queue> & queue) const
                {
                    return Write::create(fileName, info, _p->data, queue, _context);
                }

            } // namespace PPM
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::PPM,
        Data,
        DJV_TEXT("djv::AV::IO::PPM", "ASCII"),
        DJV_TEXT("djv::AV::IO::PPM", "Binary"));

} // namespace djv

