// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvAV/OpenGLMesh.h>

#include <djvCore/Range.h>
#include <djvCore/UID.h>

namespace djv
{
    namespace AV
    {
        namespace OpenGL
        {
            class VBO;
            class VAO;

            //! This class provides a mesh cache.
            class MeshCache
            {
                DJV_NON_COPYABLE(MeshCache);

            public:
                MeshCache(size_t vboSize, OpenGL::VBOType);
                ~MeshCache();

                size_t getVBOSize() const;
                OpenGL::VBOType getVBOType() const;
                const std::shared_ptr<OpenGL::VBO>& getVBO() const;
                const std::shared_ptr<OpenGL::VAO>& getVAO() const;

                bool getItem(Core::UID, Core::SizeTRange&);
                Core::UID addItem(const std::vector<uint8_t>&, Core::SizeTRange&);

                float getPercentageUsed() const;

            private:
                class BoxPackingNode;

                bool _find(size_t, Core::SizeTRange&);

                DJV_PRIVATE();
            };

        } // namespace OpenGL
    } // namespace AV
} // namespace djv
