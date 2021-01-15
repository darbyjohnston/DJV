// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2018-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/FileInfoFunc.h>

#include <djvSystem/FileInfoPrivate.h>

#include <djvCore/StringFunc.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace File
        {
            std::vector<Info> directoryList(const Path& value, const DirectoryListOptions& options)
            {
                std::vector<Info> out;
                
                // List the directory contents.
                if (auto dir = opendir(value.get().c_str()))
                {
                    dirent* de = nullptr;
                    while ((de = readdir(dir)))
                    {
                        Info info(Path(value, de->d_name));
                        
                        const std::string fileName = info.getFileName(-1, false);
                        
                        bool filter = false;
                        
                        // Filter hidden items.
                        if (!filter && fileName.size() > 0 &&
                            '.' == fileName[0])
                        {
                            filter = !options.showHidden;
                        }
                        
                        // Filter "." and ".." items.
                        if (!filter &&
                            fileName.size() == 1 &&
                            '.' == fileName[0])
                        {
                            filter = true;
                        }
                        if (!filter &&
                            fileName.size() == 2 &&
                            '.' == fileName[0] &&
                            '.' == fileName[1])
                        {
                            filter = true;
                        }
                        
                        // Filter string matches.
                        if (!filter &&
                            options.filter.size() &&
                            !String::match(fileName, options.filter))
                        {
                            filter = true;
                        }
                        
                        // Filter file extensions.
                        if (!filter &&
                            info.getType() != System::File::Type::Directory &&
                            options.extensions.size())
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
                            sequence(info, options, out);
                        }
                    }
                    closedir(dir);
                }
                    
                // Sort the items.
                sort(options, out);
                
                return out;
            }

        } // namespace File
    } // namespace System
} // namespace djv

