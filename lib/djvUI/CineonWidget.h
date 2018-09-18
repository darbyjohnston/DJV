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

#include <djvUI/ImageIOWidget.h>

#include <djvGraphics/Cineon.h>

class QCheckBox;
class QComboBox;
class QFormLayout;
class QVBoxLayout;

namespace djv
{
    namespace UI
    {
        class FloatEditSlider;
        class IntEditSlider;

        //! \class CineonWidget
        //!
        //! This class proivdes a Cineon widget.
        class CineonWidget : public ImageIOWidget
        {
            Q_OBJECT

        public:
            CineonWidget(Graphics::ImageIO *, UIContext *);

            virtual ~CineonWidget();

            virtual void resetPreferences();

        private Q_SLOTS:
            void pluginCallback(const QString &);
            void inputColorProfileCallback(int);
            void inputBlackPointCallback(int);
            void inputWhitePointCallback(int);
            void inputGammaCallback(float);
            void inputSoftClipCallback(int);
            void outputColorProfileCallback(int);
            void outputBlackPointCallback(int);
            void outputWhitePointCallback(int);
            void outputGammaCallback(float);
            
            void sizeUpdate();
            void pluginUpdate();
            void widgetUpdate();

        private:
            Graphics::Cineon::Options _options;
            QFormLayout * _inputColorProfileLayout = nullptr;
            QComboBox * _inputColorProfileWidget = nullptr;
            IntEditSlider * _inputBlackPointWidget = nullptr;
            IntEditSlider * _inputWhitePointWidget = nullptr;
            FloatEditSlider * _inputGammaWidget = nullptr;
            IntEditSlider * _inputSoftClipWidget = nullptr;
            QFormLayout * _outputColorProfileLayout = nullptr;
            QComboBox * _outputColorProfileWidget = nullptr;
            IntEditSlider * _outputBlackPointWidget = nullptr;
            IntEditSlider * _outputWhitePointWidget = nullptr;
            FloatEditSlider * _outputGammaWidget = nullptr;
            QVBoxLayout * _layout = nullptr;
        };

        //! \class CineonWidgetPlugin
        //!
        //! This class provides a Cineon widget plugin.
        class CineonWidgetPlugin : public ImageIOWidgetPlugin
        {
        public:
            CineonWidgetPlugin(Core::CoreContext *);

            virtual ImageIOWidget * createWidget(Graphics::ImageIO *) const;
            virtual QString pluginName() const;
        };

    } // namespace UI
} // namespace djv
