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

#include <djvCore/FileInfo.h>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            const std::string& getFilePermissionsLabel(int in)
            {
                static const std::vector<std::string> data =
                {
                    "",
                    "r",
                    "w",
                    "rw",
                    "x",
                    "rx",
                    "wx",
                    "rwx"
                };
                return data[in];
            }

            void FileInfo::_init()
            {
                _exists = false;
                _type = FileType::File;
                _size = 0;
                _user = 0;
                _permissions = 0;
                _time = 0;
            }

            FileInfo::FileInfo()
            {
                _init();
            }

            FileInfo::FileInfo(const Path& path, bool stat)
            {
                _init();

                setPath(path, stat);
            }

            FileInfo::FileInfo(const std::string& path, bool stat)
            {
                _init();

                setPath(path, stat);
            }

            std::string FileInfo::getFileName(Frame::Number frame, bool path) const
            {
                std::stringstream s;
                const bool isRoot = std::string(1, Path::getCurrentPathSeparator()) == _path.get();
                if (isRoot)
                {
                    s << _path;
                }
                else
                {
                    if (path)
                    {
                        s << _path.getDirectoryName();
                    }
                    s << _path.getBaseName();
                    if (FileType::Sequence == _type && _sequence.ranges.size() && frame != Frame::Invalid)
                    {
                        s << Frame::toString(frame, _sequence.pad);
                    }
                    else if (FileType::Sequence == _type && _sequence.ranges.size())
                    {
                        s << _sequence;
                    }
                    else
                    {
                        s << _path.getNumber();
                    }
                    s << _path.getExtension();
                }
                return s.str();
            }

            void FileInfo::setPath(const Path& value, bool stat)
            {
                _init();

                _path = value;
                _exists = false;
                _type = FileType::File;
                _size = 0;
                _user = 0;
                _permissions = 0;
                _time = 0;
                _sequence = Frame::Sequence();

                // Get information from the file system.
                if (stat)
                {
                    this->stat();
                }
            }

            void FileInfo::setPath(const std::string& value, bool stat)
            {
                setPath(Path(value), stat);
            }

            void FileInfo::setSequence(const Frame::Sequence& in)
            {
                _sequence = in;
                std::stringstream s;
                s << _sequence;
                _path.setNumber(s.str());
            }

            void FileInfo::evalSequence()
            {
                if (FileType::File == _type)
                {
                    try
                    {
                        std::stringstream s(_path.getNumber());
                        s >> _sequence;
                        if (_sequence.ranges.size())
                        {
                            _type = FileType::Sequence;
                        }
                    }
                    catch (const std::exception&)
                    {
                        _sequence = Frame::Sequence();
                    }
                }
            }

            void FileInfo::sortSequence()
            {
                _sequence.sort();
                std::stringstream s;
                s << _sequence;
                _path.setNumber(s.str());
            }

            FileInfo FileInfo::getFileSequence(const Path& path)
            {
                DirectoryListOptions options;
                options.fileSequences = true;
                for (const auto& fileInfo : directoryList(path.getDirectoryName(), options))
                {
                    if (fileInfo.sequenceContains(path))
                    {
                        return fileInfo;
                    }
                }
                return FileInfo(path);
            }

        } // namespace FileSystem
    } // namespace Core

    std::ostream& operator << (std::ostream& s, const Core::FileSystem::FileInfo& value)
    {
        s << value.getPath();
        return s;
    }

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::FileSystem,
        FileType,
        DJV_TEXT("File"),
        DJV_TEXT("Sequence"),
        DJV_TEXT("Directory"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::FileSystem,
        DirectoryListSort,
        DJV_TEXT("Name"),
        DJV_TEXT("Size"),
        DJV_TEXT("Permissions"),
        DJV_TEXT("Type"));

} // namespace djv
