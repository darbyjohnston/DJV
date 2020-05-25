// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
                Documents,
                LogFile,
                SettingsFile,
                Audio,
                Fonts,
                Icons,
                Images,
                Models,
                Shaders,
                Text,
                Color,
                Documentation,

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
                explicit Path(const std::string &);
                Path(const Path &, const std::string &);
                Path(const std::string &, const std::string &);

                //! \name Path
                ///@{

                std::string get() const;
                void set(std::string);
                void append(const std::string &, char separator = getCurrentSeparator());

                //! Is the path empty?
                bool isEmpty() const;

                //! Is this a root path?
                bool isRoot() const;

                //! Is the path a server name? (e.g., "//server"; this is Windows specific.)
                bool isServer() const;

                //! Go up a directory.
                bool cdUp(char separator = getCurrentSeparator());

                ///@}

                //! \name Path Components
                ///@{

                const std::string& getDirectoryName() const;
                std::string getFileName() const;
                const std::string& getBaseName() const;
                const std::string& getNumber() const;
                const std::string& getExtension() const;

                void setDirectoryName(const std::string &);
                void setFileName(const std::string &);
                void setBaseName(const std::string &);
                void setNumber(const std::string &);
                void setExtension(const std::string &);

                ///@}

                //! \name Utilities
                ///@{

                //! Is this character a path separator?
                static bool isSeparator(char);

                //! Get the path separator.
                static char getSeparator(PathSeparator);

                //! Get the current path separator.
                static char getCurrentSeparator();

                //! Remove a trailing path separator.
                static void removeTrailingSeparator(std::string&);

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
                static std::string joinDirs(const std::vector<std::string> &, char separator = getCurrentSeparator());

                //! Create a directory.
                //! Throws:
                //! - std::exception
                static void mkdir(const Path&);

                //! Remove a directory. The directory must be empty.
                //! Throws:
                //! - std::exception
                static void rmdir(const Path&);

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

                bool operator == (const Path &) const;
                bool operator != (const Path &) const;
                bool operator < (const Path &) const;

                explicit operator std::string() const;

            private:
                std::string _directoryName;
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

} // namespace djv

namespace std
{
    template<>
    struct hash<djv::Core::FileSystem::Path>
    {
        std::size_t operator() (const djv::Core::FileSystem::Path &) const noexcept;
    };

} // namespace std

#include <djvCore/PathInline.h>

