// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

#include <djvCore/MapObserver.h>

namespace djv
{
    namespace UI
    {
        struct ShortcutData;

        namespace Settings
        {
            //! This class provides the keyboard shortcut settings.
            class Shortcuts : public ISettings
            {
                DJV_NON_COPYABLE(Shortcuts);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);

                Shortcuts();

            public:
                virtual ~Shortcuts();

                static std::shared_ptr<Shortcuts> create(const std::shared_ptr<Core::Context>&);

                std::shared_ptr<Core::MapSubject<std::string, std::vector<ShortcutData> > > observeShortcuts() const;
                void setShortcuts(const std::map<std::string, std::vector<ShortcutData> >&);
                void addShortcut(const std::string&, const std::vector<ShortcutData>&, bool overwrite = false);
                void addShortcut(const std::string&, int key, bool overwrite = false);
                void addShortcut(const std::string&, int key, int keyModifiers, bool overwrite = false);

                void load(const rapidjson::Value&) override;
                rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv

