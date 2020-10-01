// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAudio/Audio.h>

#include <memory>
#include <string>
#include <vector>

namespace djv
{
    namespace Audio
    {
        //! This constant provides the default audio information name.
        const std::string defaultName = "Audio";

        //! This class provides audio data information.
        class Info
        {
        public:
            Info();
            Info(
                uint8_t channelCount,
                Type    type,
                size_t  sampleRate,
                size_t  sampleCount);

            std::string name            = defaultName;
            uint8_t     channelCount    = 0;
            Type        type            = Audio::Type::None;
            size_t      sampleRate      = 0;
            size_t      sampleCount     = 0;
            std::string codec;

            //! \name Information
            ///@{
            
            bool isValid() const;
            size_t getByteCount() const;

            ///@}

            bool operator == (const Info&) const;
            bool operator != (const Info&) const;
        };

        //! This class provides audio data.
        class Data
        {
            DJV_NON_COPYABLE(Data);

        protected:
            void _init(const Info&);
            Data();

        public:
            static std::shared_ptr<Data> create(const Info&);

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
            std::vector<uint8_t> _data;
        };

    } // namespace Audio
} // namespace djv

#include <djvAudio/AudioDataInline.h>
