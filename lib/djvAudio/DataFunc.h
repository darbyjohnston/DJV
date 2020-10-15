// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAudio/Type.h>

#include <memory>

namespace djv
{
    namespace Audio
    {
        class Data;

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

#include <djvAudio/DataFuncInline.h>
