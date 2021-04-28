// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/ListObserver.h>

namespace djv
{
    namespace System
    {
        class Context;
        
        namespace File
        {
            class Path;

            //! File system drive model.
            class DrivesModel : public std::enable_shared_from_this<DrivesModel>
            {
                DJV_NON_COPYABLE(DrivesModel);
                void _init(const std::shared_ptr<Context>&);
                DrivesModel();

            public:
                virtual ~DrivesModel();

                static std::shared_ptr<DrivesModel> create(const std::shared_ptr<Context>&);

                std::shared_ptr<Core::Observer::IListSubject<Path> > observeDrives() const;

            private:
                static std::vector<Path> _getDrives();

                DJV_PRIVATE();
            };

        } // namespace File
    } // namespace System
} // namespace djv
