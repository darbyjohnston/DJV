//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvGraphics/OpenEXR.h>

class QCheckBox;
class QComboBox;
class QFormLayout;
class QVBoxLayout;

namespace djv
{
    namespace UI
    {
        class FloatEditSlider;
        class IntEdit;

        //! This class provides an OpenEXR widget.
        class OpenEXRWidget : public ImageIOWidget
        {
            Q_OBJECT

        public:
            OpenEXRWidget(Graphics::ImageIO *, const QPointer<UIContext> &);

            void resetPreferences() override;

        private Q_SLOTS:
            void pluginCallback(const QString &);
            void threadsEnableCallback(bool);
            void threadCountCallback(int);
            void inputColorProfileCallback(int);
            void inputGammaCallback(float);
            void inputExposureCallback(float);
            void inputExposureDefogCallback(float);
            void inputExposureKneeLowCallback(float);
            void inputExposureKneeHighCallback(float);
            void channelsCallback(int);
            void compressionCallback(int);
            void dwaCompressionLevelCallback(float);

            void pluginUpdate();
            void widgetUpdate();

        private:
            Graphics::OpenEXR::Options  _options;
            QCheckBox * _threadsEnableWidget = nullptr;
            IntEdit * _threadCountWidget = nullptr;
            QComboBox * _inputColorProfileWidget = nullptr;
            QFormLayout * _inputColorProfileLayout = nullptr;
            FloatEditSlider * _inputGammaWidget = nullptr;
            FloatEditSlider * _inputExposureWidget = nullptr;
            FloatEditSlider * _inputExposureDefogWidget = nullptr;
            FloatEditSlider * _inputExposureKneeLowWidget = nullptr;
            FloatEditSlider * _inputExposureKneeHighWidget = nullptr;
            QComboBox * _channelsWidget = nullptr;
            QComboBox * _compressionWidget = nullptr;
#if OPENEXR_VERSION_HEX >= 0x02020000
            FloatEditSlider * _dwaCompressionLevelWidget = nullptr;
#endif // OPENEXR_VERSION_HEX
            QVBoxLayout * _layout = nullptr;
        };

        //! This class provides a OpenEXR widget plugin.
        class OpenEXRWidgetPlugin : public ImageIOWidgetPlugin
        {
        public:
            OpenEXRWidgetPlugin(const QPointer<Core::CoreContext> &);

            ImageIOWidget * createWidget(Graphics::ImageIO *) const override;
            QString pluginName() const override;
        };

    } // namespace UI
} // namespace djv

