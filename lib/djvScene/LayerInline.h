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
        inline ILayerItem::ILayerItem()
        {}

        inline ILayerItem::~ILayerItem()
        {}

        inline const std::weak_ptr<Layer>& ILayerItem::getLayer() const
        {
            return _layer;
        }

        inline void ILayerItem::setLayer(const std::shared_ptr<Layer>& value)
        {
            _layer = value;
        }

        inline Layer::Layer()
        {}

        inline std::shared_ptr<Layer> Layer::create()
        {
            auto out = std::shared_ptr<Layer>(new Layer);
            return out;
        }

        inline const std::string& Layer::getName() const
        {
            return _name;
        }

        inline void Layer::setName(const std::string& value)
        {
            _name = value;
        }

        inline bool Layer::getVisible() const
        {
            return _visible;
        }

        inline void Layer::setVisible(bool value)
        {
            _visible = value;
        }

        inline const AV::Image::Color& Layer::getColor() const
        {
            return _color;
        }

        inline void Layer::setColor(const AV::Image::Color& value)
        {
            _color = value;
        }

        inline const std::shared_ptr<IMaterial>& Layer::getMaterial() const
        {
            return _material;
        }

        inline void Layer::setMaterial(const std::shared_ptr<IMaterial>& value)
        {
            _material = value;
        }

        inline void Layer::addLayer(const std::shared_ptr<Layer>& value)
        {
            _layers.push_back(value);
        }

        inline void Layer::addItem(const std::shared_ptr<ILayerItem>& value)
        {
            _items.push_back(value);
        }

    } // namespace Scene
} // namespace djv
