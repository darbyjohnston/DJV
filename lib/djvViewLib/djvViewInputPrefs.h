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

//! \file djvViewInputPrefs.h

#ifndef DJV_VIEW_INPUT_PREFS_H
#define DJV_VIEW_INPUT_PREFS_H

#include <djvViewAbstractPrefs.h>
#include <djvViewUtil.h>

//! \addtogroup djvViewMisc
//@{

//------------------------------------------------------------------------------
//! \class djvViewInputPrefs
//!
//! This class provides the input preferences.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewInputPrefs : public djvViewAbstractPrefs
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvViewInputPrefs(djvViewContext *, QObject * parent = 0);

    //! Destructor.

    virtual ~djvViewInputPrefs();
    
    //! Get the default mouse wheel action.
    
    static djvViewUtil::MOUSE_WHEEL mouseWheelDefault();

    //! Get the mouse wheel action.

    djvViewUtil::MOUSE_WHEEL mouseWheel() const;

    //! Get the default shift + mouse wheel action.
    
    static djvViewUtil::MOUSE_WHEEL mouseWheelShiftDefault();

    //! Get the shift + mouse wheel action.

    djvViewUtil::MOUSE_WHEEL mouseWheelShift() const;

    //! Get the default control + mouse wheel action.
    
    static djvViewUtil::MOUSE_WHEEL mouseWheelCtrlDefault();

    //! Get the control + mouse wheel action.

    djvViewUtil::MOUSE_WHEEL mouseWheelCtrl() const;

public Q_SLOTS:

    //! Set the mouse wheel action.

    void setMouseWheel(djvViewUtil::MOUSE_WHEEL);
    
    //! Set the shift + mouse wheel action.

    void setMouseWheelShift(djvViewUtil::MOUSE_WHEEL);
    
    //! Set the control + mouse wheel action.

    void setMouseWheelCtrl(djvViewUtil::MOUSE_WHEEL);

Q_SIGNALS:

    //! This signal is emitted when a preference is changed.

    void prefChanged();

private:

    djvViewUtil::MOUSE_WHEEL _mouseWheel;
    djvViewUtil::MOUSE_WHEEL _mouseWheelShift;
    djvViewUtil::MOUSE_WHEEL _mouseWheelCtrl;
};

//@} // djvViewMisc

#endif // DJV_VIEW_INPUT_PREFS_H

