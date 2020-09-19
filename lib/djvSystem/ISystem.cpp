// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/ISystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/ResourceSystem.h>
#include <djvSystem/TextSystem.h>

#include <sstream>

namespace djv
{
    namespace System
    {
        namespace
        {
            size_t systemCount = 0;

        } // namespace

        void ISystemBase::_init(const std::string& name, const std::shared_ptr<Context>& context)
        {
            ++systemCount;
            _name = name;
            _context = context;
            context->_addSystem(std::dynamic_pointer_cast<ISystemBase>(shared_from_this()));
        }
        
        ISystemBase::ISystemBase()
        {}

        ISystemBase::~ISystemBase()
        {
            while (_dependencies.size())
            {
                _dependencies.pop_back();
            }
            --systemCount;
        }

        const std::string& ISystemBase::getSystemName() const
        {
            return _name;
        }

        const std::weak_ptr<Context>& ISystemBase::getContext() const
        {
            return _context;
        }

        const std::vector<std::shared_ptr<ISystemBase> >& ISystemBase::getDependencies() const
        {
            return _dependencies;
        }

        void ISystemBase::addDependency(const std::shared_ptr<ISystemBase>& value)
        {
            _dependencies.push_back(value);
        }
        
        void ISystemBase::tick()
        {
            // Default implementation does nothing.
        }

        void ISystem::_init(const std::string& name, const std::shared_ptr<Context>& context)
        {
            ISystemBase::_init(name, context);
            _textSystem = context->getSystemT<TextSystem>();
            _resourceSystem = context->getSystemT<ResourceSystem>();
            _logSystem = context->getSystemT<LogSystem>();
            {
                std::stringstream ss;
                ss << name << " starting...";
                _log(ss.str());
            }
        }
        
        ISystem::ISystem()
        {}

        ISystem::~ISystem()
        {
            {
                std::stringstream ss;
                ss << getSystemName() << " exiting...";
                _log(ss.str());
            }
        }

        void ISystem::_log(const std::string& message, LogLevel level)
        {
            _logSystem->log(getSystemName(), message, level);
        }

        const std::string& ISystem::_getText(const std::string& id) const
        {
            return _textSystem->getText(id);
        }

        std::shared_ptr<TextSystem> ISystem::_getTextSystem() const
        {
            return _textSystem;
        }

        std::shared_ptr<ResourceSystem> ISystem::_getResourceSystem() const
        {
            return _resourceSystem;
        }

    } // namespace System
} // namespace djv

