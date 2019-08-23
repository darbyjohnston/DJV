//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include <djvCore/ISystem.h>
#include <djvCore/ValueObserver.h>

#include <future>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class Path;
        
        } // namespace FileSystem

        //! This class provides text and translations.
        //!
        //! The current locale is determined in this order:
        //! - DJV_LANG environment variable
        //! - std::locale("")
        class TextSystem : public ISystemBase
        {
            DJV_NON_COPYABLE(TextSystem);

        protected:
            void _init(const FileSystem::Path &, const std::shared_ptr<Context>&);
            TextSystem();

        public:
            virtual ~TextSystem();
            
            //! Create a new text system.
            static std::shared_ptr<TextSystem> create(const FileSystem::Path &, const std::shared_ptr<Context>&);

            //! \name Language Locale
            ///@{

            //! Get the list of locales.
            const std::vector<std::string> & getLocales() const;

            //! Get the current locale.
            const std::string & getCurrentLocale() const;

            //! Observe the current locale.
            std::shared_ptr<IValueSubject<std::string> > observeCurrentLocale() const;

            //! Set the current locale.
            void setCurrentLocale(const std::string &);

            ///@}

            //! \name Text
            ///@{

            //! Get the text for the given ID.
            //!
            //! \todo Add a namespace argument.
            const std::string & getText(const std::string & id) const;

            ///@}

        private:
            void _readText(const FileSystem::Path &);

            DJV_PRIVATE();
        };

    } // namespace Core
} // namespace djv

