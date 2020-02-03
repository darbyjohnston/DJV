//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvAVTest/Render2DTest.h>

#include <djvAV/Color.h>
#include <djvAV/FontSystem.h>
#include <djvAV/OCIO.h>
#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/Timer.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        Render2DTest::Render2DTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::Render2DTest", context)
        {}
        
        void Render2DTest::run(const std::vector<std::string>& args)
        {
            _operators();
            _system();
        }
        
        void Render2DTest::_system()
        {
            if (auto context = getContext().lock())
            {
                const Image::Info info(1280, 720, AV::Image::Type::RGBA_U8);
                auto offscreenBuffer = AV::OpenGL::OffscreenBuffer::create(info);
                offscreenBuffer->bind();
                auto render = context->getSystemT<AV::Render::Render2D>();
                render->beginFrame(info.size);
                
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
                std::vector<Render::ImageOptions> imageOptions;
                {
                    Render::ImageOptions options;
                    imageOptions.push_back(options);
                }
                {
                    Render::ImageOptions options;
                    options.channel = Render::ImageChannel::Red;
                    options.alphaBlend = AlphaBlend::Premultiplied;
                    options.mirror.x = true;
                    options.mirror.y = true;
                    options.colorSpace = OCIO::Convert("linear", "sRGB");
                    options.cache = Render::ImageCache::Dynamic;
                    imageOptions.push_back(options);
                }
                for (const auto& i : imageOptions)
                {
                    render->drawImage(image, glm::vec2(600.f, 400.f), i);
                }

                render->setFillColor(Image::Color(.6F, 1.F, .4F));
                render->drawFilledImage(image, glm::vec2(600.f, 200.f));
                
                Font::Info fontInfo(1, 1, 64, AV::dpiDefault);
                render->setCurrentFont(fontInfo);
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
                    ss << "texture atlas percentage: " << render->getTextureAtlasPercentage();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "dynamic texture count: " << render->getDynamicTextureCount();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "vbo size: " << render->getVBOSize();
                    _print(ss.str());
                }
            }
        }

        void Render2DTest::_operators()
        {
            {
                Render::ImageOptions options;
                options.channel = Render::ImageChannel::Red;
                DJV_ASSERT(options == options);
                DJV_ASSERT(options != Render::ImageOptions());
            }
        }
        
    } // namespace AVTest
} // namespace djv

