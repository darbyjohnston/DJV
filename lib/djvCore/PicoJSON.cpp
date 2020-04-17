// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/PicoJSON.h>

#include <djvCore/FileIO.h>
#include <djvCore/String.h>

#include <sstream>

namespace djv
{
    namespace Core
    {
        namespace PicoJSON
        {
            void write(
                const picojson::value& value,
                const std::shared_ptr<FileSystem::FileIO>& fileIO,
                size_t indent,
                bool continueLine)
            {
                if (value.is<picojson::object>())
                {
                    std::string s = "{\n";
                    if (!continueLine)
                    {
                        s = String::indent(indent) + s;
                    }
                    fileIO->write(s);
                    ++indent;
                    const auto & object = value.get<picojson::object>();
                    size_t i = 0;
                    for (const auto & key : object)
                    {
                        std::stringstream ss;
                        ss << "\"" << key.first << "\": ";
                        fileIO->write(String::indent(indent) + ss.str());
                        write(key.second, fileIO, indent, true);
                        if (i < object.size() - 1)
                        {
                            fileIO->write(", ");
                        }
                        fileIO->write("\n");
                        ++i;
                    }
                    --indent;
                    fileIO->write(String::indent(indent) + "}");
                }
                else if (value.is<picojson::array>())
                {
                    std::string s = "[\n";
                    if (!continueLine)
                    {
                        s = String::indent(indent) + s;
                    }
                    fileIO->write(s);
                    ++indent;
                    const auto & array = value.get<picojson::array>();
                    for (size_t i = 0; i < array.size(); ++i)
                    {
                        write(array[i], fileIO, indent);
                        if (i < array.size() - 1)
                        {
                            fileIO->write(", ");
                        }
                        fileIO->write("\n");
                    }
                    --indent;
                    fileIO->write(String::indent(indent) + "]");
                }
                else if (value.is<double>())
                {
                    std::stringstream ss;
                    ss << "\"" << value.get<double>() << "\"";
                    std::string tmp = ss.str();
                    if (!continueLine)
                    {
                        tmp = String::indent(indent) + tmp;
                    }
                    fileIO->write(tmp);
                }
                else if (value.is<std::string>())
                {
                    std::stringstream ss;
                    ss << "\"" << value.get<std::string>() << "\"";
                    std::string tmp = ss.str();
                    if (!continueLine)
                    {
                        tmp = String::indent(indent) + tmp;
                    }
                    fileIO->write(tmp);
                }
            }

        } // namespace PicoJSON
    } // namespace Core

    picojson::value toJSON(bool value)
    {
        std::stringstream ss;
        ss << (value ? "true" : "false");
        return picojson::value(ss.str());
    }

    picojson::value toJSON(int value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    picojson::value toJSON(float value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    picojson::value toJSON(size_t value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    picojson::value toJSON(const std::string & value)
    {
        return picojson::value(Core::String::escape(value));
    }

    void fromJSON(const picojson::value & value, bool & out)
    {
        if (value.is<std::string>())
        {
            out = value.get<std::string>() == "true";
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
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
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
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
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const picojson::value& value, size_t& out)
    {
        if (value.is<std::string>())
        {
            out = std::max(static_cast<long>(0), std::stol(value.get<std::string>()));
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
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
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv
