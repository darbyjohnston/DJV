// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvGL/Mesh.h>

#include <djvMath/Range.h>

#include <djvCore/UID.h>

namespace djv
{
    namespace GL
    {
        class VBO;
        class VAO;

        //! This class provides a mesh cache.
        class MeshCache
        {
            DJV_NON_COPYABLE(MeshCache);

        public:
            MeshCache(size_t vboSize, VBOType);
            ~MeshCache();

            size_t getVBOSize() const;
            VBOType getVBOType() const;
            const std::shared_ptr<VBO>& getVBO() const;
            const std::shared_ptr<VAO>& getVAO() const;

            bool getItem(Core::UID, Math::SizeTRange&);
            Core::UID addItem(const std::vector<uint8_t>&, Math::SizeTRange&);

            float getPercentageUsed() const;

        private:
            class BoxPackingNode;

            bool _find(size_t, Math::SizeTRange&);

            DJV_PRIVATE();
        };

    } // namespace GL
} // namespace djv
