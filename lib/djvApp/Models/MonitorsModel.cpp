// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <tlPlay/RecentFilesModel.h>

#include <dtk/ui/Settings.h>

namespace tl
{
    namespace play
    {
        struct RecentFilesModel::Private
        {
            std::shared_ptr<dtk::Settings> settings;
        };

        void RecentFilesModel::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<dtk::Settings>& settings)
        {
            dtk::RecentFilesModel::_init(context);
            DTK_P();

            p.settings = settings;

            std::vector<std::filesystem::path> recent;
            nlohmann::json json;
            if (p.settings->get("Files/Recent", json))
            {
                for (auto i = json.begin(); i != json.end(); ++i)
                {
                    if (i->is_string())
                    {
                        recent.push_back(std::filesystem::u8path(i->get<std::string>()));
                    }
                }
            }
            setRecent(recent);
            size_t max = 10;
            p.settings->get("Files/RecentMax", max);
            setRecentMax(max);
        }

        RecentFilesModel::RecentFilesModel() :
            _p(new Private)
        {}

        RecentFilesModel::~RecentFilesModel()
        {
            DTK_P();
            nlohmann::json json;
            for (const auto& path : getRecent())
            {
                json.push_back(path.u8string());
            }
            p.settings->set("Files/Recent", json);
            p.settings->set("Files/RecentMax", getRecentMax());
        }

        std::shared_ptr<RecentFilesModel> RecentFilesModel::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<dtk::Settings>& settings)
        {
            auto out = std::shared_ptr<RecentFilesModel>(new RecentFilesModel);
            out->_init(context, settings);
            return out;
        }
    }
}
