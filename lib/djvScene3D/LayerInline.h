// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene3D
    {
        inline const std::weak_ptr<Layer>& ILayerItem::getLayer() const
        {
            return _layer;
        }

        inline void ILayerItem::setLayer(const std::shared_ptr<Layer>& value)
        {
            _layer = value;
        }

        inline const std::string& Layer::getName() const
        {
            return _name;
        }

        inline void Layer::setName(const std::string& value)
        {
            _name = value;
        }

        inline bool Layer::isVisible() const
        {
            return _visible;
        }

        inline void Layer::setVisible(bool value)
        {
            _visible = value;
        }

        inline const Image::Color& Layer::getColor() const
        {
            return _color;
        }

        inline void Layer::setColor(const Image::Color& value)
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

        inline const std::vector<std::shared_ptr<ILayerItem> >& Layer::getItems() const
        {
            return _items;
        }

    } // namespace Scene3D
} // namespace djv
