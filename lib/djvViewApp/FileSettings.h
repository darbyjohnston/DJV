// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ISettings.h>

#include <djvMath/BBox.h>
#include <djvCore/ListObserver.h>
#include <djvCore/MapObserver.h>
#include <djvCore/ValueObserver.h>

#include <map>

namespace djv
{
    namespace System
    {
        namespace File
        {
            class Info;

        } // namespace File
    } // namespace System

    namespace ViewApp
    {
        //! This class provides the file settings.
        class FileSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(FileSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>&);

            FileSettings();

        public:
            ~FileSettings() override;

            static std::shared_ptr<FileSettings> create(const std::shared_ptr<System::Context>&);

            //! \name Open Maximum
            ///@{
            
            std::shared_ptr<Core::Observer::IValueSubject<size_t> > observeOpenMax() const;

            void setOpenMax(size_t);

            ///@}

            //! \name Recent Files
            ///@{

            std::shared_ptr<Core::Observer::IListSubject<System::File::Info> > observeRecentFiles() const;
            std::shared_ptr<Core::Observer::IValueSubject<size_t> > observeRecentFilesMax() const;

            void setRecentFiles(const std::vector<System::File::Info>&);
            void setRecentFilesMax(size_t);

            ///@}

            //! \name Sequences
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeAutoDetectSequences() const;
            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeSequencesFirstFrame() const;

            void setAutoDetectSequences(bool);
            void setSequencesFirstFrame(bool);

            ///@}

            //! \name Cache
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeCacheEnabled() const;
            std::shared_ptr<Core::Observer::IValueSubject<int> > observeCacheSize() const;

            void setCacheEnabled(bool);
            void setCacheSize(int);

            ///@}

            void load(const rapidjson::Value&) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

