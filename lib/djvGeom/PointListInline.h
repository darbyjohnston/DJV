// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Geom
    {
        inline PointList::PointList() :
            _uid(Core::createUID())
        {}

        inline Core::UID PointList::getUID() const
        {
            return _uid;
        }

    } // namespace Geom
} // namespace djv
