// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAudio/Info.h>

#include <memory>
#include <vector>

namespace djv
{
    namespace Audio
    {
        //! Audio data.
        class Data
        {
            DJV_NON_COPYABLE(Data);

        protected:
            void _init(const Info&, size_t sampleCount);
            Data();

        public:
            static std::shared_ptr<Data> create(const Info&, size_t sampleCount);

            //! \name Information
            ///@{
            
            const Info& getInfo() const;
            uint8_t getChannelCount() const;
            Type getType() const;
            size_t getSampleRate() const;
            size_t getSampleCount() const;
            bool isValid() const;
            size_t getByteCount() const;

            ///@}
            
            //! \name Data
            ///@{
            
            uint8_t * getData();
            const uint8_t * getData() const;

            ///@}
            
            //! \name Utility
            ///@{
            
            void zero();

            ///@}
                        
            bool operator == (const Data&) const;
            bool operator != (const Data&) const;
            
        private:
            Info _info;
            size_t _sampleCount = 0;
            std::vector<uint8_t> _data;
        };

        //! \name Utility
        ///@{

        //! Adjust the volume of audio data.
        void volume(
            const uint8_t*,
            uint8_t*,
            float volume,
            size_t sampleCount,
            uint8_t channelCount,
            Type);

        //! Extract audio channels.
        template<typename T>
        void extract(
            const T*,
            T*,
            size_t sampleCount,
            uint8_t inChannelCount,
            uint8_t outChannelCount);

        ///@}
        
        //! \name Conversion
        ///@{
        
        //! Convert audio data.
        std::shared_ptr<Data> convert(const std::shared_ptr<Data>&, Type);

        //! Interleave audio data.
        std::shared_ptr<Data> planarInterleave(const std::shared_ptr<Data>&);

        //! Interleave audio data.
        template<typename T>
        void planarInterleave(const T**, T*, uint8_t channelCount, size_t sampleCount);

        //! De-interleave audio data.
        std::shared_ptr<Data> planarDeinterleave(const std::shared_ptr<Data>&);

        ///@}

    } // namespace Audio
} // namespace djv

#include <djvAudio/DataInline.h>
