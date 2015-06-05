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

//! \file djvViewHelpGroup.h

#ifndef DJV_VIEW_HELP_GROUP_H
#define DJV_VIEW_HELP_GROUP_H

#include <djvViewAbstractGroup.h>

struct djvViewHelpGroupPrivate;

class QAction;

//! \addtogroup djvViewHelp
//@{

//------------------------------------------------------------------------------
//! \class djvViewHelpGroup
//!
//! This class provides the help group. The tool group encapsulates all of the
//! functionality relating to help.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewHelpGroup : public djvViewAbstractGroup
{
    Q_OBJECT
    
public:

    //! Constructor.

    djvViewHelpGroup(
        const djvViewHelpGroup * copy,
        djvViewMainWindow *      mainWindow,
        djvViewContext *         context);

    //! Destructor.

    virtual ~djvViewHelpGroup();

private Q_SLOTS:

    void helpCallback();
    void infoCallback();
    void aboutCallback();

private:

    DJV_PRIVATE_COPY(djvViewHelpGroup);
    
    djvViewHelpGroupPrivate * _p;
};

//@} // djvViewHelp

#endif // DJV_VIEW_HELP_GROUP_H

