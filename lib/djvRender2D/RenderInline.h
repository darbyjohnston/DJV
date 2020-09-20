// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImage/Color.h>

namespace djv
{
    namespace Render2D
    {
        inline void Render::pushTransform(const glm::mat3x3& value)
        {
            _transforms.push_back(_transforms.size() ? _transforms.back() * value : value);
        }

        inline void Render::popTransform()
        {
            if (_transforms.size())
            {
                _transforms.pop_back();
            }
        }

        inline void Render::pushClipRect(const Math::BBox2f& value)
        {
            if (!_clipRects.size())
            {
                _currentClipRect = value;
            }
            else
            {
                _currentClipRect = _currentClipRect.intersect(value);
            }
            _clipRects.push_back(_currentClipRect);
        }

        inline void Render::popClipRect()
        {
            _clipRects.pop_back();
            if (_clipRects.size())
            {
                _currentClipRect = _clipRects.back();
            }
            else
            {
                _currentClipRect.zero();
            }
        }

        inline void Render::setLineWidth(float value)
        {
            _lineWidth = value;
        }

        inline const glm::mat3x3& Render::_getCurrentTransform() const
        {
            return _transforms.size() ? _transforms.back() : _identity;
        }

        inline void Render::_currentClipRectUpdate()
        {
            _currentClipRect.min.x = 0.F;
            _currentClipRect.min.y = 0.F;
            _currentClipRect.max.x = static_cast<float>(_size.w);
            _currentClipRect.max.y = static_cast<float>(_size.h);
            for (const auto& i : _clipRects)
            {
                _currentClipRect = _currentClipRect.intersect(i);
            }
        }

    } // namespace Render2D
} // namespace djv

