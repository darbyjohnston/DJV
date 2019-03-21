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

#include <djvCore/ISystem.h>

#include <djvCore/Context.h>
#include <djvCore/LogSystem.h>

namespace djv
{
    namespace Core
    {
        void ISystemBase::_init(const std::string & name, Context * context)
        {
            _name = name;
            _context = context;
            context->_addSystem(std::dynamic_pointer_cast<ISystemBase>(shared_from_this()));
        }

        ISystemBase::~ISystemBase()
        {
            while (_dependencies.size())
            {
                _dependencies.pop_back();
            }
        }

        void ISystemBase::addDependency(const std::shared_ptr<ISystemBase> & value)
        {
            _dependencies.push_back(value);
        }

        namespace
        {
            size_t systemCount = 0;

        } // namespace

        std::shared_ptr<LogSystem> ISystem::_logSystem;

        void ISystem::_init(const std::string & name, Context * context)
        {
            ISystemBase::_init(name, context);
            ++systemCount;
            if (!_logSystem)
            {
                _logSystem = context->getSystemT<LogSystem>();
            }
            {
                std::stringstream s;
                s << name << " starting...";
                _log(s.str());
            }
        }

        ISystem::~ISystem()
        {
            {
                std::stringstream s;
                s << getSystemName() << " exiting...";
                _log(s.str());
            }
            --systemCount;
            if (!systemCount)
            {
                _logSystem.reset();
            }
        }

        void ISystem::_log(const std::string & message, LogLevel level)
        {
            _logSystem->log(getSystemName(), message, level);
        }

    } // namespace Core
} // namespace djv

