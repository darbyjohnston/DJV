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

#include <djvUI/SequencePrefsWidget.h>

#include <djvUI/IntEdit.h>
#include <djvUI/IntObject.h>
#include <djvUI/SequencePrefs.h>
#include <djvUI/PrefsGroupBox.h>
#include <djvUI/UIContext.h>

#include <djvCore/Debug.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QPointer>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct SequencePrefsWidget::Private
        {
            QPointer<QComboBox> compressWidget;
            QPointer<QCheckBox> autoEnabledWidget;
            QPointer<IntEdit> maxSizeWidget;
            QPointer<QVBoxLayout> layout;
        };

        SequencePrefsWidget::SequencePrefsWidget(const QPointer<UIContext> & context, QWidget * parent) :
            AbstractPrefsWidget(
                qApp->translate("djv::UI::SequencePrefsWidget", "File Sequences"), context, parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->compressWidget = new QComboBox;
            _p->compressWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->compressWidget->addItems(Core::Sequence::compressLabels());

            _p->autoEnabledWidget = new QCheckBox(
                qApp->translate("djv::UI::SequencePrefsWidget", "Enable auto file sequencing"));

            _p->maxSizeWidget = new IntEdit;
            _p->maxSizeWidget->setMax(10000000);
            _p->maxSizeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            // Layout the widgets.
            _p->layout = new QVBoxLayout(this);

            auto prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::SequencePrefsWidget", "Compression"),
                qApp->translate("djv::UI::SequencePrefsWidget", "Set how file sequences are compressed."),
                context);
            auto formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->compressWidget);
            _p->layout->addWidget(prefsGroupBox);

            prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::SequencePrefsWidget", "Auto Sequencing"),
                context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->autoEnabledWidget);
            _p->layout->addWidget(prefsGroupBox);

            prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::SequencePrefsWidget", "Maximum Size"),
                qApp->translate("djv::UI::SequencePrefsWidget", "Set the maximum size allowed for file sequences."),
                context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->maxSizeWidget);
            _p->layout->addWidget(prefsGroupBox);

            _p->layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->compressWidget,
                SIGNAL(activated(int)),
                SLOT(compressCallback(int)));
            connect(
                _p->autoEnabledWidget,
                SIGNAL(toggled(bool)),
                SLOT(autoEnabledCallback(bool)));
            connect(
                _p->maxSizeWidget,
                SIGNAL(valueChanged(int)),
                SLOT(maxSizeCallback(int)));
            connect(
                context->sequencePrefs(),
                SIGNAL(prefChanged()),
                SLOT(widgetUpdate()));
        }

        SequencePrefsWidget::~SequencePrefsWidget()
        {}
        
        void SequencePrefsWidget::resetPreferences()
        {
            context()->sequencePrefs()->setCompress(Core::Sequence::compressDefault());
            context()->sequencePrefs()->setAutoEnabled(Core::Sequence::autoEnabledDefault());
            context()->sequencePrefs()->setMaxSize(Core::Sequence::maxSizeDefault());
        }

        void SequencePrefsWidget::compressCallback(int index)
        {
            context()->sequencePrefs()->setCompress(static_cast<Core::Sequence::COMPRESS>(index));
        }

        void SequencePrefsWidget::autoEnabledCallback(bool value)
        {
            context()->sequencePrefs()->setAutoEnabled(value);
        }

        void SequencePrefsWidget::maxSizeCallback(int size)
        {
            context()->sequencePrefs()->setMaxSize(size);
        }

        void SequencePrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->compressWidget <<
                _p->autoEnabledWidget <<
                _p->maxSizeWidget);
            _p->compressWidget->setCurrentIndex(Core::Sequence::compress());
            _p->autoEnabledWidget->setChecked(Core::Sequence::isAutoEnabled());
            _p->maxSizeWidget->setValue(Core::Sequence::maxSize());
        }

    } // namespace UI
} // namespace djv
