//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvAV/WAVLoad.h>

#include <djvCore/StringUtil.h>

namespace djv
{
    namespace AV
    {
        WAVLoad::WAVLoad(const Core::FileInfo & fileInfo, const QPointer<Core::CoreContext> & context) :
            Load(fileInfo, context)
        {
            //DJV_DEBUG("WAVLoad::WAVLoad");
            //DJV_DEBUG_PRINT("file info = " << fileInfo);

            _drwav = drwav_open_file(fileInfo.fileName().toUtf8().data());
            if (!_drwav)
            {
                throw Core::Error(
                    WAV::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_OPEN]);
            }

            _ioInfo.audio.type        = Audio::intType(_drwav->bytesPerSample);
            _ioInfo.audio.channels    = _drwav->channels;
            _ioInfo.audio.sampleRate  = _drwav->sampleRate;
            _ioInfo.audio.sampleCount = _drwav->totalSampleCount;
        }

        WAVLoad::~WAVLoad()
        {}

        void WAVLoad::read(AudioData & data, const AudioIOInfo & ioInfo)
        {
            if (!drwav_seek_to_sample(_drwav, ioInfo.samplesOffset))
            {
                throw Core::Error(
                    WAV::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_READ]);
            }

            AudioInfo info;
            info.channels    = _ioInfo.audio.channels;
            info.type        = _ioInfo.audio.type;
            info.sampleRate  = _ioInfo.audio.sampleRate;
            info.sampleCount = !ioInfo.samplesSize ? _ioInfo.audio.sampleCount : ioInfo.samplesSize;
            data.set(info);

            drwav_uint64 read = 0;
            switch (info.type)
            {
            case Audio::S16: read = drwav_read_s16(_drwav, info.sampleCount, reinterpret_cast<drwav_int16 *>(data.data())); break;
            default: break;
            }
            if (read != info.sampleCount)
            {
                throw Core::Error(
                    WAV::staticName,
                    IOPlugin::errorLabels()[IOPlugin::ERROR_READ]);
            }
        }

    } // namespace AV
} // namespace djv
