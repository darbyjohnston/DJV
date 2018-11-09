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

#include <djvGraphicsTest/ImageIOFormatsTest.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/GraphicsContext.h>
#include <djvGraphics/ImageIO.h>
#include <djvGraphics/OpenGLImage.h>
#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/Debug.h>
#include <djvCore/Error.h>
#include <djvCore/ErrorUtil.h>
#include <djvCore/FileInfo.h>
#include <djvCore/FileIO.h>
#include <djvCore/Memory.h>

#include <QPair>

#include <algorithm>

using namespace djv::Core;
using namespace djv::Graphics;

namespace djv
{
    namespace GraphicsTest
    {
        void ImageIOFormatsTest::run(int & argc, char ** argv)
        {
            DJV_DEBUG("ImageIOFormatsTest::run");

            Graphics::GraphicsContext context(argc, argv);
            initData();
            initImages();
            initPlugins(&context);

            //! \todo Fix these image I/O confidence tests.    
            typedef QPair<QString, Graphics::Pixel::PIXEL> Disable;
            QVector<Disable> disabled;
            disabled += Disable("IFF", Graphics::Pixel::L_U16);
            disabled += Disable("IFF", Graphics::Pixel::L_F16);
            disabled += Disable("IFF", Graphics::Pixel::L_F32);
            disabled += Disable("IFF", Graphics::Pixel::LA_U16);
            disabled += Disable("IFF", Graphics::Pixel::LA_F16);
            disabled += Disable("IFF", Graphics::Pixel::LA_F32);
            disabled += Disable("IFF", Graphics::Pixel::RGB_U10);
            disabled += Disable("IFF", Graphics::Pixel::RGB_U16);
            disabled += Disable("IFF", Graphics::Pixel::RGB_F16);
            disabled += Disable("IFF", Graphics::Pixel::RGB_F32);
            disabled += Disable("IFF", Graphics::Pixel::RGBA_U16);
            disabled += Disable("IFF", Graphics::Pixel::RGBA_F16);
            disabled += Disable("IFF", Graphics::Pixel::RGBA_F32);

            for (int j = 0; j < _plugins.count(); ++j)
            {
                Graphics::ImageIO * plugin = static_cast<Graphics::ImageIO *>(_plugins[j]);
                for (int i = 0; i < _images.count(); ++i)
                {
                    const Graphics::Image & image = _images[i];
                    bool test = true;
                    for (int k = 0; k < disabled.count(); ++k)
                    {
                        if (plugin->pluginName() == disabled[k].first &&
                            image.pixel() == disabled[k].second)
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

        void ImageIOFormatsTest::initPlugins(const QPointer<Graphics::GraphicsContext> & context)
        {
            DJV_DEBUG("ImageIOFormatsTest::initPlugins");

            QStringList option;
            option << "None";
            context->imageIOFactory()->setOption("Cineon", "Input Color Profile", option);
            option << "None";
            context->imageIOFactory()->setOption("Cineon", "Output Color Profile", option);
            option << "None";
            context->imageIOFactory()->setOption("DPX", "Input Color Profile", option);
            option << "None";
            context->imageIOFactory()->setOption("DPX", "Output Color Profile", option);
            option << "None";
            context->imageIOFactory()->setOption("OpenEXR", "Input Color Profile", option);

            //! \todo Fix FFmpeg image I/O testing.
            QStringList disable = QStringList() <<
                "FFmpeg";

            //! \todo JPEG is lossy which will cause the pixel comparison tests to fail.
            disable += "JPEG";

            const QList<Plugin *> & pluginsTmp = context->imageIOFactory()->plugins();
            for (int i = 0; i < pluginsTmp.count(); ++i)
            {
                if (!disable.contains(pluginsTmp[i]->pluginName()))
                {
                    _plugins.append(pluginsTmp[i]);
                }
            }
            DJV_DEBUG_PRINT("plugins = " << _plugins.count());
        }

        void ImageIOFormatsTest::initData()
        {
            DJV_DEBUG("ImageIOFormatsTest::initData");
            if (1)
            {
                //! \todo The one pixel wide data seems to be triggering errors
                //! in the floating point tests.
                _sizes += QVector<glm::ivec2>() <<
                    glm::ivec2(11, 1) <<
                    //glm::ivec2( 1,  7) <<
                    glm::ivec2(11, 7) <<
                    glm::ivec2(7, 11);
                for (int i = 0; i < Graphics::Pixel::PIXEL_COUNT; ++i)
                {
                    const Graphics::Pixel::PIXEL pixel = static_cast<Graphics::Pixel::PIXEL>(i);
                    _pixels += pixel;
                }
            }
            else
            {
                _sizes += glm::ivec2(10, 1);
                //_pixels += Graphics::Pixel::RGB_U10;
                _pixels += Graphics::Pixel::RGB_F16;
            }
            //DJV_DEBUG_PRINT("sizes = " << _sizes);
            //DJV_DEBUG_PRINT("pixels = " << _pixels);
        }

        void ImageIOFormatsTest::initImages()
        {
            DJV_DEBUG("ImageIOFormatsTest::initImages");
            for (int i = 0; i < _sizes.count(); ++i)
            {
                for (int j = 0; j < _pixels.count(); ++j)
                {
                    Graphics::Image gradient(Graphics::PixelDataInfo(_sizes[i], Graphics::Pixel::L_F32));
                    Graphics::PixelDataUtil::gradient(gradient);
                    Graphics::Image image(Graphics::PixelDataInfo(gradient.size(), _pixels[j]));
                    Graphics::OpenGLImage().copy(gradient, image);
                    DJV_DEBUG_PRINT("image = " << image);

                    Graphics::PixelData p(Graphics::PixelDataInfo(1, 1, image.pixel()));
                    Graphics::OpenGLImageOptions options;
                    options.xform.position = glm::vec2(0, 0);
                    Graphics::OpenGLImage().copy(image, p, options);
                    Graphics::Color c(p.data(), p.pixel());
                    DJV_DEBUG_PRINT("c0 = " << c);

                    options.xform.position = -glm::vec2(image.size().x - 1, image.size().y - 1);
                    Graphics::OpenGLImage().copy(image, p, options);
                    c = Graphics::Color(p.data(), p.pixel());
                    DJV_DEBUG_PRINT("c1 = " << c);

                    _images += image;
                }
            }
        }

        void ImageIOFormatsTest::runTest(Graphics::ImageIO * plugin, const Graphics::Image & image)
        {
            DJV_DEBUG("ImageIOFormatsTest::runTest");
            DJV_DEBUG_PRINT("plugin = " << plugin->pluginName());
            DJV_DEBUG_PRINT("image = " << image);

            QString fileName = QString::fromUtf8("\xd1\x82\xd0\xb5\xd1\x81\xd1\x82\xd0\xbe\xd0\xb2\xd0\xbe\xd0\xb5\x20\xd0\xb7\xd0\xb0\xd0\xb4\xd0\xb0\xd0\xbd\xd0\xb8\xd0\xb5");
            QString fileNamePartial = fileName + ' ' + QString::fromUtf8("\xd1\x87\xd0\xb0\xd1\x81\xd1\x82\xd0\xb8\xd1\x87\xd0\xbd\xd1\x8b\xd0\xb9");
            const QStringList & extensions = plugin->extensions();
            if (extensions.count())
            {
                fileName += extensions[0];
                fileNamePartial += extensions[0];
            }
            DJV_DEBUG_PRINT("file name = " << fileName);

            try
            {
                auto load = plugin->createLoad(fileName);
                auto save = plugin->createSave(fileName, image.info());
                if (!load || !save)
                    return;

                save->write(image);
                save->close();

                Graphics::Image tmp;
                load->read(tmp);
                auto info = load->imageIOInfo();
                if (info.pixel != image.pixel() ||
                    info.size != image.size())
                    return;

                Graphics::PixelData p(Graphics::PixelDataInfo(1, 1, image.pixel()));
                Graphics::OpenGLImageOptions options;
                for (int y = 0; y < info.size.y; ++y)
                {
                    for (int x = 0; x < info.size.x; ++x)
                    {
                        options.xform.position = -glm::vec2(x, y);
                        Graphics::OpenGLImage().copy(image, p, options);
                        Graphics::Color a(p.data(), p.pixel());
                        Graphics::OpenGLImage().copy(tmp, p, options);
                        Graphics::Color b(p.data(), p.pixel());
                        DJV_DEBUG_PRINT(a << " == " << b << " (" << x << " " << y << ")");
                        DJV_ASSERT(a == b);
                    }
                }
            }
            catch (const Error & error)
            {
                DJV_DEBUG_PRINT(ErrorUtil::format(error));
            }

            try
            {
                FileInfo fileInfo(fileName);
                const quint64 size = fileInfo.size();
                std::vector<quint8> buf(size / 2);

                FileIO io;
                io.open(fileName, FileIO::READ);
                io.get(buf.data(), buf.size());
                io.close();

                io.open(fileNamePartial, FileIO::WRITE);
                io.set(buf.data(), buf.size());
                io.close();

                auto load = plugin->createLoad(fileNamePartial);
                if (!load)
                    return;

                Graphics::Image tmp;
                load->read(tmp);
                DJV_ASSERT(0);
            }
            catch (const Error & error)
            {
                DJV_DEBUG_PRINT(ErrorUtil::format(error));
            }
        }

    } // namespace GraphicsTest
} // namespace djv
