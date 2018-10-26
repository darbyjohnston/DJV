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

#include <djvViewLib/FilePrefsWidget.h>

#include <djvViewLib/FilePrefs.h>
#include <djvViewLib/MiscWidget.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/Prefs.h>
#include <djvUI/PrefsGroupBox.h>

#include <djvCore/FileInfoUtil.h>
#include <djvCore/ListUtil.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        struct FilePrefsWidget::Private
        {
            QPointer<QComboBox>       proxyWidget;
            QPointer<QCheckBox>       u8ConversionWidget;
            QPointer<QCheckBox>       cacheWidget;
            QPointer<CacheSizeWidget> cacheSizeWidget;
            QPointer<QCheckBox>       preloadWidget;
            QPointer<QCheckBox>       displayCacheWidget;
        };

        FilePrefsWidget::FilePrefsWidget(const QPointer<ViewContext> & context) :
            AbstractPrefsWidget(qApp->translate("djv::ViewLib::FilePrefsWidget", "Files"), context),
            _p(new Private)
        {
            // Create the proxy scale widgets.
            _p->proxyWidget = new QComboBox;
            _p->proxyWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->proxyWidget->addItems(Graphics::PixelDataInfo::proxyLabels());

            // Create the convert to 8-bits widgets.
            _p->u8ConversionWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Enable 8-bit conversion"));

            // Create the file cache widgets.
            _p->cacheWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Enable the memory cache"));

            _p->cacheSizeWidget = new CacheSizeWidget(context.data());

            _p->preloadWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Preload cache"));

            _p->displayCacheWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Display cached frames in the timeline"));

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);

            auto prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Proxy Scale"),
                qApp->translate("djv::ViewLib::FilePrefsWidget",
                    "Proxy scaling reduces the resolution of images as they are loaded. "
                    "This allow more images to fit in the memory cache at the expense of image quality."),
                context.data());
            auto formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Proxy scale:"),
                _p->proxyWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "8-bit Conversion"),
                qApp->translate("djv::ViewLib::FilePrefsWidget",
                    "Convert images to 8-bits as they are loaded. "
                    "This allow more images to fit in the memory cache at the expense of image quality."),
                context.data());
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->u8ConversionWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Memory Cache"),
                qApp->translate("djv::ViewLib::FilePrefsWidget",
                    "The memory cache stores images for faster playback performance."),
                context.data());
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->cacheWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Cache size (gigabytes):"),
                _p->cacheSizeWidget);
            formLayout->addRow(_p->preloadWidget);
            formLayout->addRow(_p->displayCacheWidget);
            layout->addWidget(prefsGroupBox);

            layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->proxyWidget,
                SIGNAL(activated(int)),
                SLOT(proxyCallback(int)));
            connect(
                _p->u8ConversionWidget,
                SIGNAL(toggled(bool)),
                SLOT(u8ConversionCallback(bool)));
            connect(
                _p->cacheWidget,
                SIGNAL(toggled(bool)),
                SLOT(cacheEnabledCallback(bool)));
            connect(
                _p->cacheSizeWidget,
                SIGNAL(cacheSizeGBChanged(float)),
                SLOT(cacheSizeGBCallback(float)));
            connect(
                _p->preloadWidget,
                SIGNAL(toggled(bool)),
                SLOT(preloadCallback(bool)));
            connect(
                _p->displayCacheWidget,
                SIGNAL(toggled(bool)),
                SLOT(displayCacheCallback(bool)));
            connect(
                context->filePrefs(),
                SIGNAL(prefChanged()),
                SLOT(widgetUpdate()));
        }

        FilePrefsWidget::~FilePrefsWidget()
        {}

        void FilePrefsWidget::resetPreferences()
        {
            context()->filePrefs()->setProxy(FilePrefs::proxyDefault());
            context()->filePrefs()->setU8Conversion(FilePrefs::u8ConversionDefault());
            context()->filePrefs()->setCacheEnabled(FilePrefs::cacheEnabledDefault());
            context()->filePrefs()->setCacheSizeGB(FilePrefs::cacheSizeGBDefault());
            context()->filePrefs()->setPreload(FilePrefs::preloadDefault());
            context()->filePrefs()->setDisplayCache(FilePrefs::displayCacheDefault());
        }

        void FilePrefsWidget::proxyCallback(int in)
        {
            context()->filePrefs()->setProxy(static_cast<Graphics::PixelDataInfo::PROXY>(in));
        }

        void FilePrefsWidget::u8ConversionCallback(bool in)
        {
            context()->filePrefs()->setU8Conversion(in);
        }

        void FilePrefsWidget::cacheEnabledCallback(bool in)
        {
            context()->filePrefs()->setCacheEnabled(in);
        }

        void FilePrefsWidget::cacheSizeGBCallback(float in)
        {
            context()->filePrefs()->setCacheSizeGB(in);
        }

        void FilePrefsWidget::preloadCallback(bool in)
        {
            context()->filePrefs()->setPreload(in);
        }

        void FilePrefsWidget::displayCacheCallback(bool in)
        {
            context()->filePrefs()->setDisplayCache(in);
        }

        void FilePrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->proxyWidget <<
                _p->u8ConversionWidget <<
                _p->cacheWidget <<
                _p->cacheSizeWidget <<
                _p->preloadWidget <<
                _p->displayCacheWidget);
            _p->proxyWidget->setCurrentIndex(context()->filePrefs()->proxy());
            _p->u8ConversionWidget->setChecked(context()->filePrefs()->hasU8Conversion());
            _p->cacheWidget->setChecked(context()->filePrefs()->isCacheEnabled());
            _p->cacheSizeWidget->setCacheSizeGB(context()->filePrefs()->cacheSizeGB());
            _p->preloadWidget->setChecked(context()->filePrefs()->hasPreload());
            _p->displayCacheWidget->setChecked(context()->filePrefs()->hasDisplayCache());
        }

    } // namespace ViewLib
} // namespace djv
