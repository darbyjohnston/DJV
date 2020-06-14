// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

#include <djvCore/BBox.h>
#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

#include <map>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class FileInfo;

        } // namespace FileSystem
    } // namespace Core

    namespace ViewApp
    {
        //! This class provides the file settings.
        class FileSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(FileSettings);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);

            FileSettings();

        public:
            virtual ~FileSettings();

            static std::shared_ptr<FileSettings> create(const std::shared_ptr<Core::Context>&);
            
            std::shared_ptr<Core::IValueSubject<size_t> > observeOpenMax() const;
            void setOpenMax(size_t);

            std::shared_ptr<Core::IListSubject<Core::FileSystem::FileInfo> > observeRecentFiles() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeRecentFilesMax() const;
            void setRecentFiles(const std::vector<Core::FileSystem::FileInfo>&);
            void setRecentFilesMax(size_t);

            std::shared_ptr<Core::IValueSubject<bool> > observeAutoDetectSequences() const;
            std::shared_ptr<Core::IValueSubject<bool> > observeSequencesFirstFrame() const;
            void setAutoDetectSequences(bool);
            void setSequencesFirstFrame(bool);

            std::shared_ptr<Core::IValueSubject<bool> > observeCacheEnabled() const;
            std::shared_ptr<Core::IValueSubject<int> > observeCacheMaxGB() const;
            void setCacheEnabled(bool);
            void setCacheMaxGB(int);

            const std::map<std::string, Core::BBox2f>& getWidgetGeom() const;
            void setWidgetGeom(const std::map<std::string, Core::BBox2f>&);

            void load(const rapidjson::Value&) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

