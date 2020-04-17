// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene/Enum.h>
#include <djvScene/Layer.h>

#include <djvAV/Color.h>

#include <djvCore/BBox.h>
#include <djvCore/UID.h>

#include <glm/mat4x4.hpp>

#include <memory>

namespace djv
{
    namespace AV
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
    } // namespace AV

    namespace Scene
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

            const Core::BBox3f& getBBox() const;
            void setBBox(const Core::BBox3f&);

            const glm::mat4x4& getXForm() const;
            bool isXFormIdentity() const;
            void setXForm(const glm::mat4x4&);

            ColorAssignment getColorAssignment() const;
            const AV::Image::Color& getColor() const;
            void setColorAssignment(ColorAssignment);
            void setColor(const AV::Image::Color&);

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

            virtual const std::vector<std::shared_ptr<AV::Geom::TriangleMesh> >& getMeshes() const;
            virtual const std::vector<std::shared_ptr<AV::Geom::PointList> >& getPolyLines() const;
            virtual const std::shared_ptr<AV::Geom::PointList>& getPointList() const;

            virtual size_t getPointCount() const;

        private:
            Core::UID _uid = 0;
            std::string _name;
            bool _visible = true;
            Core::BBox3f _bbox = Core::BBox3f(0.F, 0.F, 0.F, 0.F, 0.F, 0.F);
            glm::mat4x4 _xform = glm::mat4x4(1.F);
            bool _xformIdentity = true;
            ColorAssignment _colorAssignment = ColorAssignment::Primitive;
            AV::Image::Color _color;
            MaterialAssignment _materialAssignment = MaterialAssignment::Primitive;
            std::shared_ptr<IMaterial> _material;
            std::weak_ptr<IPrimitive> _parent;
            std::vector<std::shared_ptr<IPrimitive> > _children;
            static std::vector<std::shared_ptr<AV::Geom::TriangleMesh> > _meshesDummy;
            static std::vector<std::shared_ptr<AV::Geom::PointList> > _polyLinesDummy;
            static std::shared_ptr<AV::Geom::PointList> _pointListDummy;
        };

    } // namespace Scene
} // namespace djv

#include <djvScene/IPrimitiveInline.h>
