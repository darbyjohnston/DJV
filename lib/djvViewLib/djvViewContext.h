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

//! \file djvViewContext.h

#ifndef DJV_VIEW_CONTEXT_H
#define DJV_VIEW_CONTEXT_H

#include <djvViewUtil.h>

#include <djvGuiContext.h>

#include <djvSequence.h>
#include <djvPixelData.h>

#include <QScopedPointer>

struct djvViewContextPrivate;
class  djvViewFileCache;
class  djvViewFilePrefs;
class  djvViewFileSave;
class  djvViewImagePrefs;
class  djvViewInputPrefs;
class  djvViewPlaybackPrefs;
class  djvViewShortcutPrefs;
class  djvViewViewPrefs;
class  djvViewWindowPrefs;

//! \addtogroup djvViewHelp
//@{

//------------------------------------------------------------------------------
//! \class djvViewContext
//!
//! This class provides global functionality for the library.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewContext : public djvGuiContext
{
    Q_OBJECT
    
public:

    //! Constructor.
    
    explicit djvViewContext(QObject * parent = 0);

    //! Destructor.

    virtual ~djvViewContext();
    
    //! Get the list of inputs.
    
    const QStringList & input() const;

    //! Get whether to combine the inputs.
    
    bool hasCombine() const;

    //! Get the file sequencing.
    
    djvSequence::COMPRESS sequence() const;
    
    //! Get whether to automatically detext sequences.

    bool hasAutoSequence() const;
    
    //! Get the file layer.

    const QScopedPointer<int> & fileLayer() const;

    //! Get the file proxy.
    
    const QScopedPointer<djvPixelDataInfo::PROXY> & fileProxy() const;

    //! Get whether the file cache is enabled.
    
    const QScopedPointer<bool> & hasFileCache() const;

    //! Get whether the window is full screen.
    
    const QScopedPointer<bool> & isWindowFullScreen() const;

    //! Get the playback.
    
    const QScopedPointer<djvViewUtil::PLAYBACK> & playback() const;

    //! Get the playback frame.
    
    const QScopedPointer<int> & playbackFrame() const;

    //! Get the playback speed.
    
    const QScopedPointer<djvSpeed> & playbackSpeed() const;
    
    //! Get the file preferences.
    
    djvViewFilePrefs * filePrefs() const;
    
    //! Get the image preferences.
    
    djvViewImagePrefs * imagePrefs() const;
    
    //! Get the input preferences.
    
    djvViewInputPrefs * inputPrefs() const;
    
    //! Get the playback preferences.
    
    djvViewPlaybackPrefs * playbackPrefs() const;
    
    //! Get the shortcut preferences.
    
    djvViewShortcutPrefs * shortcutPrefs() const;
    
    //! Get the view preferences.
    
    djvViewViewPrefs * viewPrefs() const;
    
    //! Get the window preferences.
    
    djvViewWindowPrefs * windowPrefs() const;
    
    //! Get the file cache.
    
    djvViewFileCache * fileCache() const;
    
    //! Get the file save.
    
    djvViewFileSave * fileSave() const;

    virtual void setValid(bool);

protected:

    virtual bool commandLineParse(QStringList &) throw (QString);
    
    virtual QString commandLineHelp() const;

private:

    DJV_PRIVATE_COPY(djvViewContext);
    
    djvViewContextPrivate * _p;
};

//@} // djvViewHelp

#endif // DJV_VIEW_CONTEXT_H

