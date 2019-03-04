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

#include <djvUIComponents/AVIOSettings.h>

#include <djvAV/IO.h>

#include <djvCore/Context.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            struct AVIO::Private
            {
            };

            void AVIO::_init(Context * context)
            {
                ISettings::_init("djv::UI::Settings::AVIO", context);

                DJV_PRIVATE_PTR();

                _load();

                auto weak = std::weak_ptr<AVIO>(std::dynamic_pointer_cast<AVIO>(shared_from_this()));
            }

            AVIO::AVIO() :
                _p(new Private)
            {}

            AVIO::~AVIO()
            {}

            std::shared_ptr<AVIO> AVIO::create(Context * context)
            {
                auto out = std::shared_ptr<AVIO>(new AVIO);
                out->_init(context);
                return out;
            }

            void AVIO::load(const picojson::value& value)
            {
                DJV_PRIVATE_PTR();
                if (value.is<picojson::object>())
                {
                    if (auto io = getContext()->getSystemT<AV::IO::System>().lock())
                    {
                        const auto & object = value.get<picojson::object>();
                        for (const auto & i : io->getPluginNames())
                        {
                            const auto j = object.find(i);
                            if (j != object.end())
                            {
                                io->setOptions(i, j->second);
                            }
                        }
                    }
                }
            }

            picojson::value AVIO::save()
            {
                DJV_PRIVATE_PTR();
                picojson::value out(picojson::object_type, true);
                if (auto io = getContext()->getSystemT<AV::IO::System>().lock())
                {
                    auto & object = out.get<picojson::object>();
                    for (const auto & i : io->getPluginNames())
                    {
                        object[i] = io->getOptions(i);
                    }
                }
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv

