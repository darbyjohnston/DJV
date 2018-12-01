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

#include <djvCore/Keycode.h>

#include <djvCore/String.h>

namespace djv
{
    namespace Core
    {
	    namespace Keycode
	    {
		    std::string toString(
			    int id,
			    int type,
			    int prefix,
			    int count,
			    int offset)
		    {
                std::vector<std::string> list;
                list.push_back(std::to_string(id));
                list.push_back(std::to_string(type));
                list.push_back(std::to_string(prefix));
                list.push_back(std::to_string(count));
                list.push_back(std::to_string(offset));
                return String::join(list, ':');
            }

		    void fromString(
			    const std::string& string,
			    int&               id,
			    int&               type,
			    int&               prefix,
			    int&               count,
			    int&               offset)
		    {
			    const auto pieces = String::split(string, ':');
                if (pieces.size() != 5)
                {
                    throw std::invalid_argument(DJV_TEXT("Cannot parse"));
                }
                id = std::stoi(pieces[0]);
                type = std::stoi(pieces[1]);
                prefix = std::stoi(pieces[2]);
                count = std::stoi(pieces[3]);
                offset = std::stoi(pieces[4]);
		    }

	    } // namespace Keycode
	} // namespace Core
} // namespace djv
