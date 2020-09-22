// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/Enum.h>

#include <djvCore/Core.h>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace djv
{
    namespace System
    {
        class Context;
        class LogSystem;
        class ResourceSystem;
        class TextSystem;

        //! This class provides the very base functionality for systems.
        class ISystemBase : public std::enable_shared_from_this<ISystemBase>
        {
            DJV_NON_COPYABLE(ISystemBase);
            
        protected:
            void _init(const std::string& name, const std::shared_ptr<Context>&);
            ISystemBase();

        public:
            virtual ~ISystemBase() = 0;

            //! Get the system name.
            const std::string& getSystemName() const;

            //! Get the context.
            const std::weak_ptr<Context>& getContext() const;

            //! Get the list of system dependencies.
            const std::vector<std::shared_ptr<ISystemBase> >& getDependencies() const;
            
            //! Add a system dependency.
            void addDependency(const std::shared_ptr<ISystemBase>&);

            //! Override this function to do work each frame.
            virtual void tick();

        private:
            std::string _name;
            std::weak_ptr<Context> _context;
            std::vector<std::shared_ptr<ISystemBase> > _dependencies;
        };

        //! This class provides the base functionality for systems.
        class ISystem : public ISystemBase
        {
            DJV_NON_COPYABLE(ISystem);

        protected:
            void _init(const std::string& name, const std::shared_ptr<Context>&);
            ISystem();

        public:
            ~ISystem() override;

        protected:
            //! Log a message.
            void _log(const std::string& message, LogLevel = LogLevel::Information);
            
            //! Translate a text ID.
            const std::string& _getText(const std::string& id) const;

            //! Get the text system.
            std::shared_ptr<TextSystem> _getTextSystem() const;
            
            //! Get the resource system.
            std::shared_ptr<ResourceSystem> _getResourceSystem() const;
            
        private:
            std::shared_ptr<TextSystem> _textSystem;
            std::shared_ptr<ResourceSystem> _resourceSystem;
            std::shared_ptr<LogSystem> _logSystem;
        };

    } // namespace System
} // namespace djv

#include <djvSystem/ISystemInline.h>
