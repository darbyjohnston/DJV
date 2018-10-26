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
#include <djvUI/InfoDialog.h>
#include <djvUI/LUTWidget.h>
#include <djvUI/MessagesDialog.h>
#include <djvUI/OpenGLPrefs.h>
#include <djvUI/PPMWidget.h>
#include <djvUI/PlaybackUtil.h>
#include <djvUI/Prefs.h>
#include <djvUI/PrefsDialog.h>
#include <djvUI/ProxyStyle.h>
#include <djvUI/SGIWidget.h>
#include <djvUI/SequencePrefs.h>
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
#include <QPointer>
#include <QScopedPointer>
#include <QThreadPool>

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
            Private() :
                prefs(new Prefs),
                widgets(new Widgets),
                fileBrowser(new FileBrowser)
            {}
            
            bool valid = false;
            
            QScopedPointer<IconLibrary> iconLibrary;
            
            struct Prefs
            {
                QScopedPointer<HelpPrefs>        help;
                QScopedPointer<TimePrefs>        time;
                QScopedPointer<SequencePrefs>    sequence;
                QScopedPointer<OpenGLPrefs>      openGL;
                QScopedPointer<StylePrefs>       style;
                QScopedPointer<ImageIOPrefs>     imageIO;
                QScopedPointer<FileBrowserPrefs> fileBrowser;
            };
            std::unique_ptr<Prefs> prefs;
            
            struct Widgets
            {
                QScopedPointer<ImageIOWidgetFactory> imageIOWidgetFactory;
                QScopedPointer<PrefsDialog>          prefsDialog;
                QScopedPointer<InfoDialog>           infoDialog;
                QScopedPointer<AboutDialog>          aboutDialog;
                QScopedPointer<MessagesDialog>       messagesDialog;
                QScopedPointer<DebugLogDialog>       debugLogDialog;
            };
            std::unique_ptr<Widgets> widgets;
            
            struct FileBrowser
            {
                QScopedPointer<FileBrowserCache>           cache;
                QScopedPointer<FileBrowserThumbnailSystem> thumbnailSystem;
                QScopedPointer<UI::FileBrowser>            dialog;
            };
            std::unique_ptr<FileBrowser> fileBrowser;
            
            QPointer<ProxyStyle> proxyStyle;
        };

        UIContext::UIContext(int & argc, char ** argv, QObject * parent) :
            Graphics::GraphicsContext(argc, argv, parent),
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
            _p->prefs->help.reset(new HelpPrefs);
            _p->prefs->time.reset(new TimePrefs);
            _p->prefs->sequence.reset(new SequencePrefs);
            _p->prefs->openGL.reset(new OpenGLPrefs);
            _p->prefs->style.reset(new StylePrefs);
            _p->prefs->imageIO.reset(new ImageIOPrefs(this));
            _p->prefs->fileBrowser.reset(new FileBrowserPrefs(this));

            // Initialize.
            _p->fileBrowser->cache.reset(new FileBrowserCache);
            _p->fileBrowser->cache->setMaxCost(fileBrowserPrefs()->thumbnailCache());
            _p->fileBrowser->thumbnailSystem.reset(new FileBrowserThumbnailSystem(this));
            _p->fileBrowser->thumbnailSystem->start();
            _p->iconLibrary.reset(new IconLibrary);
            _p->proxyStyle = new UI::ProxyStyle(this);
            qApp->setStyle(_p->proxyStyle);
            styleUpdate();
            
            // Setup callbacks.
            connect(_p->prefs->style.data(), SIGNAL(prefChanged()), SLOT(styleUpdate()));

            DJV_LOG(debugLog(), "djv::UI::UIContext", "Information:");
            DJV_LOG(debugLog(), "djv::UI::UIContext", info());
        }

        UIContext::~UIContext()
        {
            //DJV_DEBUG("UIContext::~UIContext");
            _p->fileBrowser->thumbnailSystem->stop();
            _p->fileBrowser->thumbnailSystem->wait();
            QThreadPool::globalInstance()->waitForDone();
            
            //! \bug We manually reset these here so that our "_p" pointer is
            //! valid when they are destructed. This is necessary because some
            //! of the objects will try and access UIContext resources as they
            //! are destructed (for example saving preferences).
            _p->fileBrowser.reset();
            _p->widgets.reset();
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

        QPointer<FileBrowser> UIContext::fileBrowser(const QString & title) const
        {
            if (!_p->fileBrowser->dialog)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->fileBrowser->dialog.reset(new FileBrowser(that));
            }
            _p->fileBrowser->dialog->close();
            _p->fileBrowser->dialog->setWindowTitle(
                !title.isEmpty() ?
                title :
                qApp->translate("djv::UI::UIContext", "File Browser"));
            _p->fileBrowser->dialog->setPinnable(false);
            _p->fileBrowser->dialog->disconnect(SIGNAL(fileInfoChanged(const djv::Core::FileInfo &)));
            return _p->fileBrowser->dialog.data();
        }

        QPointer<ImageIOWidgetFactory> UIContext::imageIOWidgetFactory() const
        {
            if (!_p->widgets->imageIOWidgetFactory)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->widgets->imageIOWidgetFactory.reset(new ImageIOWidgetFactory(that));
                _p->widgets->imageIOWidgetFactory->addPlugin(new CineonWidgetPlugin(that));
                _p->widgets->imageIOWidgetFactory->addPlugin(new DPXWidgetPlugin(that));
                _p->widgets->imageIOWidgetFactory->addPlugin(new IFFWidgetPlugin(that));
                _p->widgets->imageIOWidgetFactory->addPlugin(new LUTWidgetPlugin(that));
                _p->widgets->imageIOWidgetFactory->addPlugin(new PPMWidgetPlugin(that));
                _p->widgets->imageIOWidgetFactory->addPlugin(new SGIWidgetPlugin(that));
                _p->widgets->imageIOWidgetFactory->addPlugin(new TargaWidgetPlugin(that));
#if defined(JPEG_FOUND)
                _p->widgets->imageIOWidgetFactory->addPlugin(new JPEGWidgetPlugin(that));
#endif // JPEG_FOUND
#if defined(TIFF_FOUND)
                _p->widgets->imageIOWidgetFactory->addPlugin(new TIFFWidgetPlugin(that));
#endif // TIFF_FOUND
#if defined(OPENEXR_FOUND)
                _p->widgets->imageIOWidgetFactory->addPlugin(new OpenEXRWidgetPlugin(that));
#endif // OPENEXR_FOUND
#if defined(FFMPEG_FOUND)
                _p->widgets->imageIOWidgetFactory->addPlugin(new FFmpegWidgetPlugin(that));
#endif // FFMPEG_FOUND
            }
            return _p->widgets->imageIOWidgetFactory.data();
        }

        QPointer<PrefsDialog> UIContext::prefsDialog() const
        {
            if (!_p->widgets->prefsDialog)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->widgets->prefsDialog.reset(new PrefsDialog(that));
            }
            return _p->widgets->prefsDialog.data();
        }

        QPointer<InfoDialog> UIContext::infoDialog() const
        {
            if (!_p->widgets->infoDialog)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->widgets->infoDialog.reset(new InfoDialog(info(), that));
            }
            return _p->widgets->infoDialog.data();
        }

        QPointer<AboutDialog> UIContext::aboutDialog() const
        {
            if (!_p->widgets->aboutDialog)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->widgets->aboutDialog.reset(new AboutDialog(about(), that));
            }
            return _p->widgets->aboutDialog.data();
        }

        QPointer<MessagesDialog> UIContext::messagesDialog() const
        {
            if (!_p->widgets->messagesDialog)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->widgets->messagesDialog.reset(new MessagesDialog(that));
            }
            return _p->widgets->messagesDialog.data();
        }

        QPointer<DebugLogDialog> UIContext::debugLogDialog() const
        {
            if (!_p->widgets->debugLogDialog)
            {
                UIContext * that = const_cast<UIContext *>(this);
                _p->widgets->debugLogDialog.reset(new DebugLogDialog(that));
            }
            return _p->widgets->debugLogDialog.data();
        }

        QPointer<FileBrowserPrefs> UIContext::fileBrowserPrefs() const
        {
            return _p->prefs->fileBrowser.data();
        }

        QPointer<ImageIOPrefs> UIContext::imageIOPrefs() const
        {
            return _p->prefs->imageIO.data();
        }

        QPointer<StylePrefs> UIContext::stylePrefs() const
        {
            return _p->prefs->style.data();
        }

        QPointer<OpenGLPrefs> UIContext::openGLPrefs() const
        {
            return _p->prefs->openGL.data();
        }

        QPointer<SequencePrefs> UIContext::sequencePrefs() const
        {
            return _p->prefs->sequence.data();
        }

        QPointer<TimePrefs> UIContext::timePrefs() const
        {
            return _p->prefs->time.data();
        }

        QPointer<HelpPrefs> UIContext::helpPrefs() const
        {
            return _p->prefs->help.data();
        }

        QPointer<IconLibrary> UIContext::iconLibrary() const
        {
            return _p->iconLibrary.data();
        }

        FileBrowserCache * UIContext::fileBrowserCache() const
        {
            return _p->fileBrowser->cache.data();
        }

        QPointer<FileBrowserThumbnailSystem> UIContext::fileBrowserThumbnailSystem() const
        {
            return _p->fileBrowser->thumbnailSystem.data();
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
                "        Reset the preferences.\n"
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
            _p->iconLibrary->setColor(Graphics::ColorUtil::toQt(_p->prefs->style->palette().foreground));
            _p->proxyStyle->setFontSize(_p->prefs->style->sizeMetric().fontSize);
        }

    } // namespace UI
} // namespace djv
