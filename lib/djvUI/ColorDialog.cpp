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

#include <djvUI/ColorDialog.h>

#include <djvUI/ColorSwatch.h>
#include <djvUI/ColorWidget.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct ColorDialog::Private
        {
            Private(const Graphics::Color & color, UIContext * context) :
                color(color),
                context(context)
            {}

            Graphics::Color color;
            ColorWidget * widget = nullptr;
            ColorSwatch * swatch = nullptr;
            UIContext * context = nullptr;
        };

        void ColorDialog::init()
        {
            // Create the widgets.
            _p->widget = new ColorWidget(_p->context);

            _p->swatch = new ColorSwatch(_p->context);
            _p->swatch->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

            QDialogButtonBox * buttonBox = new QDialogButtonBox(
                QDialogButtonBox::Ok);

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);

            QHBoxLayout * hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            hLayout->addWidget(_p->swatch);
            hLayout->addWidget(_p->widget, 1);
            layout->addLayout(hLayout, 1);

            layout->addWidget(buttonBox);

            // Initialize.
            setWindowTitle(qApp->translate("ColorDialog", "Color Dialog"));
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->widget,
                SIGNAL(colorChanged(const djv::Graphics::Color &)),
                SLOT(widgetCallback(const djv::Graphics::Color &)));
            connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        }

        ColorDialog::ColorDialog(
            UIContext * context,
            QWidget * parent) :
            QDialog(parent),
            _p(new Private(Graphics::Color(), context))
        {
            init();
        }

        ColorDialog::ColorDialog(
            const Graphics::Color & color,
            UIContext * context,
            QWidget * parent) :
            QDialog(parent),
            _p(new Private(color, context))
        {
            init();
        }

        ColorDialog::~ColorDialog()
        {}

        const Graphics::Color & ColorDialog::color() const
        {
            return _p->color;
        }

        void ColorDialog::setColor(const Graphics::Color & in)
        {
            if (in == _p->color)
                return;
            _p->color = in;
            widgetUpdate();
            Q_EMIT colorChanged(_p->color);
        }

        void ColorDialog::widgetCallback(const Graphics::Color & in)
        {
            setColor(in);
        }

        void ColorDialog::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->widget <<
                _p->swatch);
            _p->widget->setColor(_p->color);
            _p->swatch->setColor(_p->color);
        }

    } // namespace UI
} // namespace djv
