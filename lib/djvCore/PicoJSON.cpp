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

#include <djvCore/PicoJSON.h>

#include <djvCore/FileIO.h>
#include <djvCore/String.h>

namespace djv
{
    namespace Core
    {
        namespace PicoJSON
        {
            void write(const picojson::value& value, FileSystem::FileIO& fileIO, size_t indent, bool continueLine)
            {
                if (value.is<picojson::object>())
                {
                    std::string s = "{\n";
                    if (!continueLine)
                    {
                        s = String::indent(indent) + s;
                    }
                    fileIO.write(s);
                    ++indent;
                    const auto& object = value.get<picojson::object>();
                    size_t i = 0;
                    for (const auto& key : object)
                    {
                        std::stringstream s;
                        s << "\"" << key.first << "\": ";
                        fileIO.write(String::indent(indent) + s.str());
                        write(key.second, fileIO, indent, true);
                        if (i < object.size() - 1)
                        {
                            fileIO.write(", ");
                        }
                        fileIO.write("\n");
                        ++i;
                    }
                    --indent;
                    fileIO.write(String::indent(indent) + "}");
                }
                else if (value.is<picojson::array>())
                {
                    std::string s = "[\n";
                    if (!continueLine)
                    {
                        s = String::indent(indent) + s;
                    }
                    fileIO.write(s);
                    ++indent;
                    const auto& array = value.get<picojson::array>();
                    for (size_t i = 0; i < array.size(); ++i)
                    {
                        write(array[i], fileIO, indent);
                        if (i < array.size() - 1)
                        {
                            fileIO.write(", ");
                        }
                        fileIO.write("\n");
                    }
                    --indent;
                    fileIO.write(String::indent(indent) + "]");
                }
                else if (value.is<double>())
                {
                    std::stringstream s;
                    s << "\"" << value.get<double>() << "\"";
                    std::string tmp = s.str();
                    if (!continueLine)
                    {
                        tmp = String::indent(indent) + tmp;
                    }
                    fileIO.write(tmp);
                }
                else if (value.is<std::string>())
                {
                    std::stringstream s;
                    s << "\"" << value.get<std::string>() << "\"";
                    std::string tmp = s.str();
                    if (!continueLine)
                    {
                        tmp = String::indent(indent) + tmp;
                    }
                    fileIO.write(tmp);
                }
            }

        } // namespace PicoJSON
    } // namespace Core

    picojson::value toJSON(int value)
    {
        std::stringstream s;
        s << value;
        return picojson::value(s.str());
    }

    picojson::value toJSON(float value)
    {
        std::stringstream s;
        s << value;
        return picojson::value(s.str());
    }

    picojson::value toJSON(const std::string& value)
    {
        return picojson::value(Core::String::escape(value));
    }

    picojson::value toJSON(const std::vector<std::string>& value)
    {
        picojson::value out(picojson::array_type, true);
        for (const auto & i : value)
        {
            out.get<picojson::array>().push_back(toJSON(i));
        }
        return out;
    }

    picojson::value toJSON(const std::map<std::string, std::string>& value)
    {
        picojson::value out(picojson::object_type, true);
        for (auto i : value)
        {
            out.get<picojson::object>()[i.first] = toJSON(i.second);
        }
        return out;
    }

    void fromJSON(const picojson::value& value, int& out)
    {
        if (value.is<std::string>())
        {
            out = std::stoi(value.get<std::string>());
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("djv::Core::PicoJSON", "Cannot parse value."));
        }
    }

    void fromJSON(const picojson::value& value, float& out)
    {
        if (value.is<std::string>())
        {
            out = std::stof(value.get<std::string>());
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("djv::Core::PicoJSON", "Cannot parse value."));
        }
    }

    void fromJSON(const picojson::value& value, std::string& out)
    {
        if (value.is<std::string>())
        {
            out = Core::String::unescape(value.get<std::string>());
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("djv::Core::PicoJSON", "Cannot parse value."));
        }
    }

    void fromJSON(const picojson::value& value, std::vector<std::string>& out)
    {
        if (value.is<picojson::array>())
        {
            for (const auto& i : value.get<picojson::array>())
            {
                std::string tmp;
                fromJSON(i, tmp);
                out.push_back(tmp);
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("djv::Core::PicoJSON", "Cannot parse value."));
        }
    }

    void fromJSON(const picojson::value& value, std::map<std::string, std::string>& out)
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
            throw std::invalid_argument(DJV_TEXT("djv::Core::PicoJSON", "Cannot parse value."));
        }
    }

} // namespace djv
