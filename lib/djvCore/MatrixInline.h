//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvCore/BoxUtil.h>

namespace djv
{
    template<typename T, glm::precision P>
    inline Core::Box2<T, P> operator * (const glm::mat3x3 & a, const Core::Box2<T, P> & b)
    {
        const glm::tvec2<T, P> lowerRight = b.lowerRight();
        glm::tvec3<T, P> pt[] =
        {
            glm::tvec3<T, P>(b.position.x, b.position.y, 1.f),
            glm::tvec3<T, P>(b.position.x, b.position.y, 1.f),
            glm::tvec3<T, P>(lowerRight.x, lowerRight.y, 1.f),
            glm::tvec3<T, P>(b.position.x, b.position.y, 1.f)
        };
        pt[1].y = lowerRight.y;
        pt[3].x = lowerRight.x;
        Core::Box2<T, P> out;
        for (int i = 0; i < 4; ++i)
        {
            pt[i] = a * pt[i];
            if (0 == i)
            {
                out.position.x = pt[i].x;
                out.position.y = pt[i].y;
            }
            else
            {
                out = Core::BoxUtil::expand(out, glm::tvec2<T, P>(pt[i].x, pt[i].y));
            }
        }
        return out;
    }

    inline Core::Debug & operator << (Core::Debug & debug, const glm::mat3x3 & in)
    {
        return debug <<
            in[0][0] << " " << in[0][1] << " " << in[0][2] << "\n" <<
            in[1][0] << " " << in[1][1] << " " << in[1][2] << "\n" <<
            in[2][0] << " " << in[2][1] << " " << in[2][2];
    }

    inline Core::Debug & operator << (Core::Debug & debug, const glm::mat4x4 & in)
    {
        return debug <<
            in[0][0] << " " << in[0][1] << " " << in[0][2] << " " << in[0][3] << "\n" <<
            in[1][0] << " " << in[1][1] << " " << in[1][2] << " " << in[1][3] << "\n" <<
            in[2][0] << " " << in[2][1] << " " << in[2][2] << " " << in[2][3] << "\n" <<
            in[3][0] << " " << in[3][1] << " " << in[3][2] << " " << in[3][3];
    }

} // namespace djv
