// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ISettings.h>

#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            struct ISettings::Private
            {
                std::weak_ptr<Context> context;
                std::string name;
            };

            void ISettings::_init(const std::string& name, const std::shared_ptr<Core::Context>& context)
            {
                DJV_PRIVATE_PTR();
                p.context = context;
                p.name = name;

                if (auto system = context->getSystemT<System>())
                {
                    system->_addSettings(shared_from_this());
                }
            }

            ISettings::ISettings() :
                _p(new Private)
            {}

            ISettings::~ISettings()
            {}

            const std::weak_ptr<Core::Context>& ISettings::getContext() const
            {
                return _p->context;
            }

            const std::string& ISettings::getName() const
            {
                return _p->name;
            }

            void ISettings::_load()
            {
                if (auto context = _p->context.lock())
                {
                    if (auto system = context->getSystemT<System>())
                    {
                        system->_loadSettings(shared_from_this());
                    }
                }
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv
