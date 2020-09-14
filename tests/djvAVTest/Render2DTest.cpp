// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/Render2DTest.h>

#include <djvAV/Color.h>
#include <djvAV/FontSystem.h>
#include <djvAV/OCIO.h>
#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/OpenGLTexture.h>
#include <djvAV/Render2D.h>
#include <djvAV/Render2DPrivate.h>

#include <djvCore/Context.h>
#include <djvCore/StringFunc.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Timer.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::Render2D;

namespace djv
{
    namespace AVTest
    {
        Render2DTest::Render2DTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::Render2DTest", tempPath, context)
        {}
        
        void Render2DTest::run()
        {
            _enum();
            _data();
            _system();
            _serialize();
        }
        
        void Render2DTest::_enum()
        {
            for (const auto& i : getImageChannelDisplayEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Image channel display: " + _getText(ss.str()));
            }

            for (const auto& i : getImageCacheEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Image channel display: " + _getText(ss.str()));
            }

            for (const auto& i : getImageFilterEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Image channel display: " + _getText(ss.str()));
            }
        }

        void Render2DTest::_data()
        {
            {
                ImageOptions options;
                options.channelDisplay = ImageChannelDisplay::Red;
                DJV_ASSERT(options == options);
                DJV_ASSERT(options != ImageOptions());
            }
        }

        void Render2DTest::_system()
        {
            if (auto context = getContext().lock())
            {
                const Image::Size size(1280, 720);
                auto offscreenBuffer = AV::OpenGL::OffscreenBuffer::create(
                    size,
                    AV::Image::Type::RGBA_U8,
                    context->getSystemT<TextSystem>());
                offscreenBuffer->bind();
                auto render = context->getSystemT<Render2D::Render>();

                ImageFilterOptions imageFilterOptions(
                    ImageFilter::Nearest,
                    ImageFilter::Nearest);
                render->setImageFilterOptions(imageFilterOptions);
                DJV_ASSERT(render->getImageFilterOptions() == imageFilterOptions);
                imageFilterOptions.min = ImageFilter::Linear;
                imageFilterOptions.min = ImageFilter::Linear;
                render->setImageFilterOptions(imageFilterOptions);

                render->beginFrame(size);
                
                render->pushTransform(glm::mat3x3(1.F));
                render->pushClipRect(BBox2f(10.F, 10.F, 1260.F, 700.F));
                render->pushClipRect(BBox2f(20.F, 20.F, 1240.F, 680.F));
                
                render->setFillColor(Image::Color(.6F, .4F, 1.F));
                render->setLineWidth(2.F);
                const std::vector<glm::vec2> polyLine =
                {
                    glm::vec2(0.F, 100.F),
                    glm::vec2(200.F, 300.F),
                    glm::vec2(400.F, 500.F)
                };
                render->drawPolyline(polyLine);
                render->setLineWidth(1.F);
                render->drawPolyline(polyLine);

                render->setFillColor(Image::Color(255, 127, 63));
                render->setColorMult(.5F);
                render->setColorMult(.5F);
                render->setAlphaMult(.5F);
                render->setAlphaMult(.5F);
                render->drawRect(BBox2f(0.F, 100.F, 200.F, 300.F));
                render->drawRects({
                    BBox2f(0.F, 0.F, 100.F, 100.F),
                    BBox2f(100.F, 100.F, 100.F, 100.F),
                    BBox2f(200.F, 200.F, 100.F, 100.F) });
                
                render->setFillColor(Image::Color(.4F, .6F, 1.F));
                render->setColorMult(1.F);
                render->setAlphaMult(1.F);
                render->drawPill(BBox2f(00.F, 100.F, 200.F, 300.F));
                
                render->setFillColor(Image::Color(1.F, .6F, .4F));
                render->drawCircle(glm::vec2(300.F, 300.F), 100.F);
                
                Image::Info imageInfo(64, 64, AV::Image::Type::RGBA_U8);
                auto image = Image::Image::create(Image::Info(64, 64, AV::Image::Type::RGBA_U8));
                image->zero();
                render->drawImage(image, glm::vec2(0.f, 100.f));
                std::vector<ImageOptions> imageOptionsList;
                ImageOptions imageOptions;
                imageOptions.channelDisplay = ImageChannelDisplay::Red;
                imageOptions.alphaBlend = AlphaBlend::Premultiplied;
                imageOptions.mirror.x = true;
                imageOptions.mirror.y = true;
                imageOptions.colorSpace = OCIO::Convert("linear", "sRGB");
                imageOptions.cache = ImageCache::Dynamic;
                imageOptionsList.push_back(imageOptions);
                imageOptions.alphaBlend = AlphaBlend::Straight;
                imageOptions.colorEnabled = true;
                imageOptions.color.brightness = 2.F;
                imageOptions.color.contrast = 2.F;
                imageOptions.color.saturation = 2.F;
                imageOptions.color.invert = true;
                imageOptions.levelsEnabled = true;
                imageOptions.levels.inLow = .5F;
                imageOptions.exposureEnabled = true;
                imageOptions.exposure.exposure = 1.F;
                imageOptions.softClipEnabled = true;
                imageOptions.softClip = 1.F;
                imageOptionsList.push_back(imageOptions);
                imageOptions.alphaBlend = AlphaBlend::None;
                imageOptionsList.push_back(imageOptions);
                for (const auto& i : imageOptionsList)
                {
                    render->drawImage(image, glm::vec2(200.f, 300.f), i);
                }
                imageInfo.layout.mirror.x = true;
                imageInfo.layout.mirror.y = true;
                for (size_t i = 0; i < dynamicTextureCacheMax; ++i)
                {
                    image = Image::Image::create(imageInfo);
                    render->drawImage(image, glm::vec2(200.f, 300.f), imageOptions);
                }
                render->setFillColor(Image::Color(.6F, 1.F, .4F));
                render->drawFilledImage(image, glm::vec2(400.f, 500.f));
                
                Font::FontInfo fontInfo(1, 1, 64, AV::dpiDefault);
                auto fontSystem = context->getSystemT<Font::System>();
                render->drawText(fontSystem->getGlyphs(String::getRandomText(5), fontInfo).get(), glm::vec2(0.F, 100.F));
                render->setTextLCDRendering(true);
                render->drawText(fontSystem->getGlyphs(String::getRandomText(5), fontInfo).get(), glm::vec2(200.F, 300.F));
                
                render->drawShadow(BBox2f(0.F, 100.F, 200.F, 300.F), Side::Right);
                render->drawShadow(BBox2f(100.F, 200.F, 300.F, 400.F), 32.F);
                
                auto texture = OpenGL::Texture::create(Image::Info(64, 64, Image::Type::RGBA_U8));
                render->drawTexture(BBox2f(0.F, 100.F, 200.F, 300.F), texture->getID());
                
                render->popClipRect();
                render->popClipRect();
                render->popTransform();
                                
                render->endFrame();
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                
                {
                    std::stringstream ss;
                    ss << "Primitives count: " << render->getPrimitivesCount();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Texture atlas percentage: " << render->getTextureAtlasPercentage();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Dynamic texture count: " << render->getDynamicTextureCount();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "VBO size: " << render->getVBOSize();
                    _print(ss.str());
                }
            }
        }
        
        void Render2DTest::_serialize()
        {
            {
                const ImageFilter value = ImageFilter::Linear;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                ImageFilter value2 = ImageFilter::First;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            try
            {
                ImageFilter value = ImageFilter::First;
                auto json = rapidjson::Value(rapidjson::kObjectType);
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
            
            {
                const ImageFilterOptions value;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                ImageFilterOptions value2;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            try
            {
                ImageFilterOptions value;
                auto json = rapidjson::Value(rapidjson::kArrayType);
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace AVTest
} // namespace djv

