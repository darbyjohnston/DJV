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

#include <djvAV/OpenGLMeshCache.h>

#include <djvCore/Range.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace OpenGL
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
                std::map<SizeTRange, UID> ranges;
                std::map<UID, SizeTRange> uids;
                std::set<SizeTRange> empty;
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
                p.empty.insert(SizeTRange(0, p.vboSize));
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

            const std::shared_ptr<VBO>& MeshCache::getVBO() const
            {
                return _p->vbo;
            }

            const std::shared_ptr<VAO>& MeshCache::getVAO() const
            {
                return _p->vao;
            }

            bool MeshCache::getItem(UID uid, SizeTRange& out)
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
                    out = i->second;
                    return true;
                }
                return false;
            }

            UID MeshCache::addItem(const std::vector<uint8_t>& data, SizeTRange& out)
            {
                DJV_PRIVATE_PTR();

                const size_t vertexByteCount = getVertexByteCount(p.vboType);
                const size_t dataSize = data.size() / vertexByteCount;
                if (_find(dataSize, out))
                {
                    const UID uid = createUID();
                    p.ranges[out] = uid;
                    p.uids[uid] = out;
                    ++_timestamp;
                    p.timestamps[_timestamp] = uid;
                    p.vbo->copy(data, out.min * vertexByteCount);
                    return uid;
                }

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
                                    SizeTRange newRange = k->first;
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

                    if (_find(dataSize, out))
                    {
                        const UID uid = createUID();
                        p.ranges[out] = uid;
                        p.uids[uid] = out;
                        ++_timestamp;
                        p.timestamps[_timestamp] = uid;
                        p.vbo->copy(data, out.min * vertexByteCount);
                        return uid;
                    }
                }

                return 0;
            }

            float MeshCache::getPercentageUsed() const
            {
                DJV_PRIVATE_PTR();
                size_t used = 0;
                for (const auto& i : p.ranges)
                {
                    used += i.first.max - i.first.min + 1;
                }
                return used / static_cast<float>(p.vboSize) * 100.F;
            }

            bool MeshCache::_find(size_t size, SizeTRange& out)
            {
                DJV_PRIVATE_PTR();
                for (auto i = p.empty.begin(); i != p.empty.end(); ++i)
                {
                    const size_t emptySize = i->max - i->min + 1;
                    if (size == emptySize)
                    {
                        out = *i;
                        p.empty.erase(i);
                        return true;
                    }
                    else if (size < emptySize)
                    {
                        out.min = i->min;
                        out.max = i->min + size - 1;
                        SizeTRange empty(out.max + 1, i->max);
                        p.empty.erase(i);
                        p.empty.insert(empty);
                        return true;
                    }
                }
                return false;
            }

        } // namespace OpenGL
    } // namespace AV
} // namespace djv
