// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/OpenGL.h>
#include <djvAV/PointList.h>
#include <djvAV/TriangleMesh.h>

#include <djvCore/Enum.h>
#include <djvCore/Range.h>

#include <memory>

namespace djv
{
    namespace AV
    {
        namespace OpenGL
        {
            //! This enumeration provides the VBO vertex types.
            enum class VBOType
            {
                Pos2_F32_UV_U16,
                Pos3_F32,
                Pos3_F32_UV_U16,
                Pos3_F32_UV_U16_Normal_U10,
                Pos3_F32_UV_U16_Normal_U10_Color_U8,
                Pos3_F32_UV_F32_Normal_F32_Color_F32,
                Pos3_F32_Color_U8,

                Count,
                First = Pos2_F32_UV_U16
            };
            DJV_ENUM_HELPERS(VBOType);

            //! Get the VBO type byte count.
            size_t getVertexByteCount(VBOType) noexcept;

            //! This class provides an OpenGL vertex buffer object.
            class VBO
            {
                DJV_NON_COPYABLE(VBO);
                void _init(size_t size, VBOType);
                VBO();

            public:
                ~VBO();

                static std::shared_ptr<VBO> create(size_t size, VBOType);

                size_t getSize() const;
                VBOType getType() const;
                GLuint getID() const;

                void copy(const std::vector<uint8_t>&);
                void copy(const std::vector<uint8_t>&, size_t offset);
                void copy(const std::vector<uint8_t>&, size_t offset, size_t size);

                static std::vector<uint8_t> convert(const Geom::PointList&, VBOType);
                static std::vector<uint8_t> convert(const Geom::TriangleMesh&, VBOType);
                static std::vector<uint8_t> convert(const Geom::TriangleMesh&, VBOType, const Core::SizeTRange&);

            private:
                size_t _size = 0;
                VBOType _type = VBOType::First;
                GLuint _vbo = 0;
            };

            //! This class provides an OpenGL vertex array object.
            class VAO
            {
                DJV_NON_COPYABLE(VAO);
                void _init(VBOType, GLuint vbo);
                VAO();

            public:
                ~VAO();

                static std::shared_ptr<VAO> create(VBOType, GLuint vbo);

                GLuint getID() const;

                void bind();
                void draw(GLenum mode, size_t offset, size_t size);

            private:
                GLuint _vao = 0;
            };

        } // namespace OpenGL
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::OpenGL::VBOType);

} // namespace djv

#include <djvAV/OpenGLMeshInline.h>

