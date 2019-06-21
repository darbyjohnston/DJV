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

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/CoreSystem.h>

#include <OpenColorIO/OpenColorIO.h>

using namespace djv::Core;
namespace OCIO = OCIO_NAMESPACE;

namespace djv
{
    namespace AV
    {
        struct OCIOSystem::Private
        {
            OCIO::ConstConfigRcPtr ocioConfig = nullptr;
        };

        void OCIOSystem::_init(Core::Context * context)
        {
            ISystem::_init("djv::AV::OCIOSystem", context);

            addDependency(context->getSystemT<CoreSystem>());

            DJV_PRIVATE_PTR();
            p.ocioConfig = OCIO::GetCurrentConfig();
            {
                std::stringstream ss;
                ss << "Version: " << OCIO::GetVersion();
                _log(ss.str());
            }
            {
                std::stringstream ss;
                ss << "Config description: " << p.ocioConfig->getDescription();
                _log(ss.str());
            }
            for (int i = 0; i < p.ocioConfig->getNumColorSpaces(); ++i)
            {
                std::stringstream ss;
                ss << "Color space " << i << ": " << p.ocioConfig->getColorSpaceNameByIndex(i);
                _log(ss.str());
            }
            {
                std::stringstream ss;
                ss << "Default display: " << p.ocioConfig->getDefaultDisplay();
                _log(ss.str());
            }
            for (int i = 0; i < p.ocioConfig->getNumDisplays(); ++i)
            {
                std::stringstream ss;
                ss << "Display " << i << ": " << p.ocioConfig->getDisplay(i);
                _log(ss.str());
            }
        }

        OCIOSystem::OCIOSystem() :
            _p(new Private)
        {}

        OCIOSystem::~OCIOSystem()
        {
        }

        std::shared_ptr<OCIOSystem> OCIOSystem::create(Core::Context * context)
        {
            auto out = std::shared_ptr<OCIOSystem>(new OCIOSystem);
            out->_init(context);
            return out;
        }

    } // namespace AV
} // namespace djv

