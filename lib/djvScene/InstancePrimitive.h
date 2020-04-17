// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene/IPrimitive.h>

namespace djv
{
    namespace Scene
    {
        //! This class provides a primitive instance.
        class InstancePrimitive : public IPrimitive
        {
            DJV_NON_COPYABLE(InstancePrimitive);

        protected:
            InstancePrimitive();

        public:
            static std::shared_ptr<InstancePrimitive> create();

            const std::vector<std::shared_ptr<IPrimitive> >& getInstances() const;
            void setInstances(const std::vector<std::shared_ptr<IPrimitive> >&);
            void addInstance(const std::shared_ptr<IPrimitive>&);

            std::string getClassName() const override;
            const std::vector<std::shared_ptr<IPrimitive> >& getPrimitives() const override;

        private:
            std::vector<std::shared_ptr<IPrimitive> > _instances;
        };

    } // namespace Scene
} // namespace djv

#include <djvScene/InstancePrimitiveInline.h>
