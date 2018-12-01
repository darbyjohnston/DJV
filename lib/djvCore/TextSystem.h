//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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
#include <djvCore/ListObserver.h>
#include <djvCore/MapObserver.h>
#include <djvCore/ValueObserver.h>

#include <future>

namespace djv
{
    namespace Core
    {
        //! This class provides text and translations.
        //!
        //! The current locale is determined in this order:
        //! - DJV_LANG environment variable
        //! - std::locale("")
        class TextSystem : public ISystem
        {
            DJV_NON_COPYABLE(TextSystem);
            void _init(const std::shared_ptr<Context>&);
            TextSystem();

        public:
            virtual ~TextSystem();
            
            //! Create a new text system.
            static std::shared_ptr<TextSystem> create(const std::shared_ptr<Context>&);

            //! \name Language Locale
            ///@{

            const std::vector<std::string>& getLocales() const;
            const std::string& getCurrentLocale() const;
            void setCurrentLocale(const std::string&);

            ///@}

            //! \name Text
            ///@{

            //! Get the text for the given ID.
            const std::string& getText(const std::string& id) const;

            ///@}

        protected:
            void _exit() override;

        private:
            void _readText();

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace Core
} // namespace djv

