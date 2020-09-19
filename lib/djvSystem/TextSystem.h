// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/ISystem.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            class Info;
        
        } // namespace File

        //! This class provides text and translations.
        //!
        //! Text files are searched for in this order:
        //! - File::ResourcePath::Text
        //! - File::ResourcePath::Documents
        //! - DJV_TEXT environment variable, a list of colon (Linux/macOS)
        //!   or semicolon (Windows) separated paths to search
        class TextSystem : public ISystemBase
        {
            DJV_NON_COPYABLE(TextSystem);

        protected:
            void _init(const std::shared_ptr<Context>&);
            TextSystem();

        public:
            ~TextSystem() override;
            
            //! Create a new text system.
            static std::shared_ptr<TextSystem> create(const std::shared_ptr<Context>&);

            //! \name Locale
            ///@{

            const std::vector<std::string>& getLocales() const;

            //! Get the system locale which is determined in this order:
            //! - DJV_LANG environment variable
            //! - std::locale("")
            const std::string& getSystemLocale() const;

            std::shared_ptr<Core::IValueSubject<std::string> > observeCurrentLocale() const;
            
            void setCurrentLocale(const std::string&);

            ///@}

            //! \name Text
            ///@{

            //! Get the text for the given ID.
            const std::string& getText(const std::string&);

            //! Get the ID for the given text.
            const std::string& getID(const std::string&);

            //! Observe whether the text has changed.
            std::shared_ptr<Core::IValueSubject<bool> > observeTextChanged() const;

            ///@}

        private:
            DJV_PRIVATE();
        };

    } // namespace System
} // namespace djv

