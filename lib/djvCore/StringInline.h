// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <algorithm>

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

            inline std::vector<std::string> split(const std::string& s, char delimeter, bool keepEmpty)
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

            inline std::vector<std::string> split(const std::string& s, const std::vector<char>& delimeters, bool keepEmpty)
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

            inline std::string join(const std::vector<std::string>& value)
            {
                std::string out;
                for (const auto& s : value)
                {
                    out += s;
                }
                return out;
            }

            inline std::string join(const std::vector<std::string>& value, char delimeter)
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

            inline std::string join(const std::vector<std::string>& value, const std::string& delimeter)
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

            inline std::string joinList(const std::list<std::string>& value)
            {
                std::string out;
                for (const auto& s : value)
                {
                    out += s;
                }
                return out;
            }

            inline std::string joinList(const std::list<std::string>& value, char delimeter)
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

            inline std::string joinList(const std::list<std::string>& value, const std::string& delimeter)
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

            inline std::string joinSet(const std::set<std::string>& value)
            {
                std::string out;
                for (const auto& s : value)
                {
                    out += s;
                }
                return out;
            }

            inline std::string joinSet(const std::set<std::string>& value, char delimeter)
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

            inline std::string joinSet(const std::set<std::string>& value, const std::string& delimeter)
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

            inline void fromString(const char * s, size_t size, int& out)
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

            inline void fromString(const char * s, size_t size, int64_t& out)
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
                int64_t tens = 1;
                for (int i = int(end) - 1; i >= 0; --i, tens *= 10)
                {
                    out += (static_cast<int64_t>(s[i]) - 48) * tens;
                }

                // Apply the sign.
                if (negativeSign)
                {
                    out = -out;
                }
            }

            inline void fromString(const char * s, size_t size, size_t& out)
            {
                out = 0;

                // Add up the digits.
                size_t tens = 1;
                for (int i = int(size) - 1; i >= 0; --i, tens *= 10)
                {
                    out += (static_cast<size_t>(s[i]) - 48) * tens;
                }
            }

        } // namespace String
    } // namespace Core
} // namespace djv
