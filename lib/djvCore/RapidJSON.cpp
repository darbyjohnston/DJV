// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/RapidJSON.h>

#include <djvCore/String.h>

#include <sstream>

namespace djv
{
    namespace Core
    {
        namespace RapidJSON
        {
            void errorLineNumber(const char* text, size_t size, size_t index, size_t& line, size_t& character)
            {
                line = 1;
                size_t lineTotal = 0;
                size_t i = 0;
                for (; i < size && i < index; ++i)
                {
                    bool newline = false;
                    if ('\r' == text[i] && i < size - 1 && '\n' == text[i + 1])
                    {
                        newline = true;
                        ++i;
                    }
                    else if ('\n' == text[i])
                    {
                        newline = true;
                    }
                    if (newline)
                    {
                        ++line;
                        lineTotal = i;
                    }
                }
                character = i - lineTotal;
            }

        } // namespace Core
    } // namespace RapidJSON

    rapidjson::Value toJSON(bool value, rapidjson::Document::AllocatorType&)
    {
        return rapidjson::Value(value);
    }

    rapidjson::Value toJSON(int value, rapidjson::Document::AllocatorType&)
    {
        return rapidjson::Value(value);
    }

    rapidjson::Value toJSON(int64_t value, rapidjson::Document::AllocatorType&)
    {
        return rapidjson::Value(value);
    }

    rapidjson::Value toJSON(float value, rapidjson::Document::AllocatorType&)
    {
        return rapidjson::Value(value);
    }

    rapidjson::Value toJSON(double value, rapidjson::Document::AllocatorType&)
    {
        return rapidjson::Value(value);
    }

    rapidjson::Value toJSON(size_t value, rapidjson::Document::AllocatorType&)
    {
        return rapidjson::Value(static_cast<int64_t>(value));
    }

    rapidjson::Value toJSON(const std::string& value, rapidjson::Document::AllocatorType& allocator)
    {
        const std::string escaped = Core::String::escape(value);
        rapidjson::Value out;
        out.SetString(escaped.c_str(), escaped.size(), allocator);
        return out;
    }

    void fromJSON(const rapidjson::Value& value, bool& out)
    {
        if (value.IsBool())
        {
            out = value.IsTrue();
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, int& out)
    {
        if (value.IsInt())
        {
            out = value.GetInt();
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, int64_t& out)
    {
        if (value.IsInt64())
        {
            out = value.GetInt64();
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, float& out)
    {
        if (value.IsFloat())
        {
            out = value.GetFloat();
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, double& out)
    {
        if (value.IsDouble())
        {
            out = value.GetDouble();
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, size_t& out)
    {
        if (value.IsInt64())
        {
            out = std::max(static_cast<int64_t>(0), value.GetInt64());
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, std::string& out)
    {
        if (value.IsString())
        {
            out = Core::String::unescape(value.GetString());
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv
