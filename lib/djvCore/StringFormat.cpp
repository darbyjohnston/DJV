//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvCore/StringFormat.h>

#include <map>
#include <regex>

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
                    ptrdiff_t pos    = 0;
                    ptrdiff_t length = 0;
                };

            } // namespace

            Format& Format::arg(const std::string& value)
            {
                try
                {
                    std::string subject = _text;
                    std::regex r("\\{([0-9]+)\\}");
                    std::smatch m;
                    std::map<int, Match> matches;
                    ptrdiff_t currentPos = 0;
                    while (std::regex_search(subject, m, r))
                    {
                        if (2 == m.size())
                        {
                            const int index = std::stoi(m[1]);
                            const auto i = matches.find(index);
                            if (i == matches.end())
                            {
                                const ptrdiff_t pos = m.position(0);
                                const ptrdiff_t len = m.length(0);
                                matches[std::stoi(m[1])] = { currentPos + pos, len };
                                currentPos += pos + len;
                            }
                            else
                            {
                                throw std::runtime_error(DJV_TEXT("Duplicate argument."));
                            }
                        }
                        else
                        {
                            throw std::runtime_error(DJV_TEXT("Cannot parse the argument."));
                        }
                        subject = m.suffix().str();
                    }
                    if (matches.size() > 0)
                    {
                        _text.replace(matches.begin()->second.pos, matches.begin()->second.length, value);
                    }
                    else
                    {
                        throw std::runtime_error(DJV_TEXT("Argument not found."));
                    }
                }
                catch (const std::exception& e)
                {
                    _error = e.what();
                }
                return *this;
            }

        } // namespace String
    } // namespace Core
} // namespace djv
