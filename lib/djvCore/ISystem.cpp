// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
        
        void ISystemBase::tick()
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

