// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene3D/IPrimitive.h>

namespace djv
{
    namespace Scene3D
    {
        //! This class provides a group.
        class Group : public IPrimitive
        {
        protected:
            Group();

        public:
            static std::shared_ptr<Group> create();

            std::string getClassName() const override;

        private:
            std::vector<std::shared_ptr<IPrimitive> > _primitives;
        };

    } // namespace Scene3D
} // namespace djv

