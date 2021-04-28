// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAudio/Type.h>

#include <string>

namespace djv
{
    namespace Audio
    {
        //! Default audio information name.
        const std::string defaultName = "Audio";

        //! Audio data information.
        class Info
        {
        public:
            Info();
            Info(
                uint8_t channelCount,
                Type    type,
                size_t  sampleRate);

            std::string name         = defaultName;
            uint8_t     channelCount = 0;
            Type        type         = Audio::Type::None;
            size_t      sampleRate   = 0;
            std::string codec;

            //! \name Information
            ///@{
            
            bool isValid() const;
            size_t getByteCount() const;

            ///@}

            bool operator == (const Info&) const;
            bool operator != (const Info&) const;
        };

    } // namespace Audio
} // namespace djv

#include <djvAudio/InfoInline.h>
