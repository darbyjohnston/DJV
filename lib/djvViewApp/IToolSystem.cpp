// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/IToolSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct IToolSystem::Private
        {
        };

        void IToolSystem::_init(const std::string & name, const std::shared_ptr<System::Context>& context)
        {
            IViewSystem::_init(name, context);
        }
            
        IToolSystem::IToolSystem() :
            _p(new Private)
        {}

        IToolSystem::~IToolSystem()
        {}

    } // namespace ViewApp
} // namespace djv
