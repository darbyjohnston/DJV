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

#pragma once

#include <djvUI/Core.h>

#include <djvGraphics/Pixel.h>

#include <djvCore/Util.h>

#include <QWidget>

#include <memory>

class djvUIContext;

//! \addtogroup djvUIWidget
//@{

//------------------------------------------------------------------------------
//! \class djvPixelMaskWidget
//!
//! This class provides a pixel mask widget.
//------------------------------------------------------------------------------

class djvPixelMaskWidget : public QWidget
{
    Q_OBJECT
    
    //! This property holds the pixel mask.    
    Q_PROPERTY(
        djvPixel::Mask mask
        READ           mask
        WRITE          setMask
        NOTIFY         maskChanged)
    
public:
    explicit djvPixelMaskWidget(djvUIContext *, QWidget * parent = nullptr);

    virtual ~djvPixelMaskWidget();

    //! Get the pixel mask.
    const djvPixel::Mask & mask() const;

public Q_SLOTS:
    //! Set the pixel mask.
    void setMask(const djvPixel::Mask &);

Q_SIGNALS:
    //! This signal is emitted when the pixel mask is changed.
    void maskChanged(const djvPixel::Mask &);

private Q_SLOTS:
    void buttonCallback();
    void soloCallback();
    void toggleCallback(bool);
    void resetCallback();

private:
    void widgetUpdate();
    
    DJV_PRIVATE_COPY(djvPixelMaskWidget);
    
    struct Private;
    std::unique_ptr<Private> _p;
};

//@} // djvUIWidget

