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
    template<typename T>
    inline picojson::value toJSON(const T & value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    template<typename T>
    inline picojson::value toJSON(const std::vector<T>& value)
    {
        picojson::value out(picojson::array_type, true);
        for (const auto& i : value)
        {
            out.get<picojson::array>().push_back(toJSON(i));
        }
        return out;
    }

    template<typename T, typename U>
    inline picojson::value toJSON(const std::map<T, U>& value)
    {
        picojson::value out(picojson::object_type, true);
        for (const auto& i : value)
        {
            std::stringstream ss;
            ss << i.first;
            out.get<picojson::object>()[ss.str()] = toJSON(i.second);
        }
        return out;
    }

    template<typename T>
    inline void fromJSON(const picojson::value& value, T & out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse"));
        }
    }

    template<typename T>
    inline void fromJSON(const picojson::value& value, std::vector<T>& out)
    {
        if (value.is<picojson::array>())
        {
            for (const auto& i : value.get<picojson::array>())
            {
                T value;
                fromJSON(i, value);
                out.push_back(value);
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse"));
        }
    }

    template<typename T, typename U>
    inline void fromJSON(const picojson::value& value, std::map<T, U>& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                T key;
                std::stringstream ss(i.first);
                ss >> key;
                U value;
                fromJSON(i.second, value);
                out[key] = value;
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse"));
        }
    }

} // namespace djv
