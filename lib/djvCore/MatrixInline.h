// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    inline std::ostream & operator << (std::ostream & s, const glm::mat4x4 & value)
    {
        for (glm::mat4x4::length_type i = 0; i < 4; ++i)
        {
            for (glm::mat4x4::length_type j = 0; j < 4; ++j)
            {
                s << value[i][j];
                if (j < 4)
                {
                    s << " ";
                }
            }
            if (i < 4)
            {
                s << " ";
            }
        }
        return s;
    }

    inline std::istream & operator >> (std::istream & s, glm::mat4x4 & out)
    {
        s.exceptions(std::istream::failbit | std::istream::badbit);
        for (glm::mat4x4::length_type i = 0; i < 4; ++i)
        {
            for (glm::mat4x4::length_type j = 0; j < 4; ++j)
            {
                s >> out[i][j];
            }
        }
        return s;
    }
    
} // namespace djv
