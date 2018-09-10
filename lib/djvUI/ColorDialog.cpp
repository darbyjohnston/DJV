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

//------------------------------------------------------------------------------
// djvColorDialog::Private
//------------------------------------------------------------------------------

struct djvColorDialog::Private
{
    Private(const djvColor & color, djvUIContext * context) :
        color  (color),
        context(context)
    {}
    
    djvColor         color;
    djvColorWidget * widget  = nullptr;
    djvColorSwatch * swatch  = nullptr;
    djvUIContext *  context = nullptr;
};

//------------------------------------------------------------------------------
// djvColorDialog
//------------------------------------------------------------------------------

void djvColorDialog::init()
{
    // Create the widgets.
    _p->widget = new djvColorWidget(_p->context);

    _p->swatch = new djvColorSwatch(_p->context);
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
    setWindowTitle(qApp->translate("djvColorDialog", "Color Dialog"));
    widgetUpdate();

    // Setup the callbacks.
    connect(
        _p->widget,
        SIGNAL(colorChanged(const djvColor &)),
        SLOT(widgetCallback(const djvColor &)));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

djvColorDialog::djvColorDialog(
    djvUIContext *  context,
    QWidget *        parent) :
    QDialog(parent),
    _p(new Private(djvColor(), context))
{
    init();
}

djvColorDialog::djvColorDialog(
    const djvColor & color,
    djvUIContext *  context,
    QWidget *        parent) :
    QDialog(parent),
    _p(new Private(color, context))
{
    init();
}

djvColorDialog::~djvColorDialog()
{}

const djvColor & djvColorDialog::color() const
{
    return _p->color;
}

void djvColorDialog::setColor(const djvColor & in)
{
    if (in == _p->color)
        return;
    _p->color = in;
    widgetUpdate();
    Q_EMIT colorChanged(_p->color);
}

void djvColorDialog::widgetCallback(const djvColor & in)
{
    setColor(in);
}

void djvColorDialog::widgetUpdate()
{
    djvSignalBlocker signalBlocker(QObjectList() <<
        _p->widget <<
        _p->swatch);
    _p->widget->setColor(_p->color);
    _p->swatch->setColor(_p->color);
}

