//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <algorithm>

namespace djv
{
    namespace Core
    {
        namespace BBox
        {
            template<>
            constexpr tBBox2<int>::tBBox2() :
                min(0, 0),
                max(0, 0)
            {}

            template<>
            constexpr tBBox2<float>::tBBox2() :
                min(0.f, 0.f),
                max(0.f, 0.f)
            {}

            template<>
            constexpr tBBox3<int>::tBBox3() :
                min(0, 0, 0),
                max(0, 0, 0)
            {}

            template<>
            constexpr tBBox3<float>::tBBox3() :
                min(0.f, 0.f, 0.f),
                max(0.f, 0.f, 0.f)
            {}

            template<typename T, glm::precision P>
            constexpr tBBox2<T, P>::tBBox2(const glm::tvec2<T, P> & value) :
                min(value),
                max(value)
            {}

            template<typename T, glm::precision P>
            constexpr tBBox3<T, P>::tBBox3(const glm::tvec3<T, P> & value) :
                min(value),
                max(value)
            {}

            template<typename T, glm::precision P>
            constexpr tBBox2<T, P>::tBBox2(const glm::tvec2<T, P> & min, const glm::tvec2<T, P> & max) :
                min(min),
                max(max)
            {}

            template<typename T, glm::precision P>
            constexpr tBBox3<T, P>::tBBox3(const glm::tvec3<T, P> & min, const glm::tvec3<T, P> & max) :
                min(min),
                max(max)
            {}

            template<typename T, glm::precision P>
            constexpr tBBox2<T, P>::tBBox2(T x, T y, T w, T h) :
                min(x, y),
                max(x + w, y + h)
            {}

            template<typename T, glm::precision P>
            constexpr tBBox3<T, P>::tBBox3(T x, T y, T z, T w, T h, T d) :
                min(x, y, z),
                max(x + w, y + h, z + d)
            {}

            template<typename T, glm::precision P>
            constexpr T tBBox2<T, P>::x() const { return min.x; }

            template<typename T, glm::precision P>
            constexpr T tBBox3<T, P>::x() const { return min.x; }

            template<typename T, glm::precision P>
            constexpr T tBBox2<T, P>::y() const { return min.y; }

            template<typename T, glm::precision P>
            constexpr T tBBox3<T, P>::y() const { return min.y; }

            template<typename T, glm::precision P>
            constexpr T tBBox3<T, P>::z() const { return min.z; }

            /*template<typename T, glm::precision P>
            constexpr T tBBox2<T, P>::w() const { return max.x - min.x; }

            template<typename T, glm::precision P>
            constexpr T tBBox3<T, P>::w() const { return max.x - min.x; }

            template<typename T, glm::precision P>
            constexpr T tBBox2<T, P>::h() const { return max.y - min.y; }

            template<typename T, glm::precision P>
            constexpr T tBBox3<T, P>::h() const { return max.y - min.y; }

            template<typename T, glm::precision P>
            constexpr T tBBox3<T, P>::d() const { return max.z - min.z; }*/

            template<>
            constexpr float tBBox2<float>::w() const { return max.x - min.x; }
            template<>
            constexpr int tBBox2<int>::w() const { return max.x - min.x + 1; }

            template<>
            constexpr float tBBox3<float>::w() const { return max.x - min.x; }
            template<>
            constexpr int tBBox3<int>::w() const { return max.x - min.x + 1; }

            template<>
            constexpr float tBBox2<float>::h() const { return max.y - min.y; }
            template<>
            constexpr int tBBox2<int>::h() const { return max.y - min.y + 1; }

            template<>
            constexpr float tBBox3<float>::h() const { return max.y - min.y; }
            template<>
            constexpr int tBBox3<int>::h() const { return max.y - min.y + 1; }

            template<>
            constexpr float tBBox3<float>::d() const { return max.z - min.z; }
            template<>
            constexpr int tBBox3<int>::d() const { return max.z - min.z + 1; }

            template<typename T, glm::precision P>
            constexpr bool tBBox2<T, P>::isValid() const
            {
                return
                    min.x < max.x &&
                    min.y < max.y;
            }

            template<typename T, glm::precision P>
            constexpr bool tBBox3<T, P>::isValid() const
            {
                return
                    min.x < max.x &&
                    min.y < max.y &&
                    min.z < max.z;
            }

            /*template<typename T, glm::precision P>
            constexpr glm::tvec2<T, P> tBBox2<T, P>::getSize() const
            {
                return glm::tvec2<T, P>(max.x - min.x, max.y - min.y);
            }

            template<typename T, glm::precision P>
            constexpr glm::tvec3<T, P> tBBox3<T, P>::getSize() const
            {
                return glm::tvec3<T, P>(max.x - min.x, max.y - min.y, max.z - min.z);
            }*/

            template<>
            constexpr glm::tvec2<float> tBBox2<float>::getSize() const
            {
                return glm::tvec2<float>(max.x - min.x, max.y - min.y);
            }

            template<>
            constexpr glm::tvec2<int> tBBox2<int>::getSize() const
            {
                return glm::tvec2<int>(max.x - min.x + 1, max.y - min.y + 1);
            }

            template<>
            constexpr glm::tvec3<float> tBBox3<float>::getSize() const
            {
                return glm::tvec3<float>(max.x - min.x, max.y - min.y, max.z - min.z);
            }

            template<>
            constexpr glm::tvec3<int> tBBox3<int>::getSize() const
            {
                return glm::tvec3<int>(max.x - min.x + 1, max.y - min.y + 1, max.z - min.z + 1);
            }

            template<typename T, glm::precision P>
            constexpr glm::tvec2<T, P> tBBox2<T, P>::getCenter() const
            {
                return glm::tvec2<T, P>(
                    min.x + (max.x - min.x) / 2.f,
                    min.y + (max.y - min.y) / 2.f);
            }

            template<typename T, glm::precision P>
            constexpr glm::tvec3<T, P> tBBox3<T, P>::getCenter() const
            {
                return glm::tvec3<T, P>(
                    min.x + (max.x - min.x) / 2.f,
                    min.y + (max.y - min.y) / 2.f,
                    min.z + (max.z - min.z) / 2.f);
            }

            template<>
            constexpr int tBBox2<int>::getArea() const
            {
                return w() * h();
            }

            template<>
            constexpr float tBBox2<float>::getArea() const
            {
                return w() * h();
            }

            template<>
            inline float tBBox2<int>::getAspect() const
            {
                const int h = this->h();
                return h != 0 ? w() / static_cast<float>(h) : 0.f;
            }

            template<>
            inline float tBBox2<float>::getAspect() const
            {
                const float h = this->h();
                return h != 0 ? w() / h : 0.f;
            }

            template<>
            inline void tBBox2<int>::zero()
            {
                min.x = min.y = max.x = max.y = 0;
            }

            template<>
            inline void tBBox2<float>::zero()
            {
                min.x = min.y = max.x = max.y = 0.f;
            }

            template<>
            inline void tBBox3<float>::zero()
            {
                min.x = min.y = min.z = max.x = max.y = max.z = 0.f;
            }

            template<typename T, glm::precision P>
            constexpr bool tBBox2<T, P>::contains(const tBBox2<T, P> & value) const
            {
                return
                    value.min.x >= min.x && value.max.x <= max.x &&
                    value.min.y >= min.y && value.max.y <= max.y;
            }

            template<typename T, glm::precision P>
            constexpr bool tBBox3<T, P>::contains(const tBBox3<T, P> & value) const
            {
                return
                    value.min.x >= min.x && value.max.x <= max.x &&
                    value.min.y >= min.y && value.max.y <= max.y &&
                    value.min.z >= min.z && value.max.z <= max.z;
            }

            template<typename T, glm::precision P>
            constexpr bool tBBox2<T, P>::contains(const glm::tvec2<T, P> & value) const
            {
                return
                    value.x >= min.x && value.x <= max.x &&
                    value.y >= min.y && value.y <= max.y;
            }

            template<typename T, glm::precision P>
            constexpr bool tBBox3<T, P>::contains(const glm::tvec3<T, P> & value) const
            {
                return
                    value.x >= min.x && value.x <= max.x &&
                    value.y >= min.y && value.y <= max.y &&
                    value.z >= min.z && value.z <= max.z;
            }

            template<typename T, glm::precision P>
            constexpr bool tBBox2<T, P>::intersects(const tBBox2<T, P> & value) const
            {
                return !(
                    value.max.x < min.x ||
                    value.min.x > max.x ||
                    value.max.y < min.y ||
                    value.min.y > max.y);
            }

            template<typename T, glm::precision P>
            constexpr bool tBBox3<T, P>::intersects(const tBBox3<T, P> & value) const
            {
                return !(
                    value.max.x < min.x ||
                    value.min.x > max.x ||
                    value.max.y < min.y ||
                    value.min.y > max.y ||
                    value.max.z < min.z ||
                    value.min.z > max.z);
            }

            template<typename T, glm::precision P>
            inline tBBox2<T, P> tBBox2<T, P>::intersect(const tBBox2<T, P> & value) const
            {
                tBBox2<T, P> out;
                out.min.x = std::max(min.x, value.min.x);
                out.min.y = std::max(min.y, value.min.y);
                out.max.x = std::min(max.x, value.max.x);
                out.max.y = std::min(max.y, value.max.y);
                return out;
            }

            template<typename T, glm::precision P>
            inline tBBox3<T, P> tBBox3<T, P>::intersect(const tBBox3<T, P> & value) const
            {
                tBBox3<T, P> out;
                out.min.x = std::max(min.x, value.min.x);
                out.min.y = std::max(min.y, value.min.y);
                out.min.z = std::max(min.z, value.min.z);
                out.max.x = std::min(max.x, value.max.x);
                out.max.y = std::min(max.y, value.max.y);
                out.max.z = std::min(max.z, value.max.z);
                return out;
            }

            namespace
            {
                //! References:
                //! - http://www.3dkingdoms.com/weekly/weekly.php?a=3

                inline bool _intersect(float fDst1, float fDst2, const glm::vec3 & start, const glm::vec3 & end, glm::vec3 & out)
                {
                    if ((fDst1 * fDst2) >= .0f) return false;
                    if (fDst1 == fDst2) return false;
                    out = start + (end - start) * (-fDst1 / (fDst2 - fDst1));
                    return true;
                }

                inline bool _isInside(const glm::vec3 & hit, const BBox3f & bbox, int axis)
                {
                    switch (axis)
                    {
                    case 0:
                        if (hit.z > bbox.min.z && hit.z < bbox.max.z && hit.y > bbox.min.y && hit.y < bbox.max.y) return true;
                        break;
                    case 1:
                        if (hit.z > bbox.min.z && hit.z < bbox.max.z && hit.x > bbox.min.x && hit.x < bbox.max.x) return true;
                        break;
                    case 2:
                        if (hit.x > bbox.min.x && hit.x < bbox.max.x && hit.y > bbox.min.y && hit.y < bbox.max.y) return true;
                        break;
                    }
                    return false;
                }

            } // namespace

            template<typename T, glm::precision P>
            inline bool tBBox3<T, P>::intersect(const glm::tvec3<T, P> & start, const glm::tvec3<T, P> & end, glm::tvec3<T, P> & out) const
            {
                // Check if the line is outside of the box.
                if (start.x < min.x && end.x < min.x) return false;
                if (start.x > max.x && end.x > max.x) return false;

                if (start.y < min.y && end.y < min.y) return false;
                if (start.y > max.y && end.y > max.y) return false;

                if (start.z < min.z && end.z < min.z) return false;
                if (start.z > max.z && end.z > max.z) return false;

                // Check if the line is inside of the box.
                if (start.x > min.x && start.x < max.x &&
                    start.y > min.y && start.y < max.y &&
                    start.z > min.z && start.z < max.z)
                {
                    out = start;
                    return true;
                }

                // Find the intersection.
                if (_intersect(start.x - min.x, end.x - min.x, start, end, out) && _isInside(out, *this, 0))
                    return true;
                if (_intersect(start.y - min.y, end.y - min.y, start, end, out) && _isInside(out, *this, 1))
                    return true;
                if (_intersect(start.z - min.z, end.z - min.z, start, end, out) && _isInside(out, *this, 2))
                    return true;
                if (_intersect(start.x - max.x, end.x - max.x, start, end, out) && _isInside(out, *this, 0))
                    return true;
                if (_intersect(start.y - max.y, end.y - max.y, start, end, out) && _isInside(out, *this, 1))
                    return true;
                if (_intersect(start.z - max.z, end.z - max.z, start, end, out) && _isInside(out, *this, 2))
                    return true;

                return false;
            }

            template<typename T, glm::precision P>
            inline void tBBox2<T, P>::expand(const tBBox2<T, P> & value)
            {
                min.x = std::min(min.x, value.min.x);
                min.y = std::min(min.y, value.min.y);
                max.x = std::max(max.x, value.max.x);
                max.y = std::max(max.y, value.max.y);
            }

            template<typename T, glm::precision P>
            inline void tBBox3<T, P>::expand(const tBBox3<T, P> & value)
            {
                min.x = std::min(min.x, value.min.x);
                min.y = std::min(min.y, value.min.y);
                min.z = std::min(min.z, value.min.z);
                max.x = std::max(max.x, value.max.x);
                max.y = std::max(max.y, value.max.y);
                max.z = std::max(max.z, value.max.z);
            }

            template<typename T, glm::precision P>
            inline void tBBox2<T, P>::expand(const glm::tvec2<T, P> & value)
            {
                min.x = std::min(min.x, value.x);
                min.y = std::min(min.y, value.y);
                max.x = std::max(max.x, value.x);
                max.y = std::max(max.y, value.y);
            }

            template<typename T, glm::precision P>
            inline void tBBox3<T, P>::expand(const glm::tvec3<T, P> & value)
            {
                min.x = std::min(min.x, value.x);
                min.y = std::min(min.y, value.y);
                min.z = std::min(min.z, value.z);
                max.x = std::max(max.x, value.x);
                max.y = std::max(max.y, value.y);
                max.z = std::max(max.z, value.z);
            }

            template<typename T, glm::precision P>
            constexpr tBBox2<T, P> tBBox2<T, P>::margin(const glm::tvec2<T, P> & value) const
            {
                return tBBox2<T, P>(
                    glm::tvec2<T, P>(min.x - value.x, min.y - value.y),
                    glm::tvec2<T, P>(max.x + value.x, max.y + value.y));
            }

            template<typename T, glm::precision P>
            constexpr tBBox2<T, P> tBBox2<T, P>::margin(T value) const
            {
                return tBBox2<T, P>(
                    glm::tvec2<T, P>(min.x - value, min.y - value),
                    glm::tvec2<T, P>(max.x + value, max.y + value));
            }

            template<typename T, glm::precision P>
            constexpr tBBox2<T, P> tBBox2<T, P>::margin(T x0, T y0, T x1, T y1) const
            {
                return tBBox2<T, P>(
                    glm::tvec2<T, P>(min.x - x0, min.y - y0),
                    glm::tvec2<T, P>(max.x + x1, max.y + y1));
            }

            template<typename T, glm::precision P>
            constexpr bool tBBox2<T, P>::operator == (const tBBox2<T, P> & value) const
            {
                return min == value.min && max == value.max;
            }

            template<typename T, glm::precision P>
            constexpr bool tBBox3<T, P>::operator == (const tBBox3<T, P> & value) const
            {
                return min == value.min && max == value.max;
            }

            template<typename T, glm::precision P>
            constexpr bool tBBox2<T, P>::operator != (const tBBox2<T, P> & value) const
            {
                return !(*this == value);
            }

            template<typename T, glm::precision P>
            constexpr bool tBBox3<T, P>::operator != (const tBBox3<T, P> & value) const
            {
                return !(*this == value);
            }

        } // namespace BBox
    } // namespace Core

    template<typename T, glm::precision P>
    inline bool fuzzyCompare(const Core::BBox::tBBox2<T, P> & a, const Core::BBox::tBBox2<T, P> & b)
    {
        return fuzzyCompare(a.min, b.min) && fuzzyCompare(a.max, b.max);
    }

    template<typename T, glm::precision P>
    inline bool fuzzyCompare(const Core::BBox::tBBox3<T, P> & a, const Core::BBox::tBBox3<T, P> & b)
    {
        return fuzzyCompare(a.min, b.min) && fuzzyCompare(a.max, b.max);
    }

    template<typename T, glm::precision P>
    inline Core::BBox::tBBox3<T, P> operator * (const Core::BBox::tBBox3<T, P> & bbox, const glm::mat4 & m)
    {
        std::vector<glm::tvec4<T, P> > pts;
        pts.push_back(glm::tvec4<T, P>(bbox.min.x, bbox.min.y, bbox.min.z, 1.f));
        pts.push_back(glm::tvec4<T, P>(bbox.max.x, bbox.min.y, bbox.min.z, 1.f));
        pts.push_back(glm::tvec4<T, P>(bbox.max.x, bbox.min.y, bbox.max.z, 1.f));
        pts.push_back(glm::tvec4<T, P>(bbox.min.x, bbox.min.y, bbox.max.z, 1.f));
        pts.push_back(glm::tvec4<T, P>(bbox.min.x, bbox.max.y, bbox.min.z, 1.f));
        pts.push_back(glm::tvec4<T, P>(bbox.max.x, bbox.max.y, bbox.min.z, 1.f));
        pts.push_back(glm::tvec4<T, P>(bbox.max.x, bbox.max.y, bbox.max.z, 1.f));
        pts.push_back(glm::tvec4<T, P>(bbox.min.x, bbox.max.y, bbox.max.z, 1.f));

        Core::BBox::tBBox3<T, P> out(m * pts[0]);
        for (size_t i = 1; i < 8; ++i)
        {
            out.expand(m * pts[i]);
        }

        return out;
    }


    template<typename T, glm::precision P>
    inline std::ostream & operator << (std::ostream & s, const Core::BBox::tBBox2<T, P> & value)
    {
        s << value.min << " ";
        s << value.max;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::ostream & operator << (std::ostream & s, const Core::BBox::tBBox3<T, P> & value)
    {
        s << value.min << " ";
        s << value.max;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::istream & operator >> (std::istream & s, Core::BBox::tBBox2<T, P> & out)
    {
        s >> out.min;
        s >> out.max;
        return s;
    }

    template<typename T, glm::precision P>
    inline std::istream & operator >> (std::istream & s, Core::BBox::tBBox3<T, P> & out)
    {
        s >> out.min;
        s >> out.max;
        return s;
    }

} // namespace djv

