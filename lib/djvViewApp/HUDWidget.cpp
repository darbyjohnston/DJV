// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/HUDWidget.h>

#include <djvRender2D/FontSystem.h>
#include <djvRender2D/Render.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            struct HUDLabel
            {
                HUDLabel()
                {}
                
                HUDLabel(
                    const std::string& text,
                    UI::Corner         corner,
                    const std::string& sortKey) :
                    text(text),
                    corner(corner),
                    sortKey(sortKey)
                {}
                
                std::string text;
                UI::Corner  corner  = UI::Corner::First;
                std::string sortKey;
            };

        } // namespace

        struct HUDWidget::Private
        {
            std::shared_ptr<Render2D::Font::FontSystem> fontSystem;
            Math::BBox2f frame;
            HUDData data;
            HUDOptions options;
            std::map<std::string, HUDLabel> labels;
            Render2D::Font::Metrics fontMetrics;
            std::future<Render2D::Font::Metrics> fontMetricsFuture;
            std::map<std::string, glm::vec2> textSizes;
            std::map<std::string, std::future<glm::vec2> > textSizeFutures;
            std::map<std::string, std::vector<std::shared_ptr<Render2D::Font::Glyph> > > glyphs;
            std::map< std::string, std::future<std::vector<std::shared_ptr<Render2D::Font::Glyph> > > > glyphsFutures;
        };

        void HUDWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::HUDWidget");

            p.fontSystem = context->getSystemT<Render2D::Font::FontSystem>();
        }

        HUDWidget::HUDWidget() :
            _p(new Private)
        {}

        HUDWidget::~HUDWidget()
        {}

        std::shared_ptr<HUDWidget> HUDWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<HUDWidget>(new HUDWidget);
            out->_init(context);
            return out;
        }

        void HUDWidget::setHUDFrame(const Math::BBox2f& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.frame)
                return;
            p.frame = value;
            _redraw();
        }

        void HUDWidget::setHUDData(const HUDData& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.data)
                return;
            p.data = value;
            _textUpdate();
        }

        void HUDWidget::setHUDOptions(const HUDOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.options)
                return;
            p.options = value;
            _redraw();
        }

        void HUDWidget::_paintEvent(System::Event::Paint&)
        {
            DJV_PRIVATE_PTR();
            if (p.options.enabled && p.glyphs.size())
            {
                const auto& style = _getStyle();
                const float m = style->getMetric(UI::MetricsRole::MarginSmall);
                const Math::BBox2f g = p.frame.margin(-m);

                struct Label
                {
                    std::string id;
                    Math::BBox2f geometry;
                };
                std::map<UI::Corner, std::map<std::string, Label> > labels;

                // Group the labels by which corner they are in.
                for (const auto& i : p.labels)
                {
                    labels[i.second.corner][i.second.sortKey].id = i.first;
                }

                // Calculate the label geometry.
                const float h = p.fontMetrics.lineHeight + m * 2.F;
                for (auto& i : labels)
                {
                    glm::vec2 pos;
                    switch (i.first)
                    {
                    case UI::Corner::UpperLeft:
                        pos = g.min;
                        for (auto& j : i.second)
                        {
                            const auto k = p.textSizes.find(j.second.id);
                            if (k != p.textSizes.end())
                            {
                                j.second.geometry = Math::BBox2f(pos.x, pos.y, k->second.x + m * 2.F, h);
                                pos.y += h;
                            }
                        }
                        break;
                    case UI::Corner::UpperRight:
                        pos.x = g.max.x;
                        pos.y = g.min.y;
                        for (auto& j : i.second)
                        {
                            const auto k = p.textSizes.find(j.second.id);
                            if (k != p.textSizes.end())
                            {
                                const float w = k->second.x + m * 2.F;
                                j.second.geometry = Math::BBox2f(pos.x - w, pos.y, w, h);
                                pos.y += h;
                            }
                        }
                        break;
                    case UI::Corner::LowerRight:
                        pos.x = g.max.x;
                        pos.y = g.max.y - h;
                        for (auto& j : i.second)
                        {
                            const auto k = p.textSizes.find(j.second.id);
                            if (k != p.textSizes.end())
                            {
                                const float w = k->second.x + m * 2.F;
                                j.second.geometry = Math::BBox2f(pos.x - w, pos.y, w, h);
                                pos.y -= h;
                            }
                        }
                        break;
                    case UI::Corner::LowerLeft:
                        pos.x = g.min.x;
                        pos.y = g.max.y - h;
                        for (auto& j : i.second)
                        {
                            const auto k = p.textSizes.find(j.second.id);
                            if (k != p.textSizes.end())
                            {
                                j.second.geometry = Math::BBox2f(pos.x, pos.y, k->second.x + m * 2.F, h);
                                pos.y -= h;
                            }
                        }
                        break;
                    default: break;
                    }
                }

                // Draw the label backgrounds.
                const auto& render = _getRender();
                switch (p.options.background)
                {
                case HUDBackground::Overlay:
                {
                    std::vector<Math::BBox2f> rects;
                    for (const auto& i : labels)
                    {
                        for (const auto& j : i.second)
                        {
                            rects.push_back(j.second.geometry);
                        }
                    }
                    render->setFillColor(style->getColor(UI::ColorRole::Overlay));
                    render->drawRects(rects);
                    break;
                }
                default: break;
                }

                // Draw the labels.
                render->setFillColor(p.options.color);
                for (const auto& i : labels)
                {
                    for (const auto& j : i.second)
                    {
                        const auto k = p.glyphs.find(j.second.id);
                        if (k != p.glyphs.end())
                        {
                            const auto& g2 = j.second.geometry;
                            //! \bug Why the extra subtract by one here?
                            render->drawText(k->second, glm::vec2(floorf(g2.min.x + m), floorf(g2.min.y + m + p.fontMetrics.ascender - 1.F)));
                        }
                    }
                }
            }
        }

        void HUDWidget::_initEvent(System::Event::Init & event)
        {
            if (event.getData().resize ||
                event.getData().font ||
                event.getData().text)
            {
                _textUpdate();
            }
        }

        void HUDWidget::_updateEvent(System::Event::Update& event)
        {
            DJV_PRIVATE_PTR();
            if (p.fontMetricsFuture.valid() &&
                p.fontMetricsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.fontMetrics = p.fontMetricsFuture.get();
                    _redraw();
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), System::LogLevel::Error);
                }
            }
            auto i = p.textSizeFutures.begin();
            while (i != p.textSizeFutures.end())
            {
                if (i->second.valid() &&
                    i->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        p.textSizes[i->first] = i->second.get();
                        _redraw();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), System::LogLevel::Error);
                    }
                    i = p.textSizeFutures.erase(i);
                }
                else
                {
                    ++i;
                }
            }
            auto j = p.glyphsFutures.begin();
            while (j != p.glyphsFutures.end())
            {
                if (j->second.valid() &&
                    j->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        p.glyphs[j->first] = j->second.get();
                        _redraw();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), System::LogLevel::Error);
                    }
                    j = p.glyphsFutures.erase(j);
                }
                else
                {
                    ++j;
                }
            }
        }

        void HUDWidget::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const auto fontInfo = style->getFontInfo(Render2D::Font::familyMono, std::string(), UI::MetricsRole::FontMedium);
            p.fontMetricsFuture = p.fontSystem->getMetrics(fontInfo);
            p.textSizeFutures.clear();
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("hud_file_name")) << ": ";
                ss << p.data.fileName;
                p.labels["FileName"] = HUDLabel(ss.str(), UI::Corner::UpperLeft, "A");
            }
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("hud_layer")) << ": ";
                ss << p.data.layer;
                p.labels["Layer"] = HUDLabel(ss.str(), UI::Corner::UpperLeft, "B");
            }
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("hud_size")) << ": ";
                ss << p.data.size.w << "x" << p.data.size.h << ":";
                ss.precision(2);
                ss << p.data.size.getAspectRatio();
                p.labels["Size"] = HUDLabel(ss.str(), UI::Corner::UpperRight, "A");
            }
            {
                std::stringstream ss;
                ss << _getText(DJV_TEXT("hud_type")) << ": ";
                std::stringstream ss2;
                ss2 << p.data.type;
                ss << _getText(ss2.str());
                p.labels["Type"] = HUDLabel(ss.str(), UI::Corner::UpperRight, "B");
            }
            if (p.data.isSequence)
            {
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("hud_frame")) << ": ";
                    ss << p.data.currentFrame;
                    p.labels["CurrentFrame"] = HUDLabel(ss.str(), UI::Corner::LowerLeft, "B");
                }
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("hud_speed")) << ": ";
                    ss.precision(2);
                    ss << std::fixed << p.data.speed.toFloat() << "/" << p.data.realSpeed;
                    p.labels["Speed"] = HUDLabel(ss.str(), UI::Corner::LowerLeft, "A");
                }
            }
            else
            {
                auto i = p.labels.find("CurrentFrame");
                if (i != p.labels.end())
                {
                    p.labels.erase(i);
                }
                i = p.labels.find("Speed");
                if (i != p.labels.end())
                {
                    p.labels.erase(i);
                }
            }
            for (const auto& i : p.labels)
            {
                p.textSizeFutures[i.first] = p.fontSystem->measure(i.second.text, fontInfo);
                p.glyphsFutures[i.first] = p.fontSystem->getGlyphs(i.second.text, fontInfo);
            }
        }

    } // namespace ViewApp

} // namespace djv

