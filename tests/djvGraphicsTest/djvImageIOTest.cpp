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

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/ErrorUtil.h>
#include <djvCore/FileInfo.h>
#include <djvGraphics/Image.h>
#include <djvGraphics/GraphicsContext.h>
#include <djvGraphics/ImageIO.h>

using namespace djv::Core;
using namespace djv::Graphics;

namespace djv
{
    namespace GraphicsTest
    {
        void ImageIOTest::run(int & argc, char ** argv)
        {
            DJV_DEBUG("ImageIOTest::run");
            info();
            plugin();
            io();
        }

        void ImageIOTest::info()
        {
            DJV_DEBUG("ImageIOTest::info");
            {
                const Graphics::ImageIOInfo info;
                DJV_ASSERT(info.layerCount() == 1);
            }
            {
                const Graphics::PixelDataInfo tmp(1, 1, Graphics::Pixel::L_U8);
                const Graphics::ImageIOInfo info(tmp);
                DJV_ASSERT(info.layerCount() == 1);
                DJV_ASSERT(static_cast<Graphics::PixelDataInfo>(info) == tmp);
            }
            {
                const Graphics::PixelDataInfo tmp(1, 1, Graphics::Pixel::L_U8);
                Graphics::ImageIOInfo info;
                info.addLayer(tmp);
                DJV_ASSERT(info.layerCount() == 2);
                DJV_ASSERT(static_cast<Graphics::PixelDataInfo>(info) == Graphics::PixelDataInfo());
                DJV_ASSERT(static_cast<Graphics::PixelDataInfo>(info[1]) == tmp);
                info[0] = tmp;
                DJV_ASSERT(static_cast<Graphics::PixelDataInfo>(info[0]) == tmp);
            }
            {
                Graphics::ImageIOInfo info;
                info.setLayerCount(10);
                DJV_ASSERT(info.layerCount() == 10);
                info.clearLayers();
                DJV_ASSERT(info.layerCount() == 1);
            }
            {
                const Graphics::PixelDataInfo tmp(1, 1, Graphics::Pixel::L_U8);
                Graphics::ImageIOInfo a(tmp), b(tmp);
                a.addLayer(tmp);
                b.addLayer(tmp);
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != Graphics::ImageIOInfo());
                Graphics::ImageIOInfo c;
                c.addLayer(tmp);
                DJV_ASSERT(a != c);
            }
            {
                Graphics::ImageIOFrameInfo info;
                DJV_ASSERT(-1 == info.frame);
                DJV_ASSERT(0 == info.layer);
                DJV_ASSERT(Graphics::PixelDataInfo::PROXY_NONE == info.proxy);
            }
            {
                Graphics::ImageIOFrameInfo info(1, 2, Graphics::PixelDataInfo::PROXY_1_2);
                DJV_ASSERT(1 == info.frame);
                DJV_ASSERT(2 == info.layer);
                DJV_ASSERT(Graphics::PixelDataInfo::PROXY_1_2 == info.proxy);
            }
            {
                Graphics::ImageIOFrameInfo
                    a(1, 2, Graphics::PixelDataInfo::PROXY_1_2),
                    b(1, 2, Graphics::PixelDataInfo::PROXY_1_2);
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != Graphics::ImageIOFrameInfo());
            }
            {
                DJV_DEBUG_PRINT(Graphics::ImageIOInfo());
                DJV_DEBUG_PRINT(Graphics::ImageIOFrameInfo());
            }
        }

        void ImageIOTest::plugin()
        {
            DJV_DEBUG("ImageIOTest::plugin");
            Graphics::GraphicsContext context;
            Graphics::ImageIOFactory * factory = context.imageIOFactory();
            Q_FOREACH(QString plugin, QStringList() << "PPM")
            {
                if (Graphics::ImageIO * io = static_cast<Graphics::ImageIO *>(factory->plugin(plugin)))
                {
                    DJV_ASSERT(io->extensions().count());
                    DJV_ASSERT(io->isSequence());
                    DJV_ASSERT(io->options().count());
                    DJV_ASSERT(factory->option("", "") == QStringList());
                    QStringList tmp;
                    DJV_ASSERT(!factory->setOption("", "", tmp));
                    Graphics::ImageIOInfo info;
                    try
                    {
                        factory->load(FileInfo(), info);
                        DJV_ASSERT(0);
                    }
                    catch (...)
                    {
                    }
                    try
                    {
                        factory->save(FileInfo(), info);

                        DJV_ASSERT(0);
                    }
                    catch (...)
                    {
                    }
                }
            }
        }

        void ImageIOTest::io()
        {
            DJV_DEBUG("ImageIOTest::io");
            Graphics::GraphicsContext context;
            QScopedPointer<Graphics::ImageLoad> load;
            QScopedPointer<Graphics::ImageSave> save;
            try
            {
                const FileInfo fileInfo("ImageIOTest.ppm");
                const Graphics::PixelDataInfo pixelDataInfo(1, 1, Graphics::Pixel::L_U8);
                save.reset(context.imageIOFactory()->save(fileInfo, pixelDataInfo));
                DJV_ASSERT(save);
                save->write(Graphics::Image(pixelDataInfo));
                save->close();
                Graphics::ImageIOInfo info;
                load.reset(context.imageIOFactory()->load(fileInfo, info));
                DJV_ASSERT(load);
                Graphics::Image image;
                load->read(image);
                DJV_ASSERT(image.info().pixel == pixelDataInfo.pixel);
                load->close();
            }
            catch (const Error & error)
            {
                DJV_DEBUG_PRINT("error = " << ErrorUtil::format(error));
            }
        }

    } // namespace GraphicsTest
} // namespace djv
