// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/UID.h>

#include <atomic>

namespace djv
{
    namespace Core
    {
        uint64_t createUID()
        {
            //! \bug Is this static OK?
            static std::atomic<uint64_t> uid(0);
            ++uid;
            return uid;
        }

    } // namespace Core
} // namespace djv
