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

#include <djvViewLib/ViewLib.h>
#include <djvViewLib/Enum.h>

#include <djvGraphics/Image.h>

#include <djvCore/Util.h>

#include <QObject>
#include <QPointer>

#include <memory>

namespace djv
{
    namespace Core
    {
        class FileInfo;
        class Speed;

    } // namespace Core

    namespace Graphics
    {
        class Image;
        class ImageIOInfo;
        class OpenGLImageOptions;

    } // Graphics

    namespace ViewLib
    {
        class FileGroup;
        class HelpGroup;
        class ImageGroup;
        class ImageView;
        class MainWindow;
        class PlaybackGroup;
        class ToolGroup;
        class ViewContext;
        class ViewGroup;
        class WindowGroup;

        class Session : public QObject
        {
            Q_OBJECT

        public:
            explicit Session(
                const QPointer<Session> & copy,
                const QPointer<ViewContext> &,
                QObject * parent = nullptr);
            ~Session() override;

            QPointer<FileGroup> fileGroup() const;
            QPointer<WindowGroup> windowGroup() const;
            QPointer<ViewGroup> viewGroup() const;
            QPointer<ImageGroup> imageGroup() const;
            QPointer<PlaybackGroup> playbackGroup() const;
            QPointer<ToolGroup> toolGroup() const;
            QPointer<HelpGroup> helpGroup() const;

            const std::shared_ptr<Graphics::Image> & image() const;
            Graphics::OpenGLImageOptions imageOptions() const;

            QPointer<MainWindow> mainWindow() const;
            const QPointer<ImageView> & viewWidget() const;

            void close();

            static QVector<QPointer<Session> > sessionList();
            
        public Q_SLOTS:
            //! Open a file.
            void fileOpen(const djv::Core::FileInfo &, bool init = true);

            //! Set the current file layer.
            void setFileLayer(int);

            //! Set the file proxy scale.
            void setFileProxy(djv::Graphics::PixelDataInfo::PROXY);

            //! Set whether the file cache is enabled.
            void setFileCacheEnabled(bool);

            //! Set full screen enabled.
            void showFullScreen();

            //! Set the playback.
            void setPlayback(djv::ViewLib::Enum::PLAYBACK);

            //! Set the playback frame.
            void setPlaybackFrame(qint64);

            //! Set the playback speed.
            void setPlaybackSpeed(const djv::Core::Speed &);

        Q_SIGNALS:
            //! This signal is emitted when the current file is changed.
            void fileChanged(bool);

            //! This signal is emitted when the image is changed.
            void imageChanged(const std::shared_ptr<djv::Graphics::Image> &);

            //! This signal is emitted when the image options are changed.
            void imageOptionsChanged(const djv::Graphics::OpenGLImageOptions &);

        private Q_SLOTS:
            void reloadFrameCallback();
            void exportSequenceCallback(const djv::Core::FileInfo &);
            void exportFrameCallback(const djv::Core::FileInfo &);
            void setFrameStoreCallback();

            void imageUpdate();
            void playbackUpdate();

        private:
            DJV_PRIVATE_COPY(Session);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
