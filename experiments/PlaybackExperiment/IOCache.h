// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "IO.h"

#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

namespace IO
{
    class Cache : public std::enable_shared_from_this<Cache>
    {
        DJV_NON_COPYABLE(Cache);

    protected:
        void _init(const std::shared_ptr<djv::System::Context>&);
        Cache();

    public:
        ~Cache();

        static std::shared_ptr<Cache> create(const std::shared_ptr<djv::System::Context>&);

        std::shared_ptr<djv::Core::Observer::IValueSubject<bool> > observeEnabled() const;
        std::shared_ptr<djv::Core::Observer::IValueSubject<size_t> > observeMaxByteCount() const;
        std::shared_ptr<djv::Core::Observer::IValueSubject<size_t> > observeByteCount() const;
        std::shared_ptr<djv::Core::Observer::IListSubject<Timestamp> > observeFrames() const;

        void setEnabled(bool);
        void setMaxByteCount(size_t);

    private:
        DJV_PRIVATE();
    };

} // namespace
