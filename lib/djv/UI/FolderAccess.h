// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the DJV project.

#pragma once

#include <djv/UI/Export.h>

#include <ftk/Core/Context.h>

#include <filesystem>
#include <functional>
#include <memory>
#include <string>

namespace ftk
{
    class IWindow;
}

namespace djv
{
    namespace ui
    {
        //! Whether the folder holding the file can be read. On a tablet a
        //! file picked in place is readable and nothing beside it, and a
        //! file that refers to others by path needs the folder's grant.
        DJV_UI_API bool hasFolderAccess(const std::filesystem::path& file);

        //! Ask for the folder holding the file where it cannot be read: a
        //! question with the file's name for "{0}", and then the system's
        //! picker for folders, whose grant covers what is beside the file
        //! and is kept as a bookmark. The continuation runs after, whether
        //! the folder was picked or not, and at once where the folder can
        //! be read already or there is no such grant to ask for.
        DJV_UI_API void requestFolderAccess(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<ftk::IWindow>&,
            const std::filesystem::path& file,
            const std::string& question,
            const std::function<void(void)>& next);
    }
}
