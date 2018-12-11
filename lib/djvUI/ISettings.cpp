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

#include <djvUI/ISettings.h>

#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>
#include <djvCore/LogSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ISettings::Private
        {
            std::weak_ptr<Context> context;
            std::string name;
        };

        void ISettings::_init(const std::string& name, const std::shared_ptr<Context>& context)
        {
            _p->context = context;
            _p->name = name;

            if (auto system = context->getSystemT<SettingsSystem>())
            {
                system->_addSettings(shared_from_this());
            }
        }

        ISettings::ISettings() :
            _p(new Private)
        {}

        ISettings::~ISettings()
        {}

        const std::weak_ptr<Context> & ISettings::getContext() const
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
                if (auto system = context->getSystemT<SettingsSystem>())
                {
                    system->_loadSettings(shared_from_this());
                }
            }
        }

        void ISettings::_readError(const std::string& value)
        {
            if (auto context = _p->context.lock())
            {
                std::stringstream s;
                s << "Error reading settings: " << _p->name << ": " << value;
                context->getSystemT<Core::LogSystem>()->log("djv::UI::ISettings", s.str(), LogLevel::Error);
            }
        }

    } // namespace UI
} // namespace djv
