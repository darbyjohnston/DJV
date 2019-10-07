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

#include <djvAV/Tags.h>

namespace djv
{
    namespace AV
    {
        const std::string Tags::_empty;

        Tags::Tags()
        {}
        
        bool Tags::isEmpty() const
        {
            return 0 == _tags.size();
        }

        size_t Tags::getCount() const
        {
            return _tags.size();
        }

        const std::map<std::string, std::string> & Tags::getTags() const
        {
            return _tags;
        }

        bool Tags::hasTag(const std::string& key) const
        {
            return _tags.find(key) != _tags.end();
        }

        const std::string & Tags::getTag(const std::string & key) const
        {
            static const std::string empty;
            const auto i = _tags.find(key);
            return i != _tags.end() ? i->second : _empty;
        }

        void Tags::setTags(const std::map<std::string, std::string> & tags)
        {
            _tags = tags;
        }

        void Tags::setTag(const std::string & key, const std::string & value)
        {
            _tags[key] = value;
        }

        bool Tags::operator == (const Tags & other) const
        {
            return _tags == other._tags;
        }

    } // namespace AV
} // namespace djv

