// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/ISystem.h>
#include <djvSystem/Path.h>

namespace djv
{
    namespace System
    {
        //! Resource system.
        //!
        //! By default the application root is used for bundled resources. This
        //! may be overridden with the DJV_RESOURCE_PATH environment variable.
        //!
        //! By default log files and settings are written to "$HOME/Documents/DJV".
        //! This may be overridden with the DJV_DOCUMENTS_PATH environment variable.
        class ResourceSystem : public ISystemBase
        {
            DJV_NON_COPYABLE(ResourceSystem);
            void _init(const std::string& argv0, const std::shared_ptr<Context>&);
            ResourceSystem();
            
        public:
            ~ResourceSystem() override;

            // Create a new resource system.
            static std::shared_ptr<ResourceSystem> create(const std::string& argv0, const std::shared_ptr<Context>&);

            //! Get a resource path.
            File::Path getPath(File::ResourcePath) const;

        private:
            DJV_PRIVATE();
        };

    } // namespace System
} // namespace djv

