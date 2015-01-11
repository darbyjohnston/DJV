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

//! \file djvViewFileGroup.cpp

#include <djvViewFileGroup.h>

#include <djvViewApplication.h>
#include <djvViewFileActions.h>
#include <djvViewFileCache.h>
#include <djvViewFileMenu.h>
#include <djvViewFilePrefs.h>
#include <djvViewFileToolBar.h>
#include <djvViewImagePrefs.h>
#include <djvViewImageView.h>
#include <djvViewMainWindow.h>

#include <djvApplicationMessageDialog.h>
#include <djvDebugLogDialog.h>
#include <djvFileBrowser.h>
#include <djvFileBrowserPrefs.h>
#include <djvPrefs.h>
#include <djvPrefsDialog.h>
#include <djvQuestionDialog.h>

#include <djvDebugLog.h>
#include <djvError.h>
#include <djvFileInfoUtil.h>
#include <djvListUtil.h>

#include <QAction>
#include <QActionGroup>
#include <QDir>
#include <QMenuBar>
#include <QToolBar>

//------------------------------------------------------------------------------
// djvViewFileGroup::P
//------------------------------------------------------------------------------

struct djvViewFileGroup::P
{
    P() :
        image       (0),
        autoSequence(djvViewFilePrefs::global()->hasAutoSequence()),
        layer       (0),
        proxy       (djvViewFilePrefs::global()->proxy()),
        u8Conversion(djvViewFilePrefs::global()->hasU8Conversion()),
        cacheEnabled(djvViewFilePrefs::global()->isCacheEnabled()),
        cacheRef    (0),
        actions     (0),
        menu        (0),
        toolBar     (0)
    {}

    djvFileInfo                  fileInfo;
    djvImageIoInfo               imageIoInfo;
    const djvImage *             image;
    djvImage                     imageTmp;
    djvImage                     imageTmp2;
    QScopedPointer<djvImageLoad> imageLoad;
    bool                         autoSequence;
    int                          layer;
    QStringList                  layers;
    djvPixelDataInfo::PROXY      proxy;
    bool                         u8Conversion;
    bool                         cacheEnabled;
    djvViewFileCacheRef *        cacheRef;
    djvViewFileActions *         actions;
    djvViewFileMenu *            menu;
    djvViewFileToolBar *         toolBar;
};

//------------------------------------------------------------------------------
// djvViewFileGroup
//------------------------------------------------------------------------------

djvViewFileGroup::djvViewFileGroup(
    djvViewMainWindow *      mainWindow,
    const djvViewFileGroup * copy) :
    djvViewAbstractGroup(mainWindow),
    _p(new P)
{
    if (copy)
    {
        _p->fileInfo     = copy->_p->fileInfo;
        _p->autoSequence = copy->_p->autoSequence;
        _p->layer        = copy->_p->layer;
        _p->proxy        = copy->_p->proxy;
        _p->u8Conversion = copy->_p->u8Conversion;
        _p->cacheEnabled = copy->_p->cacheEnabled;
    }

    // Create the actions.

    _p->actions = new djvViewFileActions(this);
    
    // Create the menus.

    _p->menu = new djvViewFileMenu(_p->actions, mainWindow->menuBar());

    mainWindow->menuBar()->addMenu(_p->menu);

    // Create the widgets.

    _p->toolBar = new djvViewFileToolBar(_p->actions);

    mainWindow->addToolBar(_p->toolBar);

    // Initialize.

    if (copy)
    {
        open(_p->fileInfo);
    }

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
        _p->actions->action(djvViewFileActions::AUTO_SEQUENCE),
        SIGNAL(toggled(bool)),
        SLOT(setAutoSequence(bool)));

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
        _p->actions->action(djvViewFileActions::CACHE_ENABLED),
        SIGNAL(toggled(bool)),
        SLOT(setCacheEnabled(bool)));

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
        djvViewFilePrefs::global(),
        SIGNAL(autoSequenceChanged(bool)),
        SLOT(setAutoSequence(bool)));

    connect(
        djvViewFilePrefs::global(),
        SIGNAL(proxyChanged(djvPixelDataInfo::PROXY)),
        SLOT(setProxy(djvPixelDataInfo::PROXY)));

    connect(
        djvViewFilePrefs::global(),
        SIGNAL(u8ConversionChanged(bool)),
        SLOT(setU8Conversion(bool)));

    connect(
        djvViewFilePrefs::global(),
        SIGNAL(cacheEnabledChanged(bool)),
        SLOT(setCacheEnabled(bool)));

    // Setup other callbacks.

    connect(
        djvImageIoFactory::global(),
        SIGNAL(optionChanged()),
        SLOT(reloadCallback()));
}

djvViewFileGroup::~djvViewFileGroup()
{
    _p->image = 0;

    // Cleanup.

    if (_p->cacheRef)
    {
        _p->cacheRef->refDec();
    }

    cacheDel();

    delete _p;
}

const djvFileInfo & djvViewFileGroup::fileInfo() const
{
    return _p->fileInfo;
}

bool djvViewFileGroup::hasAutoSequence() const
{
    return _p->autoSequence;
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

bool djvViewFileGroup::isCacheEnabled() const
{
    return _p->cacheEnabled;
}

const djvImage * djvViewFileGroup::image(qint64 frame) const
{
    //DJV_DEBUG("djvViewFileGroup::image");
    //DJV_DEBUG_PRINT("frame = " << frame);

    djvViewFileGroup * that = const_cast<djvViewFileGroup *>(this);

    that->mainWindow()->viewWidget()->makeCurrent();

    that->_p->image = 0;

    djvViewFileCacheRef * prev = _p->cacheRef;

    that->_p->cacheRef =
        _p->cacheEnabled ?
        djvViewFileCache::global()->get(mainWindow(), frame) :
        0;

    if (_p->cacheRef)
    {
        //DJV_DEBUG_PRINT("cached image");

        that->_p->image = _p->cacheRef->image();
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
                    djvImageIoFrameInfo(
                        _p->imageIoInfo.sequence.frames.count() ?
                            _p->imageIoInfo.sequence.frames[frame] :
                            -1,
                        _p->layer,
                        _p->proxy));

                //DJV_DEBUG_PRINT("image = " << *_p->image);
            }
            catch (djvError error)
            {
                error.add(
                    djvViewUtil::errorLabels()[djvViewUtil::ERROR_READ_IMAGE].
                    arg(QDir::toNativeSeparators(_p->fileInfo)));

                DJV_VIEW_APP->printError(error);
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

                    djvOpenGlImageOptions options;
                    options.colorProfile = _p->imageTmp2.colorProfile;
                    options.proxyScale = false;

                    djvOpenGlImage::copy(_p->imageTmp2, that->_p->imageTmp, options);
                }
            }
            catch (djvError error)
            {
                error.add(
                    djvViewUtil::errorLabels()[djvViewUtil::ERROR_READ_IMAGE].
                    arg(QDir::toNativeSeparators(_p->fileInfo)));

                DJV_VIEW_APP->printError(error);
            }

            that->_p->image = &_p->imageTmp;
        }

        if (_p->image && _p->cacheEnabled)
        {
            //DJV_DEBUG_PRINT("cache image");
            
            that->_p->cacheRef = djvViewFileCache::global()->create(
                new djvImage(*_p->image),
                mainWindow(),
                frame);
            
            that->_p->image = _p->cacheRef->image();
            
            _p->imageTmp.close();
            _p->imageTmp2.close();
        }
    }

    if (prev)
    {
        prev->refDec();
    }

    return _p->image;
}

const djvImageIoInfo & djvViewFileGroup::imageIoInfo() const
{
    return _p->imageIoInfo;
}

QToolBar * djvViewFileGroup::toolBar() const
{
    return _p->toolBar;
}

void djvViewFileGroup::open(const djvFileInfo & in)
{
    //DJV_DEBUG("djvViewFileGroup::open");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("in type = " << in.type());

    djvFileInfo fileInfo;
    
    if (! in.isEmpty())
    {
        fileInfo = djvFileInfoUtil::fixPath(in);
        fileInfo.setType(in.type());
    }

    // Initialize.

    cacheDel();

    _p->fileInfo = djvFileInfo();
    _p->imageIoInfo = djvImageIoInfo();
    _p->imageLoad.reset();

    // Match wildcards.

    if (fileInfo.isSequenceWildcard())
    {
        fileInfo = djvFileInfoUtil::sequenceWildcardMatch(
            fileInfo,
            djvFileInfoUtil::list(fileInfo.path()));
        
        DJV_LOG("djvViewFileGroup", QString("match wildcards = \"%1\"").arg(fileInfo));
    }
    
    //DJV_DEBUG_PRINT("file = " << fileInfo);
    //DJV_DEBUG_PRINT("file type = " << fileInfo.type());
    //DJV_DEBUG_PRINT("sequence valid = " << fileInfo.isSequenceValid());
    //DJV_DEBUG_PRINT("sequence = " << fileInfo.sequence());

    DJV_LOG("djvViewFileGroup", QString("Open file = \"%1\"").arg(fileInfo));
    
    // Automatic sequence loading.

    if (_p->autoSequence &&
        fileInfo.isSequenceValid() &&
        fileInfo.sequence().frames.count() == 1)
    {
        const djvFileInfoList items = djvFileInfoUtil::list(fileInfo.path());
        
        for (int i = 0; i < items.count(); ++i)
        {
            if (items[i].isSequenceValid() &&
                items[i].extension() == fileInfo.extension() &&
                items[i].base()      == fileInfo.base())
            {
                fileInfo.setType(djvFileInfo::SEQUENCE);
                fileInfo.setSequence(items[i].sequence());
                
                break;
            }
        }

        //DJV_DEBUG_PRINT("sequence = " << fileInfo);

        DJV_LOG("djvViewFileGroup", QString("sequence = \"%1\"").arg(fileInfo));
    }

    // Load.

    if (! fileInfo.fileName().isEmpty())
    {
        //DJV_DEBUG_PRINT("loading...");

        try
        {
            _p->imageLoad.reset(
                djvImageIoFactory::global()->load(fileInfo, _p->imageIoInfo));

            _p->fileInfo = fileInfo;

            djvViewFilePrefs::global()->addRecent(_p->fileInfo);
        }
        catch (djvError error)
        {
            error.add(
                djvViewUtil::errorLabels()[djvViewUtil::ERROR_OPEN_IMAGE].
                arg(QDir::toNativeSeparators(fileInfo)));

            DJV_VIEW_APP->printError(error);
        }
    }
    else
    {
        _p->imageTmp  = djvImage();
        _p->imageTmp2 = djvImage();    
    }

    _p->layer = 0;
    _p->layers.clear();

    for (int i = 0; i < _p->imageIoInfo.layerCount(); ++i)
    {
        _p->layers += _p->imageIoInfo[i].layerName;
    }

    update();
}

void djvViewFileGroup::setAutoSequence(bool autoSequence)
{
    if (autoSequence == _p->autoSequence)
        return;

    _p->autoSequence = autoSequence;

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
    update();

    Q_EMIT imageChanged();
}

void djvViewFileGroup::setU8Conversion(bool conversion)
{
    if (conversion == _p->u8Conversion)
        return;
        
    _p->u8Conversion = conversion;

    cacheDel();
    update();

    Q_EMIT imageChanged();
}

void djvViewFileGroup::setCacheEnabled(bool cache)
{
    if (cache == _p->cacheEnabled)
        return;

    //DJV_DEBUG("djvViewFileGroup::setCacheEnabled");
    //DJV_DEBUG_PRINT("cache = " << cache);

    _p->cacheEnabled = cache;

    cacheDel();
    update();

    Q_EMIT imageChanged();
}

void djvViewFileGroup::openCallback()
{
    djvFileBrowser * fileBrowser = djvFileBrowser::global(tr("Open"));

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

    djvFileInfo fileInfo = djvViewFilePrefs::global()->recentFiles()[index];
    
    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo << " " << fileInfo.type());

    mainWindow()->fileOpen(fileInfo);
}

void djvViewFileGroup::reloadCallback()
{
    //DJV_DEBUG("djvViewFileGroup::reloadCallback");

    if (djvViewImagePrefs::global()->hasFrameStoreFileReload())
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
                djvImageIoFactory::global()->load(_p->fileInfo, _p->imageIoInfo));
        }
        catch (djvError error)
        {
            error.add(
                djvViewUtil::errorLabels()[djvViewUtil::ERROR_OPEN_IMAGE].
                arg(QDir::toNativeSeparators(_p->fileInfo)));

            DJV_VIEW_APP->printError(error);
        }
    }

    Q_EMIT imageChanged();
}

void djvViewFileGroup::reloadFrameCallback()
{
    //DJV_DEBUG("djvViewFileGroup::reloadFrameCallback");

    if (djvViewImagePrefs::global()->hasFrameStoreFileReload())
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
                djvImageIoFactory::global()->load(_p->fileInfo, _p->imageIoInfo));
        }
        catch (djvError error)
        {
            error.add(
                djvViewUtil::errorLabels()[djvViewUtil::ERROR_OPEN_IMAGE].
                arg(QDir::toNativeSeparators(_p->fileInfo)));

            DJV_VIEW_APP->printError(error);
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

    djvFileBrowser * fileBrowser = djvFileBrowser::global(tr("Save"));

    if (fileBrowser->exec() == QDialog::Accepted)
    {
        const djvFileInfo & fileInfo = fileBrowser->fileInfo();

        //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);

        if (djvFileInfoUtil::exists(fileInfo))
        {
            //DJV_DEBUG_PRINT("exists");

            djvQuestionDialog dialog(
                tr("Overwrite existing file \"%1\"?").
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
    djvFileBrowser * fileBrowser = djvFileBrowser::global(tr("Save Frame"));
    
    if (fileBrowser->exec() == QDialog::Accepted)
    {
        const djvFileInfo & fileInfo = fileBrowser->fileInfo();

        if (djvFileInfoUtil::exists(fileInfo))
        {
            djvQuestionDialog dialog(
                tr("Overwrite existing file \"%1\"?").
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
    djvViewFileCache::global()->clear();
}

void djvViewFileGroup::messagesCallback()
{
    djvApplicationMessageDialog::global()->show();
    djvApplicationMessageDialog::global()->raise();
}

void djvViewFileGroup::prefsCallback()
{
    djvPrefsDialog::global()->show();
    djvPrefsDialog::global()->raise();
}

void djvViewFileGroup::debugLogCallback()
{
    djvDebugLogDialog::global()->show();
    djvDebugLogDialog::global()->raise();
}

void djvViewFileGroup::update()
{
    // Update actions.

    _p->actions->action(djvViewFileActions::SAVE)->
        setEnabled(_p->imageLoad.data());

    _p->actions->action(djvViewFileActions::SAVE_FRAME)->
        setEnabled(_p->imageLoad.data());

    _p->actions->action(djvViewFileActions::AUTO_SEQUENCE)->
        setChecked(_p->autoSequence);

    _p->actions->setLayers(_p->layers);
    _p->actions->setLayer(_p->layer);

    _p->actions->action(djvViewFileActions::U8_CONVERSION)->
        setChecked(_p->u8Conversion);

    _p->actions->action(djvViewFileActions::CACHE_ENABLED)->
        setChecked(_p->cacheEnabled);

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

    djvViewFileCache::global()->del(mainWindow());
}
