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

#include <djvViewLib/FileGroup.h>

#include <djvViewLib/FileActions.h>
#include <djvViewLib/FileCache.h>
#include <djvViewLib/FileMenu.h>
#include <djvViewLib/FilePrefs.h>
#include <djvViewLib/FileToolBar.h>
#include <djvViewLib/ImagePrefs.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/DebugLogDialog.h>
#include <djvUI/FileBrowser.h>
#include <djvUI/FileBrowserPrefs.h>
#include <djvUI/MessagesDialog.h>
#include <djvUI/Prefs.h>
#include <djvUI/PrefsDialog.h>
#include <djvUI/QuestionDialog.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/DebugLog.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfoUtil.h>
#include <djvCore/ListUtil.h>

#include <QApplication>
#include <QDir>

namespace djv
{
    namespace ViewLib
    {
        struct FileGroup::Private
        {
            Private(const QPointer<ViewContext> & context) :
                proxy(context->filePrefs()->proxy()),
                u8Conversion(context->filePrefs()->hasU8Conversion()),
                cacheEnabled(context->filePrefs()->isCacheEnabled()),
                preload(context->filePrefs()->hasPreload())
            {}

            Core::FileInfo fileInfo;
            Graphics::ImageIOInfo imageIOInfo;
            std::unique_ptr<Graphics::ImageLoad> imageLoad;
            std::unique_ptr<Graphics::OpenGLImage> openGLImage;
            int layer = 0;
            QStringList layers;
            Graphics::PixelDataInfo::PROXY proxy = static_cast<Graphics::PixelDataInfo::PROXY>(0);
            bool u8Conversion = false;
            bool cacheEnabled = false;
            bool preload = false;
            bool preloadActive = false;
            int preloadTimer = 0;
            qint64 preloadFrame = 0;
            QPointer<FileActions> actions;
        };

        FileGroup::FileGroup(
            const QPointer<FileGroup> & copy,
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context) :
            AbstractGroup(session, context),
            _p(new Private(context))
        {
            //DJV_DEBUG("FileGroup::FileGroup");

            if (copy)
            {
                _p->fileInfo = copy->_p->fileInfo;
                _p->layer = copy->_p->layer;
                _p->proxy = copy->_p->proxy;
                _p->u8Conversion = copy->_p->u8Conversion;
                _p->cacheEnabled = copy->_p->cacheEnabled;
                _p->preload = copy->_p->preload;
            }

            // Create the actions.
            _p->actions = new FileActions(context, this);

            // Initialize.
            if (copy)
            {
                open(_p->fileInfo);
            }
            context->makeGLContextCurrent();
            _p->openGLImage.reset(new Graphics::OpenGLImage);
            preloadUpdate();
            update();

            // Setup the action callbacks.
            connect(
                _p->actions->action(FileActions::OPEN),
                SIGNAL(triggered()),
                SLOT(openCallback()));
            connect(
                _p->actions->action(FileActions::RELOAD),
                SIGNAL(triggered()),
                SLOT(reloadCallback()));
            connect(
                _p->actions->action(FileActions::RELOAD_FRAME),
                SIGNAL(triggered()),
                SLOT(reloadFrameCallback()));
            connect(
                _p->actions->action(FileActions::CLOSE),
                SIGNAL(triggered()),
                SLOT(closeCallback()));
            connect(
                _p->actions->action(FileActions::EXPORT_SEQUENCE),
                SIGNAL(triggered()),
                SLOT(exportSequenceCallback()));
            connect(
                _p->actions->action(FileActions::EXPORT_FRAME),
                SIGNAL(triggered()),
                SLOT(exportFrameCallback()));
            connect(
                _p->actions->action(FileActions::LAYER_NEXT),
                SIGNAL(triggered()),
                SLOT(layerNextCallback()));
            connect(
                _p->actions->action(FileActions::LAYER_PREV),
                SIGNAL(triggered()),
                SLOT(layerPrevCallback()));
            connect(
                _p->actions->action(FileActions::U8_CONVERSION),
                SIGNAL(toggled(bool)),
                SLOT(setU8Conversion(bool)));
            connect(
                _p->actions->action(FileActions::CACHE),
                SIGNAL(toggled(bool)),
                SLOT(setCacheEnabled(bool)));
            connect(
                _p->actions->action(FileActions::PRELOAD),
                SIGNAL(toggled(bool)),
                SLOT(setPreload(bool)));
            connect(
                _p->actions->action(FileActions::CLEAR_CACHE),
                SIGNAL(triggered()),
                SLOT(cacheClearCallback()));
            connect(
                _p->actions->action(FileActions::MESSAGES),
                SIGNAL(triggered()),
                SLOT(messagesCallback()));
            connect(
                _p->actions->action(FileActions::PREFS),
                SIGNAL(triggered()),
                SLOT(prefsCallback()));
            connect(
                _p->actions->action(FileActions::DEBUG_LOG),
                SIGNAL(triggered()),
                SLOT(debugLogCallback()));
            connect(
                _p->actions->action(FileActions::EXIT),
                SIGNAL(triggered()),
                qApp,
                SLOT(quit()));

            // Setup the action group callbacks.
            connect(
                _p->actions->group(FileActions::RECENT_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(recentCallback(QAction *)));
            connect(
                _p->actions->group(FileActions::LAYER_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(layerCallback(QAction *)));
            connect(
                _p->actions->group(FileActions::PROXY_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(proxyCallback(QAction *)));

            // Setup the preferences callbacks.
            connect(
                context->filePrefs(),
                SIGNAL(proxyChanged(djv::Graphics::PixelDataInfo::PROXY)),
                SLOT(setProxy(djv::Graphics::PixelDataInfo::PROXY)));
            connect(
                context->filePrefs(),
                SIGNAL(u8ConversionChanged(bool)),
                SLOT(setU8Conversion(bool)));
            connect(
                context->filePrefs(),
                SIGNAL(cacheEnabledChanged(bool)),
                SLOT(setCacheEnabled(bool)));
            connect(
                context->filePrefs(),
                SIGNAL(cacheSizeGBChanged(float)),
                SLOT(preloadUpdate()));
            connect(
                context->filePrefs(),
                SIGNAL(preloadChanged(bool)),
                SLOT(setPreload(bool)));

            // Setup other callbacks.
            connect(
                context->imageIOFactory(),
                SIGNAL(optionChanged()),
                SLOT(reloadCallback()));
        }

        FileGroup::~FileGroup()
        {
            if (_p->preloadTimer)
            {
                killTimer(_p->preloadTimer);
                _p->preloadTimer = 0;
            }
            cacheDel();
            context()->makeGLContextCurrent();
            _p->openGLImage.reset();
        }

        const Core::FileInfo & FileGroup::fileInfo() const
        {
            return _p->fileInfo;
        }

        int FileGroup::layer() const
        {
            return _p->layer;
        }

        Graphics::PixelDataInfo::PROXY FileGroup::proxy() const
        {
            return _p->proxy;
        }

        bool FileGroup::hasU8Conversion() const
        {
            return _p->u8Conversion;
        }

        bool FileGroup::isCacheEnabled() const
        {
            return _p->cacheEnabled;
        }

        bool FileGroup::hasPreload() const
        {
            return _p->preload;
        }

        bool FileGroup::isPreloadActive() const
        {
            return _p->preloadActive;
        }

        qint64 FileGroup::preloadFrame() const
        {
            return _p->preloadFrame;
        }

        std::shared_ptr<Graphics::Image> FileGroup::image(qint64 frame) const
        {
            //DJV_DEBUG("FileGroup::image");
            //DJV_DEBUG_PRINT("frame = " << frame);
            std::shared_ptr<Graphics::Image> out;
            context()->makeGLContextCurrent();
            auto cache = context()->fileCache();
            const auto key = FileCacheKey(session(), frame);
            if (cache->hasItem(key))
            {
                out = cache->item(key);
            }
            else
            {
                if (_p->imageLoad)
                {
                    //DJV_DEBUG_PRINT("loading image");
                    out = std::shared_ptr<Graphics::Image>(new Graphics::Image);
                    try
                    {
                        _p->imageLoad->read(
                            *out,
                            Graphics::ImageIOFrameInfo(
                                _p->imageIOInfo.sequence.frames.count() ?
                                _p->imageIOInfo.sequence.frames[frame] :
                                -1,
                                _p->layer,
                                _p->proxy));
                    }
                    catch (Core::Error error)
                    {
                        error.add(
                            Enum::errorLabels()[Enum::ERROR_READ_IMAGE].
                            arg(QDir::toNativeSeparators(_p->fileInfo)));
                        context()->printError(error);
                    }
                    try
                    {
                        if (_p->u8Conversion)
                        {
                            //DJV_DEBUG_PRINT("u8 conversion");
                            Graphics::PixelDataInfo info(out->info());
                            info.pixel = Graphics::Pixel::pixel(Graphics::Pixel::format(info.pixel), Graphics::Pixel::U8);
                            auto tmp = out;
                            out = std::shared_ptr<Graphics::Image>(new Graphics::Image(info));
                            out->tags = tmp->tags;
                            Graphics::OpenGLImageOptions options;
                            options.colorProfile = tmp->colorProfile;
                            options.proxyScale = false;
                            _p->openGLImage->copy(*tmp, *out, options);
                        }
                    }
                    catch (Core::Error error)
                    {
                        error.add(
                            Enum::errorLabels()[Enum::ERROR_READ_IMAGE].
                            arg(QDir::toNativeSeparators(_p->fileInfo)));
                        context()->printError(error);
                    }
                    //DJV_DEBUG_PRINT("image = " << *out);
                }
                if (_p->cacheEnabled && out)
                {
                    //DJV_DEBUG_PRINT("cache image");
                    cache->addItem(key, out);
                }
            }
            return out;
        }

        const Graphics::ImageIOInfo & FileGroup::imageIOInfo() const
        {
            return _p->imageIOInfo;
        }

        QPointer<QMenu> FileGroup::createMenu() const
        {
            return new FileMenu(_p->actions.data());
        }

        QPointer<QToolBar> FileGroup::createToolBar() const
        {
            return new FileToolBar(_p->actions.data(), context());
        }

        void FileGroup::open(const Core::FileInfo & fileInfo)
        {
            //DJV_DEBUG("FileGroup::open");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            //DJV_DEBUG_PRINT("type = " << fileInfo.type());

            DJV_LOG(context()->debugLog(), "djv::ViewLib::FileGroup",
                QString("Open file = \"%1\"").arg(fileInfo));

            cacheDel();
            Core::FileInfo tmp = fileInfo;
            _p->fileInfo = Core::FileInfo();
            _p->imageIOInfo = Graphics::ImageIOInfo();
            _p->imageLoad.reset();

            // Load the file.
            if (!tmp.fileName().isEmpty())
            {
                //DJV_DEBUG_PRINT("loading...");
                try
                {
                    _p->imageLoad = context()->imageIOFactory()->load(tmp, _p->imageIOInfo);
                    _p->fileInfo = tmp;
                    context()->filePrefs()->addRecent(_p->fileInfo);
                }
                catch (Core::Error error)
                {
                    error.add(
                        Enum::errorLabels()[Enum::ERROR_OPEN_IMAGE].
                        arg(QDir::toNativeSeparators(tmp)));
                    context()->printError(error);
                }
            }

            _p->layer = 0;
            _p->layers.clear();
            for (int i = 0; i < _p->imageIOInfo.layerCount(); ++i)
            {
                _p->layers += _p->imageIOInfo[i].layerName;
            }

            preloadUpdate();
            update();

            Q_EMIT(imageIOInfoChanged(_p->imageIOInfo));
        }

        void FileGroup::setLayer(int layer)
        {
            if (layer == _p->layer)
                return;
            //DJV_DEBUG("FileGroup::setLayer");
            //DJV_DEBUG_PRINT("layer = " << layer);
            const int count = _p->layers.count();
            //DJV_DEBUG_PRINT("layer list = " << size);
            _p->layer = Core::Math::wrap(layer, 0, count - 1);
            //DJV_DEBUG_PRINT("layer = " << _layer);
            cacheDel();
            preloadUpdate();
            update();
            Q_EMIT imageChanged();
        }

        void FileGroup::setProxy(Graphics::PixelDataInfo::PROXY proxy)
        {
            if (proxy == _p->proxy)
                return;
            //DJV_DEBUG("FileGroup::setProxy");
            //DJV_DEBUG_PRINT("proxy = " << proxy);
            _p->proxy = proxy;
            cacheDel();
            preloadUpdate();
            update();
            Q_EMIT imageChanged();
        }

        void FileGroup::setU8Conversion(bool conversion)
        {
            if (conversion == _p->u8Conversion)
                return;
            _p->u8Conversion = conversion;
            cacheDel();
            preloadUpdate();
            update();
            Q_EMIT imageChanged();
        }

        void FileGroup::setCacheEnabled(bool cache)
        {
            if (cache == _p->cacheEnabled)
                return;
            //DJV_DEBUG("FileGroup::setCacheEnabled");
            //DJV_DEBUG_PRINT("cache = " << cache);
            _p->cacheEnabled = cache;
            cacheDel();
            preloadUpdate();
            update();
            Q_EMIT imageChanged();
        }

        void FileGroup::setPreload(bool preload)
        {
            if (preload == _p->preload)
                return;
            //DJV_DEBUG("FileGroup::setPreload");
            //DJV_DEBUG_PRINT("preload = " << preload);
            _p->preload = preload;
            preloadUpdate();
            update();
        }

        void FileGroup::setPreloadActive(bool active)
        {
            if (active == _p->preloadActive)
                return;
            //DJV_DEBUG("FileGroup::setPreloadActive");
            //DJV_DEBUG_PRINT("active = " << active);
            _p->preloadActive = active;
            preloadUpdate();
            update();
        }

        void FileGroup::setPreloadFrame(qint64 frame)
        {
            if (frame == _p->preloadFrame)
                return;
            _p->preloadFrame = frame;
            preloadUpdate();
            update();
        }

        void FileGroup::timerEvent(QTimerEvent *)
        {
            //DJV_DEBUG("FileGroup::timerEvent");
            //DJV_DEBUG_PRINT("preload frame        = " << _p->preloadFrame);

            auto cache = context()->fileCache();
            //DJV_DEBUG_PRINT("cache byte count     = " << cache->currentSizeBytes());
            //DJV_DEBUG_PRINT("cache max byte count = " << cache->maxSizeBytes());

            const auto items = cache->items(session());

            // Search for the next frame that isn't in the cache.
            bool      preload = false;
            quint64   byteCount = 0;
            qint64    frame = _p->preloadFrame;
            int       frameCount = 0;
            const int totalFrames = _p->imageIOInfo.sequence.frames.count();
            for (;
                byteCount <= cache->maxSizeBytes() &&
                frameCount < totalFrames;
                frame = Core::Math::wrap<qint64>(frame + 1, 0, totalFrames - 1), ++frameCount)
            {
                const auto key = FileCacheKey(session(), frame);
                if (cache->hasItem(key))
                {
                    byteCount += cache->item(key)->dataByteCount();
                }
                else
                {
                    byteCount += Graphics::PixelDataUtil::dataByteCount(_p->imageIOInfo);
                    if (byteCount <= cache->maxSizeBytes())
                    {
                        preload = true;
                    }
                    break;
                }
            }
            //DJV_DEBUG_PRINT("byteCount = " << byteCount);
            //DJV_DEBUG_PRINT("preload   = " << preload);
            //DJV_DEBUG_PRINT("frame     = " << frame);

            if (preload)
            {
                context()->makeGLContextCurrent();
                auto image = std::shared_ptr<Graphics::Image>(new Graphics::Image);
                if (_p->imageLoad)
                {
                    //DJV_DEBUG_PRINT("loading image");
                    try
                    {
                        _p->imageLoad->read(
                            *image,
                            Graphics::ImageIOFrameInfo(
                                _p->imageIOInfo.sequence.frames.count() ?
                                _p->imageIOInfo.sequence.frames[frame] :
                                -1,
                                _p->layer,
                                _p->proxy));
                        if (image->isValid() && _p->u8Conversion)
                        {
                            //DJV_DEBUG_PRINT("u8 conversion");
                            //DJV_DEBUG_PRINT("image = " << *image);
                            Graphics::PixelDataInfo info(image->info());
                            info.pixel = Graphics::Pixel::pixel(Graphics::Pixel::format(info.pixel), Graphics::Pixel::U8);
                            auto tmp = image;
                            image = std::shared_ptr<Graphics::Image>(new Graphics::Image(info));
                            image->tags = tmp->tags;
                            Graphics::OpenGLImageOptions options;
                            options.colorProfile = tmp->colorProfile;
                            options.proxyScale = false;
                            _p->openGLImage->copy(*tmp, *image, options);
                        }
                    }
                    catch (const Core::Error &)
                    {
                    }
                }
                if (image->isValid())
                {
                    //DJV_DEBUG_PRINT("image = " << *image);
                    cache->addItem(FileCacheKey(session(), frame), image);
                }
            }
            else
            {
                killTimer(_p->preloadTimer);
                _p->preloadTimer = 0;
            }
        }

        void FileGroup::openCallback()
        {
            auto fileBrowser = context()->fileBrowser(
                qApp->translate("djv::ViewLib::FileGroup", "Open"));
            fileBrowser->setPinnable(true);
            connect(
                fileBrowser,
                SIGNAL(fileInfoChanged(const djv::Core::FileInfo &)),
                SLOT(openCallback(const djv::Core::FileInfo &)));
            fileBrowser->show();
            fileBrowser->raise();
            fileBrowser->activateWindow();
        }

        void FileGroup::openCallback(const Core::FileInfo & fileInfo)
        {
            session()->fileOpen(fileInfo);
        }

        void FileGroup::recentCallback(QAction * action)
        {
            //DJV_DEBUG("FileGroup::recentCallback");
            const auto & recentFiles = context()->filePrefs()->recentFiles();
            const int index = action->data().toInt();
            if (index >= 0 && index < recentFiles.size())
            {
                auto fileInfo = recentFiles[index];
                //DJV_DEBUG_PRINT("fileInfo = " << fileInfo << " " << fileInfo.type());
                session()->fileOpen(fileInfo);
            }
        }

        void FileGroup::reloadCallback()
        {
            //DJV_DEBUG("FileGroup::reloadCallback");
            if (context()->imagePrefs()->hasFrameStoreFileReload())
            {
                Q_EMIT setFrameStore();
            }
            cacheDel();
            _p->imageLoad.reset();
            if (!_p->fileInfo.fileName().isEmpty())
            {
                try
                {
                    _p->imageLoad = context()->imageIOFactory()->load(_p->fileInfo, _p->imageIOInfo);
                }
                catch (Core::Error error)
                {
                    error.add(
                        Enum::errorLabels()[Enum::ERROR_OPEN_IMAGE].
                        arg(QDir::toNativeSeparators(_p->fileInfo)));

                    context()->printError(error);
                }
            }
            Q_EMIT imageChanged();
        }

        void FileGroup::reloadFrameCallback()
        {
            //DJV_DEBUG("FileGroup::reloadFrameCallback");
            if (context()->imagePrefs()->hasFrameStoreFileReload())
            {
                Q_EMIT setFrameStore();
            }
            Q_EMIT reloadFrame();
            _p->imageLoad.reset();
            if (!_p->fileInfo.fileName().isEmpty())
            {
                try
                {
                    _p->imageLoad = context()->imageIOFactory()->load(_p->fileInfo, _p->imageIOInfo);
                }
                catch (Core::Error error)
                {
                    error.add(
                        Enum::errorLabels()[Enum::ERROR_OPEN_IMAGE].
                        arg(QDir::toNativeSeparators(_p->fileInfo)));
                    context()->printError(error);
                }
            }
            Q_EMIT imageChanged();
        }

        void FileGroup::closeCallback()
        {
            //DJV_DEBUG("FileGroup::closeCallback");
            session()->fileOpen(Core::FileInfo());
        }

        void FileGroup::exportSequenceCallback()
        {
            //DJV_DEBUG("FileGroup::exportSequenceCallback");
            UI::FileBrowser * fileBrowser = context()->fileBrowser(
                qApp->translate("djv::ViewLib::FileGroup", "Export Sequence"));
            if (fileBrowser->exec() == QDialog::Accepted)
            {
                const Core::FileInfo & fileInfo = fileBrowser->fileInfo();
                //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
                if (Core::FileInfoUtil::exists(fileInfo))
                {
                    //DJV_DEBUG_PRINT("exists");
                    UI::QuestionDialog dialog(
                        qApp->translate("djv::ViewLib::FileGroup", "Overwrite existing file \"%1\"?").
                        arg(QDir::toNativeSeparators(fileInfo)));
                    if (QDialog::Accepted == dialog.exec())
                    {
                        Q_EMIT exportSequence(fileInfo);
                    }
                }
                else
                {
                    Q_EMIT exportSequence(fileInfo);
                }
            }
        }

        void FileGroup::exportFrameCallback()
        {
            UI::FileBrowser * fileBrowser = context()->fileBrowser(
                qApp->translate("djv::ViewLib::FileGroup", "Export Frame"));
            if (fileBrowser->exec() == QDialog::Accepted)
            {
                const Core::FileInfo & fileInfo = fileBrowser->fileInfo();
                if (Core::FileInfoUtil::exists(fileInfo))
                {
                    UI::QuestionDialog dialog(
                        qApp->translate("djv::ViewLib::FileGroup", "Overwrite existing file \"%1\"?").
                        arg(QDir::toNativeSeparators(fileInfo)));
                    if (QDialog::Accepted == dialog.exec())
                    {
                        Q_EMIT exportFrame(fileInfo);
                    }
                }
                else
                {
                    Q_EMIT exportFrame(fileInfo);
                }
            }
        }

        void FileGroup::layerCallback(QAction * action)
        {
            const int index = action->data().toInt();
            setLayer(index);
        }

        void FileGroup::layerPrevCallback()
        {
            setLayer(_p->layer - 1);
        }

        void FileGroup::layerNextCallback()
        {
            setLayer(_p->layer + 1);
        }

        void FileGroup::proxyCallback(QAction * action)
        {
            const int index = action->data().toInt();
            setProxy(static_cast<Graphics::PixelDataInfo::PROXY>(index));
        }

        void FileGroup::cacheClearCallback()
        {
            context()->fileCache()->clear();
        }

        void FileGroup::messagesCallback()
        {
            auto dialog = context()->messagesDialog();
            dialog->show();
            dialog->raise();
            dialog->activateWindow();
        }

        void FileGroup::prefsCallback()
        {
            auto dialog = context()->prefsDialog();
            dialog->show();
            dialog->raise();
            dialog->activateWindow();
        }

        void FileGroup::debugLogCallback()
        {
            auto dialog = context()->debugLogDialog();
            dialog->show();
            dialog->raise();
            dialog->activateWindow();
        }

        void FileGroup::preloadUpdate()
        {
            if (_p->cacheEnabled && _p->preload && _p->preloadActive)
            {
                if (!_p->preloadTimer)
                {
                    _p->preloadTimer = startTimer(0);
                }
            }
            else
            {
                if (_p->preloadTimer)
                {
                    killTimer(_p->preloadTimer);
                    _p->preloadTimer = 0;
                }
            }
        }

        void FileGroup::update()
        {
            _p->actions->action(FileActions::EXPORT_SEQUENCE)->setEnabled(_p->imageLoad.get());
            _p->actions->action(FileActions::EXPORT_FRAME)->setEnabled(_p->imageLoad.get());
            _p->actions->setLayers(_p->layers);
            _p->actions->setLayer(_p->layer);
            _p->actions->action(FileActions::U8_CONVERSION)->setChecked(_p->u8Conversion);
            _p->actions->action(FileActions::CACHE)->setChecked(_p->cacheEnabled);
            _p->actions->action(FileActions::PRELOAD)->setChecked(_p->preload);

            _p->actions->group(FileActions::PROXY_GROUP)->actions()[_p->proxy]->setChecked(true);
       }

        void FileGroup::cacheDel()
        {
            //DJV_DEBUG("FileGroup::cacheDel");
            context()->fileCache()->clearItems(session());
        }

    } // namespace ViewLib
} // namespace djv
