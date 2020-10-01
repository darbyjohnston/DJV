// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Geom
    {
        inline float Square::getRadius() const
        {
            return _radius;
        }

        inline float Circle::getRadius() const
        {
            return _radius;
        }
        
        inline size_t Circle::getResolution() const
        {
            return _resolution;
        }

        inline float Cube::getRadius() const
        {
            return _radius;
        }

        inline float Sphere::getRadius() const
        {
            return _radius;
        }
        
        inline const Sphere::Resolution& Sphere::getResolution() const
        {
            return _resolution;
        }
        
        inline const Sphere::Span& Sphere::getUSpan() const
        {
            return _uSpan;
        }
        
        inline const Sphere::Span& Sphere::getVSpan() const
        {
            return _vSpan;
        }
        
        inline bool Sphere::hasTextureSpan() const
        {
            return _textureSpan;
        }

        inline float Cylinder::getRadius() const
        {
            return _radius;
        }
        
        inline float Cylinder::getLength() const
        {
            return _length;
        }
        
        inline bool Cylinder::isCapped() const
        {
            return _capped;
        }
        
        inline size_t Cylinder::getResolution() const
        {
            return _resolution;
        }
        
        inline const Cylinder::Span& Cylinder::getSpan() const
        {
            return _span;
        }
        
        inline bool Cylinder::hasTextureSpan() const
        {
            return _textureSpan;
        }

    } // namespace Geom
} // namespace djv
