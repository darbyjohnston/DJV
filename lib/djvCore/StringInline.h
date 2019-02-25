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

#include <algorithm>
#include <map>

namespace djv
{
    namespace Core
    {
        namespace String
        {
            inline std::vector<std::string> split(const char * s, size_t size, char delimeter, bool keepEmpty)
            {
                std::vector<std::string> out;
                bool word = false;
                size_t wordStart = 0;
                size_t i = 0;
                for (; i < size; ++i)
                {
                    if (s[i] != delimeter)
                    {
                        if (!word)
                        {
                            word = true;
                            wordStart = i;
                        }
                    }
                    else
                    {
                        if (word)
                        {
                            word = false;
                            out.push_back(std::string(s + wordStart, i - wordStart));
                        }
                        if (keepEmpty && i > 0 && s[i - 1] == delimeter)
                        {
                            out.push_back(std::string());
                        }
                    }
                }
                if (word)
                {
                    out.push_back(std::string(s + wordStart, i - wordStart));
                }
                return out;
            }

            inline std::vector<std::string> split(const std::string & s, char delimeter, bool keepEmpty)
            {
                std::vector<std::string> out;
                bool word = false;
                size_t wordStart = 0;
                size_t i = 0;
                for (; i < s.size(); ++i)
                {
                    if (s[i] != delimeter)
                    {
                        if (!word)
                        {
                            word = true;
                            wordStart = i;
                        }
                    }
                    else
                    {
                        if (word)
                        {
                            word = false;
                            out.push_back(s.substr(wordStart, i - wordStart));
                        }
                        if (keepEmpty && i > 0 && s[i - 1] == delimeter)
                        {
                            out.push_back(std::string());
                        }
                    }
                }
                if (word)
                {
                    out.push_back(s.substr(wordStart, i - wordStart));
                }
                return out;
            }

            inline std::vector<std::string> split(const std::string & s, const std::vector<char> & delimeters, bool keepEmpty)
            {
                std::vector<std::string> out;
                bool word = false;
                size_t wordStart = 0;
                size_t i = 0;
                for (; i < s.size(); ++i)
                {
                    if (std::find(delimeters.begin(), delimeters.end(), s[i]) == delimeters.end())
                    {
                        if (!word)
                        {
                            word = true;
                            wordStart = i;
                        }
                    }
                    else
                    {
                        if (word)
                        {
                            word = false;
                            out.push_back(s.substr(wordStart, i - wordStart));
                        }
                        if (keepEmpty && i > 0 && std::find(delimeters.begin(), delimeters.end(), s[i - 1]) != delimeters.end())
                        {
                            out.push_back(std::string());
                        }
                    }
                }
                if (word)
                {
                    out.push_back(s.substr(wordStart, i - wordStart));
                }
                return out;
            }

            inline std::string join(const std::vector<std::string> & value)
            {
                std::string out;
                for (const auto & s : value)
                {
                    out += s;
                }
                return out;
            }

            inline std::string join(const std::vector<std::string> & value, char delimeter)
            {
                std::string out;
                if (const size_t size = value.size())
                {
                    size_t j = 0;
                    for (auto i = value.begin(); i != value.end(); ++i, ++j)
                    {
                        out += *i;
                        if (j < size - 1)
                        {
                            out += delimeter;
                        }
                    }
                }
                return out;
            }

            inline std::string join(const std::vector<std::string> & value, const std::string & delimeter)
            {
                std::string out;
                if (const size_t size = value.size())
                {
                    size_t j = 0;
                    for (auto i = value.begin(); i != value.end(); ++i, ++j)
                    {
                        out += *i;
                        if (j < size - 1)
                        {
                            out += delimeter;
                        }
                    }
                }
                return out;
            }

            inline std::string joinSet(const std::set<std::string> & value)
            {
                std::string out;
                for (const auto & s : value)
                {
                    out += s;
                }
                return out;
            }

            inline std::string joinSet(const std::set<std::string> & value, char delimeter)
            {
                std::string out;
                if (const size_t size = value.size())
                {
                    size_t j = 0;
                    for (auto i = value.begin(); i != value.end(); ++i, ++j)
                    {
                        out += *i;
                        if (j < size - 1)
                        {
                            out += delimeter;
                        }
                    }
                }
                return out;
            }

            inline std::string joinSet(const std::set<std::string> & value, const std::string & delimeter)
            {
                std::string out;
                if (const size_t size = value.size())
                {
                    size_t j = 0;
                    for (auto i = value.begin(); i != value.end(); ++i, ++j)
                    {
                        out += *i;
                        if (j < size - 1)
                        {
                            out += delimeter;
                        }
                    }
                }
                return out;
            }

            template<typename T>
            inline size_t intToString(T value, char * out, size_t maxLen)
            {
                const size_t max = maxLen ? maxLen - 1 : 0;
                const bool   negative = !(0 == value || value > 0);
                size_t count = 0;
                T tmp = value;
                do
                {
                    ++count;
                    tmp /= 10;
                } while (tmp && count < max);
                if (negative)
                {
                    ++count;
                    out[0] = '-';
                }
                if (count > 0)
                {
                    const int end = negative ? 1 : 0;
                    for (int i = static_cast<int>(count) - 1; i >= end; --i, value /= 10)
                    {
                        if (negative)
                        {
                            out[i] = '0' - (value % 10);
                        }
                        else
                        {
                            out[i] = '0' + (value % 10);
                        }
                    }
                }
                out[count] = 0;
                return count;
            }

            inline void fromString(const char * s, size_t size, int & out)
            {
                out = 0;

                // Find the sign.
                bool negativeSign = false;
                if ('-' == s[0])
                {
                    negativeSign = true;
                    ++s;
                    --size;
                }
                else if ('+' == s[0])
                {
                    ++s;
                    --size;
                }

                // Find the end.
                size_t end = 0;
                for (; end < size && s[end]; ++end)
                    ;

                // Add up the digits.
                int tens = 1;
                for (int i = int(end) - 1; i >= 0; --i, tens *= 10)
                {
                    out += (s[i] - 48) * tens;
                }

                // Apply the sign.
                if (negativeSign)
                {
                    out = -out;
                }
            }

            inline void fromString(const char * s, size_t size, int64_t & out)
            {
                out = 0;

                // Find the sign.
                bool negativeSign = false;
                if ('-' == s[0])
                {
                    negativeSign = true;
                    ++s;
                    --size;
                }
                else if ('+' == s[0])
                {
                    ++s;
                    --size;
                }

                // Find the end.
                size_t end = 0;
                for (; end < size && s[end]; ++end)
                    ;

                // Add up the digits.
                int tens = 1;
                for (int i = int(end) - 1; i >= 0; --i, tens *= 10)
                {
                    out += (s[i] - 48) * tens;
                }

                // Apply the sign.
                if (negativeSign)
                {
                    out = -out;
                }
            }

            inline void fromString(const char * s, size_t size, size_t & out)
            {
                out = 0;

                // Add up the digits.
                int tens = 1;
                for (int i = int(size) - 1; i >= 0; --i, tens *= 10)
                {
                    out += (s[i] - 48) * tens;
                }
            }

        } // namespace String
    } // namespace Core
} // namespace djv
