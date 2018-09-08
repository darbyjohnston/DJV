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

#include <djvViewFileGroup.h>

#include <djvViewContext.h>
#include <djvViewFileActions.h>
#include <djvViewFileCache.h>
#include <djvViewFileMenu.h>
#include <djvViewFilePrefs.h>
#include <djvViewFileToolBar.h>
#include <djvViewImagePrefs.h>
#include <djvViewImageView.h>
#include <djvViewMainWindow.h>

#include <djvDebugLogDialog.h>
#include <djvFileBrowser.h>
#include <djvFileBrowserPrefs.h>
#include <djvMessagesDialog.h>
#include <djvPrefs.h>
#include <djvPrefsDialog.h>
#include <djvQuestionDialog.h>

#include <djvPixelDataUtil.h>

#include <djvDebugLog.h>
#include <djvError.h>
#include <djvFileInfoUtil.h>
#include <djvListUtil.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDir>
#include <QMenuBar>
#include <QToolBar>

//------------------------------------------------------------------------------
// djvViewFileGroup::Private
//------------------------------------------------------------------------------

struct djvViewFileGroup::Private
{
    Private(djvViewContext * context) :
        image        (0),
        layer        (0),
        proxy        (context->filePrefs()->proxy()),
        u8Conversion (context->filePrefs()->hasU8Conversion()),
        cache        (context->filePrefs()->hasCache()),
        cacheItem    (0),
        preload      (context->filePrefs()->hasPreload()),
        preloadActive(false),
        preloadTimer (0),
        preloadFrame (0),
        actions      (0),
        menu         (0),
        toolBar      (0)
    {}

    djvFileInfo                  fileInfo;
    djvImageIOInfo               imageIOInfo;
    const djvImage *             image;
    djvImage                     imageTmp;
    djvImage                     imageTmp2;
    QScopedPointer<djvImageLoad> imageLoad;
    int                          layer;
    QStringList                  layers;
    djvPixelDataInfo::PROXY      proxy;
    bool                         u8Conversion;
    bool                         cache;
    djvViewFileCacheItem *       cacheItem;
    bool                         preload;
    bool                         preloadActive;
    int                          preloadTimer;
    qint64                       preloadFrame;
    djvViewFileActions *         actions;
    djvViewFileMenu *            menu;
    djvViewFileToolBar *         toolBar;
};

//------------------------------------------------------------------------------
// djvViewFileGroup
//------------------------------------------------------------------------------

djvViewFileGroup::djvViewFileGroup(
    const djvViewFileGroup * copy,
    djvViewMainWindow *      mainWindow,
    djvViewContext *         context) :
    djvViewAbstractGroup(mainWindow, context),
    _p(new Private(context))
{
    //DJV_DEBUG("djvViewFileGroup::djvViewFileGroup");
    
    if (copy)
    {
        _p->fileInfo     = copy->_p->fileInfo;
        _p->layer        = copy->_p->layer;
        _p->proxy        = copy->_p->proxy;
        _p->u8Conversion = copy->_p->u8Conversion;
        _p->cache        = copy->_p->cache;
        _p->preload      = copy->_p->preload;
    }

    // Create the actions.
    _p->actions = new djvViewFileActions(context, this);
    
    // Create the menus.
    _p->menu = new djvViewFileMenu(_p->actions, mainWindow->menuBar());
    mainWindow->menuBar()->addMenu(_p->menu);

    // Create the widgets.
    _p->toolBar = new djvViewFileToolBar(_p->actions, context);
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
        _p->actions->action(djvViewFileActions::OPEN),
        SIGNAL(triggered()),
        SLOT(openCallback()));
    connect(
        _p->actions->action(djvViewFileActions::RELOAD),
        SIGNAL(triggered()),
        SLOT(reloadCallback()));
    connect(
        _p->actions->action(djvViewFileActions::RELOAD_FRAME),
        SIGNAL(triggered()),
        SLOT(reloadFrameCallback()));
    connect(
        _p->actions->action(djvViewFileActions::CLOSE),
        SIGNAL(triggered()),
        SLOT(closeCallback()));
    connect(
        _p->actions->action(djvViewFileActions::SAVE),
        SIGNAL(triggered()),
        SLOT(saveCallback()));
    connect(
        _p->actions->action(djvViewFileActions::SAVE_FRAME),
        SIGNAL(triggered()),
        SLOT(saveFrameCallback()));
    connect(
        _p->actions->action(djvViewFileActions::LAYER_PREV),
        SIGNAL(triggered()),
        SLOT(layerPrevCallback()));
    connect(
        _p->actions->action(djvViewFileActions::LAYER_NEXT),
        SIGNAL(triggered()),
        SLOT(layerNextCallback()));
    connect(
        _p->actions->action(djvViewFileActions::U8_CONVERSION),
        SIGNAL(toggled(bool)),
        SLOT(setU8Conversion(bool)));
    connect(
        _p->actions->action(djvViewFileActions::CACHE),
        SIGNAL(toggled(bool)),
        SLOT(setCache(bool)));
    connect(
        _p->actions->action(djvViewFileActions::PRELOAD),
        SIGNAL(toggled(bool)),
        SLOT(setPreload(bool)));
    connect(
        _p->actions->action(djvViewFileActions::CLEAR_CACHE),
        SIGNAL(triggered()),
        SLOT(cacheClearCallback()));
    connect(
        _p->actions->action(djvViewFileActions::MESSAGES),
        SIGNAL(triggered()),
        SLOT(messagesCallback()));
    connect(
        _p->actions->action(djvViewFileActions::PREFS),
        SIGNAL(triggered()),
        SLOT(prefsCallback()));
    connect(
        _p->actions->action(djvViewFileActions::DEBUG_LOG),
        SIGNAL(triggered()),
        SLOT(debugLogCallback()));
    connect(
        _p->actions->action(djvViewFileActions::EXIT),
        SIGNAL(triggered()),
        qApp,
        SLOT(quit()));

    // Setup the action group callbacks.
    connect(
        _p->actions->group(djvViewFileActions::RECENT_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(recentCallback(QAction *)));
    connect(
        _p->actions->group(djvViewFileActions::LAYER_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(layerCallback(QAction *)));
    connect(
        _p->actions->group(djvViewFileActions::PROXY_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(proxyCallback(QAction *)));

    // Setup the preferences callbacks.
    connect(
        context->filePrefs(),
        SIGNAL(proxyChanged(djvPixelDataInfo::PROXY)),
        SLOT(setProxy(djvPixelDataInfo::PROXY)));
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

djvViewFileGroup::~djvViewFileGroup()
{
    if (_p->preloadTimer)
    {
        killTimer(_p->preloadTimer);
        _p->preloadTimer = 0;
    }
    _p->image = 0;

    // Cleanup.
    if (_p->cacheItem)
    {
        _p->cacheItem->decrement();
    }
    cacheDel();
}

const djvFileInfo & djvViewFileGroup::fileInfo() const
{
    return _p->fileInfo;
}

int djvViewFileGroup::layer() const
{
    return _p->layer;
}

djvPixelDataInfo::PROXY djvViewFileGroup::proxy() const
{
    return _p->proxy;
}

bool djvViewFileGroup::hasU8Conversion() const
{
    return _p->u8Conversion;
}

bool djvViewFileGroup::hasCache() const
{
    return _p->cache;
}

bool djvViewFileGroup::hasPreload() const
{
    return _p->preload;
}

bool djvViewFileGroup::isPreloadActive() const
{
    return _p->preloadActive;
}

qint64 djvViewFileGroup::preloadFrame() const
{
    return _p->preloadFrame;
}

const djvImage * djvViewFileGroup::image(qint64 frame) const
{
    //DJV_DEBUG("djvViewFileGroup::image");
    //DJV_DEBUG_PRINT("frame = " << frame);

    djvViewFileGroup * that = const_cast<djvViewFileGroup *>(this);
    that->mainWindow()->viewWidget()->makeCurrent();
    djvViewFileCache * cache = context()->fileCache();
    that->_p->image = 0;
    djvViewFileCacheItem * prev = _p->cacheItem;
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
                    djvImageIOFrameInfo(
                        _p->imageIOInfo.sequence.frames.count() ?
                            _p->imageIOInfo.sequence.frames[frame] :
                            -1,
                        _p->layer,
                        _p->proxy));
            }
            catch (djvError error)
            {
                error.add(
                    djvViewUtil::errorLabels()[djvViewUtil::ERROR_READ_IMAGE].
                    arg(QDir::toNativeSeparators(_p->fileInfo)));
                context()->printError(error);
            }

            try
            {
                if (_p->u8Conversion)
                {
                    //DJV_DEBUG_PRINT("u8 conversion");
                    //DJV_DEBUG_PRINT("image = " << _p->imageTmp2);
                    djvPixelDataInfo info(_p->imageTmp2.info());
                    info.pixel = djvPixel::pixel(
                        djvPixel::format(info.pixel), djvPixel::U8);
                    that->_p->imageTmp.set(info);
                    that->_p->imageTmp.tags = _p->imageTmp2.tags;
                    that->_p->imageTmp.colorProfile = djvColorProfile();
                    djvOpenGLImageOptions options;
                    options.colorProfile = _p->imageTmp2.colorProfile;
                    options.proxyScale = false;
                    djvOpenGLImage::copy(_p->imageTmp2, that->_p->imageTmp, options);
                }
            }
            catch (djvError error)
            {
                error.add(
                    djvViewUtil::errorLabels()[djvViewUtil::ERROR_READ_IMAGE].
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
                new djvImage(*_p->image),
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

const djvImageIOInfo & djvViewFileGroup::imageIOInfo() const
{
    return _p->imageIOInfo;
}

QToolBar * djvViewFileGroup::toolBar() const
{
    return _p->toolBar;
}

void djvViewFileGroup::open(const djvFileInfo & fileInfo)
{
    //DJV_DEBUG("djvViewFileGroup::open");
    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
    //DJV_DEBUG_PRINT("type = " << fileInfo.type());

    DJV_LOG(context()->debugLog(), "djvViewFileGroup",
        QString("Open file = \"%1\"").arg(fileInfo));

    cacheDel();
    djvFileInfo tmp = fileInfo;
    _p->fileInfo = djvFileInfo();
    _p->imageIOInfo = djvImageIOInfo();
    _p->imageLoad.reset();

    // Load the file.
    if (! tmp.fileName().isEmpty())
    {
        //DJV_DEBUG_PRINT("loading...");
        try
        {
            _p->imageLoad.reset(
                context()->imageIOFactory()->load(tmp, _p->imageIOInfo));
            _p->fileInfo = tmp;
            context()->filePrefs()->addRecent(_p->fileInfo);
        }
        catch (djvError error)
        {
            error.add(
                djvViewUtil::errorLabels()[djvViewUtil::ERROR_OPEN_IMAGE].
                arg(QDir::toNativeSeparators(tmp)));
            context()->printError(error);
        }
    }
    else
    {
        _p->imageTmp  = djvImage();
        _p->imageTmp2 = djvImage();    
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

void djvViewFileGroup::setLayer(int layer)
{
    if (layer == _p->layer)
        return;
    //DJV_DEBUG("djvViewFileGroup::setLayer");
    //DJV_DEBUG_PRINT("layer = " << layer);
    const int count = _p->layers.count();
    //DJV_DEBUG_PRINT("layer list = " << size);
    _p->layer = djvMath::wrap(layer, 0, count - 1);
    //DJV_DEBUG_PRINT("layer = " << _layer);
    cacheDel();
    preloadUpdate();
    update();
    Q_EMIT imageChanged();
}

void djvViewFileGroup::setProxy(djvPixelDataInfo::PROXY proxy)
{
    if (proxy == _p->proxy)
        return;
    //DJV_DEBUG("djvViewFileGroup::setProxy");
    //DJV_DEBUG_PRINT("proxy = " << proxy);
    _p->proxy = proxy;
    cacheDel();
    preloadUpdate();
    update();
    Q_EMIT imageChanged();
}

void djvViewFileGroup::setU8Conversion(bool conversion)
{
    if (conversion == _p->u8Conversion)
        return;
    _p->u8Conversion = conversion;
    cacheDel();
    preloadUpdate();
    update();
    Q_EMIT imageChanged();
}

void djvViewFileGroup::setCache(bool cache)
{
    if (cache == _p->cache)
        return;
    //DJV_DEBUG("djvViewFileGroup::setCache");
    //DJV_DEBUG_PRINT("cache = " << cache);
    _p->cache = cache;
    cacheDel();
    preloadUpdate();
    update();
    Q_EMIT imageChanged();
}

void djvViewFileGroup::setPreload(bool preload)
{
    if (preload == _p->preload)
        return;
    //DJV_DEBUG("djvViewFileGroup::setPreload");
    //DJV_DEBUG_PRINT("preload = " << preload);
    _p->preload = preload;
    preloadUpdate();
    update();
}

void djvViewFileGroup::setPreloadActive(bool active)
{
    if (active == _p->preloadActive)
        return;
    //DJV_DEBUG("djvViewFileGroup::setPreloadActive");
    //DJV_DEBUG_PRINT("active = " << active);
    _p->preloadActive = active;
    preloadUpdate();
    update();
}

void djvViewFileGroup::setPreloadFrame(qint64 frame)
{
    if (frame == _p->preloadFrame)
        return;
    _p->preloadFrame = frame;
    preloadUpdate();
    update();
}

void djvViewFileGroup::timerEvent(QTimerEvent *)
{
    //DJV_DEBUG("djvViewFileGroup::timerEvent");
    //DJV_DEBUG_PRINT("preload frame        = " << _p->preloadFrame);
    
    djvViewFileCache * cache = context()->fileCache();
    //DJV_DEBUG_PRINT("cache byte count     = " << cache->byteCount());
    //DJV_DEBUG_PRINT("cache max byte count = " << cache->maxByteCount());

    const djvViewFileCacheItemList items = cache->items(mainWindow());

    // Search for the next frame that isn't in the cache. We then load the
    // frame if it won't exceed the cache size.
    bool      preload     = false;
    quint64   byteCount   = 0;
    qint64    frame       = _p->preloadFrame;
    int       frameCount  = 0;
    const int totalFrames = _p->imageIOInfo.sequence.frames.count();
    for (;
        byteCount <= cache->maxByteCount() &&
        frameCount < totalFrames;
        frame = djvMath::wrap<qint64>(frame + 1, 0, totalFrames - 1), ++frameCount)
    {
        if (djvViewFileCacheItem * item = cache->get(mainWindow(), frame))
        {
            byteCount += djvPixelDataUtil::dataByteCount(item->image()->info());
            item->decrement();
        }
        else
        {
            byteCount += djvPixelDataUtil::dataByteCount(_p->imageIOInfo);
            if (byteCount <= cache->maxByteCount())
            {
                preload = true;
            }
            break;
        }
    }
    //DJV_DEBUG_PRINT("byteCount            = " << byteCount);
    //DJV_DEBUG_PRINT("preload              = " << preload);
    //DJV_DEBUG_PRINT("frame                = " << frame);

    if (preload)
    {
        mainWindow()->viewWidget()->makeCurrent();
        djvImage * image = 0;
        if (_p->imageLoad.data())
        {
            //DJV_DEBUG_PRINT("loading image");
            try
            {
                _p->imageLoad->read(
                    _p->u8Conversion ? _p->imageTmp2 : _p->imageTmp,
                    djvImageIOFrameInfo(
                    _p->imageIOInfo.sequence.frames.count() ?
                    _p->imageIOInfo.sequence.frames[frame] :
                    -1,
                    _p->layer,
                    _p->proxy));
            }
            catch (const djvError &)
            {}
            try
            {
                if (_p->u8Conversion)
                {
                    //DJV_DEBUG_PRINT("u8 conversion");
                    //DJV_DEBUG_PRINT("image = " << _p->imageTmp2);
                    djvPixelDataInfo info(_p->imageTmp2.info());
                    info.pixel = djvPixel::pixel(
                        djvPixel::format(info.pixel), djvPixel::U8);
                    _p->imageTmp.set(info);
                    _p->imageTmp.tags = _p->imageTmp2.tags;
                    _p->imageTmp.colorProfile = djvColorProfile();

                    djvOpenGLImageOptions options;
                    options.colorProfile = _p->imageTmp2.colorProfile;
                    options.proxyScale = false;

                    djvOpenGLImage::copy(_p->imageTmp2, _p->imageTmp, options);
                }
            }
            catch (const djvError &)
            {}
            image = &_p->imageTmp;
        }
        if (image)
        {
            //DJV_DEBUG_PRINT("image = " << *image);
            if (djvViewFileCacheItem * item = cache->create(
                new djvImage(*image),
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

void djvViewFileGroup::openCallback()
{
    djvFileBrowser * fileBrowser = context()->fileBrowser(
        qApp->translate("djvViewFileGroup", "Open"));
    fileBrowser->setPinnable(true);
    connect(
        fileBrowser,
        SIGNAL(fileInfoChanged(const djvFileInfo &)),
        SLOT(openCallback(const djvFileInfo &)));
    fileBrowser->show();
    fileBrowser->raise();
}

void djvViewFileGroup::openCallback(const djvFileInfo & fileInfo)
{
    mainWindow()->fileOpen(fileInfo);
}

void djvViewFileGroup::recentCallback(QAction * action)
{
    //DJV_DEBUG("djvViewFileGroup::recentCallback");
    const int index = action->data().toInt();
    djvFileInfo fileInfo = context()->filePrefs()->recentFiles()[index];
    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo << " " << fileInfo.type());
    mainWindow()->fileOpen(fileInfo);
}

void djvViewFileGroup::reloadCallback()
{
    //DJV_DEBUG("djvViewFileGroup::reloadCallback");
    if (context()->imagePrefs()->hasFrameStoreFileReload())
    {
        Q_EMIT loadFrameStore();
    }
    cacheDel();
    _p->imageLoad.reset();
    if (! _p->fileInfo.fileName().isEmpty())
    {
        try
        {
            _p->imageLoad.reset(
                context()->imageIOFactory()->load(_p->fileInfo, _p->imageIOInfo));
        }
        catch (djvError error)
        {
            error.add(
                djvViewUtil::errorLabels()[djvViewUtil::ERROR_OPEN_IMAGE].
                arg(QDir::toNativeSeparators(_p->fileInfo)));

            context()->printError(error);
        }
    }
    Q_EMIT imageChanged();
}

void djvViewFileGroup::reloadFrameCallback()
{
    //DJV_DEBUG("djvViewFileGroup::reloadFrameCallback");
    if (context()->imagePrefs()->hasFrameStoreFileReload())
    {
        Q_EMIT loadFrameStore();
    }
    Q_EMIT reloadFrame();
    _p->imageLoad.reset();
    if (! _p->fileInfo.fileName().isEmpty())
    {
        try
        {
            _p->imageLoad.reset(
                context()->imageIOFactory()->load(_p->fileInfo, _p->imageIOInfo));
        }
        catch (djvError error)
        {
            error.add(
                djvViewUtil::errorLabels()[djvViewUtil::ERROR_OPEN_IMAGE].
                arg(QDir::toNativeSeparators(_p->fileInfo)));

            context()->printError(error);
        }
    }
    Q_EMIT imageChanged();
}

void djvViewFileGroup::closeCallback()
{
    //DJV_DEBUG("djvViewFileGroup::closeCallback");
    mainWindow()->fileOpen(djvFileInfo());
}

void djvViewFileGroup::saveCallback()
{
    //DJV_DEBUG("djvViewFileGroup::saveCallback");
    djvFileBrowser * fileBrowser = context()->fileBrowser(
        qApp->translate("djvViewFileGroup", "Save"));
    if (fileBrowser->exec() == QDialog::Accepted)
    {
        const djvFileInfo & fileInfo = fileBrowser->fileInfo();
        //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
        if (djvFileInfoUtil::exists(fileInfo))
        {
            //DJV_DEBUG_PRINT("exists");
            djvQuestionDialog dialog(
                qApp->translate("djvViewFileGroup", "Overwrite existing file \"%1\"?").
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

void djvViewFileGroup::saveFrameCallback()
{
    djvFileBrowser * fileBrowser = context()->fileBrowser(
        qApp->translate("djvViewFileGroup", "Save Frame"));    
    if (fileBrowser->exec() == QDialog::Accepted)
    {
        const djvFileInfo & fileInfo = fileBrowser->fileInfo();
        if (djvFileInfoUtil::exists(fileInfo))
        {
            djvQuestionDialog dialog(
                qApp->translate("djvViewFileGroup", "Overwrite existing file \"%1\"?").
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

void djvViewFileGroup::layerCallback(QAction * action)
{
    const int index = action->data().toInt();
    setLayer(index);
}

void djvViewFileGroup::layerPrevCallback()
{
    setLayer(_p->layer - 1);
}

void djvViewFileGroup::layerNextCallback()
{
    setLayer(_p->layer + 1);
}

void djvViewFileGroup::proxyCallback(QAction * action)
{
    const int index = action->data().toInt();
    setProxy(static_cast<djvPixelDataInfo::PROXY>(index));
}

void djvViewFileGroup::cacheClearCallback()
{
    context()->fileCache()->clear();
}

void djvViewFileGroup::messagesCallback()
{
    context()->messagesDialog()->show();
    context()->messagesDialog()->raise();
}

void djvViewFileGroup::prefsCallback()
{
    context()->prefsDialog()->show();
    context()->prefsDialog()->raise();
}

void djvViewFileGroup::debugLogCallback()
{
    context()->debugLogDialog()->show();
    context()->debugLogDialog()->raise();
}

void djvViewFileGroup::preloadUpdate()
{
    if (_p->cache && _p->preload && _p->preloadActive)
    {
        if (! _p->preloadTimer)
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

void djvViewFileGroup::update()
{
    // Update actions.
    _p->actions->action(djvViewFileActions::SAVE)->
        setEnabled(_p->imageLoad.data());
    _p->actions->action(djvViewFileActions::SAVE_FRAME)->
        setEnabled(_p->imageLoad.data());
    _p->actions->setLayers(_p->layers);
    _p->actions->setLayer(_p->layer);
    _p->actions->action(djvViewFileActions::U8_CONVERSION)->
        setChecked(_p->u8Conversion);
    _p->actions->action(djvViewFileActions::CACHE)->
        setChecked(_p->cache);
    _p->actions->action(djvViewFileActions::PRELOAD)->
        setChecked(_p->preload);

    // Update action groups.
    if (! _p->actions->group(djvViewFileActions::PROXY_GROUP)->
        actions()[_p->proxy]->isChecked())
    {
        _p->actions->group(djvViewFileActions::PROXY_GROUP)->
            actions()[_p->proxy]->trigger();
    }
}

void djvViewFileGroup::cacheDel()
{
    //DJV_DEBUG("djvViewFileGroup::cacheDel");
    context()->fileCache()->del(mainWindow());
}
