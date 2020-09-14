// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2018-2020 Darby Johnston
// All rights reserved.

#include <djvCore/FileInfoFunc.h>

#include <djvCore/FileInfoPrivate.h>

#include <djvCore/StringFunc.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            std::vector<FileInfo> directoryList(const Path& value, const DirectoryListOptions& options)
            {
                std::vector<FileInfo> out;
                
                // List the directory contents.
                if (auto dir = opendir(value.get().c_str()))
                {
                    dirent* de = nullptr;
                    while ((de = readdir(dir)))
                    {
                        FileInfo fileInfo(Path(value, de->d_name));
                        
                        const std::string fileName = fileInfo.getFileName(-1, false);
                        
                        bool filter = false;
                        if (fileName.size() > 0 && '.' == fileName[0])
                        {
                            filter = !options.showHidden;
                        }
                        if (fileName.size() == 1 && '.' == fileName[0])
                        {
                            filter = true;
                        }
                        if (fileName.size() == 2 && '.' == fileName[0] && '.' == fileName[1])
                        {
                            filter = true;
                        }
                        if (options.filter.size() && !String::match(fileName, options.filter))
                        {
                            filter = true;
                        }
                        if (!filter && !(de->d_type & DT_DIR) && options.extensions.size())
                        {
                            bool match = false;
                            for (const auto& i : options.extensions)
                            {
                                if (String::match(fileName, '\\' + i + '$'))
                                {
                                    match = true;
                                    break;
                                }
                            }
                            if (!match)
                            {
                                filter = true;
                            }
                        }

                        if (!filter)
                        {
                            fileSequence(fileInfo, options, out);
                        }
                    }
                    closedir(dir);
                }
                    
                // Sort the items.
                sort(options, out);
                
                return out;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

