//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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
            inline size_t VideoQueue::getMax() const
            {
                return _max;
            }

            inline bool VideoQueue::isEmpty() const
            {
                return 0 == _queue.size();
            }

            inline size_t VideoQueue::getCount() const
            {
                return _queue.size();
            }

            inline VideoFrame VideoQueue::getFrame() const
            {
                return _queue.size() ? _queue.front() : VideoFrame();
            }

            inline bool VideoQueue::isFinished() const
            {
                return _finished;
            }

            inline size_t AudioQueue::getMax() const
            {
                return _max;
            }

            inline bool AudioQueue::isEmpty() const
            {
                return 0 == _queue.size();
            }

            inline size_t AudioQueue::getCount() const
            {
                return _queue.size();
            }

            inline bool AudioQueue::isFinished() const
            {
                return _finished;
            }

            inline AudioFrame AudioQueue::getFrame() const
            {
                return _queue.size() ? _queue.front() : AudioFrame();
            }

            inline std::mutex& IIO::getMutex()
            {
                return _mutex;
            }

            inline VideoQueue& IIO::getVideoQueue()
            {
                return _videoQueue;
            }

            inline AudioQueue& IIO::getAudioQueue()
            {
                return  _audioQueue;
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
