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

//! \file djvViewImageGroup.h

#ifndef DJV_VIEW_IMAGE_GROUP_H
#define DJV_VIEW_IMAGE_GROUP_H

#include <djvViewAbstractGroup.h>
#include <djvViewDisplayProfile.h>
#include <djvViewUtil.h>

struct djvViewImageGroupPrivate;

class QAction;

//! \addtogroup djvViewImage
//@{

//------------------------------------------------------------------------------
//! \class djvViewImageGroup
//!
//! This class provides the image group. The image group encapsulates all of
//! the functionality relating to images such as the image scale and rotation.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewImageGroup : public djvViewAbstractGroup
{
    Q_OBJECT
    
public:

    //! Constructor.

    djvViewImageGroup(
        const djvViewImageGroup * copy,
        djvViewMainWindow *       mainWindow,
        djvViewContext *          context);

    //! Destructor.

    virtual ~djvViewImageGroup();

    //! Get whether the frame store is enabled.

    bool hasFrameStore() const;

    //! Get the mirror.

    const djvPixelDataInfo::Mirror & mirror() const;

    //! Get the scale.

    djvViewUtil::IMAGE_SCALE scale() const;

    //! Get the rotation.

    djvViewUtil::IMAGE_ROTATE rotate() const;

    //! Get whether the color profile is enabled.

    bool hasColorProfile() const;

    //! Get the display profile.

    const djvViewDisplayProfile & displayProfile() const;

    //! Get the image channel.

    djvOpenGlImageOptions::CHANNEL channel() const;

    virtual QToolBar * toolBar() const;

public Q_SLOTS:

    //! Set the display profile.

    void setDisplayProfile(const djvViewDisplayProfile &);

Q_SIGNALS:

    //! This signal is emitted when the frame store is changed.

    void frameStoreChanged(bool);

    //! This signal is emitted to store the current frame.

    void loadFrameStore();

    //! This signal is emitted when the display profile is changed.

    void displayProfileChanged(const djvViewDisplayProfile &);

    //! This signal is emitted when a redraw is needed.

    void redrawNeeded();

    //! This signal is emitted when a resize is needed.

    void resizeNeeded();

private Q_SLOTS:

    void frameStoreCallback(bool);
    void mirrorCallback(const djvPixelDataInfo::Mirror &);
    void mirrorHCallback(bool);
    void mirrorVCallback(bool);
    void scaleCallback(djvViewUtil::IMAGE_SCALE);
    void scaleCallback(QAction *);
    void rotateCallback(djvViewUtil::IMAGE_ROTATE);
    void rotateCallback(QAction *);
    void colorProfileCallback(bool);
    void displayProfileCallback(QAction *);
    void channelCallback(djvOpenGlImageOptions::CHANNEL);
    void channelCallback(QAction *);

    void update();

private:

    DJV_PRIVATE_COPY(djvViewImageGroup);
    
    djvViewImageGroupPrivate * _p;
};

//@} // djvViewImage

#endif // DJV_VIEW_IMAGE_GROUP_H

