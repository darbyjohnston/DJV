// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvGL/GL.h>

#include <djvImage/Data.h>

namespace djv
{
    namespace GL
    {
        //! Get the OpenGL internal format.
        GLenum getInternalFormat2D(Image::Type);

        //! Get the OpenGL internal format.
        //GLenum getInternalFormat1D(Image::Type);
        
        //! Two-dimensonal OpenGL texture.
        class Texture2D
        {
            DJV_NON_COPYABLE(Texture2D);
            void _init(
                const Image::Info&,
                GLenum filterMin = GL_LINEAR,
                GLenum filterMag = GL_LINEAR);
            Texture2D();

        public:
            ~Texture2D();

            static std::shared_ptr<Texture2D> create(
                const Image::Info&,
                GLenum filterMin = GL_LINEAR,
                GLenum filterMag = GL_LINEAR);

            //! \name Information
            ///@{

            const Image::Info& getInfo() const;

            ///@}

            //! \name Data
            ///@{

            GLuint getID() const;

            void set(const Image::Info&);
            void copy(const Image::Data&);
            void copy(const Image::Data&, uint16_t x, uint16_t y);

            void bind();

            ///@}

        private:
            Image::Info _info;
            GLenum _filterMin = GL_LINEAR;
            GLenum _filterMag = GL_LINEAR;
            GLuint _id = 0;
#if defined(DJV_GL_PBO)
            GLuint _pbo = 0;
#endif // DJV_GL_PBO
        };

        //! One-dimensional OpenGL texture.
        /*class Texture1D
        {
            DJV_NON_COPYABLE(Texture1D);
            void _init(
                const Image::Info&,
                GLenum filter = GL_LINEAR);
            Texture1D();

        public:
            ~Texture1D();

            static std::shared_ptr<Texture1D> create(
                const Image::Info&,
                GLenum filter = GL_LINEAR);

            //! \name Information
            ///@{

            const Image::Info& getInfo() const;

            ///@}

            //! \name Data
            ///@{

            GLuint getID() const;

            void copy(const Image::Data&);
            void copy(const Image::Data&, uint16_t);

            void bind();

            ///@}

        private:
            Image::Info _info;
            GLenum _filter = GL_LINEAR;
            GLuint _id = 0;
#if defined(DJV_GL_PBO)
            GLuint _pbo = 0;
#endif // DJV_GL_PBO
        };*/

    } // namespace GL
} // namespace djv

#include <djvGL/TextureInline.h>

