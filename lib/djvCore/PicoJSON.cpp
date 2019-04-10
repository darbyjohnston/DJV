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

#include <djvCore/PicoJSON.h>

#include <djvCore/FileIO.h>
#include <djvCore/StringUtil.h>

#include <sstream>

namespace djv
{
    namespace
    {
        std::string _indent(size_t indent, size_t count = 4)
        {
            return std::string(indent * count, ' ');
        }

    } // namespace

    namespace Core
    {
        namespace PicoJSON
        {
            std::string escape(const std::string & value)
            {
                std::string out;
                for (const auto i : value)
                {
                    if ('\\' == i)
                    {
                        out.push_back('\\');
                        out.push_back('\\');
                    }
                    else if ('\n' == i)
                    {
                        out.push_back('\\');
                        out.push_back('n');
                    }
                    else if ('\r' == i)
                    {
                        out.push_back('\\');
                        out.push_back('r');
                    }
                    else if ('\t' == i)
                    {
                        out.push_back('\\');
                        out.push_back('t');
                    }
                    else
                    {
                        out.push_back(i);
                    }
                }
                return out;
            }

            void write(const picojson::value & value, FileIO & fileIO, size_t indent, bool continueLine)
            {
                if (value.is<picojson::object>())
                {
                    std::string s = "{\n";
                    if (!continueLine)
                    {
                        s = _indent(indent) + s;
                    }
                    fileIO.set(s);
                    ++indent;
                    const auto & object = value.get<picojson::object>();
                    size_t i = 0;
                    for (const auto & key : object)
                    {
                        std::stringstream s;
                        s << "\"" << key.first << "\": ";
                        fileIO.set(_indent(indent) + s.str());
                        write(key.second, fileIO, indent, true);
                        if (i < object.size() - 1)
                        {
                            fileIO.set(", ");
                        }
                        fileIO.set("\n");
                        ++i;
                    }
                    --indent;
                    fileIO.set(_indent(indent) + "}");
                }
                else if (value.is<picojson::array>())
                {
                    std::string s = "[\n";
                    if (!continueLine)
                    {
                        s = _indent(indent) + s;
                    }
                    fileIO.set(s);
                    ++indent;
                    const auto & array = value.get<picojson::array>();
                    for (size_t i = 0; i < array.size(); ++i)
                    {
                        write(array[i], fileIO, indent);
                        if (i < array.size() - 1)
                        {
                            fileIO.set(", ");
                        }
                        fileIO.set("\n");
                    }
                    --indent;
                    fileIO.set(_indent(indent) + "]");
                }
                else if (value.is<double>())
                {
                    std::stringstream s;
                    s << "\"" << value.get<double>() << "\"";
                    std::string tmp = s.str();
                    if (!continueLine)
                    {
                        tmp = _indent(indent) + tmp;
                    }
                    fileIO.set(tmp);
                }
                else if (value.is<std::string>())
                {
                    std::stringstream s;
                    s << "\"" << value.get<std::string>() << "\"";
                    std::string tmp = s.str();
                    if (!continueLine)
                    {
                        tmp = _indent(indent) + tmp;
                    }
                    fileIO.set(tmp);
                }
            }

        } // namespace PicoJSON
    } // namespace Core

    picojson::value toJSON(bool value)
    {
        std::stringstream s;
        s << (value ? "true" : "false");
        return picojson::value(s.str());
    }

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

    picojson::value toJSON(const std::string & value)
    {
        return picojson::value(Core::PicoJSON::escape(value));
    }

    void fromJSON(const picojson::value & value, bool & out)
    {
        if (value.is<std::string>())
        {
            out = value.get<std::string>() == "true";
        }
        else
        {
            throw std::invalid_argument("Cannot parse the value.");
        }
    }

    void fromJSON(const picojson::value & value, int & out)
    {
        if (value.is<std::string>())
        {
            out = std::stoi(value.get<std::string>());
        }
        else
        {
            throw std::invalid_argument("Cannot parse the value.");
        }
    }

    void fromJSON(const picojson::value & value, float & out)
    {
        if (value.is<std::string>())
        {
            out = std::stof(value.get<std::string>());
        }
        else
        {
            throw std::invalid_argument("Cannot parse the value.");
        }
    }

    void fromJSON(const picojson::value & value, std::string & out)
    {
        if (value.is<std::string>())
        {
            //out = Core::String::unescape(value.get<std::string>());
            out = value.get<std::string>();
        }
        else
        {
            throw std::invalid_argument("Cannot parse the value.");
        }
    }

} // namespace djv
