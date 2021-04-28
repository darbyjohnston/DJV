// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <memory>
#include <string>
#include <vector>

namespace djv
{
    namespace System
    {
        class Context;
        
        namespace File
        {
            class Path;
            
        } // namespace File
    } // namespace System

    namespace Test
    {
        //! Base class for confidence tests.
        class ITest
        {
        public:
            ITest(
                const std::string& name,
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            virtual ~ITest() = 0;

            const std::weak_ptr<System::Context>& getContext() const;
            const std::string& getName() const;
            const System::File::Path& getTempPath() const;
            
            virtual void run() = 0;

        protected:
            std::string _getText(const std::string&) const;
            void _print(const std::string&);

        private:
            DJV_PRIVATE();
        };
        
    } // namespace Test
} // namespace djv

