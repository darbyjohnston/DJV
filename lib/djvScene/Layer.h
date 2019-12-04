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
