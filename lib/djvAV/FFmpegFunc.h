// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/FFmpeg.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace FFmpeg
            {
                Audio::Type toAudioType(AVSampleFormat);
                std::string toString(AVSampleFormat);

                void extractAudio(
                    uint8_t**                    inData,
                    int                          inFormat,
                    uint8_t                      inChannelCount,
                    std::shared_ptr<Audio::Data> out);

                std::string getErrorString(int);

            } // namespace FFmpeg
        } // namespace IO
    } // namespace AV

    rapidjson::Value toJSON(const AV::IO::FFmpeg::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::IO::FFmpeg::Options&);

} // namespace djv
