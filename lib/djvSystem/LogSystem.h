// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/ISystem.h>

#include <djvCore/ListObserver.h>

namespace djv
{
    namespace System
    {
        //! This class provides logging functionality.
        //!
        //! Logging output is written to the given file, and can also be written to
        //! std::cout if the environment variable DJV_LOG_CONSOLE is set to a non-zero
        //! value.
        class LogSystem : public ISystemBase
        {
            DJV_NON_COPYABLE(LogSystem);
            void _init(const std::shared_ptr<Context>&);
            LogSystem();

        public:
            ~LogSystem() override;
            
            //! Create a new logging system.
            static std::shared_ptr<LogSystem> create(const std::shared_ptr<Context>&);
            
            //! Log a message.
            void log(const std::string& prefix, const std::string& message, LogLevel = LogLevel::Information);

            //! \name Warning and Errors
            ///@{

            std::shared_ptr<Core::Observer::IListSubject<std::string> > observeWarnings() const;
            std::shared_ptr<Core::Observer::IListSubject<std::string> > observeErrors() const;

            ///@}

            //! \name Logging Options
            ///@{
            
            bool hasConsoleOutput() const;
            
            void setConsoleOutput(bool);
            
            ///@}

        private:
            void _writeMessages();

            DJV_PRIVATE();
        };

    } // namespace System
} // namespace djv
