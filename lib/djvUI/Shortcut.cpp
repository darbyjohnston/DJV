// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Shortcut.h>

#include <djvUI/ShortcutData.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Shortcut::Private
        {
            std::shared_ptr<Observer::ValueSubject<ShortcutData> > shortcut;
            std::function<void(void)> callback;
        };

        void Shortcut::_init()
        {
            DJV_PRIVATE_PTR();
            p.shortcut = Observer::ValueSubject<ShortcutData>::create();
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

        std::shared_ptr<Shortcut> Shortcut::create(const ShortcutData& value)
        {
            auto out = std::shared_ptr<Shortcut>(new Shortcut);
            out->_init();
            out->setShortcut(value);
            return out;
        }

        std::shared_ptr<Shortcut> Shortcut::create(int key)
        {
            auto out = std::shared_ptr<Shortcut>(new Shortcut);
            out->_init();
            out->setShortcut(ShortcutData(key));
            return out;
        }

        std::shared_ptr<Shortcut> Shortcut::create(int key, int modifiers)
        {
            auto out = std::shared_ptr<Shortcut>(new Shortcut);
            out->_init();
            out->setShortcut(ShortcutData(key, modifiers));
            return out;
        }

        std::shared_ptr<Observer::IValueSubject<ShortcutData> > Shortcut::observeShortcut() const
        {
            return _p->shortcut;
        }

        void Shortcut::setShortcut(const ShortcutData& value)
        {
            _p->shortcut->setIfChanged(value);
        }

        void Shortcut::setCallback(const std::function<void(void)>& value)
        {
            _p->callback = value;
        }

        void Shortcut::doCallback()
        {
            DJV_PRIVATE_PTR();
            if (p.callback)
            {
                p.callback();
            }
        }
        
    } // namespace UI
} // namespace djv
