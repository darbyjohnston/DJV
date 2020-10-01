// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace System
    {
        inline const std::weak_ptr<Context>& ISystemBase::getContext() const
        {
            return _context;
        }

        inline const std::string& ISystemBase::getSystemName() const
        {
            return _name;
        }

        inline const std::vector<std::shared_ptr<ISystemBase> >& ISystemBase::getDependencies() const
        {
            return _dependencies;
        }

        inline std::shared_ptr<TextSystem> ISystem::_getTextSystem() const
        {
            return _textSystem;
        }

        inline std::shared_ptr<ResourceSystem> ISystem::_getResourceSystem() const
        {
            return _resourceSystem;
        }

    } // namespace System
} // namespace djv
