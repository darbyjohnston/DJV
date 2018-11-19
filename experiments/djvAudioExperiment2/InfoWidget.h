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

#pragma once

#include <Util.h>

#include <QAbstractListModel>
#include <QPointer>
#include <QWidget>

class QLabel;
class QListView;

namespace djv
{
    namespace AudioExperiment2
    {
        class Context;

        class VideoFrameModel : public QAbstractListModel
        {
        public:
            VideoFrameModel(const std::shared_ptr<Util::AVQueue> &);

            int rowCount(const QModelIndex & parent = QModelIndex()) const override;
            QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

        protected:
            void timerEvent(QTimerEvent *) override;

        private:
            std::shared_ptr<Util::AVQueue> _queue;
            int _queueTimer = 0;
            std::vector<int64_t> _pts;
        };

        class AudioFrameModel : public QAbstractListModel
        {
        public:
            AudioFrameModel(const std::shared_ptr<Util::AVQueue> &);

            int rowCount(const QModelIndex & parent = QModelIndex()) const override;
            QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

        protected:
            void timerEvent(QTimerEvent *) override;

        private:
            std::shared_ptr<Util::AVQueue> _queue;
            int _queueTimer = 0;
            std::vector<int64_t> _pts;
        };

        class InfoWidget : public QWidget
        {
            Q_OBJECT

        public:
            InfoWidget(const QPointer<Context> &, QWidget * parent = nullptr);
            ~InfoWidget() override;

        protected:
            void timerEvent(QTimerEvent *) override;

        private Q_SLOTS:
            void widgetUpdate();

        private:
            QPointer<Context> _context;
            QPointer<QLabel> _currentTimeLabel;
            QPointer<QLabel> _durationLabel;
            QScopedPointer< VideoFrameModel> _queueVideoModel;
            QPointer<QListView> _queueVideoView;
            QPointer<QLabel> _queueVideoLabel;
            QScopedPointer< AudioFrameModel> _queueAudioModel;
            QPointer<QListView> _queueAudioView;
            QPointer<QLabel> _queueAudioLabel;
            QPointer<QLabel> _alUnqueuedBuffersLabel;
            int _queueTimer = 0;
        };

    } // namespace AudioExperiment2
} // namespace djv
