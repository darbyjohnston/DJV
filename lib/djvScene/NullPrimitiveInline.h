// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene
    {
        inline NullPrimitive::NullPrimitive()
        {}

        inline std::shared_ptr<NullPrimitive> NullPrimitive::create()
        {
            return std::shared_ptr<NullPrimitive>(new NullPrimitive);
        }

        inline std::string NullPrimitive::getClassName() const
        {
            return "NullPrimitive";
        }

    } // namespace Scene
} // namespace djv

