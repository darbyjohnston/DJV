//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

namespace djv
{
    namespace Scene
    {
        inline IPrimitive::IPrimitive() :
            _uid(Core::createUID())
        {}

        inline IPrimitive::~IPrimitive()
        {}

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

        inline const Core::BBox3f& IPrimitive::getBBox() const
        {
            return _bbox;
        }

        inline void IPrimitive::setBBox(const Core::BBox3f& value)
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

        inline const AV::Image::Color& IPrimitive::getColor() const
        {
            return _color;
        }

        inline void IPrimitive::setColorAssignment(ColorAssignment value)
        {
            _colorAssignment = value;
        }

        inline void IPrimitive::setColor(const AV::Image::Color& value)
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

        inline const std::vector<std::shared_ptr<AV::Geom::TriangleMesh> >& IPrimitive::getMeshes() const
        {
            return _meshesDummy;
        }

        inline const std::vector<std::shared_ptr<AV::Geom::PointList> >& IPrimitive::getPolyLines() const
        {
            return _polyLinesDummy;
        }

        inline const std::shared_ptr<AV::Geom::PointList>& IPrimitive::getPointList() const
        {
            return _pointListDummy;
        }

        inline size_t IPrimitive::getPointCount() const
        {
            return 0;
        }

    } // namespace Scene
} // namespace djv

