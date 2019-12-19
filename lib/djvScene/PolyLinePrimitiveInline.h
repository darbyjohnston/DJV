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

namespace djv
{
    namespace Scene
    {
        inline PolyLinePrimitive::PolyLinePrimitive()
        {}

        inline std::shared_ptr<PolyLinePrimitive> PolyLinePrimitive::create()
        {
            return std::shared_ptr<PolyLinePrimitive>(new PolyLinePrimitive);
        }

        inline void PolyLinePrimitive::setPointLists(const std::vector<std::shared_ptr<AV::Geom::PointList> >& value)
        {
            _pointLists = value;
            _pointCount = 0;
            Core::BBox3f bbox = getBBox();
            for (const auto& i : _pointLists)
            {
                auto j = i->v.begin();
                if (j != i->v.end())
                {
                    bbox.min = bbox.max = *j++;
                    for (; j != i->v.end(); ++j)
                    {
                        bbox.expand(*j);
                    }
                }
                _pointCount += i->v.size();
            }
            setBBox(bbox);
        }

        inline void PolyLinePrimitive::addPointList(const std::shared_ptr<AV::Geom::PointList>& value)
        {
            _pointLists.push_back(value);
            _pointCount += value->v.size();
            Core::BBox3f bbox = getBBox();
            auto i = value->v.begin();
            if (i != value->v.end())
            {
                bbox.min = bbox.max = *i++;
                for (; i != value->v.end(); ++i)
                {
                    bbox.expand(*i);
                }
            }
            setBBox(bbox);
        }

        inline std::string PolyLinePrimitive::getClassName() const
        {
            return "PolyLinePrimitive";
        }

        inline bool PolyLinePrimitive::isShaded() const
        {
            return false;
        }

        inline const std::vector<std::shared_ptr<AV::Geom::PointList> >& PolyLinePrimitive::getPolyLines() const
        {
            return _pointLists;
        }

        inline size_t PolyLinePrimitive::getPointCount() const
        {
            return _pointCount;
        }

    } // namespace Scene
} // namespace djv

