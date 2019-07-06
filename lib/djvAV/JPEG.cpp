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

#include <djvAV/JPEG.h>

#include <djvCore/Context.h>
#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace JPEG
            {
                struct Plugin::Private
                {
                    Options options;
                };

                Plugin::Plugin() :
                    _p(new Private)
                {}

                Plugin::~Plugin()
                {}

                std::shared_ptr<Plugin> Plugin::create(
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(
                        pluginName,
                        DJV_TEXT("This plugin provides Joint Photographic Experts Group (JPEG) image I/O."),
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

                extern "C"
                {
                    void djvJPEGError(j_common_ptr in)
                    {
                        auto error = reinterpret_cast<JPEGErrorStruct *>(in->err);
                        in->err->format_message(in, error->msg);
                        ::longjmp(error->jump, 1);
                    }

                    void djvJPEGWarning(j_common_ptr in, int level)
                    {
                        if (level > 0)
                        {
                            //! \todo Should we send "trace" messages to the debug log?
                            return;
                        }
                        auto error = reinterpret_cast<JPEGErrorStruct *>(in->err);
                        in->err->format_message(in, error->msg);
                        ::longjmp(error->jump, 1);
                    }

                } // extern "C"

            } // namespace JPEG
        } // namespace IO
    } // namespace AV
    
    picojson::value toJSON(const AV::IO::JPEG::Options & value)
    {
        picojson::value out(picojson::object_type, true);
        {
            std::stringstream ss;
            ss << value.quality;
            out.get<picojson::object>()["Quality"] = picojson::value(ss.str());
        }
        return out;
    }

    void fromJSON(const picojson::value & value, AV::IO::JPEG::Options & out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto & i : value.get<picojson::object>())
            {
                if ("Quality" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.quality;
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }
} // namespace djv

