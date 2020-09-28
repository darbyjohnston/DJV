// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene3D/Group.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene3D
    {
        Group::Group()
        {}

        std::shared_ptr<Group> Group::create()
        {
            auto out = std::shared_ptr<Group>(new Group);
            return out;
        }

        std::string Group::getClassName() const
        {
            return "Group";
        }

    } // namespace Scene3D
} // namespace djv

