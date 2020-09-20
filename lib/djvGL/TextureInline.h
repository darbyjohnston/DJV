// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace GL
    {
        inline const Image::Info& Texture::getInfo() const
        {
            return _info;
        }

        inline GLuint Texture::getID() const
        {
            return _id;
        }

        /*inline const Image::Info& Texture1D::getInfo() const
        {
            return _info;
        }

        inline GLuint Texture1D::getID() const
        {
            return _id;
        }*/
            
    } // namespace GL
} // namespace djv

