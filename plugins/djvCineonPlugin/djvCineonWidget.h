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

//! \file djvCineonWidget.h

#ifndef DJV_CINEON_WIDGET_H
#define DJV_CINEON_WIDGET_H

#include <djvCineonPlugin.h>

#include <djvAbstractPrefsWidget.h>

class djvFloatEditSlider;
class djvIntEditSlider;

class QCheckBox;
class QComboBox;
class QFormLayout;

//! \addtogroup djvCineonPlugin
//@{

//------------------------------------------------------------------------------
//! \class djvCineonWidget
//!
//! This class proivdes a Cineon widget.
//------------------------------------------------------------------------------

class djvCineonWidget : public djvAbstractPrefsWidget
{
    Q_OBJECT
    
public:

    //! Constructor.

    djvCineonWidget(djvCineonPlugin *);

    //! Destructor.

    virtual ~djvCineonWidget();
    
    virtual void resetPreferences();

private Q_SLOTS:

    void pluginCallback(const QString &);
    void inputColorProfileCallback(int);
    void inputBlackPointCallback(int);
    void inputWhitePointCallback(int);
    void inputGammaCallback(double);
    void inputSoftClipCallback(int);
    void outputColorProfileCallback(int);
    void outputBlackPointCallback(int);
    void outputWhitePointCallback(int);
    void outputGammaCallback(double);

    void pluginUpdate();
    void widgetUpdate();

private:

    djvCineonPlugin *        _plugin;
    djvCineonPlugin::Options _options;
    QFormLayout *            _colorProfileLayout;
    QComboBox *              _inputColorProfileWidget;
    djvIntEditSlider *       _inputBlackPointWidget;
    djvIntEditSlider *       _inputWhitePointWidget;
    djvFloatEditSlider *     _inputGammaWidget;
    djvIntEditSlider *       _inputSoftClipWidget;
    QComboBox *              _outputColorProfileWidget;
    djvIntEditSlider *       _outputBlackPointWidget;
    djvIntEditSlider *       _outputWhitePointWidget;
    djvFloatEditSlider *     _outputGammaWidget;
};

//@} // djvCineonPlugin

#endif // DJV_CINEON_WIDGET_H

