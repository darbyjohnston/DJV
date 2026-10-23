// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2026 Darby Johnston
// All rights reserved.

#include <djv/App/RecentLabels.h>

#include <filesystem>
#include <map>

namespace djv
{
    namespace app
    {
        std::vector<std::string> getRecentLabels(const std::vector<ftk::Path>& value)
        {
            std::vector<std::string> names;
            std::map<std::string, size_t> counts;
            for (const auto& path : value)
            {
                const std::string name = std::filesystem::path(path.get()).filename().string();
                names.push_back(name);
                ++counts[name];
            }
            std::vector<std::string> out;
            for (size_t i = 0; i < value.size(); ++i)
            {
                std::string label = names[i];
                if (counts[names[i]] > 1)
                {
                    const std::string dir = std::filesystem::path(value[i].get()).parent_path().filename().string();
                    if (!dir.empty())
                    {
                        label = dir + "/" + label;
                    }
                }
                out.push_back(label);
            }
            return out;
        }
    }
}
