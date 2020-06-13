// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        inline ISystemBase::ISystemBase()
        {}

        inline const std::string& ISystemBase::getSystemName() const
        {
            return _name;
        }

        inline const std::weak_ptr<Context>& ISystemBase::getContext() const
        {
            return _context;
        }

        inline const std::vector<std::shared_ptr<ISystemBase> >& ISystemBase::getDependencies() const
        {
            return _dependencies;
        }

        inline ISystem::ISystem()
        {}

    } // namespace Core
} // namespace djv

