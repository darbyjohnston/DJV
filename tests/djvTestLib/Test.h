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
    namespace Core
    {
        class Context;
        
    } // namespace Core

    //! This namespace provides confidence test functionality.
    namespace Test
    {
        //! This class provides an interface for confidence tests.
        class ITest
        {
        public:
            ITest(const std::string& name, const std::shared_ptr<Core::Context>&);
            virtual ~ITest() = 0;

            const std::weak_ptr<Core::Context>& getContext() const;
            const std::string& getName() const;
            
            virtual void run() = 0;

        protected:
            std::string _getText(const std::string&) const;
            void _print(const std::string&);

        private:
            DJV_PRIVATE();
        };
        
    } // namespace Test
} // namespace djv

