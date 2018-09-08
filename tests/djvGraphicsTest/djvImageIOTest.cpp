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

#include <djvImageIOTest.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvErrorUtil.h>
#include <djvFileInfo.h>
#include <djvImage.h>
#include <djvGraphicsContext.h>
#include <djvImageIO.h>

void djvImageIOTest::run(int & argc, char ** argv)
{
    DJV_DEBUG("djvImageIOTest::run");
    info();
    plugin();
    io();
}

void djvImageIOTest::info()
{
    DJV_DEBUG("djvImageIOTest::info");
    {
        const djvImageIOInfo info;
        DJV_ASSERT(info.layerCount() == 1);
    }
    {
        const djvPixelDataInfo tmp(1, 1, djvPixel::L_U8);
        const djvImageIOInfo info(tmp);
        DJV_ASSERT(info.layerCount() == 1);
        DJV_ASSERT(static_cast<djvPixelDataInfo>(info) == tmp);
    }
    {
        const djvPixelDataInfo tmp(1, 1, djvPixel::L_U8);
        djvImageIOInfo info;
        info.addLayer(tmp);
        DJV_ASSERT(info.layerCount() == 2);
        DJV_ASSERT(static_cast<djvPixelDataInfo>(info) == djvPixelDataInfo());
        DJV_ASSERT(static_cast<djvPixelDataInfo>(info[1]) == tmp);
        info[0] = tmp;
        DJV_ASSERT(static_cast<djvPixelDataInfo>(info[0]) == tmp);
    }
    {
        djvImageIOInfo info;
        info.setLayerCount(10);
        DJV_ASSERT(info.layerCount() == 10);
        info.clearLayers();
        DJV_ASSERT(info.layerCount() == 1);
    }
    {
        const djvPixelDataInfo tmp(1, 1, djvPixel::L_U8);
        djvImageIOInfo a(tmp), b(tmp);
        a.addLayer(tmp);
        b.addLayer(tmp);
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvImageIOInfo());
        djvImageIOInfo c;
        c.addLayer(tmp);
        DJV_ASSERT(a != c);
    }
    {
        djvImageIOFrameInfo info;
        DJV_ASSERT(-1 == info.frame);
        DJV_ASSERT(0 == info.layer);
        DJV_ASSERT(djvPixelDataInfo::PROXY_NONE == info.proxy);
    }
    {
        djvImageIOFrameInfo info(1, 2, djvPixelDataInfo::PROXY_1_2);
        DJV_ASSERT(1 == info.frame);
        DJV_ASSERT(2 == info.layer);
        DJV_ASSERT(djvPixelDataInfo::PROXY_1_2 == info.proxy);
    }
    {
        djvImageIOFrameInfo
            a(1, 2, djvPixelDataInfo::PROXY_1_2),
            b(1, 2, djvPixelDataInfo::PROXY_1_2);
        DJV_ASSERT(a == b);
        DJV_ASSERT(a != djvImageIOFrameInfo());
    }
    {
        DJV_DEBUG_PRINT(djvImageIOInfo());
        DJV_DEBUG_PRINT(djvImageIOFrameInfo());
    }
}

void djvImageIOTest::plugin()
{
    DJV_DEBUG("djvImageIOTest::plugin");
    djvGraphicsContext context;
    djvImageIOFactory * factory = context.imageIOFactory();
    Q_FOREACH(QString plugin, QStringList() << "PPM")
    {
        if (djvImageIO * io = static_cast<djvImageIO *>(factory->plugin(plugin)))
        {
            DJV_ASSERT(io->extensions().count());
            DJV_ASSERT(io->isSequence());
            DJV_ASSERT(io->options().count());
            DJV_ASSERT(factory->option("", "") == QStringList());
            QStringList tmp;
            DJV_ASSERT(! factory->setOption("", "", tmp));
            djvImageIOInfo info;
            try
            {
                factory->load(djvFileInfo(), info);
                DJV_ASSERT(0);
            }
            catch (...)
            {
            }
            try
            {
                factory->save(djvFileInfo(), info);

                DJV_ASSERT(0);
            }
            catch (...)
            {
            }
        }
    }
}

void djvImageIOTest::io()
{
    DJV_DEBUG("djvImageIOTest::io");
    djvGraphicsContext context;
    QScopedPointer<djvImageLoad> load;
    QScopedPointer<djvImageSave> save;
    try
    {
        const djvFileInfo fileInfo("djvImageIOTest.ppm");
        const djvPixelDataInfo pixelDataInfo(1, 1, djvPixel::L_U8);
        save.reset(context.imageIOFactory()->save(fileInfo, pixelDataInfo));
        DJV_ASSERT(save);
        save->write(djvImage(pixelDataInfo));
        save->close();
        djvImageIOInfo info;
        load.reset(context.imageIOFactory()->load(fileInfo, info));
        DJV_ASSERT(load);
        djvImage image;
        load->read(image);
        DJV_ASSERT(image.info().pixel == pixelDataInfo.pixel);
        load->close();
    }
    catch (const djvError & error)
    {
        DJV_DEBUG_PRINT("error = " << djvErrorUtil::format(error));
    }
}

