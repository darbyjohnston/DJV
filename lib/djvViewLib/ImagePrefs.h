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

#pragma once

#include <djvViewLib/AbstractPrefs.h>
#include <djvViewLib/DisplayProfile.h>
#include <djvViewLib/Enum.h>

namespace djv
{
    namespace ViewLib
    {
        //! This class provides the image group preferences.
        class ImagePrefs : public AbstractPrefs
        {
            Q_OBJECT

        public:
            explicit ImagePrefs(const QPointer<ViewContext> &, QObject * parent = nullptr);
            ~ImagePrefs() override;

            //! Get the default for whether to store the frame when reloading files.    
            static bool frameStoreFileReloadDefault();

            //! Get whether to store the frame when reloading files.
            bool hasFrameStoreFileReload() const;

            //! Get the default mirror.
            static AV::PixelDataInfo::Mirror mirrorDefault();

            //! Get the mirror.
            const AV::PixelDataInfo::Mirror & mirror() const;

            //! Get the default scale.
            static Enum::IMAGE_SCALE scaleDefault();

            //! Get the scale.
            Enum::IMAGE_SCALE scale() const;

            //! Get the default rotation.
            static Enum::IMAGE_ROTATE rotateDefault();

            //! Get the rotation.
            Enum::IMAGE_ROTATE rotate() const;

            //! Get the default premultiplied alpha.
            static bool premultipliedAlphaDefault();

            //! Get the premultiplied alpha.
            bool hasPremultipliedAlpha() const;

            //! Get the default for whether the color profile is enabled.
            static bool colorProfileDefault();

            //! Get whether the color profile is enabled.
            bool hasColorProfile() const;

            //! Get the default display profile index.
            static int displayProfileIndexDefault();

            //! Get the display profile index.
            int displayProfileIndex() const;

            //! Get the current display profile.
            DisplayProfile displayProfile() const;

            //! Get the list of display profiles.
            const QVector<DisplayProfile> & displayProfiles() const;

            //! Get the display profile names.
            QStringList displayProfileNames() const;

            //! Get the default image channel.
            static AV::OpenGLImageOptions::CHANNEL channelDefault();

            //! Get the image channel.
            AV::OpenGLImageOptions::CHANNEL channel() const;

        public Q_SLOTS:
            //! Set whether to store the frame when reloading files.
            void setFrameStoreFileReload(bool);

            //! Set the mirror.
            void setMirror(const djv::AV::PixelDataInfo::Mirror &);

            //! Set the scale.
            void setScale(djv::ViewLib::Enum::IMAGE_SCALE);

            //! Set the rotation.
            void setRotate(djv::ViewLib::Enum::IMAGE_ROTATE);

            //! Set the premultiplied alpha.
            void setPremultipliedAlpha(bool);

            //! Set whether the color profile is enabled.
            void setColorProfile(bool);

            //! Set the current display profile index.
            void setDisplayProfileIndex(int);

            //! Set the list of display profiles.
            void setDisplayProfiles(const QVector<djv::ViewLib::DisplayProfile> &);

            //! Set the image channel.
            void setChannel(djv::AV::OpenGLImageOptions::CHANNEL);

        Q_SIGNALS:
            //! This signal is emitted when the mirror is changed.
            void mirrorChanged(djv::AV::PixelDataInfo::Mirror);

            //! This signal is emitted when the scale is changed.
            void scaleChanged(djv::ViewLib::Enum::IMAGE_SCALE);

            //! This signal is emitted when the rotation is changed.
            void rotateChanged(djv::ViewLib::Enum::IMAGE_ROTATE);

            //! This signal is emitted when the premultiplied alpha is changed.
            void premultipliedAlphaChanged(bool);

            //! This signal is emitted when the color profile is changed.
            void colorProfileChanged(bool);

            //! This signal is emitted when the current display profile is
            //! changed.
            void displayProfileChanged(const djv::ViewLib::DisplayProfile &);

            //! This signal is emitted the the display profiles are changed.
            void displayProfilesChanged(const QVector<djv::ViewLib::DisplayProfile> &);

            //! This signal is emitted when the image channel is changed.
            void channelChanged(djv::AV::OpenGLImageOptions::CHANNEL);

        private:
            bool                            _frameStoreFileReload;
            AV::PixelDataInfo::Mirror       _mirror;
            Enum::IMAGE_SCALE               _scale;
            Enum::IMAGE_ROTATE              _rotate;
            bool                            _premultipliedAlpha;
            bool                            _colorProfile;
            int                             _displayProfileIndex;
            QVector<DisplayProfile>         _displayProfiles;
            AV::OpenGLImageOptions::CHANNEL _channel;
        };

    } // namespace ViewLib
} // namespace djv
