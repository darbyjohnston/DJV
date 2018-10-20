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

#include <djvUI/OpenGLPrefsWidget.h>

#include <djvUI/IntEdit.h>
#include <djvUI/IntObject.h>
#include <djvUI/OpenGLPrefs.h>
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
        struct OpenGLPrefsWidget::Private
        {
            QPointer<QComboBox> filterMinWidget;
            QPointer<QComboBox> filterMagWidget;
            QPointer<QVBoxLayout> layout;
        };

        OpenGLPrefsWidget::OpenGLPrefsWidget(const QPointer<UIContext> & context, QWidget * parent) :
            AbstractPrefsWidget(
                qApp->translate("djv::UI::OpenGLPrefsWidget", "OpenGL"), context, parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->filterMinWidget = new QComboBox;
            _p->filterMinWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->filterMinWidget->addItems(Graphics::OpenGLImageFilter::filterLabels());

            _p->filterMagWidget = new QComboBox;
            _p->filterMagWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->filterMagWidget->addItems(Graphics::OpenGLImageFilter::filterLabels());
            
            // Layout the widgets.
            _p->layout = new QVBoxLayout(this);

            auto prefsGroupBox= new PrefsGroupBox(
                qApp->translate("djv::UI::OpenGLPrefsWidget", "Scaling"),
                qApp->translate("djv::UI::OpenGLPrefsWidget",
                    "Set the image scaling quality. The filters \"Nearest\" and "
                    "\"Linear\" are generally the fastest. The other filters can provide "
                    "higher quality but are slower."),
                context);
            auto formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::OpenGLPrefsWidget", "Scale down:"),
                _p->filterMinWidget);
            formLayout->addRow(
                qApp->translate("djv::UI::OpenGLPrefsWidget", "Scale up:"),
                _p->filterMagWidget);
            _p->layout->addWidget(prefsGroupBox);

            _p->layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->filterMinWidget,
                SIGNAL(activated(int)),
                SLOT(filterMinCallback(int)));
            connect(
                _p->filterMagWidget,
                SIGNAL(activated(int)),
                SLOT(filterMagCallback(int)));
            connect(
                context->openGLPrefs(),
                SIGNAL(prefChanged()),
                SLOT(widgetUpdate()));
        }

        OpenGLPrefsWidget::~OpenGLPrefsWidget()
        {}
        
        void OpenGLPrefsWidget::resetPreferences()
        {
            context()->openGLPrefs()->setFilter(Graphics::OpenGLImageFilter::filterDefault());
        }

        void OpenGLPrefsWidget::filterMinCallback(int in)
        {
            context()->openGLPrefs()->setFilter(
                Graphics::OpenGLImageFilter(
                    static_cast<Graphics::OpenGLImageFilter::FILTER>(in),
                    context()->openGLPrefs()->filter().mag));
        }

        void OpenGLPrefsWidget::filterMagCallback(int in)
        {
            context()->openGLPrefs()->setFilter(
                Graphics::OpenGLImageFilter(
                    context()->openGLPrefs()->filter().min,
                    static_cast<Graphics::OpenGLImageFilter::FILTER>(in)));
        }

        void OpenGLPrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->filterMinWidget <<
                _p->filterMagWidget);
            _p->filterMinWidget->setCurrentIndex(context()->openGLPrefs()->filter().min);
            _p->filterMagWidget->setCurrentIndex(context()->openGLPrefs()->filter().mag);
        }

    } // namespace UI
} // namespace djv
