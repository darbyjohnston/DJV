// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace System
    {
        template<typename T>
        inline std::vector<std::shared_ptr<T> > Context::getSystemsT() const
        {
            std::vector<std::shared_ptr<T> > out;
            for (const auto& i : _systems)
            {
                if (auto system = std::dynamic_pointer_cast<T>(i))
                {
                    out.push_back(system);
                }
            }
            return out;
        }

        template<typename T>
        inline std::shared_ptr<T> Context::getSystemT() const
        {
            std::shared_ptr<T> out;
            for (const auto& i : _systems)
            {
                if (auto system = std::dynamic_pointer_cast<T>(i))
                {
                    out = system;
                    break;
                }
            }
            return out;
        }

    } // namespace System
} // namespace djv

