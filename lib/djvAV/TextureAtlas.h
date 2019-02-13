//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/ImageData.h>
#include <djvAV/OpenGL.h>

#include <djvCore/Range.h>
#include <djvCore/UID.h>

namespace djv
{
    namespace AV
    {
        namespace Render
        {
            //! This struct provides information about a texture atlas item.
            struct TextureAtlasItem
            {
                glm::ivec2 size;
                size_t textureIndex = 0;
                Core::FloatRange textureU;
                Core::FloatRange textureV;
            };

            //! This class provides a texture atlas.
            class TextureAtlas
            {
                DJV_NON_COPYABLE(TextureAtlas);

            public:
                TextureAtlas(size_t textureCount, int textureSize, Image::Type, gl::GLenum filter = gl::GL_LINEAR, int border = 1);
                ~TextureAtlas();

                size_t getTextureCount() const;
                int getTextureSize() const;
                Image::Type getTextureType() const;
                std::vector<gl::GLuint> getTextures() const;

                bool getItem(Core::UID, TextureAtlasItem&);
                Core::UID addItem(const std::shared_ptr<Image::Data>&, TextureAtlasItem&);

                float getPercentageUsed() const;

            private:
                struct BoxPackingNode;

                void _getAllNodes(BoxPackingNode*, std::vector<BoxPackingNode*>&);
                void _getLeafNodes(const BoxPackingNode*, std::vector<const BoxPackingNode*>&) const;
                void _toTextureAtlasItem(const BoxPackingNode*, TextureAtlasItem&);
                void _removeFromAtlas(BoxPackingNode*);

				DJV_PRIVATE();
            };

        } // namespace Render
    } // namespace AV
} // namespace djv
