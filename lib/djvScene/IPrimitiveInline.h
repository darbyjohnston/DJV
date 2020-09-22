// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene
    {
        inline Core::UID IPrimitive::getUID() const
        {
            return _uid;
        }

        inline const std::string& IPrimitive::getName() const
        {
            return _name;
        }

        inline void IPrimitive::setName(const std::string& value)
        {
            _name = value;
        }

        inline bool IPrimitive::isVisible() const
        {
            return _visible;
        }

        inline void IPrimitive::setVisible(bool value)
        {
            _visible = value;
        }

        inline const Math::BBox3f& IPrimitive::getBBox() const
        {
            return _bbox;
        }

        inline void IPrimitive::setBBox(const Math::BBox3f& value)
        {
            _bbox = value;
        }

        inline const glm::mat4x4& IPrimitive::getXForm() const
        {
            return _xform;
        }

        inline bool IPrimitive::isXFormIdentity() const
        {
            return _xformIdentity;
        }

        inline void IPrimitive::setXForm(const glm::mat4x4& value)
        {
            _xform = value;
            _xformIdentity = value == glm::mat4x4(1.F);
        }

        inline ColorAssignment IPrimitive::getColorAssignment() const
        {
            return _colorAssignment;
        }

        inline const Image::Color& IPrimitive::getColor() const
        {
            return _color;
        }

        inline void IPrimitive::setColorAssignment(ColorAssignment value)
        {
            _colorAssignment = value;
        }

        inline void IPrimitive::setColor(const Image::Color& value)
        {
            _color = value;
        }

        inline MaterialAssignment IPrimitive::getMaterialAssignment() const
        {
            return _materialAssignment;
        }

        inline const std::shared_ptr<IMaterial>& IPrimitive::getMaterial() const
        {
            return _material;
        }

        inline bool IPrimitive::isShaded() const
        {
            return true;
        }

        inline void IPrimitive::setMaterialAssignment(MaterialAssignment value)
        {
            _materialAssignment = value;
        }

        inline void IPrimitive::setMaterial(const std::shared_ptr<IMaterial>& value)
        {
            _material = value;
        }

        inline const std::weak_ptr<IPrimitive>& IPrimitive::getParent() const
        {
            return _parent;
        }

        inline const std::vector<std::shared_ptr<IPrimitive> >& IPrimitive::getChildren() const
        {
            return _children;
        }

        inline const std::vector<std::shared_ptr<IPrimitive> >& IPrimitive::getPrimitives() const
        {
            return _children;
        }

        inline const std::vector<std::shared_ptr<Geom::TriangleMesh> >& IPrimitive::getMeshes() const
        {
            return _meshesDummy;
        }

        inline const std::vector<std::shared_ptr<Geom::PointList> >& IPrimitive::getPolyLines() const
        {
            return _polyLinesDummy;
        }

        inline const std::shared_ptr<Geom::PointList>& IPrimitive::getPointList() const
        {
            return _pointListDummy;
        }

        inline size_t IPrimitive::getPointCount() const
        {
            return 0;
        }

    } // namespace Scene
} // namespace djv

