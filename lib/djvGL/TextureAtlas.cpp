// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvGL/TextureAtlas.h>

#include <djvGL/Texture.h>

#include <djvMath/BBox.h>

#include <map>
#include <tuple>

using namespace djv::Core;

namespace djv
{
    namespace GL
    {
        namespace
        {
            uint64_t _timestamp = 0;

        } // namespace

        //! Box packing node.
        //!
        //! References:
        //! - http://blackpawn.com/texts/lightmaps/
        class TextureAtlas::BoxPackingNode : public std::enable_shared_from_this< TextureAtlas::BoxPackingNode>
        {
        private:
            explicit BoxPackingNode(int border);

        public:
            static std::shared_ptr<BoxPackingNode> create(int border);

            UID uid = 0;
            uint64_t timestamp = 0;

            Math::BBox2i bbox;
            uint8_t border = 0;
            uint8_t textureIndex = 0;
            std::shared_ptr<BoxPackingNode> children[2];

            bool isBranch() const { return children[0].get(); }
            bool isOccupied() const { return uid != 0; }

            std::shared_ptr<BoxPackingNode> insert(const std::shared_ptr<Image::Data>&);
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

        std::shared_ptr<TextureAtlas::BoxPackingNode> TextureAtlas::BoxPackingNode::insert(const std::shared_ptr<Image::Data>& data)
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
                const glm::ivec2& dataSize = glm::ivec2(data->getWidth(), data->getHeight()) + border * 2;
                const glm::ivec2& bboxSize = bbox.getSize();
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
            uint8_t textureCount = 0;
            uint16_t textureSize = 0;
            Image::Type textureType = Image::Type::None;
            uint8_t border = 0;
            UID uid = 0;
            std::vector<std::shared_ptr<Texture2D> > textures;
            std::vector<std::shared_ptr<BoxPackingNode> > boxPackingNodes;
            std::map<UID, std::shared_ptr<BoxPackingNode> > cache;
        };

        TextureAtlas::TextureAtlas(uint8_t textureCount, uint16_t textureSize, Image::Type textureType, GLenum filter, uint8_t border) :
            _p(new Private)
        {
            DJV_PRIVATE_PTR();
            p.textureCount = textureCount;
            p.textureSize = textureSize;
            p.textureType = textureType;

            for (uint8_t i = 0; i < p.textureCount; ++i)
            {
                auto texture = Texture2D::create(Image::Info(textureSize, textureSize, textureType), filter, filter);
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

        uint8_t TextureAtlas::getTextureCount() const
        {
            return _p->textureCount;
        }

        uint16_t TextureAtlas::getTextureSize() const
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
            for (const auto& i : p.textures)
            {
                out.push_back(i->getID());
            }
            return out;
        }

        bool TextureAtlas::getItem(UID uid, TextureAtlasItem& out)
        {
            DJV_PRIVATE_PTR();
            const auto& i = p.cache.find(uid);
            if (i != p.cache.end())
            {
                _toTextureAtlasItem(i->second, out);
                i->second->timestamp = ++_timestamp;
                return true;
            }
            return false;
        }

        UID TextureAtlas::addItem(const std::shared_ptr<Image::Data>& data, TextureAtlasItem& out)
        {
            DJV_PRIVATE_PTR();

            for (uint8_t i = 0; i < p.textureCount; ++i)
            {
                if (auto node = p.boxPackingNodes[i]->insert(data))
                {
                    // The data has been added to the atlas.
                    node->uid = ++p.uid;
                    p.textures[node->textureIndex]->copy(*data, node->bbox.min.x + p.border, node->bbox.min.y + p.border);
                    p.cache[node->uid] = node;
                    _toTextureAtlasItem(node, out);
                    return node->uid;
                }
            }

            // The atlas is full, over-write older data.
            std::vector<std::shared_ptr<BoxPackingNode> > nodes;
            for (uint8_t i = 0; i < p.textureCount; ++i)
            {
                _getAllNodes(p.boxPackingNodes[i], nodes);
            }
            std::sort(nodes.begin(), nodes.end(),
                [](const std::shared_ptr<BoxPackingNode>& a, const std::shared_ptr<BoxPackingNode>& b)
            {
                const int aArea = a->bbox.getArea();
                const int bArea = b->bbox.getArea();
                return std::tie(aArea, a->timestamp) < std::tie(bArea, b->timestamp);
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
                        for (uint8_t i = 0; i < 2; ++i)
                        {
                            old->children[i].reset();
                        }
                    }
                    old->uid = 0;
                    old->timestamp = ++_timestamp;
                    if (auto node2 = old->insert(data))
                    {
                        node2->uid = ++p.uid;

                        //! \todo Do we need to zero out the old data?
                        //auto zero = Image::Data::create(Image::Info(data->getSize() + p.border * 2, p.textureType));
                        //zero->zero();
                        //p.textures[node2->texture]->copy(zero, node2->bbox.min);

                        p.textures[node2->textureIndex]->copy(
                            *data,
                            static_cast<uint16_t>(node2->bbox.min.x + p.border),
                            static_cast<uint16_t>(node2->bbox.min.y + p.border));
                        p.cache[node2->uid] = node2;
                        _toTextureAtlasItem(node2, out);

                        return node2->uid;
                    }
                }
            }
            return 0;
        }

        float TextureAtlas::getPercentageUsed() const
        {
            DJV_PRIVATE_PTR();
            float out = 0.F;
            if (p.textureCount && p.textureSize)
            {
                for (uint8_t i = 0; i < p.textureCount; ++i)
                {
                    size_t used = 0;
                    std::vector<std::shared_ptr<BoxPackingNode> > leafs;
                    _getLeafNodes(p.boxPackingNodes[i], leafs);
                    for (const auto& j : leafs)
                    {
                        if (j->isOccupied())
                        {
                            used += j->bbox.getArea();
                        }
                    }
                    out += static_cast<float>(used);
                }
                out /= static_cast<float>(p.textureSize * p.textureSize);
                out /= static_cast<float>(p.textureCount);
                out *= 100.F;
            }
            return out;
        }

        void TextureAtlas::_getAllNodes(
            const std::shared_ptr<BoxPackingNode>& node,
            std::vector<std::shared_ptr<BoxPackingNode> >& out)
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
            std::vector<std::shared_ptr<BoxPackingNode> >& out) const
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
            TextureAtlasItem& out)
        {
            DJV_PRIVATE_PTR();
            out.w = node->bbox.w();
            out.h = node->bbox.h();
            out.textureIndex = node->textureIndex;
            out.textureU = Math::FloatRange(
                (node->bbox.min.x + static_cast<float>(p.border))       / static_cast<float>(p.textureSize),
                (node->bbox.max.x - static_cast<float>(p.border) + 1.F) / static_cast<float>(p.textureSize));
            out.textureV = Math::FloatRange(
                (node->bbox.min.y + static_cast<float>(p.border))       / static_cast<float>(p.textureSize),
                (node->bbox.max.y - static_cast<float>(p.border) + 1.F) / static_cast<float>(p.textureSize));
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

    } // namespace GL
} // namespace djv
