// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2018-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace GL
    {
        inline const Image::Size& OffscreenBuffer::getSize() const
        {
            return _size;
        }

        inline Image::Type OffscreenBuffer::getColorType() const
        {
            return _colorType;
        }

        inline OffscreenDepthType OffscreenBuffer::getDepthType() const
        {
            return _depthType;
        }

        inline OffscreenSampling OffscreenBuffer::getSampling() const
        {
            return _sampling;
        }

        inline GLuint OffscreenBuffer::getID() const
        {
            return _id;
        }

        inline GLuint OffscreenBuffer::getColorID() const
        {
            return _colorID;
        }

        inline GLuint OffscreenBuffer::getDepthID() const
        {
            return _depthID;
        }

    } // namespace GL
} // namespace djv

