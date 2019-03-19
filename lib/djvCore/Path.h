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

#pragma once

#include <djvCore/Enum.h>
#include <djvCore/PicoJSON.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            //! This enumeration provides the OS specific path separators.
            enum class PathSeparator
            {
                Unix,
                Windows
            };

            //! This enumeration provides resource paths.
            enum class ResourcePath
            {
                Application,
                LogFile,
                SettingsFile,
                AudioDirectory,
                FontsDirectory,
                IconsDirectory,
                ImagesDirectory,
                ModelsDirectory,
                ShadersDirectory,
                TextDirectory,

                Count,
                First = Application
            };
            DJV_ENUM_HELPERS(ResourcePath);

            //! This class provides a file system path.
            //!
            //! Paths are broken into components that represent the directory name,
            //! file name, base name, number, and extension.
            //!
            //! Example path: <code>/scratch/render.exr</code>
            //!
            //! - directory name = /scratch/
            //! - file name      = render.exr
            //! - base name      = render
            //! - number         =
            //! - frame padding  = 0
            //! - extension      = .exr
            //!
            //! Example path: <code>/scratch/render.1-100.exr</code>
            //!
            //! - directory name = /scratch/
            //! - file name      = render.1-100.exr
            //! - base name      = render.
            //! - number         = 1-100
            //! - frame padding  = 0
            //! - extension      = .exr
            //!
            //! Example path: <code>/scratch/render.0001-0100.exr</code>
            //!
            //! - directory name = /scratch/
            //! - file name      = render.0001-0100.exr
            //! - base name      = render.
            //! - number         = 0001-0100
            //! - frame padding  = 4
            //! - extension      = .exr
            //!
            class Path
            {
            public:
                Path();
                Path(const std::string &);
                Path(const Path &, const std::string &);
                Path(const std::string &, const std::string &);

                //! \name Path
                ///@{

                const std::string & get() const { return _value; }
                void set(std::string);
                void append(const std::string &);

                bool isEmpty() const { return _value.empty(); }
                bool isRoot() const;

                //! Go up a directory.
                bool cdUp();

                ///@}

                //! \name Path Components
                ///@{

                const std::string & getDirectoryName() const { return _directoryName; }
                const std::string & getFileName() const { return _fileName; }
                const std::string & getBaseName() const { return _baseName; }
                const std::string & getNumber() const { return _number; }
                const std::string & getExtension() const { return _extension; }
                void setDirectoryName(const std::string &);
                void setFileName(const std::string &);
                void setBaseName(const std::string &);
                void setNumber(const std::string &);
                void setExtension(const std::string &);

                ///@}

                //! \name Utilities
                ///@{

                inline static bool isPathSeparator(char);
                inline static bool isDigit(char);
                inline static bool isSequence(char);
                inline static bool isSequenceSeparator(char);

                //! Get the path separator.
                inline static char getPathSeparator(PathSeparator);

                //! Get the current path separator.
                inline static char getCurrentPathSeparator();

                //! Remove a trailing path separator.
                static void removeTrailingSeparator(std::string &);

                //! Split a path into components.
                static void split(
                    const std::string & in,
                    std::string &       directoryName,
                    std::string &       baseName,
                    std::string &       number,
                    std::string &       extension);

                //! Split a directory path into sub-directories.
                //! For example: "var/tmp" -> { "var", "tmp" }
                static std::vector<std::string> splitDir(const std::string &);

                //! Join a list of sub-directories into a directory path.
                //! For example: var, tmp -> var/tmp
                static std::string joinDirs(const std::vector<std::string>&, char separator = getCurrentPathSeparator());

                //! Create a directory.
                //! Throws:
                //! - std::exception
                static void mkdir(const Path &);

                //! Get the absolute path.
                //! Throws:
                //! - std::exception
                static Path getAbsolute(const Path &);

                //! Get the current working directory path.
                //! Throws:
                //! - std::exception
                static Path getCWD();

                //! Get the temp file path.
                //! Throws:
                //! - std::exception
                static Path getTemp();

                ///@}

                inline bool operator == (const Path &) const;
                inline bool operator != (const Path &) const;
                inline bool operator < (const Path &) const;

                inline operator std::string() const;

            private:
                std::string _value;
                std::string _directoryName;
                std::string _fileName;
                std::string _baseName;
                std::string _number;
                std::string _extension;
            };

        } // namespace FileSystem
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::FileSystem::ResourcePath);

    picojson::value toJSON(const Core::FileSystem::Path &);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value &, Core::FileSystem::Path &);

    std::ostream & operator << (std::ostream &, const Core::FileSystem::Path &);
    std::istream & operator >> (std::istream &, Core::FileSystem::Path &);

} // namespace djv

namespace std
{
    template<>
    struct hash<djv::Core::FileSystem::Path>
    {
        inline std::size_t operator() (const djv::Core::FileSystem::Path &) const noexcept;
    };

} // namespace std

#include <djvCore/PathInline.h>

