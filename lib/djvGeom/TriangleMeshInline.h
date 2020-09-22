// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Geom
    {
        inline Core::UID TriangleMesh::getUID() const
        {
            return _uid;
        }

        constexpr TriangleMesh::Vertex::Vertex(size_t v, size_t t, size_t n) :
            v(v),
            t(t),
            n(n)
        {}

        inline bool TriangleMesh::Vertex::operator == (const TriangleMesh::Vertex & other) const
        {
            return
                v == other.v &&
                t == other.t &&
                n == other.n;
        }

        inline bool TriangleMesh::Face::operator == (const TriangleMesh::Face & other) const
        {
            return v == other.v;
        }

        inline bool TriangleMesh::Triangle::operator == (const TriangleMesh::Triangle & other) const
        {
            return
                v0 == other.v0 &&
                v1 == other.v1 &&
                v2 == other.v2;
        }

    } // namespace Geom
} // namespace djv
