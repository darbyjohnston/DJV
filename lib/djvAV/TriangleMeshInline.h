// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace AV
    {
        namespace Geom
        {
            inline bool TriangleMesh::Vertex::operator == (const TriangleMesh::Vertex& other) const
            {
                return
                    v == other.v &&
                    t == other.t &&
                    n == other.n;
            }

            inline bool TriangleMesh::Face::operator == (const TriangleMesh::Face& other) const
            {
                return v == other.v;
            }

            inline bool TriangleMesh::Triangle::operator == (const TriangleMesh::Triangle& other) const
            {
                return
                    v0 == other.v0 &&
                    v1 == other.v1 &&
                    v2 == other.v2;
            }

            inline TriangleMesh::TriangleMesh() :
                _uid(Core::createUID())
            {}

            inline Core::UID TriangleMesh::getUID() const
            {
                return _uid;
            }

        } // namespace Geom
    } // namespace AV
} // namespace djv
