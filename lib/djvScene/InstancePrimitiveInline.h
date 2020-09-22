// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene
    {

        inline const std::vector<std::shared_ptr<IPrimitive> >& InstancePrimitive::getInstances() const
        {
            return _instances;
        }

        inline void InstancePrimitive::setInstances(const std::vector<std::shared_ptr<IPrimitive> >& value)
        {
            _instances = value;
        }

        inline void InstancePrimitive::addInstance(const std::shared_ptr<IPrimitive>& value)
        {
            _instances.push_back(value);
        }

        inline std::string InstancePrimitive::getClassName() const
        {
            return "InstancePrimitive";
        }

        inline const std::vector<std::shared_ptr<IPrimitive> >& InstancePrimitive::getPrimitives() const
        {
            return _instances;
        }

    } // namespace Scene
} // namespace djv

