// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <memory>

namespace djv
{
    namespace Geom
    {
        class TriangleMesh;

        //! This class provides the interface for shapes.
        class IShape
        {
        public:
            virtual ~IShape() = 0;

            virtual void triangulate(TriangleMesh&) const = 0;
        };

        //! This class provides a square shape.
        class Square : public IShape
        {
        public:
            explicit Square(float radius = .5F);

            //! \name Radius
            ///@{

            float getRadius() const;

            void setRadius(float);

            ///@}
        
            //! \name Utility
            ///@{

            void triangulate(TriangleMesh&) const override;

            ///@}

        private:
            float _radius = .5F;
        };

        //! This class provides a circle shape.
        class Circle : public IShape
        {
        public:
            Circle(float radius = .5F, size_t resolution = 10);

            //! \name Radius
            ///@{

            float getRadius() const;

            void setRadius(float);

            ///@}
        
            //! \name Resolution
            ///@{

            size_t getResolution() const;

            void setResolution(size_t);

            ///@}
        
            //! \name Utility
            ///@{

            void triangulate(TriangleMesh&) const override;

            ///@}

        private:
            float _radius = .5F;
            size_t _resolution = 10;
        };

        //! This class provides a cube shape.
        class Cube : public IShape
        {
        public:
            explicit Cube(float radius = .5F);

            //! \name Radius
            ///@{

            float getRadius() const;

            void setRadius(float);

            ///@}

            //! \name Utility
            ///@{

            void triangulate(TriangleMesh&) const override;

            ///@}

        private:
            float _radius = .5F;
        };

        //! This class provides a sphere shape.
        class Sphere : public IShape
        {
        public:
            typedef std::pair<size_t, size_t> Resolution;
            typedef std::pair<float, float> Span;

            Sphere(float radius = .5F, const Resolution& = Resolution(10, 10));

            //! \name Radius
            ///@{

            float getRadius() const;

            void setRadius(float);

            ///@}

            //! \name Resolution
            ///@{

            const Resolution& getResolution() const;

            void setResolution(const Resolution&);

            ///@}

            //! \name Span
            ///@{

            const Span& getUSpan() const;
            const Span& getVSpan() const;
            bool hasTextureSpan() const;

            void setUSpan(const Span&);
            void setVSpan(const Span&);
            void setTextureSpan(bool);

            ///@}

            //! \name Utility
            ///@{

            void triangulate(TriangleMesh&) const override;

            ///@}

        private:
            float _radius = .5F;
            Resolution _resolution;
            Span _uSpan = Span(0.F, 1.F);
            Span _vSpan = Span(0.F, 1.F);
            bool _textureSpan = true;
        };

        //! This class provides a cylinder shape.
        class Cylinder : public IShape
        {
        public:
            typedef std::pair<float, float> Span;

            Cylinder(float radius = .5F, float length = 1.F, size_t resolution = 100);

            //! \name Radius
            ///@{

            float getRadius() const;

            void setRadius(float);

            ///@}

            //! \name Length
            ///@{

            float getLength() const;

            void setLength(float);

            ///@}

            //! \name Options
            ///@{

            bool isCapped() const;

            void setCapped(bool);

            ///@}

            //! \name Resolution
            ///@{

            size_t getResolution() const;

            void setResolution(size_t);

            ///@}

            //! \name Span
            ///@{

            const Span& getSpan() const;
            bool hasTextureSpan() const;

            void setSpan(const Span&);
            void setTextureSpan(bool);

            ///@}

            //! \name Utility
            ///@{

            void triangulate(TriangleMesh&) const override;

            ///@}

        private:
            float _radius = .5F;
            float _length = 1.F;
            bool _capped = false;
            size_t _resolution = 10;
            Span _span = Span(0.F, 1.F);
            bool _textureSpan = true;
        };

    } // namespace Geom
} // namespace djv

#include <djvGeom/ShapeInline.h>
