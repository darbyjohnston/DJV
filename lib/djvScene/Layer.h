// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Color.h>

#include <memory>
#include <vector>

namespace djv
{
    namespace Scene
    {
        class IMaterial;
        class Layer;

        //! This class provides the base functionality for layer items.
        class ILayerItem : public std::enable_shared_from_this<ILayerItem>
        {
            DJV_NON_COPYABLE(ILayerItem);

        protected:
            ILayerItem();

        public:
            virtual ~ILayerItem() = 0;

            const std::weak_ptr<Layer>& getLayer() const;
            void setLayer(const std::shared_ptr<Layer>&);

        protected:
            std::weak_ptr<Layer> _layer;

            friend class Layer;
        };

        //! This class provides a layer.
        class Layer : public ILayerItem
        {
            DJV_NON_COPYABLE(Layer);
            Layer();

        public:
            static std::shared_ptr<Layer> create();

            const std::string& getName() const;
            void setName(const std::string&);

            bool isVisible() const;
            void setVisible(bool);

            const AV::Image::Color& getColor() const;
            void setColor(const AV::Image::Color&);

            const std::shared_ptr<IMaterial>& getMaterial() const;
            void setMaterial(const std::shared_ptr<IMaterial>&);

            const std::vector<std::shared_ptr<ILayerItem> >& getItems() const;
            void addItem(const std::shared_ptr<ILayerItem>&);
            void removeItem(const std::shared_ptr<ILayerItem>&);
            void clearItems();

        private:
            std::string _name;
            bool _visible = true;
            AV::Image::Color _color = AV::Image::Color(0.F, 0.F, 0.F);
            std::shared_ptr<IMaterial> _material;
            std::vector<std::shared_ptr<ILayerItem> > _items;
        };

    } // namespace Scene
} // namespace djv

#include <djvScene/LayerInline.h>
