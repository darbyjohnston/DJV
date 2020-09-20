// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/StringFormat.h>

#include <map>
#include <regex>
#include <sstream>

namespace djv
{
    namespace Core
    {
        namespace String
        {
            namespace
            {
                struct Match
                {
                    Match()
                    {}

                    Match(std::ptrdiff_t pos, std::ptrdiff_t length) :
                        pos(pos),
                        length(length)
                    {}

                    std::ptrdiff_t pos    = 0;
                    std::ptrdiff_t length = 0;
                };

            } // namespace
            
            Format::Format(const std::string& value) :
                _text(value)
            {}

            Format& Format::arg(const std::string& value)
            {
                try
                {
                    std::string subject = _text;
                    std::regex r("\\{([0-9]+)\\}");
                    std::smatch m;
                    std::map<int, Match> matches;
                    std::ptrdiff_t currentPos = 0;
                    while (std::regex_search(subject, m, r))
                    {
                        if (2 == m.size())
                        {
                            const int index = std::stoi(m[1]);
                            const auto i = matches.find(index);
                            if (i == matches.end())
                            {
                                const std::ptrdiff_t pos = m.position(0);
                                const std::ptrdiff_t len = m.length(0);
                                matches[std::stoi(m[1])] = Match(currentPos + pos, len);
                                currentPos += pos + len;
                            }
                            else
                            {
                                throw std::runtime_error(DJV_TEXT("error_duplicate_argument"));
                            }
                        }
                        else
                        {
                            throw std::runtime_error(DJV_TEXT("error_cannot_parse_argument"));
                        }
                        subject = m.suffix().str();
                    }
                    if (matches.size() > 0)
                    {
                        _text.replace(matches.begin()->second.pos, matches.begin()->second.length, value);
                    }
                    else
                    {
                        throw std::runtime_error(DJV_TEXT("error_argument_not_found"));
                    }
                }
                catch (const std::exception& e)
                {
                    _error = e.what();
                }
                return *this;
            }

            Format& Format::arg(int value)
            {
                std::stringstream ss;
                ss << value;
                return arg(ss.str());
            }

            Format& Format::arg(float value)
            {
                std::stringstream ss;
                ss << value;
                return arg(ss.str());
            }

            Format& Format::arg(size_t value)
            {
                std::stringstream ss;
                ss << value;
                return arg(ss.str());
            }

        } // namespace String
    } // namespace Core
} // namespace djv
