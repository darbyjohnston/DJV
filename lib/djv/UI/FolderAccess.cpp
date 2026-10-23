// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the DJV project.

#include <djv/UI/FolderAccess.h>

#include <ftk/UI/DialogSystem.h>
#include <ftk/UI/FileBrowser.h>
#include <ftk/Core/Format.h>

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif // __APPLE__

namespace djv
{
    namespace ui
    {
        bool hasFolderAccess(const std::filesystem::path& file)
        {
#if defined(__APPLE__) && TARGET_OS_IPHONE
            // Listing the folder tells: the application's own folders list,
            // and so does one picked before, whose bookmark took the grant
            // up again at the start.
            std::error_code ec;
            std::filesystem::directory_iterator(file.parent_path(), ec);
            return !ec;
#else // TARGET_OS_IPHONE
            return true;
#endif // TARGET_OS_IPHONE
        }

        void requestFolderAccess(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWindow>& window,
            const std::filesystem::path& file,
            const std::string& question,
            const std::function<void(void)>& next)
        {
            if (hasFolderAccess(file) || !window)
            {
                next();
                return;
            }
            auto dialogSystem = context->getSystem<ftk::DialogSystem>();
            dialogSystem->confirm(
                "Media Folder",
                ftk::Format(question).arg(ftk::fromFileSystem(file.filename())),
                window,
                [context, window, file, next](bool value)
                {
                    if (value)
                    {
                        ftk::FileBrowserOpenOptions options;
                        options.title = "Media Folder";
                        options.mode = ftk::FileBrowserMode::Dir;
                        options.path = file.parent_path();
                        auto fileBrowserSystem = context->getSystem<ftk::FileBrowserSystem>();
                        fileBrowserSystem->open(
                            window,
                            [next](const std::vector<ftk::Path>&)
                            {
                                next();
                            },
                            options);
                    }
                    else
                    {
                        next();
                    }
                },
                "Pick...",
                "Skip");
        }
    }
}
