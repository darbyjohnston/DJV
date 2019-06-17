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

#include <djvAV/Cineon.h>

#include <djvCore/FileIO.h>
#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace Cineon
            {
                struct Plugin::Private
                {
                    Settings settings;
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
                        DJV_TEXT("This plugin provides Cineon image I/O."),
                        fileExtensions,
                        resourceSystem,
                        logSystem);
                    return out;
                }

                picojson::value Plugin::getOptions() const
                {
                    return toJSON(_p->settings);
                }

                void Plugin::setOptions(const picojson::value & value)
                {
                    fromJSON(value, _p->settings);
                }

                std::shared_ptr<IRead> Plugin::read(const std::string & fileName) const
                {
                    return Read::create(fileName, _resourceSystem, _logSystem);
                }

                std::shared_ptr<IWrite> Plugin::write(const std::string & fileName, const Info & info) const
                {
                    return Write::create(fileName, _p->settings, info, _resourceSystem, _logSystem);
                }

            } // namespace Cineon
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::Cineon,
        ColorProfile,
        DJV_TEXT("Auto"),
        DJV_TEXT("Raw"),
        DJV_TEXT("FilmPrint"));

    picojson::value toJSON(const AV::IO::Cineon::LinearToFilmPrint& value)
    {
        picojson::value out(picojson::object_type, true);
        {
            std::stringstream ss;
            ss << value.black;
            out.get<picojson::object>()["Black"] = picojson::value(ss.str());
            ss << value.white;
            out.get<picojson::object>()["White"] = picojson::value(ss.str());
            ss << value.gamma;
            out.get<picojson::object>()["Gamma"] = picojson::value(ss.str());
        }
        return out;
    }

    picojson::value toJSON(const AV::IO::Cineon::FilmPrintToLinear& value)
    {
        picojson::value out(picojson::object_type, true);
        {
            std::stringstream ss;
            ss << value.black;
            out.get<picojson::object>()["Black"] = picojson::value(ss.str());
            ss << value.white;
            out.get<picojson::object>()["White"] = picojson::value(ss.str());
            ss << value.gamma;
            out.get<picojson::object>()["Gamma"] = picojson::value(ss.str());
            ss << value.softClip;
            out.get<picojson::object>()["SoftClip"] = picojson::value(ss.str());
        }
        return out;
    }

    picojson::value toJSON(const AV::IO::Cineon::Settings& value)
    {
        picojson::value out(picojson::object_type, true);
        {
            std::stringstream ss;
            ss << value.inputProfile;
            out.get<picojson::object>()["InputProfile"] = picojson::value(ss.str());
            out.get<picojson::object>()["InputFilmPrint"] = toJSON(value.inputFilmPrint);
            ss << value.outputProfile;
            out.get<picojson::object>()["OutputProfile"] = picojson::value(ss.str());
            out.get<picojson::object>()["OutputFilmPrint"] = toJSON(value.outputFilmPrint);
        }
        return out;
    }

    void fromJSON(const picojson::value& value, AV::IO::Cineon::LinearToFilmPrint& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("Black" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.black;
                }
                else if ("White" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.white;
                }
                else if ("Gamma" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.gamma;
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

    void fromJSON(const picojson::value& value, AV::IO::Cineon::FilmPrintToLinear& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("Black" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.black;
                }
                else if ("White" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.white;
                }
                else if ("Gamma" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.gamma;
                }
                else if ("SoftClip" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.softClip;
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

    void fromJSON(const picojson::value& value, AV::IO::Cineon::Settings& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("InputProfile" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.inputProfile;
                }
                else if ("InputFilmPrint" == i.first)
                {
                    fromJSON(i.second, out.inputFilmPrint);
                }
                else if ("OutputProfile" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.outputProfile;
                }
                else if ("OutputFilmPrint" == i.first)
                {
                    fromJSON(i.second, out.outputFilmPrint);
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

} // namespace djv

