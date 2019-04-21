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
                Context * context = nullptr;
                std::string name;
            };

            void ISettings::_init(const std::string & name, Context * context)
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

            Context * ISettings::getContext() const
            {
                return _p->context;
            }

            const std::string & ISettings::getName() const
            {
                return _p->name;
            }

            void ISettings::_load()
            {
                if (auto system = _p->context->getSystemT<System>())
                {
                    system->_loadSettings(shared_from_this());
                }
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv
