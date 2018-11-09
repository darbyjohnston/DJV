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

#include <djvAVTest/ImageIOTest.h>

#include <djvAV/Image.h>
#include <djvAV/AVContext.h>
#include <djvAV/ImageIO.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/ErrorUtil.h>
#include <djvCore/FileInfo.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        void ImageIOTest::run(int & argc, char ** argv)
        {
            DJV_DEBUG("ImageIOTest::run");
            info();
            plugin(argc, argv);
            io(argc, argv);
        }

        void ImageIOTest::info()
        {
            DJV_DEBUG("ImageIOTest::info");
            {
                const AV::ImageIOInfo info;
                DJV_ASSERT(info.layerCount() == 1);
            }
            {
                const AV::PixelDataInfo tmp(1, 1, AV::Pixel::L_U8);
                const AV::ImageIOInfo info(tmp);
                DJV_ASSERT(info.layerCount() == 1);
                DJV_ASSERT(static_cast<AV::PixelDataInfo>(info) == tmp);
            }
            {
                const AV::PixelDataInfo tmp(1, 1, AV::Pixel::L_U8);
                AV::ImageIOInfo info;
                info.addLayer(tmp);
                DJV_ASSERT(info.layerCount() == 2);
                DJV_ASSERT(static_cast<AV::PixelDataInfo>(info) == AV::PixelDataInfo());
                DJV_ASSERT(static_cast<AV::PixelDataInfo>(info[1]) == tmp);
                info[0] = tmp;
                DJV_ASSERT(static_cast<AV::PixelDataInfo>(info[0]) == tmp);
            }
            {
                AV::ImageIOInfo info;
                info.setLayerCount(10);
                DJV_ASSERT(info.layerCount() == 10);
                info.clearLayers();
                DJV_ASSERT(info.layerCount() == 1);
            }
            {
                const AV::PixelDataInfo tmp(1, 1, AV::Pixel::L_U8);
                AV::ImageIOInfo a(tmp), b(tmp);
                a.addLayer(tmp);
                b.addLayer(tmp);
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != AV::ImageIOInfo());
                AV::ImageIOInfo c;
                c.addLayer(tmp);
                DJV_ASSERT(a != c);
            }
            {
                AV::ImageIOFrameInfo info;
                DJV_ASSERT(-1 == info.frame);
                DJV_ASSERT(0 == info.layer);
                DJV_ASSERT(AV::PixelDataInfo::PROXY_NONE == info.proxy);
            }
            {
                AV::ImageIOFrameInfo info(1, 2, AV::PixelDataInfo::PROXY_1_2);
                DJV_ASSERT(1 == info.frame);
                DJV_ASSERT(2 == info.layer);
                DJV_ASSERT(AV::PixelDataInfo::PROXY_1_2 == info.proxy);
            }
            {
                AV::ImageIOFrameInfo
                    a(1, 2, AV::PixelDataInfo::PROXY_1_2),
                    b(1, 2, AV::PixelDataInfo::PROXY_1_2);
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != AV::ImageIOFrameInfo());
            }
            {
                DJV_DEBUG_PRINT(AV::ImageIOInfo());
                DJV_DEBUG_PRINT(AV::ImageIOFrameInfo());
            }
        }

        void ImageIOTest::plugin(int & argc, char ** argv)
        {
            DJV_DEBUG("ImageIOTest::plugin");
            AV::AVContext context(argc, argv);
            AV::ImageIOFactory * factory = context.imageIOFactory();
            Q_FOREACH(QString plugin, QStringList() << "PPM")
            {
                if (AV::ImageIO * io = static_cast<AV::ImageIO *>(factory->plugin(plugin)))
                {
                    DJV_ASSERT(io->extensions().count());
                    DJV_ASSERT(io->isSequence());
                    DJV_ASSERT(io->options().count());
                    DJV_ASSERT(factory->option("", "") == QStringList());
                    QStringList tmp;
                    DJV_ASSERT(!factory->setOption("", "", tmp));
                    AV::ImageIOInfo info;
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

        void ImageIOTest::io(int & argc, char ** argv)
        {
            DJV_DEBUG("ImageIOTest::io");
            AV::AVContext context(argc, argv);
            QScopedPointer<AV::ImageLoad> load;
            QScopedPointer<AV::ImageSave> save;
            try
            {
                const FileInfo fileInfo("ImageIOTest.ppm");
                const AV::PixelDataInfo pixelDataInfo(1, 1, AV::Pixel::L_U8);
                auto save = context.imageIOFactory()->save(fileInfo, pixelDataInfo);
                DJV_ASSERT(save);
                save->write(AV::Image(pixelDataInfo));
                save->close();
                AV::ImageIOInfo info;
                auto load = context.imageIOFactory()->load(fileInfo, info);
                DJV_ASSERT(load);
                AV::Image image;
                load->read(image);
                DJV_ASSERT(image.info().pixel == pixelDataInfo.pixel);
            }
            catch (const Error & error)
            {
                DJV_DEBUG_PRINT("error = " << ErrorUtil::format(error));
            }
        }

    } // namespace AVTest
} // namespace djv
