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

#include <MainWindow.h>

#include <AudioSystem.h>
#include <Context.h>
#include <TimelineWidget.h>

#include <QDockWidget>

using namespace djv;

namespace djv
{
    namespace AudioExperiment2
    {
        MainWindow::MainWindow(const QPointer<Context> & context) :
            _context(context)
        {
            _imageView = new UI::ImageView(context.data());
            setCentralWidget(_imageView);

            auto timelineWidget = new TimelineWidget(context);
            auto timelineDockWidget = new QDockWidget;
            timelineDockWidget->setWidget(timelineWidget);
            addDockWidget(Qt::BottomDockWidgetArea, timelineDockWidget);

            timelineWidget->setDuration(context->audioSystem()->duration());
            timelineWidget->setCurrentTime(context->audioSystem()->currentTime());

            connect(
                context->audioSystem(),
                SIGNAL(currentTimeChanged(int64_t)),
                SLOT(currentTimeCallback(int64_t)));

            connect(
                context->audioSystem(),
                SIGNAL(durationChanged(int64_t)),
                timelineWidget,
                SLOT(setDuration(int64_t)));
            connect(
                context->audioSystem(),
                SIGNAL(currentTimeChanged(int64_t)),
                timelineWidget,
                SLOT(setCurrentTime(int64_t)));
            connect(
                context->audioSystem(),
                SIGNAL(playbackChanged(Util::PLAYBACK)),
                timelineWidget,
                SLOT(setPlayback(Util::PLAYBACK)));

            connect(
                timelineWidget,
                SIGNAL(playbackChanged(Util::PLAYBACK)),
                context->audioSystem(),
                SLOT(setPlayback(Util::PLAYBACK)));
        }

        MainWindow::~MainWindow()
        {}

        void MainWindow::currentTimeCallback(int64_t value)
        {
            DJV_DEBUG("MainWindow::currentTimeCallback");
            DJV_DEBUG_PRINT("value = " << value);
            {
                auto queue = _context->io()->queue();
                std::lock_guard<std::mutex> lock(queue->mutex);
                DJV_DEBUG_PRINT("queued = " << queue->video.size());
                if (queue->video.size())
                {
                    DJV_DEBUG_PRINT("front = " << queue->video.front().first);
                }
                if (queue->video.size() && queue->video.front().first < value)
                {
                    _videoFrame = queue->video.front();
                    queue->video.pop();
                }
            }
            if (_videoFrame.second)
            {
                _imageView->setData(&*_videoFrame.second);
            }
        }

    } // namespace AudioExperiment2
} // namespace djv
