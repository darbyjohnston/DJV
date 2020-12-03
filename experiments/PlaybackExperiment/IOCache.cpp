// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "IOCache.h"

using namespace djv;

namespace IO
{
    struct Cache::Private
    {
        std::shared_ptr<Core::Observer::ValueSubject<bool> > enabledSubject;
        std::shared_ptr<Core::Observer::ValueSubject<size_t> > maxByteCountSubject;
        std::shared_ptr<Core::Observer::ValueSubject<size_t> > byteCountSubject;
        std::shared_ptr<Core::Observer::ListSubject<Timestamp> > framesSubject;
    };

    void Cache::_init(const std::shared_ptr<System::Context>& context)
    {
        DJV_PRIVATE_PTR();
        p.enabledSubject = Core::Observer::ValueSubject<bool>::create(false);
        p.maxByteCountSubject = Core::Observer::ValueSubject<size_t>::create(0);
        p.byteCountSubject = Core::Observer::ValueSubject<size_t>::create(0);
        p.framesSubject = Core::Observer::ListSubject<Timestamp>::create();
    }

    Cache::Cache() :
        _p(new Private)
    {}

    Cache::~Cache()
    {}

    std::shared_ptr<Cache> Cache::create(const std::shared_ptr<System::Context>& context)
    {
        auto out = std::shared_ptr<Cache>(new Cache);
        out->_init(context);
        return out;
    }

    std::shared_ptr<Core::Observer::IValueSubject<bool> > Cache::observeEnabled() const
    {
        return _p->enabledSubject;
    }

    std::shared_ptr<Core::Observer::IValueSubject<size_t> > Cache::observeMaxByteCount() const
    {
        return _p->maxByteCountSubject;
    }

    std::shared_ptr<Core::Observer::IValueSubject<size_t> > Cache::observeByteCount() const
    {
        return _p->byteCountSubject;
    }

    std::shared_ptr<Core::Observer::IListSubject<Timestamp> > Cache::observeFrames() const
    {
        return _p->framesSubject;
    }

} // namespace IO
