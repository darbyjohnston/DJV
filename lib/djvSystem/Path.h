// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <string>

namespace djv
{
    namespace System
    {
        namespace File
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
            
            //! This enumeration provides directory shortcuts.
            enum class DirectoryShortcut
            {
                Home,
                Desktop,
                Documents,
                Downloads,

                Count,
                First = Home
            };

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
                explicit Path(const std::string&);
                Path(const Path&, const std::string&);
                Path(const std::string&, const std::string&);
                Path(
                    const std::string& directoryName,
                    const std::string& baseName,
                    const std::string& number,
                    const std::string& extension);

                //! \name Path
                ///@{

                std::string get() const;
                
                void set(std::string);

                //! Is this character a path separator?
                static bool isSeparator(char) noexcept;

                //! Get the path separator.
                static char getSeparator(PathSeparator) noexcept;

                //! Get the current path separator.
                static char getCurrentSeparator() noexcept;
                
                void append(const std::string&, char separator = getCurrentSeparator());

                //! Is the path empty?
                bool isEmpty() const noexcept;

                //! Is this a root path?
                bool isRoot() const;

                //! Is the path a server name? (e.g., "//server"; this is Windows specific.)
                bool isServer() const;

                //! Go up a directory.
                bool cdUp(char separator = getCurrentSeparator());

                ///@}

                //! \name Components
                ///@{

                const std::string& getDirectoryName() const noexcept;
                std::string getFileName() const;
                const std::string& getBaseName() const noexcept;
                const std::string& getNumber() const noexcept;
                const std::string& getExtension() const noexcept;

                void setDirectoryName(const std::string&);
                void setFileName(const std::string&);
                void setBaseName(const std::string&);
                void setNumber(const std::string&);
                void setExtension(const std::string&);

                ///@}

                bool operator == (const Path&) const;
                bool operator != (const Path&) const;
                bool operator < (const Path&) const;

                explicit operator std::string() const;

            private:
                std::string _directoryName;
                std::string _baseName;
                std::string _number;
                std::string _extension;
            };

        } // namespace File
    } // namespace System
} // namespace djv

#include <djvSystem/PathInline.h>
