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

#include <djvViewLib/Enum.h>

#include <djvGraphics/Image.h>

#include <QMainWindow>

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
        class ImageIOInfo;
        class OpenGLImageOptions;

    } // Graphics

    namespace ViewLib
    {
        class ImageView;
        class ViewContext;

        //! This class provides a main window.
        //!
        //! \todo Move groups to the context.
        class MainWindow : public QMainWindow
        {
            Q_OBJECT

        public:
            explicit MainWindow(
                const QPointer<MainWindow> & copy,
                const QPointer<ViewContext> &);
            ~MainWindow() override;

            //! Get the image I/O information.
            const Graphics::ImageIOInfo & imageIOInfo() const;

            //! Get the view widget.
            const QPointer<ImageView> & viewWidget() const;

            //! Get the list of main windows.
            static QVector<QPointer<MainWindow> > mainWindowList();

            //! Create a new main window.
            static QPointer<MainWindow> createWindow(const QPointer<ViewContext> &);

        public Q_SLOTS:
            //! Open a file.
            void fileOpen(const djv::Core::FileInfo &, bool init = true);

            //! Set the file layer to open.
            void setFileLayer(int);

            //! Set the file proxy scale.
            void setFileProxy(djv::Graphics::PixelDataInfo::PROXY);

            //! Set whether the file cache is enabled.
            void setFileCacheEnabled(bool);

            //! Fit the window to the image.
            void fitWindow(bool move = true);

            //! Set the playback.
            void setPlayback(djv::ViewLib::Enum::PLAYBACK);

            //! Set the playback frame.
            void setPlaybackFrame(qint64);

            //! Set the playback speed.
            void setPlaybackSpeed(const djv::Core::Speed &);

        Q_SIGNALS:
            //! This signal is emitted when the image is changed.
            void imageChanged();

        protected:
            void showEvent(QShowEvent *) override;
            void closeEvent(QCloseEvent *) override;
            void keyPressEvent(QKeyEvent *) override;

        private Q_SLOTS:
            void windowResizeCallback();
            void enableUpdatesCallback();
            void reloadFrameCallback();
            void saveCallback(const djv::Core::FileInfo &);
            void saveFrameCallback(const djv::Core::FileInfo &);
            void setFrameStoreCallback();
            void pickCallback(const glm::ivec2 &);
            void mouseWheelCallback(djv::ViewLib::Enum::MOUSE_WHEEL);
            void mouseWheelValueCallback(int);

            void fileUpdate();
            void fileCacheUpdate();
            void imageUpdate();
            void windowUpdate();
            void viewOverlayUpdate();
            void viewPickUpdate();
            void playbackUpdate();

        private:
            //! Get the current image.
            const std::shared_ptr<Graphics::Image> & image() const;

            //! Get the image drawing options.
            Graphics::OpenGLImageOptions imageOptions() const;

            DJV_PRIVATE_COPY(MainWindow);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
