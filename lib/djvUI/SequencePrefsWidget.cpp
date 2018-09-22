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

#include <djvUI/SequencePrefsWidget.h>

#include <djvUI/UIContext.h>
#include <djvUI/IntEdit.h>
#include <djvUI/IntObject.h>
#include <djvUI/SequencePrefs.h>
#include <djvUI/PrefsGroupBox.h>

#include <djvCore/Debug.h>
#include <djvCore/Sequence.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct SequencePrefsWidget::Private
        {
            IntEdit * maxFramesWidget = nullptr;
            QVBoxLayout * layout = nullptr;
        };

        SequencePrefsWidget::SequencePrefsWidget(UIContext * context, QWidget * parent) :
            AbstractPrefsWidget(qApp->translate("djv::UI::SequencePrefsWidget", "Sequences"), context, parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->maxFramesWidget = new IntEdit;
            _p->maxFramesWidget->setMax(IntObject::intMax);
            _p->maxFramesWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            // Layout the widgets.
            _p->layout = new QVBoxLayout(this);

            PrefsGroupBox * prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::SequencePrefsWidget", "Max Frames"),
                qApp->translate("djv::UI::SequencePrefsWidget",
                    "Set the maximum number of frames allowed in a sequence. This is used "
                    "as a safety feature to prevent file sequences from becoming too large."),
                context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::SequencePrefsWidget", "Frames:"),
                _p->maxFramesWidget);
            _p->layout->addWidget(prefsGroupBox);

            _p->layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->maxFramesWidget,
                SIGNAL(valueChanged(int)),
                SLOT(maxFramesCallback(int)));
        }
        
        SequencePrefsWidget::~SequencePrefsWidget()
        {}

        void SequencePrefsWidget::resetPreferences()
        {
            context()->sequencePrefs()->setMaxFrames(Core::Sequence::maxFramesDefault());
            widgetUpdate();
        }

        void SequencePrefsWidget::maxFramesCallback(int size)
        {
            context()->sequencePrefs()->setMaxFrames(size);
        }

        void SequencePrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->maxFramesWidget);
            _p->maxFramesWidget->setValue(Core::Sequence::maxFrames());
        }

    } // namespace UI
} // namespace djv
