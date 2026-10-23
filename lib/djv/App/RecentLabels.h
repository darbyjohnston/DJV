// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2026 Darby Johnston
// All rights reserved.

#pragma once

#include <djv/App/Export.h>

#include <ftk/Core/Path.h>

#include <string>
#include <vector>

namespace djv
{
    namespace app
    {
        //! Labels for a menu of recent files: the file names, since a path
        //! is long, and on a tablet it is a container or a share's mount
        //! that means nothing to anyone; the whole path belongs in the
        //! tooltip. A name that appears twice gets its directory's name
        //! in front, so that two shots' "plate.mov" can be told apart.
        DJV_APP_API std::vector<std::string> getRecentLabels(
            const std::vector<ftk::Path>&);
    }
}
