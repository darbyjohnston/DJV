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

#include <djvUI/ImagePrefsWidget.h>

#include <djvUI/UIContext.h>
#include <djvUI/ImagePrefs.h>
#include <djvUI/PrefsGroupBox.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QPointer>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct ImagePrefsWidget::Private
        {
            QPointer<QComboBox> filterMinWidget;
            QPointer<QComboBox> filterMagWidget;
            QPointer<QVBoxLayout> layout;
        };

        ImagePrefsWidget::ImagePrefsWidget(UIContext * context, QWidget * parent) :
            AbstractPrefsWidget(qApp->translate("djv::UI::ImagePrefsWidget", "Images"), context, parent),
            _p(new Private)
        {
            // Create the filter widgets.
            _p->filterMinWidget = new QComboBox;
            _p->filterMinWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->filterMinWidget->addItems(Graphics::OpenGLImageFilter::filterLabels());

            _p->filterMagWidget = new QComboBox;
            _p->filterMagWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->filterMagWidget->addItems(Graphics::OpenGLImageFilter::filterLabels());

            // Layout the widgets.
            _p->layout = new QVBoxLayout(this);

            PrefsGroupBox * prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::ImagePrefsWidget", "Scaling"),
                qApp->translate("djv::UI::ImagePrefsWidget",
                    "Set the image scaling quality. The filters \"Nearest\" and "
                    "\"Linear\" are generally the fastest. The other filters can provide "
                    "higher quality but are generally slower."),
                context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::ImagePrefsWidget", "Scale down:"),
                _p->filterMinWidget);
            formLayout->addRow(
                qApp->translate("djv::UI::ImagePrefsWidget", "Scale up:"),
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
        }
        
        ImagePrefsWidget::~ImagePrefsWidget()
        {}

        void ImagePrefsWidget::resetPreferences()
        {
            //DJV_DEBUG("ImagePrefsWidget::resetPreferences");
            context()->imagePrefs()->setFilter(Graphics::OpenGLImageFilter::filterDefault());
            widgetUpdate();
        }

        void ImagePrefsWidget::filterMinCallback(int in)
        {
            context()->imagePrefs()->setFilter(
                Graphics::OpenGLImageFilter(
                    static_cast<Graphics::OpenGLImageFilter::FILTER>(in),
                    context()->imagePrefs()->filter().mag));
            widgetUpdate();
        }

        void ImagePrefsWidget::filterMagCallback(int in)
        {
            context()->imagePrefs()->setFilter(
                Graphics::OpenGLImageFilter(
                    context()->imagePrefs()->filter().min,
                    static_cast<Graphics::OpenGLImageFilter::FILTER>(in)));
            widgetUpdate();
        }

        void ImagePrefsWidget::widgetUpdate()
        {
            //DJV_DEBUG("ImagePrefsWidget::widgetUpdate");
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->filterMinWidget <<
                _p->filterMagWidget);
            //DJV_DEBUG_PRINT("filter = " << context()->imagePrefs()->filter());
            _p->filterMinWidget->setCurrentIndex(context()->imagePrefs()->filter().min);
            _p->filterMagWidget->setCurrentIndex(context()->imagePrefs()->filter().mag);
        }

    } // namespace UI
} // namespace djv
