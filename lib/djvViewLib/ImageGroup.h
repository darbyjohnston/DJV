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

#include <djvViewLib/AbstractGroup.h>
#include <djvViewLib/DisplayProfile.h>
#include <djvViewLib/Enum.h>

#include <memory>

class QAction;

namespace djv
{
    namespace ViewLib
    {
        //! This class provides the image group. The image group encapsulates all of
        //! the functionality relating to images such as the image scale and rotation.
        class ImageGroup : public AbstractGroup
        {
            Q_OBJECT

        public:
            ImageGroup(
                const QPointer<ImageGroup> & copy,
                const QPointer<MainWindow> &,
                const QPointer<Context> &);
            ~ImageGroup() override;

            //! Get whether the frame store is visible.
            bool isFrameStoreVisible() const;

            //! Get the mirror.
            const Graphics::PixelDataInfo::Mirror & mirror() const;

            //! Get the scale.
            Enum::IMAGE_SCALE scale() const;

            //! Get the rotation.
            Enum::IMAGE_ROTATE rotate() const;

            //! Get whether the color profile is enabled.
            bool hasColorProfile() const;

            //! Get the display profile.
            const DisplayProfile & displayProfile() const;

            //! Get the image channel.
            Graphics::OpenGLImageOptions::CHANNEL channel() const;

            QPointer<QToolBar> toolBar() const override;

        public Q_SLOTS:
            //! Set the display profile.
            void setDisplayProfile(const djv::ViewLib::DisplayProfile &);

        Q_SIGNALS:
            //! This signal is emitted when the frame store visibility is changed.
            void showFrameStoreChanged(bool);

            //! This signal is emitted to set the frame store.
            void setFrameStore();

            //! This signal is emitted when the display profile is changed.
            void displayProfileChanged(const djv::ViewLib::DisplayProfile &);

            //! This signal is emitted when a redraw is needed.
            void redrawNeeded();

            //! This signal is emitted when a resize is needed.
            void resizeNeeded();

        private Q_SLOTS:
            void showFrameStoreCallback(bool);
            void mirrorCallback(const djv::Graphics::PixelDataInfo::Mirror &);
            void mirrorHCallback(bool);
            void mirrorVCallback(bool);
            void scaleCallback(djv::ViewLib::Enum::IMAGE_SCALE);
            void scaleCallback(QAction *);
            void rotateCallback(djv::ViewLib::Enum::IMAGE_ROTATE);
            void rotateCallback(QAction *);
            void colorProfileCallback(bool);
            void displayProfileCallback(QAction *);
            void channelCallback(djv::Graphics::OpenGLImageOptions::CHANNEL);
            void channelCallback(QAction *);

            void update();

        private:
            DJV_PRIVATE_COPY(ImageGroup);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
