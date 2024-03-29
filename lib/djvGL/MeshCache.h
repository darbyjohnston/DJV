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

        //! Mesh cache.
        class MeshCache
        {
            DJV_NON_COPYABLE(MeshCache);

        public:
            MeshCache(size_t vboSize, VBOType);
            ~MeshCache();

            //! \name Information
            ///@{

            size_t getVBOSize() const;
            VBOType getVBOType() const;
            float getPercentageUsed() const;

            ///@}

            //! \name Data
            ///@{

            const std::shared_ptr<VBO>& getVBO() const;
            const std::shared_ptr<VAO>& getVAO() const;

            bool get(Core::UID, Math::SizeTRange&);

            Core::UID add(const std::vector<uint8_t>&, Math::SizeTRange&);

            ///@}

        private:
            class BoxPackingNode;

            bool _find(size_t, Math::SizeTRange&);

            DJV_PRIVATE();
        };

    } // namespace GL
} // namespace djv
