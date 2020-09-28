// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene3D/Enum.h>
#include <djvScene3D/Layer.h>

#include <djvImage/Color.h>

#include <djvMath/BBox.h>

#include <djvCore/UID.h>

#include <glm/mat4x4.hpp>

#include <memory>

namespace djv
{
    namespace Geom
    {
        class PointList;
        class TriangleMesh;

    } // namespace Geom

    namespace Render3D
    {
        class Render;

    } // namespace Render3D

    namespace Scene3D
    {
        class IMaterial;

        //! This class provides the base functionality for primitives.
        class IPrimitive : public ILayerItem
        {
            DJV_NON_COPYABLE(IPrimitive);

        protected:
            IPrimitive();

        public:
            virtual ~IPrimitive() = 0;

            virtual std::string getClassName() const = 0;

            Core::UID getUID() const;

            const std::string& getName() const;

            void setName(const std::string&);

            bool isVisible() const;

            void setVisible(bool);

            const Math::BBox3f& getBBox() const;

            void setBBox(const Math::BBox3f&);

            const glm::mat4x4& getXForm() const;
            bool isXFormIdentity() const;

            void setXForm(const glm::mat4x4&);

            ColorAssignment getColorAssignment() const;
            const Image::Color& getColor() const;

            void setColorAssignment(ColorAssignment);
            void setColor(const Image::Color&);

            MaterialAssignment getMaterialAssignment() const;
            const std::shared_ptr<IMaterial>& getMaterial() const;
            virtual bool isShaded() const;

            void setMaterialAssignment(MaterialAssignment);
            void setMaterial(const std::shared_ptr<IMaterial>&);

            const std::weak_ptr<IPrimitive>& getParent() const;
            const std::vector<std::shared_ptr<IPrimitive> >& getChildren() const;

            void addChild(const std::shared_ptr<IPrimitive>&);
            void removeChild(const std::shared_ptr<IPrimitive>&);
            void clearChildren();

            virtual const std::vector<std::shared_ptr<IPrimitive> >& getPrimitives() const;

            virtual const std::vector<std::shared_ptr<Geom::TriangleMesh> >& getMeshes() const;
            virtual const std::vector<std::shared_ptr<Geom::PointList> >& getPolyLines() const;
            virtual const std::shared_ptr<Geom::PointList>& getPointList() const;

            virtual size_t getPointCount() const;

        private:
            Core::UID _uid = 0;
            std::string _name;
            bool _visible = true;
            Math::BBox3f _bbox = Math::BBox3f(0.F, 0.F, 0.F, 0.F, 0.F, 0.F);
            glm::mat4x4 _xform = glm::mat4x4(1.F);
            bool _xformIdentity = true;
            ColorAssignment _colorAssignment = ColorAssignment::Primitive;
            Image::Color _color;
            MaterialAssignment _materialAssignment = MaterialAssignment::Primitive;
            std::shared_ptr<IMaterial> _material;
            std::weak_ptr<IPrimitive> _parent;
            std::vector<std::shared_ptr<IPrimitive> > _children;
            static std::vector<std::shared_ptr<Geom::TriangleMesh> > _meshesDummy;
            static std::vector<std::shared_ptr<Geom::PointList> > _polyLinesDummy;
            static std::shared_ptr<Geom::PointList> _pointListDummy;
        };

    } // namespace Scene3D
} // namespace djv

#include <djvScene3D/IPrimitiveInline.h>
