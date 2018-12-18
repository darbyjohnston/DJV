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

#include <djvUI/Shortcut.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Shortcut::Private
        {
            std::shared_ptr<ValueSubject<int> > shortcutKey;
            std::shared_ptr<ValueSubject<int> > shortcutModifiers;
            std::function<void(void)> callback;
        };

        void Shortcut::_init()
        {
            _p->shortcutKey = ValueSubject<int>::create(0);
            _p->shortcutModifiers = ValueSubject<int>::create(0);
        }

        Shortcut::Shortcut() :
            _p(new Private)
        {}

        Shortcut::~Shortcut()
        {}

        std::shared_ptr<Shortcut> Shortcut::create()
        {
            auto out = std::shared_ptr<Shortcut>(new Shortcut);
            out->_init();
            return out;
        }

        std::shared_ptr<Shortcut> Shortcut::create(int key)
        {
            auto out = std::shared_ptr<Shortcut>(new Shortcut);
            out->_init();
            out->setShortcutKey(key);
            return out;
        }

        std::shared_ptr<Shortcut> Shortcut::create(int key, int modifiers)
        {
            auto out = std::shared_ptr<Shortcut>(new Shortcut);
            out->_init();
            out->setShortcutKey(key);
            out->setShortcutModifiers(modifiers);
            return out;
        }

        std::shared_ptr<IValueSubject<int> > Shortcut::getShortcutKey() const
        {
            return _p->shortcutKey;
        }

        void Shortcut::setShortcutKey(int value)
        {
            _p->shortcutKey->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<int> > Shortcut::getShortcutModifiers() const
        {
            return _p->shortcutModifiers;
        }

        void Shortcut::setShortcutModifiers(int value)
        {
            _p->shortcutModifiers->setIfChanged(value);
        }

        void Shortcut::setCallback(const std::function<void(void)>& value)
        {
            _p->callback = value;
        }

        void Shortcut::doCallback()
        {
            if (_p->callback)
            {
                _p->callback();
            }
        }

    } // namespace UI
} // namespace djv
