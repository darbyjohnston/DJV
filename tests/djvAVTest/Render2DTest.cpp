// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/Render2DTest.h>

#include <djvAV/Color.h>
#include <djvAV/FontSystem.h>
#include <djvAV/OCIO.h>
#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Timer.h>

using namespace djv::Core;
using namespace djv::AV;

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
            _operators();
            _system();
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
                auto render = context->getSystemT<AV::Render2D::Render>();
                render->beginFrame(size);
                
                render->pushTransform(glm::mat3x3(1.F));
                render->pushClipRect(BBox2f(10.F, 10.F, 1260.F, 700.F));

                render->setFillColor(Image::Color(.6F, .4F, 1.F));
                render->setColorMult(.5F);
                render->setAlphaMult(.5F);
                render->drawRect(BBox2f(100.F, 100.F, 300.F, 200.F));
                
                render->setFillColor(Image::Color(.4F, .6F, 1.F));
                render->setColorMult(1.F);
                render->setAlphaMult(1.F);
                render->drawPill(BBox2f(400.F, 500.F, 200.F, 100.F));
                
                render->setFillColor(Image::Color(1.F, .6F, .4F));
                render->drawCircle(glm::vec2(300.F, 300.F), 100.F);
                
                auto image = Image::Image::create(Image::Info(64, 64, AV::Image::Type::RGBA_U8));
                image->zero();
                std::vector<Render2D::ImageOptions> imageOptions;
                {
                    Render2D::ImageOptions options;
                    imageOptions.push_back(options);
                }
                {
                    Render2D::ImageOptions options;
                    options.channelDisplay = Render2D::ImageChannelDisplay::Red;
                    options.alphaBlend = AlphaBlend::Premultiplied;
                    options.mirror.x = true;
                    options.mirror.y = true;
                    options.colorSpace = OCIO::Convert("linear", "sRGB");
                    options.cache = Render2D::ImageCache::Dynamic;
                    imageOptions.push_back(options);
                }
                for (const auto& i : imageOptions)
                {
                    render->drawImage(image, glm::vec2(600.f, 400.f), i);
                }

                render->setFillColor(Image::Color(.6F, 1.F, .4F));
                render->drawFilledImage(image, glm::vec2(600.f, 200.f));
                
                Font::FontInfo fontInfo(1, 1, 64, AV::dpiDefault);
                auto fontSystem = context->getSystemT<Font::System>();
                render->drawText(fontSystem->getGlyphs(String::getRandomText(5), fontInfo).get(), glm::vec2(200.F, 200.F));
                
                render->drawShadow(BBox2f(700.F, 500.F, 100.F, 100.F), Side::Right);
                render->drawShadow(BBox2f(800.F, 600.F, 100.F, 100.F), 32.F);
                
                render->popClipRect();
                render->popTransform();
                                
                render->endFrame();
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                
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

        void Render2DTest::_operators()
        {
            {
                Render2D::ImageOptions options;
                options.channelDisplay = Render2D::ImageChannelDisplay::Red;
                DJV_ASSERT(options == options);
                DJV_ASSERT(options != Render2D::ImageOptions());
            }
        }
        
    } // namespace AVTest
} // namespace djv

