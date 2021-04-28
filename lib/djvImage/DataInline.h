// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImage/Type.h>

namespace djv
{
    namespace Image
    {
        inline Core::UID Data::getUID() const
        {
            return _uid;
        }

        inline bool Data::isValid() const
        {
            return _info.isValid();
        }

        inline const Info& Data::getInfo() const
        {
            return _info;
        }

        inline const Size& Data::getSize() const
        {
            return _info.size;
        }

        inline uint16_t Data::getWidth() const
        {
            return _info.size.w;
        }

        inline uint16_t Data::getHeight() const
        {
            return _info.size.h;
        }

        inline float Data::getAspectRatio() const
        {
            return _info.getAspectRatio();
        }

        inline Type Data::getType() const
        {
            return _info.type;
        }

        inline GLenum Data::getGLFormat() const
        {
            return _info.getGLFormat();
        }

        inline GLenum Data::getGLType() const
        {
            return _info.getGLType();
        }

        inline const Layout& Data::getLayout() const
        {
            return _info.layout;
        }

        inline uint8_t Data::getPixelByteCount() const
        {
            return _pixelByteCount;
        }

        inline size_t Data::getScanlineByteCount() const
        {
            return _scanlineByteCount;
        }

        inline size_t Data::getDataByteCount() const
        {
            return _dataByteCount;
        }

        inline const std::string& Data::getPluginName() const
        {
            return _pluginName;
        }

        inline const uint8_t* Data::getData() const
        {
            return _p;
        }

        inline const uint8_t* Data::getData(uint16_t y) const
        {
            return _p + y * _scanlineByteCount;
        }

        inline const uint8_t* Data::getData(uint16_t x, uint16_t y) const
        {
            return _p + y * _scanlineByteCount + x * static_cast<size_t>(_pixelByteCount);
        }

        inline uint8_t* Data::getData()
        {
            return _data;
        }

        inline uint8_t* Data::getData(uint16_t y)
        {
            return _data + y * _scanlineByteCount;
        }

        inline uint8_t* Data::getData(uint16_t x, uint16_t y)
        {
            return _data + y * _scanlineByteCount + x * static_cast<size_t>(_pixelByteCount);
        }

        inline const Tags& Data::getTags() const
        {
            return _tags;
        }

    } // namespace Image
} // namespace djv
