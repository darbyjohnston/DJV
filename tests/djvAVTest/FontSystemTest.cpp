//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAVTest/FontSystemTest.h>

#include <djvAV/FontSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Timer.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        FontSystemTest::FontSystemTest(const std::shared_ptr<Core::Context>& context) :
            ITickTest("djv::AVTest::FontSystemTest", context)
        {}
        
        void FontSystemTest::run(const std::vector<std::string>& args)
        {
            _info();
            _metrics();
            _textLine();
            _glyphInfo();
            _glyph();
            _system();
            _operators();
        }        

        void FontSystemTest::_info()
        {
            {
                const Font::Info info;
                DJV_ASSERT(1 == info.getFamily());
                DJV_ASSERT(1 == info.getFace());
                DJV_ASSERT(0 == info.getSize());
                DJV_ASSERT(dpiDefault == info.getDPI());
            }
            
            {
                const Font::Info info(2, 3, 4, 5);
                DJV_ASSERT(2 == info.getFamily());
                DJV_ASSERT(3 == info.getFace());
                DJV_ASSERT(4 == info.getSize());
                DJV_ASSERT(5 == info.getDPI());
            }
        }
        
        void FontSystemTest::_metrics()
        {
            const Font::Metrics metrics;
            DJV_ASSERT(0 == metrics.ascender);
            DJV_ASSERT(0 == metrics.descender);
            DJV_ASSERT(0 == metrics.lineHeight);
        }
        
        void FontSystemTest::_textLine()
        {
            {
                const Font::TextLine textLine;
                DJV_ASSERT(textLine.text.empty());
                DJV_ASSERT(0.F == textLine.size.x);
                DJV_ASSERT(0.F == textLine.size.y);
            }
            
            {
                const Font::TextLine textLine("line", glm::vec2(1.F, 2.F), {});
                DJV_ASSERT(!textLine.text.empty());
                DJV_ASSERT(1.F == textLine.size.x);
                DJV_ASSERT(2.F == textLine.size.y);
                DJV_ASSERT(0 == textLine.glyphs.size());
            }
        }
        
        void FontSystemTest::_glyphInfo()
        {
            {
                const Font::GlyphInfo glyphInfo;
                DJV_ASSERT(0 == glyphInfo.code);
                DJV_ASSERT(Font::Info() == glyphInfo.info);
            }
            
            {
                const Font::Info info(2, 3, 4, 5);
                const Font::GlyphInfo glyphInfo(1, info);
                DJV_ASSERT(1 == glyphInfo.code);
                DJV_ASSERT(info == glyphInfo.info);
            }
        }
        
        void FontSystemTest::_glyph()
        {
            {
                auto glyph = Font::Glyph::create();
                DJV_ASSERT(Font::GlyphInfo() == glyph->info);
                DJV_ASSERT(!glyph->imageData);
                DJV_ASSERT(glm::vec2(0.F, 0.F) == glyph->offset);
                DJV_ASSERT(0 == glyph->lsbDelta);
                DJV_ASSERT(0 == glyph->rsbDelta);
            }
        }
        
        void FontSystemTest::_system()
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<Font::System>();
                                
                auto fontNamesObserver = MapObserver<Font::FamilyID, std::string>::create(
                    system->observeFontNames(),
                    [this](const std::map<Font::FamilyID, std::string>& value)
                    {
                        for (const auto& i : value)
                        {
                            std::stringstream ss;
                            ss << "font: " << i.second;
                            _print(ss.str());
                        }
                    });

                Font::Info info(1, 1, 14, dpiDefault);
                auto metricsFuture = system->getMetrics(info);
                const std::string text = String::getRandomText(5);
                auto measureFuture = system->measure(text, info);
                auto measureGlyphsFuture = system->measureGlyphs(text, info);
                auto textLinesFuture = system->textLines(text, 100, info);
                auto glyphsFuture = system->getGlyphs(text, info);
                system->cacheGlyphs(text, info);
                
                Font::Metrics metrics;
                glm::vec2 measure = glm::vec2(0.F, 0.F);
                std::vector<BBox2f> measureGlyphs;
                std::vector<Font::TextLine> textLines;
                std::vector<std::shared_ptr<Font::Glyph> > glyphs;
                while (
                    metricsFuture.valid() ||
                    measureFuture.valid() ||
                    measureGlyphsFuture.valid() ||
                    textLinesFuture.valid() ||
                    glyphsFuture.valid())
                {
                    _tickFor(Time::getMilliseconds(Time::TimerValue::Fast));
                    if (metricsFuture.valid() &&
                        metricsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        metrics = metricsFuture.get();
                    }
                    if (measureFuture.valid() &&
                        measureFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        measure = measureFuture.get();
                    }
                    if (measureGlyphsFuture.valid() &&
                        measureGlyphsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        measureGlyphs = measureGlyphsFuture.get();
                    }
                    if (textLinesFuture.valid() &&
                        textLinesFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        textLines = textLinesFuture.get();
                    }
                    if (glyphsFuture.valid() &&
                        glyphsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        glyphs = glyphsFuture.get();
                    }
                }
                
                {
                    std::stringstream ss;
                    ss << "ascender: " << metrics.ascender;
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "descender: " << metrics.descender;
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "line height: " << metrics.lineHeight;
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "measure: " << measure;
                    _print(ss.str());
                }
                /*for (const auto& i : measureGlyphs)
                {
                    std::stringstream ss;
                    ss << "measure glyph: " << i;
                    _print(ss.str());
                }*/
                for (const auto& i : textLines)
                {
                    std::stringstream ss;
                    ss << "text line: " << i.text;
                    _print(ss.str());
                }
                /*for (const auto& i : glyphs)
                {
                    std::stringstream ss;
                    ss << "glyph: " << i->info.code;
                    _print(ss.str());
                }*/
                
                {
                    std::stringstream ss;
                    ss << "glyph cache size: " << system->getGlyphCacheSize();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "glyph cache percentage: " << system->getGlyphCachePercentage();
                    _print(ss.str());
                }
            }
        }

        void FontSystemTest::_operators()
        {
            {
                const Font::Info info(2, 3, 4, 5);
                DJV_ASSERT(info == info);
                DJV_ASSERT(Font::Info() < info);
            }
            
            {
                const Font::Info info(2, 3, 4, 5);
                const Font::GlyphInfo glyphInfo(1, info);
                DJV_ASSERT(glyphInfo == glyphInfo);
                DJV_ASSERT(Font::GlyphInfo() < glyphInfo);
            }
        }
        
    } // namespace AVTest
} // namespace djv

