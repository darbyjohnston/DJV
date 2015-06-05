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

//! \file djvViewHistogramTool.h

#ifndef DJV_VIEW_HISTOGRAM_TOOL_H
#define DJV_VIEW_HISTOGRAM_TOOL_H

#include <djvViewAbstractTool.h>

#include <djvPixel.h>

struct djvViewHistogramToolPrivate;

//! \addtogroup djvViewTool
//@{

//------------------------------------------------------------------------------
//! \class djvViewHistogramTool
//!
//! This class provides a histogram tool.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewHistogramTool : public djvViewAbstractTool
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvViewHistogramTool(
        djvViewMainWindow * mainWindow,
        djvViewContext *    context,
        QWidget *           parent = 0);

    //! Destructor.

    virtual ~djvViewHistogramTool();

protected:

    virtual void showEvent(QShowEvent *);
    virtual void resizeEvent(QResizeEvent *);

private Q_SLOTS:

    void sizeCallback(int);
    void maskCallback(const djvPixel::Mask &);
    void colorProfileCallback(bool);
    void displayProfileCallback(bool);

    void widgetUpdate();
    
private:

    DJV_PRIVATE_COPY(djvViewHistogramTool);
    
    djvViewHistogramToolPrivate * _p;
};

//@} // djvViewTool

#endif // DJV_VIEW_HISTOGRAM_TOOL_H

