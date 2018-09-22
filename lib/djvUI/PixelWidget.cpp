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

#include <djvUI/PixelWidget.h>

#include <djvCore/SignalBlocker.h>

#include <QComboBox>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct PixelWidget::Private
        {
            Graphics::Pixel::PIXEL pixel = static_cast<Graphics::Pixel::PIXEL>(0);
            QComboBox * comboBox = nullptr;
        };

        PixelWidget::PixelWidget(QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->comboBox = new QComboBox;
            _p->comboBox->addItems(Graphics::Pixel::pixelLabels());

            QVBoxLayout * layout = new QVBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->comboBox);

            widgetUpdate();

            connect(_p->comboBox, SIGNAL(activated(int)), SLOT(widgetCallback(int)));
        }
        
        PixelWidget::~PixelWidget()
        {}

        Graphics::Pixel::PIXEL PixelWidget::pixel() const
        {
            return _p->pixel;
        }

        void PixelWidget::setPixel(Graphics::Pixel::PIXEL pixel)
        {
            if (pixel == _p->pixel)
                return;
            _p->pixel = pixel;
            widgetUpdate();
            Q_EMIT pixelChanged(_p->pixel);
        }

        void PixelWidget::widgetCallback(int in)
        {
            setPixel(static_cast<Graphics::Pixel::PIXEL>(in));
        }

        void PixelWidget::widgetUpdate()
        {
            _p->comboBox->setCurrentIndex(_p->pixel);
        }

    } // namespace UI
} // namespace djv
