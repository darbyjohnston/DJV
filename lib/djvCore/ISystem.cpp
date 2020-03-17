//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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
#include <djvCore/ResourceSystem.h>
#include <djvCore/TextSystem.h>

namespace djv
{
    namespace Core
    {
        namespace
        {
            size_t systemCount = 0;

        } // namespace

        void ISystemBase::_init(const std::string & name, const std::shared_ptr<Context>& context)
        {
            ++systemCount;
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
            --systemCount;
        }

        void ISystemBase::addDependency(const std::shared_ptr<ISystemBase> & value)
        {
            _dependencies.push_back(value);
        }
        
        void ISystemBase::tick(const std::chrono::steady_clock::time_point&, const Time::Duration&)
        {
            // Default implementation does nothing.
        }

        void ISystem::_init(const std::string & name, const std::shared_ptr<Context>& context)
        {
            ISystemBase::_init(name, context);
            _logSystem = context->getSystemT<LogSystem>();
            _resourceSystem = context->getSystemT<ResourceSystem>();
            _textSystem = context->getSystemT<TextSystem>();
            {
                std::stringstream ss;
                ss << name << " starting...";
                _log(ss.str());
            }
        }

        ISystem::~ISystem()
        {
            {
                std::stringstream ss;
                ss << getSystemName() << " exiting...";
                _log(ss.str());
            }
        }

        void ISystem::_log(const std::string & message, LogLevel level)
        {
            _logSystem->log(getSystemName(), message, level);
        }

        std::shared_ptr<ResourceSystem> ISystem::_getResourceSystem() const
        {
            return _resourceSystem;
        }

        const std::string &ISystem::_getText(const std::string & id) const
        {
            return _textSystem->getText(id);
        }

    } // namespace Core
} // namespace djv

