//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            inline std::mutex & Queue::getMutex()
            {
                return _mutex;
            }

            inline size_t Queue::getVideoMax() const
            {
                return _finished ? 0 : _videoMax;
            }

            inline size_t Queue::getAudioMax() const
            {
                return _finished ? 0 : _audioMax;
            }

            inline size_t Queue::getVideoCount() const
            {
                return _video.size();
            }

            inline size_t Queue::getAudioCount() const
            {
                return _audio.size();
            }

            inline bool Queue::hasVideo() const
            {
                return _video.size() > 0;
            }

            inline bool Queue::hasAudio() const
            {
                return _audio.size() > 0;
            }

            inline VideoFrame Queue::getVideo() const
            {
                return _video.size() > 0 ? _video.front() : VideoFrame();
            }

            inline AudioFrame Queue::getAudio() const
            {
                return _audio.size() > 0 ? _audio.front() : AudioFrame();
            }

            inline bool Queue::isFinished() const
            {
                return _finished;
            }

            inline bool Queue::hasCloseOnFinish() const
            {
                return _closeOnFinish;
            }

            inline const std::string & IPlugin::getPluginName() const
            {
                return _pluginName;
            }

            inline const std::string & IPlugin::getPluginInfo() const
            {
                return _pluginInfo;
            }

            inline const std::set<std::string> & IPlugin::getFileExtensions() const
            {
                return _fileExtensions;
            }

        } // namespace IO
    } // namespace AV
} // namespace djv
