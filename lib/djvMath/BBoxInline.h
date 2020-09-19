// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <algorithm>

namespace djv
{
    namespace Math
    {
        template<>
        inline tBBox2<int>::tBBox2() noexcept :
            min(0, 0),
            max(0, 0)
        {}

        template<>
        inline tBBox2<float>::tBBox2() noexcept :
            min(0.F, 0.F),
            max(0.F, 0.F)
        {}

        template<>
        inline tBBox3<float>::tBBox3() noexcept :
            min(0.F, 0.F, 0.F),
            max(0.F, 0.F, 0.F)
        {}

        template<typename T, glm::precision P>
        inline tBBox2<T, P>::tBBox2(const glm::tvec2<T, P>& value) noexcept :
            min(value),
            max(value)
        {}

        template<typename T, glm::precision P>
        inline tBBox3<T, P>::tBBox3(const glm::tvec3<T, P>& value) noexcept :
            min(value),
            max(value)
        {}

        template<typename T, glm::precision P>
        inline tBBox2<T, P>::tBBox2(const glm::tvec2<T, P>& min, const glm::tvec2<T, P>& max) noexcept :
            min(min),
            max(max)
        {}

        template<typename T, glm::precision P>
        inline tBBox3<T, P>::tBBox3(const glm::tvec3<T, P>& min, const glm::tvec3<T, P>& max) noexcept :
            min(min),
            max(max)
        {}

        template<>
        inline tBBox2<int>::tBBox2(int x, int y, int w, int h) noexcept :
            min(x, y),
            max(x + w - 1, y + h - 1)
        {}

        template<>
        inline tBBox2<float>::tBBox2(float x, float y, float w, float h) noexcept :
            min(x, y),
            max(x + w, y + h)
        {}

        template<typename T, glm::precision P>
        inline tBBox3<T, P>::tBBox3(T x, T y, T z, T w, T h, T d) noexcept :
            min(x, y, z),
            max(x + w, y + h, z + d)
        {}

        template<typename T, glm::precision P>
        inline T tBBox2<T, P>::x() const noexcept { return min.x; }
        template<typename T, glm::precision P>
        inline T tBBox3<T, P>::x() const noexcept { return min.x; }

        template<typename T, glm::precision P>
        inline T tBBox2<T, P>::y() const noexcept { return min.y; }
        template<typename T, glm::precision P>
        inline T tBBox3<T, P>::y() const noexcept { return min.y; }

        template<typename T, glm::precision P>
        inline T tBBox3<T, P>::z() const noexcept { return min.z; }

        template<>
        inline float tBBox2<float>::w() const noexcept { return max.x - min.x; }
        template<>
        inline int tBBox2<int>::w() const noexcept { return max.x - min.x + 1; }
        template<>
        inline float tBBox3<float>::w() const noexcept { return max.x - min.x; }

        template<>
        inline float tBBox2<float>::h() const noexcept { return max.y - min.y; }
        template<>
        inline int tBBox2<int>::h() const noexcept { return max.y - min.y + 1; }
        template<>
        inline float tBBox3<float>::h() const noexcept { return max.y - min.y; }

        template<>
        inline float tBBox3<float>::d() const noexcept { return max.z - min.z; }

        template<typename T, glm::precision P>
        constexpr bool tBBox2<T, P>::isValid() const noexcept
        {
            return
                min.x < max.x &&
                min.y < max.y;
        }

        template<typename T, glm::precision P>
        constexpr bool tBBox3<T, P>::isValid() const noexcept
        {
            return
                min.x < max.x &&
                min.y < max.y &&
                min.z < max.z;
        }

        template<>
        inline glm::tvec2<int> tBBox2<int>::getSize() const noexcept
        {
            return glm::tvec2<int>(max.x - min.x + 1, max.y - min.y + 1);
        }

        template<>
        inline glm::tvec2<float> tBBox2<float>::getSize() const noexcept
        {
            return glm::tvec2<float>(max.x - min.x, max.y - min.y);
        }

        template<typename T, glm::precision P>
        inline glm::tvec3<T, P> tBBox3<T, P>::getSize() const noexcept
        {
            return glm::tvec3<T, P>(max.x - min.x, max.y - min.y, max.z - min.z);
        }

        template<>
        inline glm::tvec2<int> tBBox2<int>::getCenter() const noexcept
        {
            return glm::tvec2<int>(
                min.x + (max.x - min.x + 1) / 2.F,
                min.y + (max.y - min.y + 1) / 2.F);
        }

        template<>
        inline glm::tvec2<float> tBBox2<float>::getCenter() const noexcept
        {
            return glm::tvec2<float>(
                min.x + (max.x - min.x) / 2.F,
                min.y + (max.y - min.y) / 2.F);
        }

        template<typename T, glm::precision P>
        inline glm::tvec3<T, P> tBBox3<T, P>::getCenter() const noexcept
        {
            return glm::tvec3<T, P>(
                min.x + (max.x - min.x) / 2.F,
                min.y + (max.y - min.y) / 2.F,
                min.z + (max.z - min.z) / 2.F);
        }

        template<>
        inline int tBBox2<int>::getArea() const noexcept
        {
            return w() * h();
        }

        template<>
        inline float tBBox2<float>::getArea() const noexcept
        {
            return w() * h();
        }

        template<>
        inline float tBBox2<int>::getAspect() const noexcept
        {
            const int h = this->h();
            return h != 0 ? w() / static_cast<float>(h) : 0.F;
        }

        template<>
        inline float tBBox2<float>::getAspect() const noexcept
        {
            const float h = this->h();
            return h != 0 ? w() / h : 0.F;
        }

        template<>
        inline void tBBox2<int>::zero() noexcept
        {
            min.x = min.y = max.x = max.y = 0;
        }

        template<>
        inline void tBBox2<float>::zero() noexcept
        {
            min.x = min.y = max.x = max.y = 0.F;
        }

        template<>
        inline void tBBox3<float>::zero() noexcept
        {
            min.x = min.y = min.z = max.x = max.y = max.z = 0.F;
        }

        template<>
        inline bool tBBox2<int>::contains(const tBBox2<int>& value) const noexcept
        {
            return
                value.min.x >= min.x && value.max.x < max.x &&
                value.min.y >= min.y && value.max.y < max.y;
        }

        template<>
        inline bool tBBox2<float>::contains(const tBBox2<float>& value) const noexcept
        {
            return
                value.min.x >= min.x && value.max.x <= max.x &&
                value.min.y >= min.y && value.max.y <= max.y;
        }

        template<typename T, glm::precision P>
        inline bool tBBox3<T, P>::contains(const tBBox3<T, P>& value) const noexcept
        {
            return
                value.min.x >= min.x && value.max.x <= max.x &&
                value.min.y >= min.y && value.max.y <= max.y &&
                value.min.z >= min.z && value.max.z <= max.z;
        }

        template<>
        inline bool tBBox2<int>::contains(const glm::tvec2<int>& value) const noexcept
        {
            return
                value.x >= min.x && value.x < max.x &&
                value.y >= min.y && value.y < max.y;
        }

        template<>
        inline bool tBBox2<float>::contains(const glm::tvec2<float>& value) const noexcept
        {
            return
                value.x >= min.x && value.x <= max.x &&
                value.y >= min.y && value.y <= max.y;
        }

        template<typename T, glm::precision P>
        inline bool tBBox3<T, P>::contains(const glm::tvec3<T, P>& value) const noexcept
        {
            return
                value.x >= min.x && value.x <= max.x &&
                value.y >= min.y && value.y <= max.y &&
                value.z >= min.z && value.z <= max.z;
        }

        template<>
        inline bool tBBox2<int>::intersects(const tBBox2<int>& value) const noexcept
        {
            return !(
                value.max.x <= min.x ||
                value.min.x >= max.x ||
                value.max.y <= min.y ||
                value.min.y >= max.y);
        }

        template<>
        inline bool tBBox2<float>::intersects(const tBBox2<float>& value) const noexcept
        {
            return !(
                value.max.x < min.x ||
                value.min.x > max.x ||
                value.max.y < min.y ||
                value.min.y > max.y);
        }

        template<typename T, glm::precision P>
        inline bool tBBox3<T, P>::intersects(const tBBox3<T, P>& value) const noexcept
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
        inline tBBox2<T, P> tBBox2<T, P>::intersect(const tBBox2<T, P>& value) const
        {
            tBBox2<T, P> out;
            out.min.x = std::max(min.x, value.min.x);
            out.min.y = std::max(min.y, value.min.y);
            out.max.x = std::min(max.x, value.max.x);
            out.max.y = std::min(max.y, value.max.y);
            return out;
        }

        template<typename T, glm::precision P>
        inline tBBox3<T, P> tBBox3<T, P>::intersect(const tBBox3<T, P>& value) const
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

        //! References:
        //! - http://www.3dkingdoms.com/weekly/weekly.php?a=3

        inline bool _intersect(float fDst1, float fDst2, const glm::vec3& start, const glm::vec3& end, glm::vec3& out) noexcept
        {
            if ((fDst1 * fDst2) >= .0F) return false;
            if (fDst1 == fDst2) return false;
            out = start + (end - start) * (-fDst1 / (fDst2 - fDst1));
            return true;
        }

        inline bool _isInside(const glm::vec3& hit, const BBox3f& bbox, int axis) noexcept
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
            default: break;
            }
            return false;
        }

        template<typename T, glm::precision P>
        inline bool tBBox3<T, P>::intersect(const glm::tvec3<T, P>& start, const glm::tvec3<T, P>& end, glm::tvec3<T, P>& out) const
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
        inline void tBBox2<T, P>::expand(const tBBox2<T, P>& value)
        {
            min.x = std::min(min.x, value.min.x);
            min.y = std::min(min.y, value.min.y);
            max.x = std::max(max.x, value.max.x);
            max.y = std::max(max.y, value.max.y);
        }

        template<typename T, glm::precision P>
        inline void tBBox3<T, P>::expand(const tBBox3<T, P>& value)
        {
            min.x = std::min(min.x, value.min.x);
            min.y = std::min(min.y, value.min.y);
            min.z = std::min(min.z, value.min.z);
            max.x = std::max(max.x, value.max.x);
            max.y = std::max(max.y, value.max.y);
            max.z = std::max(max.z, value.max.z);
        }

        template<typename T, glm::precision P>
        inline void tBBox2<T, P>::expand(const glm::tvec2<T, P>& value)
        {
            min.x = std::min(min.x, value.x);
            min.y = std::min(min.y, value.y);
            max.x = std::max(max.x, value.x);
            max.y = std::max(max.y, value.y);
        }

        template<typename T, glm::precision P>
        inline void tBBox3<T, P>::expand(const glm::tvec3<T, P>& value)
        {
            min.x = std::min(min.x, value.x);
            min.y = std::min(min.y, value.y);
            min.z = std::min(min.z, value.z);
            max.x = std::max(max.x, value.x);
            max.y = std::max(max.y, value.y);
            max.z = std::max(max.z, value.z);
        }

        template<typename T, glm::precision P>
        constexpr tBBox2<T, P> tBBox2<T, P>::margin(const glm::tvec2<T, P>& value) const noexcept
        {
            return tBBox2<T, P>(
                glm::tvec2<T, P>(min.x - value.x, min.y - value.y),
                glm::tvec2<T, P>(max.x + value.x, max.y + value.y));
        }

        template<typename T, glm::precision P>
        constexpr tBBox2<T, P> tBBox2<T, P>::margin(T value) const noexcept
        {
            return tBBox2<T, P>(
                glm::tvec2<T, P>(min.x - value, min.y - value),
                glm::tvec2<T, P>(max.x + value, max.y + value));
        }

        template<typename T, glm::precision P>
        constexpr tBBox2<T, P> tBBox2<T, P>::margin(T x0, T y0, T x1, T y1) const noexcept
        {
            return tBBox2<T, P>(
                glm::tvec2<T, P>(min.x - x0, min.y - y0),
                glm::tvec2<T, P>(max.x + x1, max.y + y1));
        }

        template<typename T, glm::precision P>
        constexpr bool tBBox2<T, P>::operator == (const tBBox2<T, P>& value) const noexcept
        {
            return min == value.min && max == value.max;
        }

        template<typename T, glm::precision P>
        constexpr bool tBBox3<T, P>::operator == (const tBBox3<T, P>& value) const noexcept
        {
            return min == value.min && max == value.max;
        }

        template<typename T, glm::precision P>
        constexpr bool tBBox2<T, P>::operator != (const tBBox2<T, P>& value) const noexcept
        {
            return !(*this == value);
        }

        template<typename T, glm::precision P>
        constexpr bool tBBox3<T, P>::operator != (const tBBox3<T, P>& value) const noexcept
        {
            return !(*this == value);
        }

    } // namespace Math
} // namespace djv

