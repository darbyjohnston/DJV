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

//! \file djvViewAbstractToolBar.h

#ifndef DJV_VIEW_ABSTRACT_TOOL_BAR_H
#define DJV_VIEW_ABSTRACT_TOOL_BAR_H

#include <djvViewLibExport.h>

#include <djvUtil.h>

#include <QToolBar>

class  djvViewAbstractActions;
struct djvViewAbstractToolBarPrivate;
class  djvViewContext;

//! \addtogroup djvViewLib
//@{

//------------------------------------------------------------------------------
//! \class djvViewAbstractToolBar
//!
//! This class provides the base functionality for group tool bars.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewAbstractToolBar : public QToolBar
{
    Q_OBJECT

public:

    //! Constructor.

    explicit djvViewAbstractToolBar(
        djvViewAbstractActions * actions,
        djvViewContext *         context,
        QWidget *                parent  = 0);

    //! Destructor.

    virtual ~djvViewAbstractToolBar() = 0;

    //! Get the actions.

    djvViewAbstractActions * actions() const;

    //! Get the context.
    
    djvViewContext * context() const;
    
private:

    DJV_PRIVATE_COPY(djvViewAbstractToolBar);
    
    djvViewAbstractToolBarPrivate * _p;
};

//@} // djvViewLib

#endif // DJV_VIEW_ABSTRACT_TOOL_BAR_H

