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

#include <djvUI/MiscPrefsWidget.h>

#include <djvUI/IntEdit.h>
#include <djvUI/IntObject.h>
#include <djvUI/MiscPrefs.h>
#include <djvUI/PrefsGroupBox.h>
#include <djvUI/UIContext.h>

#include <djvCore/Debug.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QPointer>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct MiscPrefsWidget::Private
        {
            QPointer<QComboBox> timeUnitsWidget;
            QPointer<QComboBox> speedWidget;
            QPointer<IntEdit> maxFramesWidget;
            QPointer<QComboBox> filterMinWidget;
            QPointer<QComboBox> filterMagWidget;
            QPointer<QCheckBox> toolTipsWidget;
            QPointer<QVBoxLayout> layout;
        };

        MiscPrefsWidget::MiscPrefsWidget(const QPointer<UIContext> & context, QWidget * parent) :
            AbstractPrefsWidget(
                qApp->translate("djv::UI::MiscPrefsWidget", "Miscellaneous"), context, parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->timeUnitsWidget = new QComboBox;
            _p->timeUnitsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->timeUnitsWidget->addItems(Core::Time::unitsLabels());

            _p->speedWidget = new QComboBox;
            _p->speedWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->speedWidget->addItems(Core::Speed::fpsLabels());
            
            _p->maxFramesWidget = new IntEdit;
            _p->maxFramesWidget->setMax(IntObject::intMax);
            _p->maxFramesWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            
            _p->filterMinWidget = new QComboBox;
            _p->filterMinWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->filterMinWidget->addItems(Graphics::OpenGLImageFilter::filterLabels());

            _p->filterMagWidget = new QComboBox;
            _p->filterMagWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->filterMagWidget->addItems(Graphics::OpenGLImageFilter::filterLabels());
            
            _p->toolTipsWidget = new QCheckBox(
                qApp->translate("djv::UI::MiscPrefsWidget", "Enable tool tips"));

            // Layout the widgets.
            _p->layout = new QVBoxLayout(this);

            auto prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::MiscPrefsWidget", "Time"), context);
            auto formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::MiscPrefsWidget", "Time units:"),
                _p->timeUnitsWidget);
            auto hLayout = new QHBoxLayout;
            hLayout->addWidget(_p->speedWidget);
            hLayout->addWidget(
                new QLabel(qApp->translate("djv::UI::MiscPrefsWidget", "(frames per second)")));
            formLayout->addRow(
                qApp->translate("djv::UI::MiscPrefsWidget", "Default speed:"), hLayout);
            _p->layout->addWidget(prefsGroupBox);

            prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::MiscPrefsWidget", "Max Frames"),
                qApp->translate("djv::UI::MiscPrefsWidget",
                    "Set the maximum number of frames allowed in a file sequence. This is used "
                    "as a safety feature to prevent file sequences from becoming too large."),
                context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::MiscPrefsWidget", "Frames:"),
                _p->maxFramesWidget);
            _p->layout->addWidget(prefsGroupBox);

            prefsGroupBox= new PrefsGroupBox(
                qApp->translate("djv::UI::MiscPrefsWidget", "Scaling"),
                qApp->translate("djv::UI::MiscPrefsWidget",
                    "Set the image scaling quality. The filters \"Nearest\" and "
                    "\"Linear\" are generally the fastest. The other filters can provide "
                    "higher quality but are slower."),
                context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::MiscPrefsWidget", "Scale down:"),
                _p->filterMinWidget);
            formLayout->addRow(
                qApp->translate("djv::UI::MiscPrefsWidget", "Scale up:"),
                _p->filterMagWidget);
            _p->layout->addWidget(prefsGroupBox);

            prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::MiscPrefsWidget", "Tool Tips"), context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->toolTipsWidget);
            _p->layout->addWidget(prefsGroupBox);

            _p->layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->timeUnitsWidget,
                SIGNAL(activated(int)),
                SLOT(timeUnitsCallback(int)));
            connect(
                _p->speedWidget,
                SIGNAL(activated(int)),
                SLOT(speedCallback(int)));
            connect(
                _p->maxFramesWidget,
                SIGNAL(valueChanged(int)),
                SLOT(maxFramesCallback(int)));
            connect(
                _p->filterMinWidget,
                SIGNAL(activated(int)),
                SLOT(filterMinCallback(int)));
            connect(
                _p->filterMagWidget,
                SIGNAL(activated(int)),
                SLOT(filterMagCallback(int)));
            connect(
                _p->toolTipsWidget,
                SIGNAL(toggled(bool)),
                SLOT(toolTipsCallback(bool)));
        }

        MiscPrefsWidget::~MiscPrefsWidget()
        {}
        
        void MiscPrefsWidget::resetPreferences()
        {
            context()->miscPrefs()->setTimeUnits(Core::Time::unitsDefault());
            context()->miscPrefs()->setSpeed(Core::Speed::speedDefault());
            context()->miscPrefs()->setMaxFrames(Core::Sequence::maxFramesDefault());
            context()->miscPrefs()->setFilter(Graphics::OpenGLImageFilter::filterDefault());
            context()->miscPrefs()->setToolTips(MiscPrefs::toolTipsDefault());
            widgetUpdate();
        }

        void MiscPrefsWidget::timeUnitsCallback(int index)
        {
            context()->miscPrefs()->setTimeUnits(static_cast<Core::Time::UNITS>(index));
        }

        void MiscPrefsWidget::speedCallback(int index)
        {
            context()->miscPrefs()->setSpeed(static_cast<Core::Speed::FPS>(index));
        }

        void MiscPrefsWidget::maxFramesCallback(int size)
        {
            context()->miscPrefs()->setMaxFrames(size);
        }

        void MiscPrefsWidget::filterMinCallback(int in)
        {
            context()->miscPrefs()->setFilter(
                Graphics::OpenGLImageFilter(
                    static_cast<Graphics::OpenGLImageFilter::FILTER>(in),
                    context()->miscPrefs()->filter().mag));
            widgetUpdate();
        }

        void MiscPrefsWidget::filterMagCallback(int in)
        {
            context()->miscPrefs()->setFilter(
                Graphics::OpenGLImageFilter(
                    context()->miscPrefs()->filter().min,
                    static_cast<Graphics::OpenGLImageFilter::FILTER>(in)));
            widgetUpdate();
        }

        void MiscPrefsWidget::toolTipsCallback(bool toolTips)
        {
            context()->miscPrefs()->setToolTips(toolTips);
        }

        void MiscPrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->timeUnitsWidget <<
                _p->speedWidget <<
                _p->maxFramesWidget <<
                _p->filterMinWidget <<
                _p->filterMagWidget <<
                _p->toolTipsWidget);
            _p->timeUnitsWidget->setCurrentIndex(Core::Time::units());
            _p->speedWidget->setCurrentIndex(Core::Speed::speed());
            _p->maxFramesWidget->setValue(Core::Sequence::maxFrames());
            _p->filterMinWidget->setCurrentIndex(context()->miscPrefs()->filter().min);
            _p->filterMagWidget->setCurrentIndex(context()->miscPrefs()->filter().mag);
            _p->toolTipsWidget->setChecked(context()->miscPrefs()->hasToolTips());
        }

    } // namespace UI
} // namespace djv
