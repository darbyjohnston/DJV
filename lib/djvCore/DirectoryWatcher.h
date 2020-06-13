// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <functional>
#include <memory>

namespace djv
{
    namespace Core
    {
        class Context;

        namespace FileSystem
        {
            class Path;

            //! This class provides functionality for watching directory changes.
            //!
            //! \bug What do we do about changes to the directory path (like deletion or moving)?
            class DirectoryWatcher : public std::enable_shared_from_this<DirectoryWatcher>
            {
                DJV_NON_COPYABLE(DirectoryWatcher);
                void _init(const std::shared_ptr<Context>&);
                DirectoryWatcher();

            public:
                ~DirectoryWatcher();

                static std::shared_ptr<DirectoryWatcher> create(const std::shared_ptr<Context>&);

                const Path& getPath() const;
                void setPath(const Path&);

                void setCallback(const std::function<void(void)>&);

            private:
                DJV_PRIVATE();
            };

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

