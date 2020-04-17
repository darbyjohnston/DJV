// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene/IPrimitive.h>

namespace djv
{
    namespace Scene
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

    } // namespace Scene
} // namespace djv

