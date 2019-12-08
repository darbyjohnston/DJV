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
        inline const std::vector<std::shared_ptr<IPrimitive> >& Scene::getPrimitives() const
        {
            return _primitives;
        }

        inline const std::vector<std::shared_ptr<Layer> >& Scene::getLayers() const
        {
            return _layers;
        }

        inline SceneOrient Scene::getSceneOrient() const
        {
            return _orient;
        }

        inline void Scene::setSceneOrient(SceneOrient value)
        {
            _orient = value;
        }

        inline const glm::mat4x4& Scene::getSceneXForm() const
        {
            return _xform;
        }

        inline void Scene::setSceneXForm(const glm::mat4x4& value)
        {
            _xform = value;
        }

        inline const Core::BBox3f& Scene::getBBox() const
        {
            return _bbox;
        }

        inline const glm::mat4x4& Scene::_getCurrentXForm() const
        {
            return _xforms.size() ? _xforms.back() : _identity;
        }

        inline void Scene::_pushXForm(const glm::mat4x4& value)
        {
            _xforms.push_back(_getCurrentXForm() * value);
        }

        inline void Scene::_popXForm()
        {
            _xforms.pop_back();
        }

    } // namespace Scene
} // namespace djv
