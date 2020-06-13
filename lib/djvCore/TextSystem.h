// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/ISystem.h>
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

        //! This class provides text and translations.
        //!
        //! Text files are searched for in this order:
        //! - FileSystem::ResourcePath::Text
        //! - FileSystem::ResourcePath::Documents
        //! - DJV_TEXT environment variable, a list of colon (Linux/OSX)
        //!   or semicolon (Windows) separated paths to search
        class TextSystem : public ISystemBase
        {
            DJV_NON_COPYABLE(TextSystem);

        protected:
            void _init(const std::shared_ptr<Context>&);
            TextSystem();

        public:
            virtual ~TextSystem();
            
            //! Create a new text system.
            static std::shared_ptr<TextSystem> create(const std::shared_ptr<Context>&);

            //! \name Locale
            ///@{

            const std::vector<std::string>& getLocales() const;

            //! Get the system locale which is determined in this order:
            //! - DJV_LANG environment variable
            //! - std::locale("")
            const std::string& getSystemLocale() const;

            std::shared_ptr<IValueSubject<std::string> > observeCurrentLocale() const;
            void setCurrentLocale(const std::string &);

            ///@}

            //! \name Text
            ///@{

            //! Get the text for the given ID.
            const std::string& getText(const std::string&);

            //! Get the ID for the given text.
            const std::string& getID(const std::string&);

            //! Observe whether the text has changed.
            std::shared_ptr<IValueSubject<bool> > observeTextChanged() const;

            ///@}

        private:
            std::vector<FileSystem::FileInfo> _getTextFiles() const;
            void _reload(const FileSystem::FileInfo&);
            typedef std::map<std::string, std::map<std::string, std::string> > TextMap;
            TextMap _readText(const FileSystem::FileInfo&);
            void _readAllFutures();

            DJV_PRIVATE();
        };

    } // namespace Core
} // namespace djv

