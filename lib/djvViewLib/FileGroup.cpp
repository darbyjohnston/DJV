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

#include <djvViewLib/Context.h>
#include <djvViewLib/FileActions.h>
#include <djvViewLib/FileCache.h>
#include <djvViewLib/FileMenu.h>
#include <djvViewLib/FilePrefs.h>
#include <djvViewLib/FileToolBar.h>
#include <djvViewLib/ImagePrefs.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/MainWindow.h>

#include <djvUI/DebugLogDialog.h>
#include <djvUI/FileBrowser.h>
#include <djvUI/FileBrowserPrefs.h>
#include <djvUI/MessagesDialog.h>
#include <djvUI/Prefs.h>
#include <djvUI/PrefsDialog.h>
#include <djvUI/QuestionDialog.h>

#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/DebugLog.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfoUtil.h>
#include <djvCore/ListUtil.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDir>
#include <QMenuBar>
#include <QToolBar>

namespace djv
{
    namespace ViewLib
    {
        struct FileGroup::Private
        {
            Private(Context * context) :
                proxy(context->filePrefs()->proxy()),
                u8Conversion(context->filePrefs()->hasU8Conversion()),
                cache(context->filePrefs()->hasCache()),
                preload(context->filePrefs()->hasPreload())
            {}

            Core::FileInfo fileInfo;
            Graphics::ImageIOInfo imageIOInfo;
            const Graphics::Image * image = nullptr;
            Graphics::Image imageTmp;
            Graphics::Image imageTmp2;
            QScopedPointer<Graphics::ImageLoad> imageLoad;
            std::unique_ptr<Graphics::OpenGLImage> openGLImage;
            int layer = 0;
            QStringList layers;
            Graphics::PixelDataInfo::PROXY proxy = static_cast<Graphics::PixelDataInfo::PROXY>(0);
            bool u8Conversion = false;
            bool cache = false;
            FileCacheItem * cacheItem = nullptr;
            bool preload = false;
            bool preloadActive = false;
            int preloadTimer = 0;
            qint64 preloadFrame = 0;
            FileActions * actions = nullptr;
            FileMenu * menu = nullptr;
            FileToolBar * toolBar = nullptr;
        };

        FileGroup::FileGroup(
            const FileGroup * copy,
            MainWindow *      mainWindow,
            Context *         context) :
            AbstractGroup(mainWindow, context),
            _p(new Private(context))
        {
            //DJV_DEBUG("FileGroup::FileGroup");

            if (copy)
            {
                _p->fileInfo = copy->_p->fileInfo;
                _p->layer = copy->_p->layer;
                _p->proxy = copy->_p->proxy;
                _p->u8Conversion = copy->_p->u8Conversion;
                _p->cache = copy->_p->cache;
                _p->preload = copy->_p->preload;
            }

            // Create the actions.
            _p->actions = new FileActions(context, this);

            // Create the menus.
            _p->menu = new FileMenu(_p->actions, mainWindow->menuBar());
            mainWindow->menuBar()->addMenu(_p->menu);

            // Create the widgets.
            _p->toolBar = new FileToolBar(_p->actions, context);
            mainWindow->addToolBar(_p->toolBar);

            // Initialize.
            if (copy)
            {
                open(_p->fileInfo);
            }
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
                _p->actions->action(FileActions::SAVE),
                SIGNAL(triggered()),
                SLOT(saveCallback()));
            connect(
                _p->actions->action(FileActions::SAVE_FRAME),
                SIGNAL(triggered()),
                SLOT(saveFrameCallback()));
            connect(
                _p->actions->action(FileActions::LAYER_PREV),
                SIGNAL(triggered()),
                SLOT(layerPrevCallback()));
            connect(
                _p->actions->action(FileActions::LAYER_NEXT),
                SIGNAL(triggered()),
                SLOT(layerNextCallback()));
            connect(
                _p->actions->action(FileActions::U8_CONVERSION),
                SIGNAL(toggled(bool)),
                SLOT(setU8Conversion(bool)));
            connect(
                _p->actions->action(FileActions::CACHE),
                SIGNAL(toggled(bool)),
                SLOT(setCache(bool)));
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
                SIGNAL(cacheChanged(bool)),
                SLOT(setCache(bool)));
            connect(
                context->filePrefs(),
                SIGNAL(cacheSizeChanged(float)),
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
            _p->image = 0;

            if (_p->cacheItem)
            {
                _p->cacheItem->decrement();
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

        bool FileGroup::hasCache() const
        {
            return _p->cache;
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

        const Graphics::Image * FileGroup::image(qint64 frame) const
        {
            //DJV_DEBUG("FileGroup::image");
            //DJV_DEBUG_PRINT("frame = " << frame);

            context()->makeGLContextCurrent();

            FileGroup * that = const_cast<FileGroup *>(this);
            that->_p->image = 0;
            FileCache * cache = context()->fileCache();
            FileCacheItem * prev = _p->cacheItem;
            that->_p->cacheItem = _p->cache ? cache->get(mainWindow(), frame) : 0;
            if (_p->cacheItem)
            {
                //DJV_DEBUG_PRINT("cached image");
                that->_p->image = _p->cacheItem->image();
            }
            else
            {
                if (_p->imageLoad.data())
                {
                    //DJV_DEBUG_PRINT("loading image");
                    try
                    {
                        _p->imageLoad->read(
                            _p->u8Conversion ? that->_p->imageTmp2 : that->_p->imageTmp,
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
                            Util::errorLabels()[Util::ERROR_READ_IMAGE].
                            arg(QDir::toNativeSeparators(_p->fileInfo)));
                        context()->printError(error);
                    }

                    try
                    {
                        if (_p->u8Conversion)
                        {
                            //DJV_DEBUG_PRINT("u8 conversion");
                            //DJV_DEBUG_PRINT("image = " << _p->imageTmp2);
                            Graphics::PixelDataInfo info(_p->imageTmp2.info());
                            info.pixel = Graphics::Pixel::pixel(Graphics::Pixel::format(info.pixel), Graphics::Pixel::U8);
                            that->_p->imageTmp.set(info);
                            that->_p->imageTmp.tags = _p->imageTmp2.tags;
                            that->_p->imageTmp.colorProfile = Graphics::ColorProfile();
                            Graphics::OpenGLImageOptions options;
                            options.colorProfile = _p->imageTmp2.colorProfile;
                            options.proxyScale = false;
                            _p->openGLImage->copy(_p->imageTmp2, that->_p->imageTmp, options);
                        }
                    }
                    catch (Core::Error error)
                    {
                        error.add(
                            Util::errorLabels()[Util::ERROR_READ_IMAGE].
                            arg(QDir::toNativeSeparators(_p->fileInfo)));
                        context()->printError(error);
                    }
                    that->_p->image = &_p->imageTmp;
                    //DJV_DEBUG_PRINT("image = " << *that->_p->image);
                }
                if (_p->cache && _p->image)
                {
                    //DJV_DEBUG_PRINT("cache image");   
                    that->_p->cacheItem = cache->create(
                        new Graphics::Image(*_p->image),
                        mainWindow(),
                        frame);
                    that->_p->image = _p->cacheItem->image();
                    _p->imageTmp.close();
                    _p->imageTmp2.close();
                }
            }
            if (prev)
            {
                prev->decrement();
            }
            return _p->image;
        }

        const Graphics::ImageIOInfo & FileGroup::imageIOInfo() const
        {
            return _p->imageIOInfo;
        }

        QToolBar * FileGroup::toolBar() const
        {
            return _p->toolBar;
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
                    _p->imageLoad.reset(
                        context()->imageIOFactory()->load(tmp, _p->imageIOInfo));
                    _p->fileInfo = tmp;
                    context()->filePrefs()->addRecent(_p->fileInfo);
                }
                catch (Core::Error error)
                {
                    error.add(
                        Util::errorLabels()[Util::ERROR_OPEN_IMAGE].
                        arg(QDir::toNativeSeparators(tmp)));
                    context()->printError(error);
                }
            }
            else
            {
                _p->imageTmp = Graphics::Image();
                _p->imageTmp2 = Graphics::Image();
            }

            _p->layer = 0;
            _p->layers.clear();
            for (int i = 0; i < _p->imageIOInfo.layerCount(); ++i)
            {
                _p->layers += _p->imageIOInfo[i].layerName;
            }

            preloadUpdate();
            update();
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

        void FileGroup::setCache(bool cache)
        {
            if (cache == _p->cache)
                return;
            //DJV_DEBUG("FileGroup::setCache");
            //DJV_DEBUG_PRINT("cache = " << cache);
            _p->cache = cache;
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

            FileCache * cache = context()->fileCache();
            //DJV_DEBUG_PRINT("cache byte count     = " << cache->byteCount());
            //DJV_DEBUG_PRINT("cache max byte count = " << cache->maxByteCount());

            const FileCacheItemList items = cache->items(mainWindow());

            // Search for the next frame that isn't in the cache. We then load the
            // frame if it won't exceed the cache size.
            bool      preload = false;
            quint64   byteCount = 0;
            qint64    frame = _p->preloadFrame;
            int       frameCount = 0;
            const int totalFrames = _p->imageIOInfo.sequence.frames.count();
            for (;
                byteCount <= cache->maxByteCount() &&
                frameCount < totalFrames;
                frame = Core::Math::wrap<qint64>(frame + 1, 0, totalFrames - 1), ++frameCount)
            {
                if (FileCacheItem * item = cache->get(mainWindow(), frame))
                {
                    byteCount += Graphics::PixelDataUtil::dataByteCount(item->image()->info());
                    item->decrement();
                }
                else
                {
                    byteCount += Graphics::PixelDataUtil::dataByteCount(_p->imageIOInfo);
                    if (byteCount <= cache->maxByteCount())
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
                if (!_p->openGLImage)
                {
                    _p->openGLImage.reset(new Graphics::OpenGLImage);
                }
                Graphics::Image * image = 0;
                if (_p->imageLoad.data())
                {
                    //DJV_DEBUG_PRINT("loading image");
                    try
                    {
                        _p->imageLoad->read(
                            _p->u8Conversion ? _p->imageTmp2 : _p->imageTmp,
                            Graphics::ImageIOFrameInfo(
                                _p->imageIOInfo.sequence.frames.count() ?
                                _p->imageIOInfo.sequence.frames[frame] :
                                -1,
                                _p->layer,
                                _p->proxy));
                    }
                    catch (const Core::Error &)
                    {
                    }
                    try
                    {
                        if (_p->u8Conversion)
                        {
                            //DJV_DEBUG_PRINT("u8 conversion");
                            //DJV_DEBUG_PRINT("image = " << _p->imageTmp2);
                            Graphics::PixelDataInfo info(_p->imageTmp2.info());
                            info.pixel = Graphics::Pixel::pixel(Graphics::Pixel::format(info.pixel), Graphics::Pixel::U8);
                            _p->imageTmp.set(info);
                            _p->imageTmp.tags = _p->imageTmp2.tags;
                            _p->imageTmp.colorProfile = Graphics::ColorProfile();

                            Graphics::OpenGLImageOptions options;
                            options.colorProfile = _p->imageTmp2.colorProfile;
                            options.proxyScale = false;

                            _p->openGLImage->copy(_p->imageTmp2, _p->imageTmp, options);
                        }
                    }
                    catch (const Core::Error &)
                    {
                    }
                    image = &_p->imageTmp;
                }
                if (image)
                {
                    //DJV_DEBUG_PRINT("image = " << *image);
                    if (FileCacheItem * item = cache->create(
                        new Graphics::Image(*image),
                        mainWindow(),
                        frame))
                    {
                        item->decrement();
                    }
                    _p->imageTmp.close();
                    _p->imageTmp2.close();
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
            UI::FileBrowser * fileBrowser = context()->fileBrowser(
                qApp->translate("djv::ViewLib::FileGroup", "Open"));
            fileBrowser->setPinnable(true);
            connect(
                fileBrowser,
                SIGNAL(fileInfoChanged(const djv::Core::FileInfo &)),
                SLOT(openCallback(const djv::Core::FileInfo &)));
            fileBrowser->show();
            fileBrowser->raise();
        }

        void FileGroup::openCallback(const Core::FileInfo & fileInfo)
        {
            mainWindow()->fileOpen(fileInfo);
        }

        void FileGroup::recentCallback(QAction * action)
        {
            //DJV_DEBUG("FileGroup::recentCallback");
            const int index = action->data().toInt();
            Core::FileInfo fileInfo = context()->filePrefs()->recentFiles()[index];
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo << " " << fileInfo.type());
            mainWindow()->fileOpen(fileInfo);
        }

        void FileGroup::reloadCallback()
        {
            //DJV_DEBUG("FileGroup::reloadCallback");
            if (context()->imagePrefs()->hasFrameStoreFileReload())
            {
                Q_EMIT loadFrameStore();
            }
            cacheDel();
            _p->imageLoad.reset();
            if (!_p->fileInfo.fileName().isEmpty())
            {
                try
                {
                    _p->imageLoad.reset(
                        context()->imageIOFactory()->load(_p->fileInfo, _p->imageIOInfo));
                }
                catch (Core::Error error)
                {
                    error.add(
                        Util::errorLabels()[Util::ERROR_OPEN_IMAGE].
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
                Q_EMIT loadFrameStore();
            }
            Q_EMIT reloadFrame();
            _p->imageLoad.reset();
            if (!_p->fileInfo.fileName().isEmpty())
            {
                try
                {
                    _p->imageLoad.reset(
                        context()->imageIOFactory()->load(_p->fileInfo, _p->imageIOInfo));
                }
                catch (Core::Error error)
                {
                    error.add(
                        Util::errorLabels()[Util::ERROR_OPEN_IMAGE].
                        arg(QDir::toNativeSeparators(_p->fileInfo)));

                    context()->printError(error);
                }
            }
            Q_EMIT imageChanged();
        }

        void FileGroup::closeCallback()
        {
            //DJV_DEBUG("FileGroup::closeCallback");
            mainWindow()->fileOpen(Core::FileInfo());
        }

        void FileGroup::saveCallback()
        {
            //DJV_DEBUG("FileGroup::saveCallback");
            UI::FileBrowser * fileBrowser = context()->fileBrowser(
                qApp->translate("djv::ViewLib::FileGroup", "Save"));
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
                        Q_EMIT save(fileInfo);
                    }
                }
                else
                {
                    Q_EMIT save(fileInfo);
                }
            }
        }

        void FileGroup::saveFrameCallback()
        {
            UI::FileBrowser * fileBrowser = context()->fileBrowser(
                qApp->translate("djv::ViewLib::FileGroup", "Save Frame"));
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
                        Q_EMIT saveFrame(fileInfo);
                    }
                }
                else
                {
                    Q_EMIT saveFrame(fileInfo);
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
            context()->messagesDialog()->show();
            context()->messagesDialog()->raise();
        }

        void FileGroup::prefsCallback()
        {
            context()->prefsDialog()->show();
            context()->prefsDialog()->raise();
        }

        void FileGroup::debugLogCallback()
        {
            context()->debugLogDialog()->show();
            context()->debugLogDialog()->raise();
        }

        void FileGroup::preloadUpdate()
        {
            if (_p->cache && _p->preload && _p->preloadActive)
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
            // Update actions.
            _p->actions->action(FileActions::SAVE)->
                setEnabled(_p->imageLoad.data());
            _p->actions->action(FileActions::SAVE_FRAME)->
                setEnabled(_p->imageLoad.data());
            _p->actions->setLayers(_p->layers);
            _p->actions->setLayer(_p->layer);
            _p->actions->action(FileActions::U8_CONVERSION)->
                setChecked(_p->u8Conversion);
            _p->actions->action(FileActions::CACHE)->
                setChecked(_p->cache);
            _p->actions->action(FileActions::PRELOAD)->
                setChecked(_p->preload);

            // Update action groups.
            if (!_p->actions->group(FileActions::PROXY_GROUP)->
                actions()[_p->proxy]->isChecked())
            {
                _p->actions->group(FileActions::PROXY_GROUP)->
                    actions()[_p->proxy]->trigger();
            }
        }

        void FileGroup::cacheDel()
        {
            //DJV_DEBUG("FileGroup::cacheDel");
            context()->fileCache()->del(mainWindow());
        }

    } // namespace ViewLib
} // namespace djv
