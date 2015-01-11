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

//! \file djvImageIoTest.cpp

#include <djvImageIoTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvFileInfo.h>
#include <djvImage.h>
#include <djvImageApplication.h>
#include <djvImageIo.h>

void djvImageIoTest::run(int & argc, char ** argv)
{
    DJV_DEBUG("djvImageIoTest::run");
    
    djvImageApplication app("djvImageIoTest", argc, argv);
    
    info();
    plugin();
    io();
}

void djvImageIoTest::info()
{
    DJV_DEBUG("djvImageIoTest::info");
    
    {
        const djvImageIoInfo info;
        
        DJV_ASSERT(info.layerCount() == 1);
    }
    
    {
        const djvPixelDataInfo tmp(1, 1, djvPixel::L_U8);
        
        const djvImageIoInfo info(tmp);
        
        DJV_ASSERT(info.layerCount() == 1);
        DJV_ASSERT(static_cast<djvPixelDataInfo>(info) == tmp);
    }
    
    {
        const djvPixelDataInfo tmp(1, 1, djvPixel::L_U8);
        
        djvImageIoInfo info;

        info.addLayer(tmp);
        
        DJV_ASSERT(info.layerCount() == 2);
        DJV_ASSERT(static_cast<djvPixelDataInfo>(info) == djvPixelDataInfo());
        DJV_ASSERT(static_cast<djvPixelDataInfo>(info[1]) == tmp);
        
        info[0] = tmp;

        DJV_ASSERT(static_cast<djvPixelDataInfo>(info[0]) == tmp);
    }
    
    {
        djvImageIoInfo info;

        info.setLayerCount(10);
        
        DJV_ASSERT(info.layerCount() == 10);

        info.clearLayers();
        
        DJV_ASSERT(info.layerCount() == 1);
    }
    
    {
        const djvPixelDataInfo tmp(1, 1, djvPixel::L_U8);
        
        djvImageIoInfo a(tmp), b(tmp);
        
        a.addLayer(tmp);
        b.addLayer(tmp);
        
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvImageIoInfo());
        
        djvImageIoInfo c;
        c.addLayer(tmp);
        
        DJV_ASSERT(a != c);
    }
    
    {
        djvImageIoFrameInfo info;
        
        DJV_ASSERT(-1 == info.frame);
        DJV_ASSERT(0 == info.layer);
        DJV_ASSERT(djvPixelDataInfo::PROXY_NONE == info.proxy);
    }
    
    {
        djvImageIoFrameInfo info(1, 2, djvPixelDataInfo::PROXY_1_2);
        
        DJV_ASSERT(1 == info.frame);
        DJV_ASSERT(2 == info.layer);
        DJV_ASSERT(djvPixelDataInfo::PROXY_1_2 == info.proxy);
    }
    
    {
        djvImageIoFrameInfo
            a(1, 2, djvPixelDataInfo::PROXY_1_2),
            b(1, 2, djvPixelDataInfo::PROXY_1_2);
    
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvImageIoFrameInfo());
    }
    
    {
        DJV_DEBUG_PRINT(djvImageIoInfo());
        
        DJV_DEBUG_PRINT(djvImageIoFrameInfo());
    }
}
    
void djvImageIoTest::plugin()
{
    DJV_DEBUG("djvImageIoTest::plugin");
    
    djvImageIoFactory * factory = djvImageIoFactory::global();
    
    Q_FOREACH(QString plugin, QStringList() << "PPM" << "PIC")
    {
        djvImageIo * io = static_cast<djvImageIo *>(factory->plugin("PPM"));
        
        DJV_ASSERT(io);
        DJV_ASSERT(io->extensions().count());
        DJV_ASSERT(io->isSequence());
        DJV_ASSERT(io->options().count());
        
        DJV_ASSERT(factory->option("", "") == QStringList());

        QStringList tmp;
        
        DJV_ASSERT(! factory->setOption("", "", tmp));

        djvImageIoInfo info;
        
        try
        {
            factory->load(djvFileInfo(), info);
        
            DJV_ASSERT(0);
        }
        catch (...)
        {}
        
        try
        {
            factory->save(djvFileInfo(), info);
        
            DJV_ASSERT(0);
        }
        catch (...)
        {}
    }
}

void djvImageIoTest::io()
{
    DJV_DEBUG("djvImageIoTest::io");
    
    int argc = 0;
    
    const djvFileInfo fileInfo("djvImageIoTest.ppm");
    
    const djvPixelDataInfo pixelDataInfo(1, 1, djvPixel::L_U8);
    
    djvImageSave * save = djvImageIoFactory::global()->save(
        fileInfo,
        pixelDataInfo);
    
    DJV_ASSERT(save);
    
    save->write(djvImage(pixelDataInfo));
    
    save->close();
    
    delete save;
    
    djvImageIoInfo info;
    
    DJV_DEBUG_PRINT("!");
    
    djvImageLoad * load = djvImageIoFactory::global()->load(fileInfo, info);

    DJV_ASSERT(load);
    
    DJV_DEBUG_PRINT("!");
    
    djvImage image;
    
    load->read(image);
    
    DJV_ASSERT(image.info().pixel == pixelDataInfo.pixel);
    
    load->close();
    
    delete load;
}

