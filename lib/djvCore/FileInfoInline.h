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

namespace djv
{
    namespace Core
    {
        inline bool FileInfo::doesExist() const
        {
            return _exists;
        }

        inline FileType FileInfo::getType() const
        {
            return _type;
        }

        inline uint64_t FileInfo::getSize() const
        {
            return _size;
        }

        inline uid_t FileInfo::getUser() const
        {
            return _user;
        }

        inline int FileInfo::getPermissions() const
        {
            return _permissions;
        }

        inline time_t FileInfo::getTime() const
        {
            return _time;
        }

        inline bool FileInfo::isSequenceValid() const
        {
            return
                _type != FileType::Directory &&
                !_path.getNumber().empty() &&
                _sequence.ranges.size();
        }

        inline bool FileInfo::isSequenceWildcard() const
        {
            return
                _type != FileType::Directory &&
                "#" == _path.getNumber();
        }

        inline bool FileInfo::sequenceContains(const FileInfo& value) const
        {
            if (!isSequenceValid())
                return false;
            if (_path.getNumber().empty())
                return false;
            if (_path.getExtension() != value._path.getExtension())
                return false;
            if (_path.getBaseName() != value._path.getBaseName())
                return false;
            return true;
        }

        inline bool FileInfo::addToSequence(const FileInfo& value)
        {
            if (sequenceContains(value))
            {
                for (const auto& range : value._sequence.ranges)
                {
                    _sequence.ranges.push_back(range);
                }
                if (value._sequence.pad > _sequence.pad)
                {
                    _sequence.pad = value._sequence.pad;
                }
                _size += value._size;
                if (value._user > _user)
                {
                    _user = value._user;
                }
                if (value._time > _time)
                {
                    _time = value._time;
                }
                return true;
            }
            return false;
        }

        inline bool FileInfo::operator == (const FileInfo& in) const
        {
            return
                in._path == _path        &&
                in._type == _type        &&
                in._size == _size        &&
                in._user == _user        &&
                in._permissions == _permissions &&
                in._time == _time;
        }

        inline bool FileInfo::operator != (const FileInfo& in) const
        {
            return !(in == *this);
        }

        inline bool FileInfo::operator < (const FileInfo& in) const
        {
            return in._path.get() < _path.get();
        }

        inline FileInfo::operator std::string() const
        {
            return _path;
        }

    } // namespace Core

    template<>
    inline picojson::value toJSON<Core::FileInfo>(const Core::FileInfo& value)
    {
        picojson::value out(picojson::array_type, true);
        out.get<picojson::array>().push_back(toJSON(value.getPath()));
        out.get<picojson::array>().push_back(toJSON(value.getType()));
        return out;
    }

    template<>
    inline void fromJSON<Core::FileInfo>(const picojson::value& value, Core::FileInfo& out)
    {
        if (value.is<picojson::array>() && 2 == value.get<picojson::array>().size())
        {
            Core::Path path;
            fromJSON(value.get<picojson::array>()[0], path);
            out.setPath(path);
            Core::FileType fileType = Core::FileType::First;
            fromJSON(value.get<picojson::array>()[1], fileType);
            if (Core::FileType::Sequence == fileType)
            {
                out.evalSequence();
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse"));
        }
    }

} // namespace djv
