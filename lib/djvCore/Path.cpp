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

#include <djvCore/Path.h>

#include <djvCore/FileInfo.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>

namespace djv
{
    namespace Core
    {
        Path::Path()
        {}
        
        Path::Path(const std::string& value)
        {
            set(value);
        }

        Path::Path(const Path& value, const std::string& append)
        {
            *this = value;
            this->append(append);
        }

        Path::Path(const std::string& value, const std::string& append)
        {
            set(value);
            this->append(append);
        }

        void Path::set(std::string value)
        {
            if (value == _value)
                return;

#if defined(DJV_PLATFORM_WINDOWS)
            if (!value.empty() && value[value.size() - 1] == ':')
            {
                value += getCurrentPathSeparator();
            }
#endif // DJV_PLATFORM_WINDOWS

            _value = value;

            split(_value, _directoryName, _baseName, _number, _extension);

            _fileName = _baseName + _number + _extension;
        }

        void Path::append(const std::string& value)
        {
            const size_t pathSize = _value.size();
            if (value.size() && pathSize && !isPathSeparator(_value[pathSize - 1]))
            {
                set(_value + getCurrentPathSeparator() + value);
            }
            else
            {
                set(_value + value);
            }
        }

        bool Path::isRoot() const
        {
            const bool unixStyle = _value.size() == 1 ? '/' == _value[0] : false;
            //! \todo This is buggy.
            const bool windowsStyle1 = _value.size() == 3 ? (':' == _value[1] && '\\' == _value[2]) : false;
            const bool windowsStyle2 = _value.size() == 3 ? (':' == _value[1] && '/' == _value[2]) : false;
            return unixStyle || windowsStyle1 || windowsStyle2;
        }

        bool Path::cdUp()
        {
            auto subDirectories = splitDir(_value);
            if (subDirectories.size() > 1)
            {
                subDirectories.pop_back();
                const auto join = joinDirs(subDirectories);
                set(join);
                return true;
            }
            return false;
        }

        void Path::setDirectoryName(const std::string& value)
        {
            if (value == _directoryName)
                return;

            _directoryName = value;

            _value = _directoryName + _baseName + _number + _extension;
        }

        void Path::setFileName(const std::string& value)
        {
            set(_directoryName + value);
        }

        void Path::setBaseName(const std::string& value)
        {
            if (value == _baseName)
                return;

            _baseName = value;

            _fileName = _baseName + _number + _extension;
            _value = _directoryName + _fileName;
        }

        void Path::setNumber(const std::string& value)
        {
            if (value == _number)
                return;

            _number = value;

            _fileName = _baseName + _number + _extension;
            _value = _directoryName + _fileName;
        }

        void Path::setExtension(const std::string& value)
        {
            if (value == _extension)
                return;

            _extension = value;

            _fileName = _baseName + _number + _extension;
            _value = _directoryName + _fileName;
        }

        void Path::removeTrailingSeparator(std::string& value)
        {
            if (value.size() > 2)
            {
                const auto i = value.end() - 1;
                if (getPathSeparator(PathSeparator::Unix) == *i || getPathSeparator(PathSeparator::Windows) == *i)
                {
                    value.erase(i);
                }
            }
        }

        namespace
        {
            inline bool matchPadding(const std::string& a, const std::string& b)
            {
                return
                    ((a.length() > 1 && '0' == a[0]) || (b.length() > 1 && '0' == b[0])) ?
                    (a.length() == b.length()) :
                    true;
            }

        } // namespace

        void Path::split(
            const std::string& in,
            std::string&       path,
            std::string&       base,
            std::string&       number,
            std::string&       extension)
        {
            path.resize(0);
            base.resize(0);
            number.resize(0);
            extension.resize(0);

            const int length = static_cast<int>(in.length());
            if (!length)
                return;

            // Extension.
            int i = length - 1;
            int tmp = i;
            for (; in[i] != '.' && !isPathSeparator(in[i]) && i > 0; --i)
            {}
            if (i > 0 && '.' == in[i] && !isPathSeparator(in[i - 1]))
            {
                extension = in.substr(i, tmp - i + 1);
                --i;
            }
            else
            {
                i = length - 1;
            }

            // Number.
            if (i >= 0 && isSequence(in[i]))
            {
                tmp = i;
                int separator = -1;
                std::string word;
                for (; i > 0; --i)
                {
                    if (!isSequence(in[i - 1]))
                    {
                        if (separator != -1 && !matchPadding(in.substr(i, separator - i), word))
                        {
                            i = separator + 1;
                            break;
                        }
                        else
                        {
                            word = in.substr(i, -1 == separator ? (tmp - i + 1) : (separator - i));
                            separator = i - 1;
                        }
                    }

                    if (!(isSequence(in[i - 1])))
                        break;
                }

                number = in.substr(i, tmp - i + 1);
                --i;
            }

            // Base.
            if (i >= 0 && !isPathSeparator(in[i]))
            {
                tmp = i;
                for (; i > 0 && !isPathSeparator(in[i - 1]); --i)
                    ;
                base = in.substr(i, tmp - i + 1);
                --i;
            }

            // Path.
            if (i >= 0)
            {
                path = in.substr(0, i + 1);
            }
        }

        std::vector<std::string> Path::splitDir(const std::string& value)
        {
            std::vector<std::string> out;
            
            // Save the root path.
            std::string tmp = value;
            if (tmp.size() && isPathSeparator(tmp[0]))
            {
                out.push_back(std::string(1, tmp[0]));
                tmp.erase(tmp.begin());
            }
            
            for (const auto & i : String::split(tmp, { '/', '\\' }))
            {
                out.push_back(i);
            }
            
            return out;
        }

        std::string Path::joinDirs(const std::vector<std::string>& value, char separator)
        {
            std::string out;
            
            // Restore the root path.
            std::vector<std::string> tmp = value;
            if (tmp.size() && tmp[0].size() == 1 && isPathSeparator(tmp[0][0]))
            {
                out += std::string(1, getCurrentPathSeparator());
                tmp.erase(tmp.begin());
            }
            
            out += String::join(tmp, separator);
            
            return out;
        }

    } // namespace Core

    std::ostream& operator << (std::ostream& os, const Core::Path& value)
    {
        os << value.get();
        return os;
    }
    
    std::istream& operator >> (std::istream& is, Core::Path& value)
    {
        std::string s;
        is >> s;
        value = Core::Path(s);
        return is;
    }

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core,
        ResourcePath,
        DJV_TEXT("Application"),
        DJV_TEXT("Log File"),
        DJV_TEXT("Settings File"),
        DJV_TEXT("Audio Directory"),
        DJV_TEXT("Fonts Directory"),
        DJV_TEXT("Icons Directory"),
        DJV_TEXT("Images Directory"),
        DJV_TEXT("Models Directory"),
        DJV_TEXT("Shaders Directory"),
        DJV_TEXT("Text Directory"));

} // namespace djv

