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

#include <sstream>

namespace djv
{
    template<>
    inline picojson::value toJSON<UICore::Palette>(const UICore::Palette& value)
    {
        picojson::value out(picojson::object_type, true);
        {
            picojson::value object(picojson::object_type, true);
            for (auto role : UICore::getColorRoleEnums())
            {
                std::stringstream ss;
                ss << role;
                object.get<picojson::object>()[ss.str()] = picojson::value(toJSON(value.getColor(role)));
            }
            out.get<picojson::object>()["Roles"] = object;
        }
        return out;
    }

    template<>
    inline picojson::value toJSON<std::string, UICore::Palette>(const std::map<std::string, UICore::Palette>& value)
    {
        picojson::value out(picojson::object_type, true);
        for (const auto& i : value)
        {
            out.get<picojson::object>()[i.first] = toJSON(i.second);
        }
        return out;
    }

    template<>
    inline picojson::value toJSON<UICore::Metrics>(const UICore::Metrics& value)
    {
        picojson::value out(picojson::object_type, true);
        {
            picojson::value object(picojson::object_type, true);
            for (auto role : UICore::getMetricsRoleEnums())
            {
                std::stringstream ss;
                ss << role;
                object.get<picojson::object>()[ss.str()] = picojson::value(std::to_string(value.getMetric(role)));
            }
            out.get<picojson::object>()["Roles"] = object;
        }
        out.get<picojson::object>()["Scale"] = picojson::value(std::to_string(value.getScale()));
        return out;
    }

    template<>
    inline picojson::value toJSON<std::string, UICore::Metrics>(const std::map<std::string, UICore::Metrics>& value)
    {
        picojson::value out(picojson::object_type, true);
        for (const auto& i : value)
        {
            out.get<picojson::object>()[i.first] = toJSON(i.second);
        }
        return out;
    }

    template<>
    inline void fromJSON<UICore::Palette>(const picojson::value& value, UICore::Palette& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("Roles" == i.first)
                {
                    std::map<UICore::ColorRole, AV::Image::Color> value;
                    fromJSON(i.second, value);
                    for (const auto& i : value)
                    {
                        out.setColor(i.first, i.second);
                    }
                }
            }
        }
        else
        {
            throw std::invalid_argument("Cannot parse");
        }
    }

    template<>
    inline void fromJSON<std::string, UICore::Palette>(const picojson::value& value, std::map<std::string, UICore::Palette>& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                fromJSON(i.second, out[i.first]);
            }
        }
        else
        {
            throw std::invalid_argument("Cannot parse");
        }
    }

    template<>
    inline void fromJSON<UICore::Metrics>(const picojson::value& value, UICore::Metrics& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("Roles" == i.first)
                {
                    if (i.second.is<picojson::object>())
                    {
                        for (const auto& j : i.second.get<picojson::object>())
                        {
                            UICore::MetricsRole role = UICore::MetricsRole::First;
                            std::stringstream ss(j.first);
                            ss >> role;
                            if (j.second.is<std::string>())
                            {
                                out.setMetric(role, std::stof(j.second.get<std::string>()));
                            }
                            else
                            {
                                throw std::invalid_argument("Cannot parse");
                            }
                        }
                    }
                    else
                    {
                        throw std::invalid_argument("Cannot parse");
                    }
                }
                else if ("Scale" == i.first)
                {
                    if (i.second.is<std::string>())
                    {
                        out.setScale(std::stof(i.second.get<std::string>()));
                    }
                    else
                    {
                        throw std::invalid_argument("Cannot parse");
                    }
                }
            }
        }
        else
        {
            throw std::invalid_argument("Cannot parse");
        }
    }

    template<>
    inline void fromJSON<std::string, UICore::Metrics>(const picojson::value& value, std::map<std::string, UICore::Metrics>& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                fromJSON(i.second, out[i.first]);
            }
        }
        else
        {
            throw std::invalid_argument("Cannot parse");
        }
    }

} // namespace djv
