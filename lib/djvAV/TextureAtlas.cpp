//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include "TextureAtlas.h"

#include <djvAV/OpenGLTexture.h>

#include <djvCore/BBox.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Render
        {
            namespace
            {
                uint64_t _timestamp = 0;

            } // namespace

            //! This class provides the nodes used for box packing.
            //!
            //! References:
            //! - http://blackpawn.com/texts/lightmaps/
            class TextureAtlas::BoxPackingNode : public std::enable_shared_from_this< TextureAtlas::BoxPackingNode>
            {
            private:
                BoxPackingNode(int border);

            public:
                static std::shared_ptr<BoxPackingNode> create(int border);

                UID uid = 0;
                uint64_t timestamp = 0;

                BBox2i bbox;
                int border = 0;
                size_t textureIndex = 0;
                std::shared_ptr<BoxPackingNode> children[2];

                bool isBranch() const { return children[0].get(); }
                bool isOccupied() const { return uid != 0; }

                std::shared_ptr<BoxPackingNode> insert(const std::shared_ptr<Image::Data> &);
            };

            TextureAtlas::BoxPackingNode::BoxPackingNode(int border) :
                timestamp(++_timestamp),
                border(border)
            {}

            std::shared_ptr<TextureAtlas::BoxPackingNode> TextureAtlas::BoxPackingNode::create(int border)
            {
                auto out = std::shared_ptr<BoxPackingNode>(new BoxPackingNode(border));
                return out;
            }

            std::shared_ptr<TextureAtlas::BoxPackingNode> TextureAtlas::BoxPackingNode::insert(const std::shared_ptr<Image::Data> & data)
            {
                if (isBranch())
                {
                    if (auto node = children[0]->insert(data))
                    {
                        return node;
                    }
                    return children[1]->insert(data);
                }
                else if (isOccupied())
                {
                    return nullptr;
                }
                else
                {
                    const glm::ivec2 & dataSize = glm::ivec2(data->getWidth(), data->getHeight()) + border * 2;
                    const glm::ivec2 & bboxSize = bbox.getSize();
                    if (dataSize.x > bboxSize.x || dataSize.y > bboxSize.y)
                    {
                        return nullptr;
                    }
                    if (dataSize == bboxSize)
                    {
                        return shared_from_this();
                    }
                    children[0] = create(border);
                    children[1] = create(border);
                    const int dw = bboxSize.x - dataSize.x;
                    const int dh = bboxSize.y - dataSize.y;
                    if (dw > dh)
                    {
                        children[0]->bbox.min.x = bbox.min.x;
                        children[0]->bbox.min.y = bbox.min.y;
                        children[0]->bbox.max.x = bbox.min.x + dataSize.x - 1;
                        children[0]->bbox.max.y = bbox.max.y;
                        children[1]->bbox.min.x = bbox.min.x + dataSize.x;
                        children[1]->bbox.min.y = bbox.min.y;
                        children[1]->bbox.max.x = bbox.max.x;
                        children[1]->bbox.max.y = bbox.max.y;
                    }
                    else
                    {
                        children[0]->bbox.min.x = bbox.min.x;
                        children[0]->bbox.min.y = bbox.min.y;
                        children[0]->bbox.max.x = bbox.max.x;
                        children[0]->bbox.max.y = bbox.min.y + dataSize.y - 1;
                        children[1]->bbox.min.x = bbox.min.x;
                        children[1]->bbox.min.y = bbox.min.y + dataSize.y;
                        children[1]->bbox.max.x = bbox.max.x;
                        children[1]->bbox.max.y = bbox.max.y;
                    }
                    children[0]->textureIndex = textureIndex;
                    children[1]->textureIndex = textureIndex;
                    return children[0]->insert(data);
                }
            }

            struct TextureAtlas::Private
            {
                size_t textureCount = 0;
                int textureSize = 0;
                Image::Type textureType = Image::Type::None;
                int border = 0;
                std::vector<std::shared_ptr<OpenGL::Texture> > textures;
                std::vector<std::shared_ptr<BoxPackingNode> > boxPackingNodes;
                std::map<UID, std::shared_ptr<BoxPackingNode> > cache;
            };

            TextureAtlas::TextureAtlas(size_t textureCount, int textureSize, Image::Type textureType, GLenum filter, int border) :
                _p(new Private)
            {
                DJV_PRIVATE_PTR();
                p.textureCount = textureCount;
                p.textureSize = textureSize;
                p.textureType = textureType;

                for (size_t i = 0; i < p.textureCount; ++i)
                {
                    auto texture = OpenGL::Texture::create(Image::Info(textureSize, textureSize, textureType), filter, filter);
                    p.textures.push_back(std::move(texture));

                    auto node = BoxPackingNode::create(border);
                    node->bbox.min.x = 0;
                    node->bbox.min.y = 0;
                    node->bbox.max.x = textureSize - 1;
                    node->bbox.max.y = textureSize - 1;
                    node->textureIndex = i;
                    p.boxPackingNodes.push_back(node);
                }
            }

            TextureAtlas::~TextureAtlas()
            {}

            size_t TextureAtlas::getTextureCount() const
            {
                return _p->textureCount;
            }

            int TextureAtlas::getTextureSize() const
            {
                return _p->textureSize;
            }

            Image::Type TextureAtlas::getTextureType() const
            {
                return _p->textureType;
            }

            std::vector<GLuint> TextureAtlas::getTextures() const
            {
                DJV_PRIVATE_PTR();
                std::vector<GLuint> out;
                for (const auto & i : p.textures)
                {
                    out.push_back(i->getID());
                }
                return out;
            }

            bool TextureAtlas::getItem(UID uid, TextureAtlasItem & out)
            {
                DJV_PRIVATE_PTR();
                const auto & i = p.cache.find(uid);
                if (i != p.cache.end())
                {
                    _toTextureAtlasItem(i->second, out);
                    return true;
                }
                return false;
            }

            UID TextureAtlas::addItem(const std::shared_ptr<Image::Data> & data, TextureAtlasItem & out)
            {
                DJV_PRIVATE_PTR();

                static UID _uid = 0;

                for (size_t i = 0; i < p.textureCount; ++i)
                {
                    if (auto node = p.boxPackingNodes[i]->insert(data))
                    {
                        // The data has been added to the atlas.
                        node->uid = ++_uid;
                        p.textures[node->textureIndex]->copy(*data, node->bbox.min.x + p.border, node->bbox.min.y + p.border);
                        p.cache[node->uid] = node;
                        _toTextureAtlasItem(node, out);
                        return node->uid;
                    }
                }

                // The atlas is full, over-write older data.
                std::vector<std::shared_ptr<BoxPackingNode> > nodes;
                for (size_t i = 0; i < p.textureCount; ++i)
                {
                    _getAllNodes(p.boxPackingNodes[i], nodes);
                }
                std::sort(nodes.begin(), nodes.end(),
                    [](const std::shared_ptr<BoxPackingNode>& a, const std::shared_ptr<BoxPackingNode>& b) -> bool
                {
                    return a->timestamp < b->timestamp;
                });
                const glm::ivec2 dataSize = glm::ivec2(data->getWidth(), data->getHeight()) + p.border * 2;
                for (auto node : nodes)
                {
                    const glm::ivec2 nodeSize = node->bbox.getSize();
                    if (dataSize.x <= nodeSize.x && dataSize.y <= nodeSize.y)
                    {
                        auto old = node;
                        _removeFromAtlas(old);
                        if (old->isBranch())
                        {
                            for (size_t i = 0; i < 2; ++i)
                            {
                                old->children[i].reset();
                            }
                        }
                        old->uid = 0;
                        old->timestamp = ++_timestamp;
                        if (auto node = old->insert(data))
                        {
                            node->uid = ++_uid;

                            //! \todo Do we need to zero out the old data?
                            //auto zero = Image::Data::create(Image::Info(data->getSize() + p.border * 2, p.textureType));
                            //zero->zero();
                            //p.textures[node->texture]->copy(zero, node->bbox.min);

                            p.textures[node->textureIndex]->copy(
                                *data,
                                static_cast<uint16_t>(node->bbox.min.x + p.border),
                                static_cast<uint16_t>(node->bbox.min.y + p.border));
                            p.cache[node->uid] = node;
                            _toTextureAtlasItem(node, out);

                            return node->uid;
                        }
                    }
                }
                return 0;
            }

            float TextureAtlas::getPercentageUsed() const
            {
                DJV_PRIVATE_PTR();
                float out = 0.F;
                for (size_t i = 0; i < p.textureCount; ++i)
                {
                    size_t used = 0;
                    std::vector<std::shared_ptr<BoxPackingNode> > leafs;
                    _getLeafNodes(p.boxPackingNodes[i], leafs);
                    for (const auto & i : leafs)
                    {
                        if (i->isOccupied())
                        {
                            used += i->bbox.getArea();
                        }
                    }
                    out += used;
                }
                return out / static_cast<float>(p.textureSize * p.textureSize) / static_cast<float>(p.textureCount) * 100.F;
            }

            void TextureAtlas::_getAllNodes(
                const std::shared_ptr<BoxPackingNode>& node,
                std::vector<std::shared_ptr<BoxPackingNode> > & out)
            {
                out.push_back(node);
                if (node->isBranch())
                {
                    _getAllNodes(node->children[0], out);
                    _getAllNodes(node->children[1], out);
                }
            }

            void TextureAtlas::_getLeafNodes(
                const std::shared_ptr<BoxPackingNode>& node,
                std::vector<std::shared_ptr<BoxPackingNode> > & out) const
            {
                if (node->isBranch())
                {
                    _getLeafNodes(node->children[0], out);
                    _getLeafNodes(node->children[1], out);
                }
                else
                {
                    out.push_back(node);
                }
            }

            void TextureAtlas::_toTextureAtlasItem(
                const std::shared_ptr<BoxPackingNode>& node,
                TextureAtlasItem & out)
            {
                DJV_PRIVATE_PTR();
                out.w = node->bbox.w();
                out.h = node->bbox.h();
                out.textureIndex = node->textureIndex;
                out.textureU = FloatRange(
                    (node->bbox.min.x + p.border)     / static_cast<float>(p.textureSize),
                    (node->bbox.max.x - p.border + 1) / static_cast<float>(p.textureSize));
                out.textureV = FloatRange(
                    (node->bbox.min.y + p.border)     / static_cast<float>(p.textureSize),
                    (node->bbox.max.y - p.border + 1) / static_cast<float>(p.textureSize));
            }

            void TextureAtlas::_removeFromAtlas(const std::shared_ptr<BoxPackingNode>& node)
            {
                DJV_PRIVATE_PTR();
                auto i = p.cache.find(node->uid);
                if (i != p.cache.end())
                {
                    node->uid = 0;
                    p.cache.erase(i);
                }
                if (node->isBranch())
                {
                    _removeFromAtlas(node->children[0]);
                    _removeFromAtlas(node->children[1]);
                }
            }

        } // namespace Render
    } // namespace AV
} // namespace djv
