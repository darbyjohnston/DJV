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

#include "TextureCache.h"

#include <djvAV/OpenGLTexture.h>

#include <djvCore/BBox.h>

using namespace djv::Core;

using namespace gl;

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

            //! This struct provides the nodes used for box packing.
            //!
            //! References:
            //! - http://blackpawn.com/texts/lightmaps/
            struct TextureCache::BoxPackingNode
            {
                BoxPackingNode(int border);
                ~BoxPackingNode();

                UID uid = 0;
                uint64_t timestamp = 0;

                BBox2i bbox;
                int border = 0;
                GLuint texture = 0;
                BoxPackingNode* children[2] = { nullptr, nullptr };

                bool isBranch() const { return children[0]; }
                bool isOccupied() const { return uid != 0; }

                BoxPackingNode* insert(const std::shared_ptr<Image::Data>&);
            };

            TextureCache::BoxPackingNode::BoxPackingNode(int border) :
                timestamp(++_timestamp),
                border(border)
            {}

            TextureCache::BoxPackingNode::~BoxPackingNode()
            {
                delete children[0];
                delete children[1];
            }

            TextureCache::BoxPackingNode* TextureCache::BoxPackingNode::insert(const std::shared_ptr<Image::Data>& data)
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
                    const glm::ivec2& dataSize = data->getSize() + border * 2;
                    const glm::ivec2& bboxSize = bbox.getSize();
                    if (dataSize.x > bboxSize.x || dataSize.y > bboxSize.y)
                    {
                        return nullptr;
                    }
                    if (dataSize == bboxSize)
                    {
                        return this;
                    }
                    children[0] = new BoxPackingNode(border);
                    children[1] = new BoxPackingNode(border);
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
                    children[0]->texture = texture;
                    children[1]->texture = texture;
                    return children[0]->insert(data);
                }
            }

            struct TextureCache::Private
            {
                size_t textureCount = 0;
                int textureSize = 0;
                Image::Type textureType = Image::Type::None;
                int border = 0;
                std::vector<std::shared_ptr<OpenGL::Texture> > textures;
                std::vector<BoxPackingNode*> boxPackingNodes;
                std::map<UID, BoxPackingNode*> cache;
            };

            TextureCache::TextureCache(size_t textureCount, int textureSize, Image::Type textureType, GLenum filter, int border) :
                _p(new Private)
            {
                _p->textureCount = textureCount;
                _p->textureSize = textureSize;
                _p->textureType = textureType;

                for (size_t i = 0; i < _p->textureCount; ++i)
                {
                    auto texture = OpenGL::Texture::create(Image::Info(textureSize, textureSize, textureType), filter);
                    _p->textures.push_back(std::move(texture));

                    auto node = new BoxPackingNode(border);
                    node->bbox.min.x = 0;
                    node->bbox.min.y = 0;
                    node->bbox.max.x = textureSize - 1;
                    node->bbox.max.y = textureSize - 1;
                    node->texture = static_cast<GLuint>(i);
                    _p->boxPackingNodes.push_back(node);
                }
            }

            TextureCache::~TextureCache()
            {
                for (auto i : _p->boxPackingNodes)
                {
                    delete i;
                }
            }

            size_t TextureCache::getTextureCount() const
            {
                return _p->textureCount;
            }

            int TextureCache::getTextureSize() const
            {
                return _p->textureSize;
            }

            Image::Type TextureCache::getTextureType() const
            {
                return _p->textureType;
            }

            std::vector<GLuint> TextureCache::getTextures() const
            {
                std::vector<GLuint> out;
                for (const auto& i : _p->textures)
                {
                    out.push_back(i->getID());
                }
                return out;
            }

            bool TextureCache::getItem(UID uid, TextureCacheItem& out)
            {
                const auto& i = _p->cache.find(uid);
                if (i != _p->cache.end())
                {
                    _toTextureCacheItem(i->second, out);
                    return true;
                }
                return false;
            }

            UID TextureCache::addItem(const std::shared_ptr<Image::Data>& data, TextureCacheItem& out)
            {
                static UID _uid = 0;

                for (size_t i = 0; i < _p->textureCount; ++i)
                {
                    if (auto node = _p->boxPackingNodes[i]->insert(data))
                    {
                        // The data has been added to the cache.
                        node->uid = ++_uid;
                        _p->textures[node->texture]->copy(*data, node->bbox.min + _p->border);
                        _p->cache[node->uid] = node;
                        _toTextureCacheItem(node, out);
                        return node->uid;
                    }
                }

                // The cache is full, over-write older data.
                std::vector<BoxPackingNode*> nodes;
                for (size_t i = 0; i < _p->textureCount; ++i)
                {
                    _getAllNodes(_p->boxPackingNodes[i], nodes);
                }
                std::sort(nodes.begin(), nodes.end(),
                    [](const BoxPackingNode* a, const BoxPackingNode* b) -> bool
                {
                    return a->timestamp < b->timestamp;
                });
                const glm::ivec2 dataSize = data->getSize() + _p->border * 2;
                for (auto node : nodes)
                {
                    const glm::ivec2 nodeSize = node->bbox.getSize();
                    if (dataSize.x <= nodeSize.x && dataSize.y <= nodeSize.y)
                    {
                        auto old = node;
                        _removeFromCache(old);
                        if (old->isBranch())
                        {
                            for (size_t i = 0; i < 2; ++i)
                            {
                                delete old->children[i];
                                old->children[i] = nullptr;
                            }
                        }
                        old->uid = 0;
                        old->timestamp = ++_timestamp;
                        if (auto node = old->insert(data))
                        {
                            node->uid = ++_uid;

                            //! \todo [1.0 M] Do we need to zero out the old data?
                            //auto zero = Image::Data::create(Image::Info(data->getSize() + _p->border * 2, _p->textureType));
                            //zero->zero();
                            //_p->textures[node->texture]->copy(zero, node->bbox.min);

                            _p->textures[node->texture]->copy(*data, node->bbox.min + _p->border);
                            _p->cache[node->uid] = node;
                            _toTextureCacheItem(node, out);

                            return node->uid;
                        }
                    }
                }
                return 0;
            }

            float TextureCache::getPercentageUsed() const
            {
                float out = 0.f;
                for (size_t i = 0; i < _p->textureCount; ++i)
                {
                    size_t used = 0;
                    std::vector<const BoxPackingNode*> leafs;
                    _getLeafNodes(_p->boxPackingNodes[i], leafs);
                    for (const auto& i : leafs)
                    {
                        if (i->isOccupied())
                        {
                            used += i->bbox.getArea();
                        }
                    }
                    out += used;
                }
                return out / static_cast<float>(_p->textureSize * _p->textureSize) / static_cast<float>(_p->textureCount) * 100.f;
            }

            void TextureCache::_getAllNodes(BoxPackingNode* node, std::vector<BoxPackingNode*>& out)
            {
                out.push_back(node);
                if (node->isBranch())
                {
                    _getAllNodes(node->children[0], out);
                    _getAllNodes(node->children[1], out);
                }
            }

            void TextureCache::_getLeafNodes(const BoxPackingNode* node, std::vector<const BoxPackingNode*>& out) const
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

            void TextureCache::_toTextureCacheItem(const BoxPackingNode* node, TextureCacheItem& out)
            {
                out.size = node->bbox.getSize();
                out.texture = node->texture;
                out.textureU = Range::FloatRange(
                    (node->bbox.min.x + _p->border) / static_cast<float>(_p->textureSize),
                    (node->bbox.max.x - _p->border + 1) / static_cast<float>(_p->textureSize));
                out.textureV = Range::FloatRange(
                    (node->bbox.min.y + _p->border) / static_cast<float>(_p->textureSize),
                    (node->bbox.max.y - _p->border + 1) / static_cast<float>(_p->textureSize));
            }

            void TextureCache::_removeFromCache(BoxPackingNode* node)
            {
                auto i = _p->cache.find(node->uid);
                if (i != _p->cache.end())
                {
                    node->uid = 0;
                    _p->cache.erase(i);
                }
                if (node->isBranch())
                {
                    _removeFromCache(node->children[0]);
                    _removeFromCache(node->children[1]);
                }
            }

        } // namespace Render
    } // namespace AV
} // namespace djv
