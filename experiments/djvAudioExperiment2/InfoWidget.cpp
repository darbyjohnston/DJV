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

#include <InfoWidget.h>

#include <Context.h>
#include <IO.h>
#include <PlaybackSystem.h>
#include <Util.h>

#include <QFormLayout>
#include <QLabel>
#include <QListView>

using namespace djv;

namespace djv
{
    namespace AudioExperiment2
    {
        VideoFrameModel::VideoFrameModel(const std::shared_ptr<Util::AVQueue> & queue) :
            _queue(queue)
        {
            _queueTimer = startTimer(100);
        }

        int VideoFrameModel::rowCount(const QModelIndex & parent) const
        {
            return static_cast<int>(_pts.size());
        }

        QVariant VideoFrameModel::data(const QModelIndex & index, int role) const
        {
            if (!index.isValid())
                return QVariant();
            if (Qt::DisplayRole == role)
                return static_cast<qlonglong>(_pts[index.row()]);
            return QVariant();
        }

        void VideoFrameModel::timerEvent(QTimerEvent *)
        {
            beginResetModel();
            _pts.clear();
            {
                std::lock_guard<std::mutex> lock(_queue->mutex);
                _pts.resize(_queue->video.size());
                size_t j = 0;
                for (auto i = _queue->video.begin(); i != _queue->video.end(); ++i, ++j)
                {
                    _pts[j] = i->first;
                }
            }
            endResetModel();
        }

        AudioFrameModel::AudioFrameModel(const std::shared_ptr<Util::AVQueue> & queue) :
            _queue(queue)
        {
            _queueTimer = startTimer(100);
        }

        int AudioFrameModel::rowCount(const QModelIndex & parent) const
        {
            return static_cast<int>(_pts.size());
        }

        QVariant AudioFrameModel::data(const QModelIndex & index, int role) const
        {
            if (!index.isValid())
                return QVariant();
            if (Qt::DisplayRole == role)
                return static_cast<qlonglong>(_pts[index.row()]);
            return QVariant();
        }

        void AudioFrameModel::timerEvent(QTimerEvent *)
        {
            beginResetModel();
            _pts.clear();
            {
                std::lock_guard<std::mutex> lock(_queue->mutex);
                _pts.resize(_queue->audio.size());
                size_t j = 0;
                for (auto i = _queue->audio.begin(); i != _queue->audio.end(); ++i, ++j)
                {
                    _pts[j] = i->first;
                }
            }
            endResetModel();
        }

        InfoWidget::InfoWidget(const QPointer<Context> & context, QWidget * parent) :
            QWidget(parent),
            _context(context)
        {
            _currentTimeLabel = new QLabel;
            _durationLabel = new QLabel;

            _queueVideoModel.reset(new VideoFrameModel(context->ioQueue()));
            _queueVideoView = new QListView;
            _queueVideoView->setModel(_queueVideoModel.data());
            _queueVideoLabel = new QLabel;

            _queueAudioModel.reset(new AudioFrameModel(context->ioQueue()));
            _queueAudioView = new QListView;
            _queueAudioView->setModel(_queueAudioModel.data());
            _queueAudioLabel = new QLabel;

            _alUnqueuedBuffersLabel = new QLabel;

            auto layout = new QFormLayout(this);
            layout->addRow("Current time:", _currentTimeLabel);
            layout->addRow("Duration:", _durationLabel);
            layout->addRow("Video frames:", _queueVideoLabel);
            layout->addRow(_queueVideoView);
            layout->addRow("Audio frames:", _queueAudioLabel);
            layout->addRow(_queueAudioView);
            layout->addRow("OpenAL unqueued buffers:", _alUnqueuedBuffersLabel);

            widgetUpdate();

            _queueTimer = startTimer(100);
        }

        InfoWidget::~InfoWidget()
        {}

        void InfoWidget::timerEvent(QTimerEvent *)
        {
            widgetUpdate();
        }

        void InfoWidget::widgetUpdate()
        {
            _currentTimeLabel->setText(QString("%1").arg(_context->playbackSystem()->currentTime()));
            _durationLabel->setText(QString("%1").arg(_context->playbackSystem()->duration()));
            {
                auto queue = _context->ioQueue();
                std::lock_guard<std::mutex> lock(queue->mutex);
                _queueVideoLabel->setText(QString("%1").arg(queue->video.size()));
                _queueAudioLabel->setText(QString("%1").arg(queue->audio.size()));
            }
            _alUnqueuedBuffersLabel->setText(QString("%1").arg(_context->playbackSystem()->alUnqueuedBuffers()));
        }

    } // namespace AudioExperiment2
} // namespace djv
