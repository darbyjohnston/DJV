//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

//! \file djvViewImagePrefs.h

#ifndef DJV_VIEW_IMAGE_PREFS_H
#define DJV_VIEW_IMAGE_PREFS_H

#include <djvViewAbstractPrefs.h>
#include <djvViewDisplayProfile.h>
#include <djvViewUtil.h>

//! \addtogroup djvViewImage
//@{

//------------------------------------------------------------------------------
//! \class djvViewImagePrefs
//!
//! This class provides the image group preferences.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewImagePrefs : public djvViewAbstractPrefs
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvViewImagePrefs(djvViewContext *, QObject * parent = 0);

    //! Destructor.

    virtual ~djvViewImagePrefs();
    
    //! Get the default for whether to store the frame when reloading files.
    
    static bool frameStoreFileReloadDefault();

    //! Get whether to store the frame when reloading files.

    bool hasFrameStoreFileReload() const;

    //! Get the default mirror.
    
    static djvPixelDataInfo::Mirror mirrorDefault();
    
    //! Get the mirror.

    const djvPixelDataInfo::Mirror & mirror() const;

    //! Get the default scale.
    
    static djvViewUtil::IMAGE_SCALE scaleDefault();

    //! Get the scale.

    djvViewUtil::IMAGE_SCALE scale() const;

    //! Get the default rotation.
    
    static djvViewUtil::IMAGE_ROTATE rotateDefault();

    //! Get the rotation.

    djvViewUtil::IMAGE_ROTATE rotate() const;

    //! Get the default for whether the color profile is enabled.
    
    static bool colorProfileDefault();

    //! Get whether the color profile is enabled.

    bool hasColorProfile() const;

    //! Get the default display profile index.

    static int displayProfileIndexDefault();

    //! Get the display profile index.

    int displayProfileIndex() const;

    //! Get the current display profile.

    djvViewDisplayProfile displayProfile() const;

    //! Get the list of display profiles.

    const QVector<djvViewDisplayProfile> & displayProfiles() const;

    //! Get the display profile names.

    QStringList displayProfileNames() const;

    //! Get the default image channel.
    
    static djvOpenGlImageOptions::CHANNEL channelDefault();

    //! Get the image channel.

    djvOpenGlImageOptions::CHANNEL channel() const;

public Q_SLOTS:

    //! Set whether to store the frame when reloading files.

    void setFrameStoreFileReload(bool);

    //! Set the mirror.

    void setMirror(const djvPixelDataInfo::Mirror &);

    //! Set the scale.

    void setScale(djvViewUtil::IMAGE_SCALE);

    //! Set the rotation.

    void setRotate(djvViewUtil::IMAGE_ROTATE);

    //! Set whether the color profile is enabled.

    void setColorProfile(bool);

    //! Set the current display profile index.

    void setDisplayProfileIndex(int);

    //! Set the list of display profiles.

    void setDisplayProfiles(const QVector<djvViewDisplayProfile> &);

    //! Set the image channel.

    void setChannel(djvOpenGlImageOptions::CHANNEL);

Q_SIGNALS:

    //! This signal is emitted when the mirror is changed.

    void mirrorChanged(djvPixelDataInfo::Mirror);
    
    //! This signal is emitted when the scale is changed.

    void scaleChanged(djvViewUtil::IMAGE_SCALE);
    
    //! This signal is emitted when the rotation is changed.

    void rotateChanged(djvViewUtil::IMAGE_ROTATE);
    
    //! This signal is emitted when the color profile is changed.

    void colorProfileChanged(bool);
    
    //! This signal is emitted when the current display profile is
    //! changed.

    void displayProfileChanged(const djvViewDisplayProfile &);

    //! This signal is emitted the the display profiles are changed.

    void displayProfilesChanged(const QVector<djvViewDisplayProfile> &);

    //! This signal is emitted when the image channel is changed.

    void channelChanged(djvOpenGlImageOptions::CHANNEL);

private:

    bool                           _frameStoreFileReload;
    djvPixelDataInfo::Mirror       _mirror;
    djvViewUtil::IMAGE_SCALE       _scale;
    djvViewUtil::IMAGE_ROTATE      _rotate;
    bool                           _colorProfile;
    int                            _displayProfileIndex;
    QVector<djvViewDisplayProfile> _displayProfiles;
    djvOpenGlImageOptions::CHANNEL _channel;
};

//@} // djvViewImage

#endif // DJV_VIEW_IMAGE_PREFS_H

