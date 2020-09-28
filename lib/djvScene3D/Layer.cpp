// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene3D/Layer.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene3D
    {
        ILayerItem::ILayerItem()
        {}

        ILayerItem::~ILayerItem()
        {}

        Layer::Layer()
        {}

        std::shared_ptr<Layer> Layer::create()
        {
            auto out = std::shared_ptr<Layer>(new Layer);
            return out;
        }

        void Layer::addItem(const std::shared_ptr<ILayerItem>& value)
        {
            if (auto prevLayer = value->getLayer().lock())
            {
                prevLayer->removeItem(value);
            }
            value->_layer = std::dynamic_pointer_cast<Layer>(shared_from_this());
            _items.push_back(value);
        }

        void Layer::removeItem(const std::shared_ptr<ILayerItem>& value)
        {
            const auto i = std::find(_items.begin(), _items.end(), value);
            if (i != _items.end())
            {
                (*i)->_layer.reset();
                _items.erase(i);
            }
        }

        void Layer::clearItems()
        {
            for (auto& i : _items)
            {
                i->_layer.reset();
            }
            _items.clear();
        }

    } // namespace Scene3D
} // namespace djv

