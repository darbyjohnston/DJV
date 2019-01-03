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

#include <djvUI/Enum.h>

#include <djvCore/ValueObserver.h>

#include <memory>

namespace djv
{
    namespace Core
    {
        class TextSystem;
    
    } // namespace Core

    namespace UI
    {
        //! This class provides a keyboard shortcut.
        //!
        //! \todo [1.0 S] Use the command key modifier on OSX.
        class Shortcut : public std::enable_shared_from_this<Shortcut>
        {
            DJV_NON_COPYABLE(Shortcut);

        protected:
            void _init();
            Shortcut();

        public:
            virtual ~Shortcut();

            static std::shared_ptr<Shortcut> create();
            static std::shared_ptr<Shortcut> create(int key);
            static std::shared_ptr<Shortcut> create(int key, int modifiers);

            std::shared_ptr<Core::IValueSubject<int> > getShortcutKey() const;
            void setShortcutKey(int key);

            std::shared_ptr<Core::IValueSubject<int> > getShortcutModifiers() const;
            void setShortcutModifiers(int);

            void setCallback(const std::function<void(void)>&);
            void doCallback();

            static const std::string & getKeyString(int);
            static const std::string & getModifierString(int);
            static std::string getText(int key, int keyModifiers, const std::shared_ptr<Core::TextSystem> &);

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
