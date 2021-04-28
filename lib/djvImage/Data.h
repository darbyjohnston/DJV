// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Color.h>
#include <djvImage/Info.h>
#include <djvImage/Tags.h>

#include <djvCore/UID.h>

#include <memory>

namespace djv
{
    namespace Image
    {
        //! This class provides image data.
        class Data
        {
            DJV_NON_COPYABLE(Data);

        protected:
            void _init(const Info&);
            Data();

        public:
            ~Data();

            static std::shared_ptr<Data> create(const Info&);

            //! \name Information
            ///@{

            Core::UID getUID() const;

            const Info& getInfo() const;
            const Size& getSize() const;
            uint16_t getWidth() const;
            uint16_t getHeight() const;
            float getAspectRatio() const;

            Type getType() const;
            GLenum getGLFormat() const;
            GLenum getGLType() const;
            const Layout& getLayout() const;

            bool isValid() const;
            uint8_t getPixelByteCount() const;
            size_t getScanlineByteCount() const;
            size_t getDataByteCount() const;

            const std::string& getPluginName() const;

            void setPluginName(const std::string&);

            ///@}

            //! \name Data
            ///@{

            const uint8_t* getData() const;
            const uint8_t* getData(uint16_t y) const;
            const uint8_t* getData(uint16_t x, uint16_t y) const;
            uint8_t* getData();
            uint8_t* getData(uint16_t y);
            uint8_t* getData(uint16_t x, uint16_t y);

            ///@}

            //! \name Tags
            ///@{

            const Tags& getTags() const;

            void setTags(const Tags&);

            ///@}

            //! \name Utility
            ///@{

            void zero();

            ///@}

            bool operator == (const Data&) const;
            bool operator != (const Data&) const;

        private:
            Core::UID _uid = 0;
            Info _info;
            uint8_t _pixelByteCount = 0;
            size_t _scanlineByteCount = 0;
            size_t _dataByteCount = 0;
            std::string _pluginName;
            uint8_t* _data = nullptr;
            const uint8_t* _p = nullptr;
            Tags _tags;
        };

        //! \name Utility
        ///@{

        Color getAverageColor(const std::shared_ptr<Data>&);

        ///@}

    } // namespace Image
} // namespace djv

#include <djvImage/DataInline.h>
