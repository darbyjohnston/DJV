// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvGL/MeshCache.h>

#include <djvGL/MeshFunc.h>

#include <djvMath/Range.h>

#include <djvCore/UIDFunc.h>

#include <map>
#include <set>

using namespace djv::Core;

namespace djv
{
    namespace GL
    {
        namespace
        {
            uint64_t _timestamp = 0;

        } // namespace

        struct MeshCache::Private
        {
            size_t vboSize = 0;
            VBOType vboType = VBOType::Pos3_F32_UV_U16_Normal_U10;
            std::shared_ptr<VBO> vbo;
            std::shared_ptr<VAO> vao;
            std::map<Math::SizeTRange, UID> ranges;
            std::map<UID, Math::SizeTRange> uids;
            std::set<Math::SizeTRange> empty;
            std::map<uint64_t, UID> timestamps;
        };

        MeshCache::MeshCache(size_t vboSize, VBOType vboType) :
            _p(new Private)
        {
            DJV_PRIVATE_PTR();
            p.vboSize = vboSize;
            p.vboType = vboType;
            p.vbo = VBO::create(vboSize, vboType);
            p.vao = VAO::create(p.vbo->getType(), p.vbo->getID());
            p.empty.insert(Math::SizeTRange(0, p.vboSize));
        }

        MeshCache::~MeshCache()
        {}

        size_t MeshCache::getVBOSize() const
        {
            return _p->vboSize;
        }

        VBOType MeshCache::getVBOType() const
        {
            return _p->vboType;
        }

        float MeshCache::getPercentageUsed() const
        {
            DJV_PRIVATE_PTR();
            size_t used = 0;
            for (const auto& i : p.ranges)
            {
                used += i.first.getMax() - i.first.getMin() + 1;
            }
            return static_cast<float>(used) / static_cast<float>(p.vboSize) * 100.F;
        }

        const std::shared_ptr<VBO>& MeshCache::getVBO() const
        {
            return _p->vbo;
        }

        const std::shared_ptr<VAO>& MeshCache::getVAO() const
        {
            return _p->vao;
        }

        bool MeshCache::get(UID uid, Math::SizeTRange& range)
        {
            DJV_PRIVATE_PTR();
            const auto i = p.uids.find(uid);
            if (i != p.uids.end())
            {
                const auto j = p.timestamps.find(uid);
                if (j != p.timestamps.end())
                {
                    j->second = ++_timestamp;
                }
                range = i->second;
                return true;
            }
            return false;
        }

        UID MeshCache::add(const std::vector<uint8_t>& data, Math::SizeTRange& range)
        {
            DJV_PRIVATE_PTR();
            
            UID out = 0;

            const size_t vertexByteCount = getVertexByteCount(p.vboType);
            const size_t dataSize = data.size() / vertexByteCount;
            if (_find(dataSize, range))
            {
                out = createUID();
                p.ranges[range] = out;
                p.uids[out] = range;
                ++_timestamp;
                p.timestamps[_timestamp] = out;
                p.vbo->copy(data, range.getMin() * vertexByteCount);
            }
            else
            {
                auto i = p.timestamps.begin();
                while (i != p.timestamps.end())
                {
                    const auto j = p.uids.find(i->second);
                    if (j != p.uids.end())
                    {
                        const auto k = p.ranges.find(j->second);
                        if (k != p.ranges.end())
                        {
                            bool intersects = false;
                            for (auto l = p.empty.begin(); l != p.empty.end(); ++l)
                            {
                                if (k->first.intersects(*l))
                                {
                                    intersects = true;
                                    Math::SizeTRange newRange = k->first;
                                    newRange.expand(*l);
                                    p.empty.erase(l);
                                    p.empty.insert(newRange);
                                }
                            }
                            if (!intersects)
                            {
                                p.empty.insert(k->first);
                            }
                            p.ranges.erase(k);
                        }
                        p.uids.erase(j);
                    }
                    i = p.timestamps.erase(i);

                    if (_find(dataSize, range))
                    {
                        out = createUID();
                        p.ranges[range] = out;
                        p.uids[out] = range;
                        ++_timestamp;
                        p.timestamps[_timestamp] = out;
                        p.vbo->copy(data, range.getMin() * vertexByteCount);
                        break;
                    }
                }
            }
            
            return out;
        }

        bool MeshCache::_find(size_t size, Math::SizeTRange& range)
        {
            DJV_PRIVATE_PTR();
            for (auto i = p.empty.begin(); i != p.empty.end(); ++i)
            {
                const size_t emptySize = i->getMax() - i->getMin() + 1;
                if (size == emptySize)
                {
                    range = *i;
                    p.empty.erase(i);
                    return true;
                }
                else if (size < emptySize)
                {
                    range = Math::SizeTRange(i->getMin(), i->getMin() + size - 1);
                    Math::SizeTRange empty(range.getMax() + 1, i->getMax());
                    p.empty.erase(i);
                    p.empty.insert(empty);
                    return true;
                }
            }
            return false;
        }

    } // namespace GL
} // namespace djv
