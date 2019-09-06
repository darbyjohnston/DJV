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

#include <djvCore/FileInfo.h>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            std::string getFilePermissionsLabel(int in)
            {
                const std::vector<std::string> data =
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

            std::string FileInfo::getFileName(Frame::Number frame, bool path) const
            {
                std::stringstream s;
                const bool isRoot = std::string(1, Path::getCurrentSeparator()) == _path.get();
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
                    if (FileType::Sequence == _type && _sequence.ranges.size() && frame != Frame::invalid)
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
                _path           = value;
                _exists         = false;
                _type           = FileType::File;
                _size           = 0;
                _user           = 0;
                _permissions    = 0;
                _time           = 0;
                _sequence       = Frame::Sequence();

                // Get information from the file system.
                if (stat)
                {
                    this->stat();
                }
            }

            void FileInfo::setPath(const Path& value, FileType fileType, bool stat)
            {
                _path           = value;
                _exists         = false;
                _type           = fileType;
                _size           = 0;
                _user           = 0;
                _permissions    = 0;
                _time           = 0;
                _sequence       = Frame::Sequence();

                if (FileType::Sequence == fileType)
                {
                    try
                    {
                        std::stringstream s(_path.getNumber());
                        s >> _sequence;
                    }
                    catch (const std::exception&)
                    {
                        _sequence = Frame::Sequence();
                    }
                }

                // Get information from the file system.
                if (stat)
                {
                    this->stat();
                }
            }

            void FileInfo::setSequence(const Frame::Sequence & in)
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
                    catch (const std::exception &)
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

            FileInfo FileInfo::getFileSequence(const Path & path, const std::vector<std::string>& extensions)
            {
                DirectoryListOptions options;
                options.fileSequences = true;
                options.fileSequenceExtensions = extensions;
                for (const auto & fileInfo : directoryList(path.getDirectoryName(), options))
                {
                    if (fileInfo.getSequence().getSize() > 1 &&
                        fileInfo.isCompatible(path))
                    {
                        return fileInfo;
                    }
                }
                return FileInfo(path);
            }

            void FileInfo::_fileSequence(FileInfo& fileInfo, const DirectoryListOptions& options, std::vector<FileInfo>& out)
            {
                std::string extension = fileInfo.getPath().getExtension();
                std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
                const auto i = std::find(
                    options.fileSequenceExtensions.begin(),
                    options.fileSequenceExtensions.end(),
                    extension);
                if (options.fileSequences && i != options.fileSequenceExtensions.end())
                {
                    fileInfo.evalSequence();
                    if (fileInfo.isSequenceValid())
                    {
                        const size_t size = out.size();
                        size_t i = 0;
                        for (; i < size; ++i)
                        {
                            if (out[i].addToSequence(fileInfo))
                            {
                                break;
                            }
                        }
                        if (size == i)
                        {
                            out.push_back(fileInfo);
                        }
                    }
                    else
                    {
                        out.push_back(fileInfo);
                    }
                }
                else
                {
                    out.push_back(fileInfo);
                }
            }

            void FileInfo::_sort(const DirectoryListOptions& options, std::vector<FileInfo>& out)
            {
                for (auto & i : out)
                {
                    if (i.isSequenceValid())
                    {
                        i.sortSequence();
                    }
                }

                switch (options.sort)
                {
                case DirectoryListSort::Name:
                    std::sort(
                        out.begin(), out.end(),
                        [&options](const FileInfo & a, const FileInfo & b)
                    {
                        return options.reverseSort ?
                            (a.getFileName(Frame::invalid, false) > b.getFileName(Frame::invalid, false)) :
                            (a.getFileName(Frame::invalid, false) < b.getFileName(Frame::invalid, false));
                    });
                    break;
                case DirectoryListSort::Size:
                    std::sort(
                        out.begin(), out.end(),
                        [&options](const FileInfo & a, const FileInfo & b)
                    {
                        return options.reverseSort ? (a.getSize() > b.getSize()) : (a.getSize() < b.getSize());
                    });
                    break;
                case DirectoryListSort::Time:
                    std::sort(
                        out.begin(), out.end(),
                        [&options](const FileInfo & a, const FileInfo & b)
                    {
                        return options.reverseSort ? (a.getTime() > b.getTime()) : (a.getTime() < b.getTime());
                    });
                    break;
                default: break;
                }
                if (options.sortDirectoriesFirst)
                {
                    std::stable_sort(
                        out.begin(), out.end(),
                        [](const FileInfo & a, const FileInfo & b)
                    {
                        return FileType::Directory == a.getType() && b.getType() != FileType::Directory;
                    });
                }
            }

        } // namespace FileSystem
    } // namespace Core

    picojson::value toJSON(Core::FileSystem::FileType value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    picojson::value toJSON(Core::FileSystem::DirectoryListSort value)
    {
        std::stringstream ss;
        ss << value;
        return picojson::value(ss.str());
    }

    picojson::value toJSON(const Core::FileSystem::FileInfo& value)
    {
        picojson::value out(picojson::object_type, true);
        out.get<picojson::object>()["Path"] = toJSON(value.getPath());
        out.get<picojson::object>()["Type"] = toJSON(value.getType());
        return out;
    }

    void fromJSON(const picojson::value& value, Core::FileSystem::FileType& out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

    void fromJSON(const picojson::value& value, Core::FileSystem::DirectoryListSort& out)
    {
        if (value.is<std::string>())
        {
            std::stringstream ss(value.get<std::string>());
            ss >> out;
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

    void fromJSON(const picojson::value& value, Core::FileSystem::FileInfo& out)
    {
        if (value.is<picojson::object>())
        {
            Core::FileSystem::Path path;
            Core::FileSystem::FileType type = Core::FileSystem::FileType::First;
            for (const auto& i : value.get<picojson::object>())
            {
                if ("Path" == i.first)
                {
                    fromJSON(i.second, path);
                }
                else if ("Type" == i.first)
                {
                    fromJSON(i.second, type);
                }
            }
            out = Core::FileSystem::FileInfo(path, type);
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

    std::ostream & operator << (std::ostream & s, const Core::FileSystem::FileInfo & value)
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
        DJV_TEXT("Type"));

} // namespace djv
