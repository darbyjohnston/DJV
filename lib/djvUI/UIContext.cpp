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

#include <djvUI/UIContext.h>

#include <djvUI/AboutDialog.h>
#include <djvUI/CineonWidget.h>
#include <djvUI/DPXWidget.h>
#include <djvUI/DebugLogDialog.h>
#include <djvUI/FileBrowser.h>
#include <djvUI/FileBrowserCache.h>
#include <djvUI/FileBrowserPrefs.h>
#include <djvUI/FileBrowserThumbnailSystem.h>
#include <djvUI/HelpPrefs.h>
#include <djvUI/IFFWidget.h>
#include <djvUI/IconLibrary.h>
#include <djvUI/ImageIOPrefs.h>
#include <djvUI/ImageIOWidget.h>
#include <djvUI/ImagePrefs.h>
#include <djvUI/InfoDialog.h>
#include <djvUI/LUTWidget.h>
#include <djvUI/MessagesDialog.h>
#include <djvUI/PPMWidget.h>
#include <djvUI/PlaybackUtil.h>
#include <djvUI/Prefs.h>
#include <djvUI/PrefsDialog.h>
#include <djvUI/ProxyStyle.h>
#include <djvUI/SequencePrefs.h>
#include <djvUI/SGIWidget.h>
#include <djvUI/StylePrefs.h>
#include <djvUI/TargaWidget.h>
#include <djvUI/TimePrefs.h>
#if defined(JPEG_FOUND)
#include <djvUI/JPEGWidget.h>
#endif // JPEG_FOUND
#if defined(TIFF_FOUND)
#include <djvUI/TIFFWidget.h>
#endif // TIFF_FOUND
#if defined(OPENEXR_FOUND)
#include <djvUI/OpenEXRWidget.h>
#endif // OPENEXR_FOUND
#if defined(FFMPEG_FOUND)
#include <djvUI/FFmpegWidget.h>
#endif // FFMPEG_FOUND

#include <djvCore/Debug.h>
#include <djvCore/DebugLog.h>

#include <QApplication>
#include <QDesktopServices>
#include <QPointer>
#include <QScopedPointer>
#include <QThreadPool>
#include <QUrl>

static void initResources()
{
    Q_INIT_RESOURCE(djvUI);
}

namespace djv
{
    namespace UI
    {
        struct UIContext::Private
        {
            bool                                 valid = false;
            QScopedPointer<IconLibrary>          iconLibrary;
            QScopedPointer<StylePrefs>           stylePrefs;
            QScopedPointer<FileBrowserPrefs>     fileBrowserPrefs;
            QScopedPointer<HelpPrefs>            helpPrefs;
            QScopedPointer<ImagePrefs>           imagePrefs;
            QScopedPointer<ImageIOPrefs>         imageIOPrefs;
            QScopedPointer<SequencePrefs>        sequencePrefs;
            QScopedPointer<TimePrefs>            timePrefs;
            QScopedPointer<FileBrowserCache>     fileBrowserCache;
            QPointer<FileBrowserThumbnailSystem> fileBrowserThumbnailSystem;
            QScopedPointer<ImageIOWidgetFactory> imageIOWidgetFactory;
            QScopedPointer<PrefsDialog>          prefsDialog;
            QScopedPointer<InfoDialog>           infoDialog;
            QScopedPointer<AboutDialog>          aboutDialog;
            QScopedPointer<MessagesDialog>       messagesDialog;
            QScopedPointer<DebugLogDialog>       debugLogDialog;
            QScopedPointer<FileBrowser>          fileBrowser;
            QPointer<ProxyStyle>                 proxyStyle;
        };

        UIContext::UIContext(QObject * parent) :
            Graphics::GraphicsContext(parent),
            _p(new Private)
        {
            //DJV_DEBUG("UIContext::UIContext");

            initResources();
            
            // Register meta types.
            qRegisterMetaType<PlaybackUtil::PLAYBACK>("djv::UI::PlaybackUtil::PLAYBACK");

            // Load translators.
            loadTranslator("djvUI");

            //! \todo Check the command line for whether to reset the preferences.
            Q_FOREACH(const QString & arg, qApp->arguments())
            {
                if (qApp->translate("djv::UI::UIContext", "-reset_prefs") == arg)
                {
                    DJV_LOG(debugLog(), "djv::UI::UIContext", "Reset the preferences...");
                    Prefs::setReset(true);
                }
            }

            // Load preferences.
            DJV_LOG(debugLog(), "djv::UI::UIContext", "Load the preferences...");
            _p->stylePrefs.reset(new StylePrefs);
            _p->fileBrowserPrefs.reset(new FileBrowserPrefs(this));
            _p->helpPrefs.reset(new HelpPrefs);
            _p->imagePrefs.reset(new ImagePrefs);
            _p->imageIOPrefs.reset(new ImageIOPrefs(this));
            _p->sequencePrefs.reset(new SequencePrefs);
            _p->timePrefs.reset(new TimePrefs);
            DJV_LOG(debugLog(), "djv::UI::UIContext", "");

            // Initialize.
            _p->fileBrowserCache.reset(new FileBrowserCache);
            _p->fileBrowserCache->setMaxCost(fileBrowserPrefs()->thumbnailCache());
            _p->fileBrowserThumbnailSystem = new FileBrowserThumbnailSystem(imageIOFactory(), this);
            _p->fileBrowserThumbnailSystem->start();
            _p->iconLibrary.reset(new IconLibrary);
            _p->proxyStyle = new UI::ProxyStyle(this);
            qApp->setStyle(_p->proxyStyle);
            styleUpdate();
            
            // Setup callbacks.
            connect(_p->stylePrefs.data(), SIGNAL(prefChanged()), SLOT(styleUpdate()));

            DJV_LOG(debugLog(), "djv::UI::UIContext", "Information:");
            DJV_LOG(debugLog(), "djv::UI::UIContext", "");
            DJV_LOG(debugLog(), "djv::UI::UIContext", info());
        }

        UIContext::~UIContext()
        {
            //DJV_DEBUG("UIContext::~UIContext");
            _p->fileBrowserThumbnailSystem->stop();
            _p->fileBrowserThumbnailSystem->wait();
            QThreadPool::globalInstance()->waitForDone();
        }

        bool UIContext::isValid() const
        {
            return _p->valid;
        }

        void UIContext::setValid(bool in)
        {
            //DJV_DEBUG("UIContext::setValid");
            //DJV_DEBUG_PRINT("in = " << in);
            _p->valid = in;
        }

        void UIContext::help()
        {
            //DJV_DEBUG("UIContext::help");
            //DJV_DEBUG_PRINT("url = " << doc());
            QDesktopServices::openUrl(QUrl::fromLocalFile(doc()));
        }

        FileBrowser * UIContext::fileBrowser(const QString & title) const
        {
            if (!_p->fileBrowser)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->fileBrowser.reset(new FileBrowser(that));
            }
            _p->fileBrowser->close();
            _p->fileBrowser->setWindowTitle(
                !title.isEmpty() ?
                title :
                qApp->translate("djv::UI::UIContext", "File Browser"));
            _p->fileBrowser->setPinnable(false);
            _p->fileBrowser->disconnect(SIGNAL(fileInfoChanged(const djv::Core::FileInfo &)));
            return _p->fileBrowser.data();
        }

        ImageIOWidgetFactory * UIContext::imageIOWidgetFactory() const
        {
            if (!_p->imageIOWidgetFactory)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->imageIOWidgetFactory.reset(new ImageIOWidgetFactory(that));
                _p->imageIOWidgetFactory->addPlugin(new CineonWidgetPlugin(that));
                _p->imageIOWidgetFactory->addPlugin(new DPXWidgetPlugin(that));
                _p->imageIOWidgetFactory->addPlugin(new IFFWidgetPlugin(that));
                _p->imageIOWidgetFactory->addPlugin(new LUTWidgetPlugin(that));
                _p->imageIOWidgetFactory->addPlugin(new PPMWidgetPlugin(that));
                _p->imageIOWidgetFactory->addPlugin(new SGIWidgetPlugin(that));
                _p->imageIOWidgetFactory->addPlugin(new TargaWidgetPlugin(that));
#if defined(JPEG_FOUND)
                _p->imageIOWidgetFactory->addPlugin(new JPEGWidgetPlugin(that));
#endif // JPEG_FOUND
#if defined(TIFF_FOUND)
                _p->imageIOWidgetFactory->addPlugin(new TIFFWidgetPlugin(that));
#endif // TIFF_FOUND
#if defined(OPENEXR_FOUND)
                _p->imageIOWidgetFactory->addPlugin(new OpenEXRWidgetPlugin(that));
#endif // OPENEXR_FOUND
#if defined(FFMPEG_FOUND)
                _p->imageIOWidgetFactory->addPlugin(new FFmpegWidgetPlugin(that));
#endif // FFMPEG_FOUND
            }
            return _p->imageIOWidgetFactory.data();
        }

        PrefsDialog * UIContext::prefsDialog() const
        {
            if (!_p->prefsDialog)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->prefsDialog.reset(new PrefsDialog(that));
            }
            return _p->prefsDialog.data();
        }

        InfoDialog * UIContext::infoDialog() const
        {
            if (!_p->infoDialog)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->infoDialog.reset(new InfoDialog(info(), that));
            }
            return _p->infoDialog.data();
        }

        AboutDialog * UIContext::aboutDialog() const
        {
            if (!_p->aboutDialog)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->aboutDialog.reset(new AboutDialog(about(), that));
            }
            return _p->aboutDialog.data();
        }

        MessagesDialog * UIContext::messagesDialog() const
        {
            if (!_p->messagesDialog)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->messagesDialog.reset(new MessagesDialog(that));
            }
            return _p->messagesDialog.data();
        }

        DebugLogDialog * UIContext::debugLogDialog() const
        {
            if (!_p->debugLogDialog)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->debugLogDialog.reset(new DebugLogDialog(that));
            }
            return _p->debugLogDialog.data();
        }

        FileBrowserPrefs * UIContext::fileBrowserPrefs() const
        {
            return _p->fileBrowserPrefs.data();
        }

        HelpPrefs * UIContext::helpPrefs() const
        {
            return _p->helpPrefs.data();
        }

        ImagePrefs * UIContext::imagePrefs() const
        {
            return _p->imagePrefs.data();
        }

        ImageIOPrefs * UIContext::imageIOPrefs() const
        {
            return _p->imageIOPrefs.data();
        }

        SequencePrefs * UIContext::sequencePrefs() const
        {
            return _p->sequencePrefs.data();
        }

        TimePrefs * UIContext::timePrefs() const
        {
            return _p->timePrefs.data();
        }

        StylePrefs * UIContext::stylePrefs() const
        {
            return _p->stylePrefs.data();
        }

        IconLibrary * UIContext::iconLibrary() const
        {
            return _p->iconLibrary.data();
        }

        FileBrowserCache * UIContext::fileBrowserCache() const
        {
            return _p->fileBrowserCache.data();
        }

        const QPointer<FileBrowserThumbnailSystem> & UIContext::fileBrowserThumbnailSystem() const
        {
            return _p->fileBrowserThumbnailSystem;
        }

        QString UIContext::info() const
        {
            static const QString label = qApp->translate("djv::UI::UIContext",
                "%1"
                "\n"
                "Image I/O Widgets\n"
                "\n"
                "    Plugins: %2\n"
                "\n"
                "Preferences\n"
                "\n"
                "    User: %3\n"
                "    System: %4\n");
            return QString(label).
                arg(Graphics::GraphicsContext::info()).
                arg(imageIOWidgetFactory()->names().join(", ")).
                arg(Prefs(QString(), Prefs::USER).fileName()).
                arg(Prefs(QString(), Prefs::SYSTEM).fileName());
        }

        bool UIContext::commandLineParse(QStringList & in)
        {
            //DJV_DEBUG("UIContext::commandLineParse");
            //DJV_DEBUG_PRINT("in = " << in);
            if (!Graphics::GraphicsContext::commandLineParse(in))
                return false;
            QStringList tmp;
            QString     arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;
                    if (qApp->translate("djv::UI::UIContext", "-reset_prefs") == arg)
                    {
                    }
                    else
                    {
                        tmp << arg;
                    }
                }
            }
            catch (const QString &)
            {
                in = tmp;
                throw QString(arg);
            }
            in = tmp;
            return true;
        }

        QString UIContext::commandLineHelp() const
        {
            static const QString label = qApp->translate("djv::UI::UIContext",
                "\n"
                "User Interface Options\n"
                "\n"
                "    -reset_prefs\n"
                "        Do not load the preference at start up.\n"
                "%2");
            return QString(label).
                arg(Graphics::GraphicsContext::commandLineHelp());
        }

        void UIContext::print(const QString & string, bool newLine, int indent)
        {
            if (_p->valid)
            {
                messagesDialog()->message(string);
            }
            else
            {
                Graphics::GraphicsContext::print(string, newLine, indent);
            }
        }
        
        void UIContext::styleUpdate()
        {
            _p->iconLibrary->setColor(Graphics::ColorUtil::toQt(_p->stylePrefs->palette().foreground));
            _p->proxyStyle->setFontSize(_p->stylePrefs->sizeMetric().fontSize);
        }

    } // namespace UI
} // namespace djv
