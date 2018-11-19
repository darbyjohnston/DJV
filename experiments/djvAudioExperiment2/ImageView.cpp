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

#include <ImageView.h>

#include <Context.h>
#include <IO.h>

using namespace djv;

namespace djv
{
    namespace AudioExperiment2
    {
        namespace
        {
            const size_t timeout = 10;

        } // namespace

        ImageView::ImageView(const QPointer<Context> & context, QWidget * parent) :
            UI::ImageView(context.data(), parent),
            _context(context)
        {
            _timer = startTimer(10);
        }

        ImageView::~ImageView()
        {}

        void ImageView::timerEvent(QTimerEvent *)
        {
            auto queue = _context->ioQueue();
            std::unique_lock<std::mutex> lock(queue->mutex);
            if (_queueCV.wait_for(
                lock,
                std::chrono::milliseconds(timeout),
                [this, queue]
            {
                return !queue->video.empty();
            }))
            {
                setData(queue->video.front().second);
            }
        }

    } // namespace AudioExperiment2
} // namespace djv
