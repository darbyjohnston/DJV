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

namespace djv
{
    namespace Core
    {
        inline QString FileInfo::name() const
        {
            return _base + _number + _extension;
        }

        inline const QString & FileInfo::path() const
        {
            return _path;
        }

        inline const QString & FileInfo::base() const
        {
            return _base;
        }

        inline const QString & FileInfo::number() const
        {
            return _number;
        }

        inline const QString & FileInfo::extension() const
        {
            return _extension;
        }

        inline bool FileInfo::isEmpty() const
        {
            return
                _path.isEmpty() &&
                _base.isEmpty() &&
                _number.isEmpty() &&
                _extension.isEmpty();
        }

        inline bool FileInfo::exists() const
        {
            return _exists;
        }

        inline bool FileInfo::isDotFile() const
        {
            return _dotFile;
        }

        inline FileInfo::TYPE FileInfo::type() const
        {
            return _type;
        }

        inline quint64 FileInfo::size() const
        {
            return _size;
        }

        inline uid_t FileInfo::user() const
        {
            return _user;
        }

        inline int FileInfo::permissions() const
        {
            return _permissions;
        }

        inline time_t FileInfo::time() const
        {
            return _time;
        }

        inline const Sequence & FileInfo::sequence() const
        {
            return _sequence;
        }

        inline bool FileInfo::isSequenceValid() const
        {
            return
                _type != DIRECTORY &&
                _number.length() &&
                _sequence.frames.count();
        }

        inline bool FileInfo::isSequenceWildcard() const
        {
            return
                _type != DIRECTORY &&
                (_number.length() && '#' == _number.at(0));
        }

        inline bool FileInfo::addSequence(const FileInfo & in)
        {
            // Compare.
            if (_extension != in._extension)
                return false;
            if (_base != in._base)
                return false;
            if (!isSequenceValid() || !in.isSequenceValid())
                return false;

            // Update sequence.
            _type = SEQUENCE;
            _sequence.frames += in._sequence.frames;
            if (in._sequence.pad > _sequence.pad)
                _sequence.pad = in._sequence.pad;

            // Update information.
            _size += in._size;
            if (in._user > _user)
                _user = in._user;
            if (in._time > _time)
                _time = in._time;

            return true;
        }

        inline bool FileInfo::operator == (const FileInfo & in) const
        {
            return
                in._path == _path &&
                in._base == _base &&
                in._number == _number &&
                in._extension == _extension &&
                in._type == _type &&
                in._size == _size &&
                in._user == _user &&
                in._permissions == _permissions &&
                in._time == _time;
        }

        inline bool FileInfo::operator != (const FileInfo & in) const
        {
            return !(in == *this);
        }

        inline FileInfo::operator QString() const
        {
            return fileName();
        }

    } // namespace Core
} // namespace djv
