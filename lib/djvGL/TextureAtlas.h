// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvGL/GL.h>

#include <djvImage/Type.h>

#include <djvMath/Range.h>

#include <djvCore/UID.h>

namespace djv
{
    namespace Image
    {
        class Data;

    } // namespace Image

    namespace GL
    {
        //! This struct provides information about a texture atlas item.
        struct TextureAtlasItem
        {
            uint16_t w = 0;
            uint16_t h = 0;
            uint8_t textureIndex = 0;
            Math::FloatRange textureU;
            Math::FloatRange textureV;
        };

        //! This class provides a texture atlas.
        class TextureAtlas
        {
            DJV_NON_COPYABLE(TextureAtlas);

        public:
            TextureAtlas(uint8_t textureCount, uint16_t textureSize, Image::Type, GLenum filter = GL_LINEAR, uint8_t border = 1);
            ~TextureAtlas();

            uint8_t getTextureCount() const;
            uint16_t getTextureSize() const;
            Image::Type getTextureType() const;
            std::vector<GLuint> getTextures() const;

            bool getItem(Core::UID, TextureAtlasItem&);

            Core::UID addItem(const std::shared_ptr<Image::Data>&, TextureAtlasItem&);

            float getPercentageUsed() const;

        private:
            class BoxPackingNode;

            void _getAllNodes(
                const std::shared_ptr<BoxPackingNode>&,
                std::vector<std::shared_ptr<BoxPackingNode> >&);
            void _getLeafNodes(
                const std::shared_ptr<BoxPackingNode>&,
                std::vector<std::shared_ptr<BoxPackingNode> >&) const;
            void _toTextureAtlasItem(
                const std::shared_ptr<BoxPackingNode>&,
                TextureAtlasItem&);
            void _removeFromAtlas(const std::shared_ptr<BoxPackingNode>&);

            DJV_PRIVATE();
        };

    } // namespace GL
} // namespace djv
