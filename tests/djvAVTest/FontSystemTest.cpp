// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/FontSystemTest.h>

#include <djvAV/FontSystem.h>

#include <djvCore/Context.h>
#include <djvCore/StringFunc.h>
#include <djvCore/Timer.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        FontSystemTest::FontSystemTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITickTest("djv::AVTest::FontSystemTest", tempPath, context)
        {}
        
        void FontSystemTest::run()
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
                const Font::FontInfo fontInfo;
                DJV_ASSERT(1 == fontInfo.getFamily());
                DJV_ASSERT(1 == fontInfo.getFace());
                DJV_ASSERT(0 == fontInfo.getSize());
                DJV_ASSERT(dpiDefault == fontInfo.getDPI());
            }
            
            {
                const Font::FontInfo fontInfo(2, 3, 4, 5);
                DJV_ASSERT(2 == fontInfo.getFamily());
                DJV_ASSERT(3 == fontInfo.getFace());
                DJV_ASSERT(4 == fontInfo.getSize());
                DJV_ASSERT(5 == fontInfo.getDPI());
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
                DJV_ASSERT(Font::FontInfo() == glyphInfo.fontInfo);
            }
            
            {
                const Font::FontInfo fontInfo(2, 3, 4, 5);
                const Font::GlyphInfo glyphInfo(1, fontInfo);
                DJV_ASSERT(1 == glyphInfo.code);
                DJV_ASSERT(fontInfo == glyphInfo.fontInfo);
            }
        }
        
        void FontSystemTest::_glyph()
        {
            {
                auto glyph = Font::Glyph::create();
                DJV_ASSERT(Font::GlyphInfo() == glyph->glyphInfo);
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
                            ss << "Font: " << i.second;
                            _print(ss.str());
                        }
                    });

                Font::FontInfo fontInfo(1, 1, 14, dpiDefault);
                auto metricsFuture = system->getMetrics(fontInfo);
                const std::string text = String::getRandomText(50);
                auto measureFuture = system->measure(text, fontInfo);
                auto measureGlyphsFuture = system->measureGlyphs(text, fontInfo);
                auto textLinesFuture = system->textLines(text, 100, fontInfo);
                auto glyphsFuture = system->getGlyphs(text, fontInfo);
                system->cacheGlyphs(text, fontInfo);
                
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
                    _tickFor(Time::getTime(Time::TimerValue::Fast));
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
                    ss << "Ascender: " << metrics.ascender;
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Descender: " << metrics.descender;
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Line height: " << metrics.lineHeight;
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Measure: " << measure;
                    _print(ss.str());
                }
                for (const auto& i : textLines)
                {
                    std::stringstream ss;
                    ss << "Text line: " << i.text;
                    _print(ss.str());
                }
                
                system->setLCDRendering(true);
                system->setLCDRendering(true);
                const uint16_t elide = text.size() / 2;
                measureFuture = system->measure(text, fontInfo, elide);
                measureGlyphsFuture = system->measureGlyphs(text, fontInfo, elide);
                glyphsFuture = system->getGlyphs(text, fontInfo, elide);
                glyphs.clear();
                measure = glm::vec2(0.F, 0.F);
                measureGlyphs.clear();
                while (
                    measureFuture.valid() ||
                    measureGlyphsFuture.valid() ||
                    glyphsFuture.valid())
                {
                    _tickFor(Time::getTime(Time::TimerValue::Fast));
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
                    if (glyphsFuture.valid() &&
                        glyphsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        glyphs = glyphsFuture.get();
                    }
                }
                
                {
                    std::stringstream ss;
                    ss << "Glyph cache size: " << system->getGlyphCacheSize();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Glyph cache percentage: " << system->getGlyphCachePercentage();
                    _print(ss.str());
                }
            }
        }

        void FontSystemTest::_operators()
        {
            {
                const Font::FontInfo fontInfo(2, 3, 4, 5);
                DJV_ASSERT(fontInfo == fontInfo);
                DJV_ASSERT(Font::FontInfo() < fontInfo);
            }
            
            {
                const Font::FontInfo fontInfo(2, 3, 4, 5);
                const Font::GlyphInfo glyphInfo(1, fontInfo);
                DJV_ASSERT(glyphInfo == glyphInfo);
                DJV_ASSERT(Font::GlyphInfo() < glyphInfo);
            }
        }
        
    } // namespace AVTest
} // namespace djv

