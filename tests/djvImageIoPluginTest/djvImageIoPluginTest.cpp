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

//! \file djvImageIoPluginTest.cpp

#include <djvImageIoPluginTest.h>

#include <djvDebug.h>
#include <djvError.h>
#include <djvErrorUtil.h>
#include <djvFileInfo.h>
#include <djvFileIo.h>
#include <djvImageApplication.h>
#include <djvImage.h>
#include <djvImageIo.h>
#include <djvMemory.h>
#include <djvOpenGlImage.h>
#include <djvOpenGlOffscreenBuffer.h>
#include <djvPixelDataUtil.h>

#include <QPair>

#include <algorithm>

void djvImageIoPluginTest::run(int & argc, char ** argv)
{
    DJV_DEBUG("djvImageIoPluginTest::run");

    djvImageApplication app("djvImageIoPluginTest", argc, argv);
    
    initData();
    initImages();
    initPlugins();

    typedef QPair<QString, djvPixel::PIXEL> Disable;
    
    //! \todo Fix these image I/O confidence tests.
    
    QVector<Disable> disabled;
    disabled += Disable("IFF", djvPixel::RGB_U16);
    disabled += Disable("IFF", djvPixel::RGBA_U16);
    
    for (int j = 0; j < _plugins.count(); ++j)
    {
        djvImageIo * plugin = static_cast<djvImageIo *>(_plugins[j]);
    
        for (int i = 0; i < _images.count(); ++i)
        {
            const djvImage & image = _images[i];
            
            bool test = true;
            
            for (int k = 0; k < disabled.count(); ++k)
            {
                if (plugin->pluginName() == disabled[k].first &&
                    image.pixel()  == disabled[k].second)
                {
                    test = false;
                    
                    break;
                }
            }
            
            if (test)
            {
                runTest(plugin, image);
            }
        }
    }
}

void djvImageIoPluginTest::initPlugins()
{
    DJV_DEBUG("djvImageIoPluginTest::initPlugins");

    QStringList option;
    option << "None";
    djvImageIoFactory::global()->setOption("Cineon", "Input Color Profile", option);
    option << "None";
    djvImageIoFactory::global()->setOption("Cineon", "Output Color Profile", option);
    option << "None";
    djvImageIoFactory::global()->setOption("DPX", "Input Color Profile", option);
    option << "None";
    djvImageIoFactory::global()->setOption("DPX", "Output Color Profile", option);
    option << "None";
    djvImageIoFactory::global()->setOption("OpenEXR", "Input Color Profile", option);

    //! \todo Fix libquicktime and QuickTime image I/O testing.
    
    QStringList disable = QStringList() <<
        "libquicktime" <<
        "QuickTime";
    
    //! \todo JPEG is lossy which will cause the pixel comparison tests to fail.
    
    disable += "JPEG";

    const QList<djvPlugin *> & pluginsTmp = djvImageIoFactory::global()->plugins();
    
    for (int i = 0; i < pluginsTmp.count(); ++i)
    {
        if (! disable.contains(pluginsTmp[i]->pluginName()))
        {
            _plugins.append(pluginsTmp[i]);
        }
    }

    DJV_DEBUG_PRINT("plugins = " << _plugins.count());
}

void djvImageIoPluginTest::initData()
{
    DJV_DEBUG("djvImageIoPluginTest::initData");

    if (1)
    {
        //! \todo The one pixel wide data seems to be triggering errors
        //! in the floating point tests.
        
        _sizes += QVector<djvVector2i>() <<
            djvVector2i(11,  1) <<
            //djvVector2i( 1,  7) <<
            djvVector2i(11,  7) <<
            djvVector2i( 7, 11);

        for (int i = 0; i < djvPixel::PIXEL_COUNT; ++i)
        {
            const djvPixel::PIXEL pixel = static_cast<djvPixel::PIXEL>(i);
            
            _pixels += pixel;
        }
    }
    else
    {
        _sizes += djvVector2i(10, 1);
        //_pixels += djvPixel::RGB_U10;
        _pixels += djvPixel::RGB_F16;
    }
    
    DJV_DEBUG_PRINT("sizes = " << _sizes);
    DJV_DEBUG_PRINT("pixels = " << _pixels);
}

void djvImageIoPluginTest::initImages()
{
    DJV_DEBUG("djvImageIoPluginTest::initImages");

    for (int i = 0; i < _sizes.count(); ++i)
    {
        for (int j = 0; j < _pixels.count(); ++j)
        {
            djvImage gradient(djvPixelDataInfo(_sizes[i], djvPixel::L_F32));
            djvPixelDataUtil::gradient(gradient);
            
            djvImage image(djvPixelDataInfo(gradient.size(), _pixels[j]));
            djvOpenGlImage::copy(gradient, image);

            DJV_DEBUG_PRINT("image = " << image);

            djvPixelData p(djvPixelDataInfo(1, 1, image.pixel()));
            djvOpenGlImageOptions options;
            options.xform.position = djvVector2f(0, 0);
            djvOpenGlImage::copy(image, p, options);
            djvColor c(p.data(), p.pixel());
            
            DJV_DEBUG_PRINT("c0 = " << c);

            options.xform.position = -djvVector2f(
                image.size().x - 1, image.size().y - 1);
            djvOpenGlImage::copy(image, p, options);
            c = djvColor(p.data(), p.pixel());
            
            DJV_DEBUG_PRINT("c1 = " << c);

            _images += image;
        }
    }
}

void djvImageIoPluginTest::runTest(djvImageIo * plugin, const djvImage & image)
{
    DJV_DEBUG("djvImageIoPluginTest::runTest");
    DJV_DEBUG_PRINT("plugin = " << plugin->pluginName());
    DJV_DEBUG_PRINT("image = " << image);
    
    QString fileName = "djvImageIoPluginTest";
    QString fileNamePartial = fileName + "Partial";
    
    const QStringList & extensions = plugin->extensions();
                
    if (extensions.count())
    {
        fileName += extensions[0];
        fileNamePartial += extensions[0];
    }
    
    DJV_DEBUG_PRINT("file name = " << fileName);
    
    try
    {
        QScopedPointer<djvImageLoad> load(plugin->createLoad());
        QScopedPointer<djvImageSave> save(plugin->createSave());
        
        if (! load.data() || ! save.data())
            return;
        
        save->open(fileName, image.info());
        save->write(image);
        save->close();
        
        djvImageIoInfo info;
        load->open(fileName, info);
        djvImage tmp;
        load->read(tmp);
        load->close();
        
        if (info.pixel != image.pixel() ||
            info.size  != image.size())
            return;
        
        djvPixelData p(djvPixelDataInfo(1, 1, image.pixel()));
        
        djvOpenGlImageOptions    options;
        djvOpenGlImageState      state;
        djvOpenGlOffscreenBuffer buffer(p.info());

        for (int y = 0; y < info.size.y; ++y)
        {
            for (int x = 0; x < info.size.x; ++x)
            {
                options.xform.position = -djvVector2f(x, y);

                djvOpenGlImage::copy(image, p, options, &state, &buffer);
                djvColor a(p.data(), p.pixel());
                
                djvOpenGlImage::copy(tmp, p, options, &state, &buffer);
                djvColor b(p.data(), p.pixel());
                
                DJV_DEBUG_PRINT(a << " == " << b << " (" << x << " " << y << ")");
                
                DJV_ASSERT(a == b);
            }
        }
    }
    catch (const djvError & error)
    {
        DJV_DEBUG_PRINT(djvErrorUtil::format(error));
    }

    try
    {
        djvFileInfo fileInfo(fileName);
        
        const quint64 size = fileInfo.size();
        
        djvMemoryBuffer<quint8> buf(size / 2);
        
        djvFileIo io;
        io.open(fileName, djvFileIo::READ);
        io.get(buf.data(), buf.size());
        io.close();
        
        io.open(fileNamePartial, djvFileIo::WRITE);
        io.set(buf.data(), buf.size());
        io.close();

        QScopedPointer<djvImageLoad> load(plugin->createLoad());
        
        if (! load.data())
            return;
        
        djvImageIoInfo info;
        load->open(fileNamePartial, info);
        djvImage tmp;
        load->read(tmp);
        load->close();
        
        DJV_ASSERT(0);
    }
    catch (const djvError & error)
    {
        DJV_DEBUG_PRINT(djvErrorUtil::format(error));
    }
}

