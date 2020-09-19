// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Label.h>

#include <djvUI/Style.h>

#include <djvRender2D/FontSystem.h>
#include <djvRender2D/Render.h>

#include <djvSystem/Context.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Label::Private
        {
            std::shared_ptr<Render2D::Font::FontSystem> fontSystem;

            std::string text;
            TextHAlign textHAlign = TextHAlign::Center;
            TextVAlign textVAlign = TextVAlign::Center;
            ColorRole textColorRole = ColorRole::Foreground;

            std::string fontFamily;
            std::string fontFace;
            MetricsRole fontSizeRole = MetricsRole::FontMedium;
            Render2D::Font::FontInfo fontInfo;
            Render2D::Font::Metrics fontMetrics;
            std::future<Render2D::Font::Metrics> fontMetricsFuture;

            int elide = 0;

            glm::vec2 textSize = glm::vec2(0.F, 0.F);
            std::future<glm::vec2> textSizeFuture;

            std::string sizeString;
            glm::vec2 sizeStringSize = glm::vec2(0.F, 0.F);
            std::future<glm::vec2> sizeStringFuture;

            std::vector<std::shared_ptr<Render2D::Font::Glyph> > glyphs;
            std::future<std::vector<std::shared_ptr<Render2D::Font::Glyph> > > glyphsFuture;

            bool labelMinimumSizeInit = true;
            std::weak_ptr<LabelSizeGroup> sizeGroup;

            Math::BBox2f paintGeometry = Math::BBox2f(0.F, 0.F, 0.F, 0.F);
            glm::vec2 paintCenter = glm::vec2(0.F, 0.F);
        };

        void Label::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            setClassName("djv::UI::Label");
            setVAlign(VAlign::Center);
            _p->fontSystem = context->getSystemT<Render2D::Font::FontSystem>();
        }
        
        Label::Label() :
            _p(new Private)
        {}

        Label::~Label()
        {}

        std::shared_ptr<Label> Label::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<Label>(new Label);
            out->_init(context);
            return out;
        }

        const std::string& Label::getText() const
        {
            return _p->text;
        }

        void Label::setText(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.text)
                return;
            p.text = value;
            _textUpdate();
        }

        TextHAlign Label::getTextHAlign() const
        {
            return _p->textHAlign;
        }
        
        TextVAlign Label::getTextVAlign() const
        {
            return _p->textVAlign;
        }
        
        void Label::setTextHAlign(TextHAlign value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textHAlign)
                return;
            p.textHAlign = value;
            _resize();
        }
        
        void Label::setTextVAlign(TextVAlign value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textVAlign)
                return;
            p.textVAlign = value;
            _resize();
        }
            
        ColorRole Label::getTextColorRole() const
        {
            return _p->textColorRole;
        }

        void Label::setTextColorRole(ColorRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.textColorRole)
                return;
            p.textColorRole = value;
            _redraw();
        }

        const std::string& Label::getFontFamily() const
        {
            return _p->fontFamily;
        }

        const std::string& Label::getFontFace() const
        {
            return _p->fontFace;
        }

        MetricsRole Label::getFontSizeRole() const
        {
            return _p->fontSizeRole;
        }

        void Label::setFontFamily(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontFamily)
                return;
            p.fontFamily = value;
            _fontUpdate();
        }

        void Label::setFontFace(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontFace)
                return;
            p.fontFace = value;
            _fontUpdate();
        }

        void Label::setFontSizeRole(MetricsRole value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.fontSizeRole)
                return;
            p.fontSizeRole = value;
            _fontUpdate();
        }

        int Label::getElide() const
        {
            return _p->elide;
        }

        void Label::setElide(int value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.elide)
                return;
            p.elide = value;
            _textUpdate();
        }

        const std::string& Label::getSizeString() const
        {
            return _p->sizeString;
        }

        void Label::setSizeString(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.sizeString)
                return;
            p.sizeString = value;
            _sizeStringUpdate();
        }

        void Label::setSizeGroup(const std::weak_ptr<LabelSizeGroup>& value)
        {
            auto label = std::dynamic_pointer_cast<Label>(shared_from_this());
            if (auto sizeGroup = _p->sizeGroup.lock())
            {
                sizeGroup->removeLabel(label);
            }
            _p->sizeGroup = value;
            if (auto sizeGroup = _p->sizeGroup.lock())
            {
                sizeGroup->addLabel(label);
            }
        }

        void Label::_initLayoutEvent(System::Event::InitLayout&)
        {
            DJV_PRIVATE_PTR();
            if (p.labelMinimumSizeInit)
            {
                p.labelMinimumSizeInit = false;
                const glm::vec2 size(glm::max(p.textSize.x, p.sizeStringSize.x), p.fontMetrics.lineHeight);
                const auto& style = _getStyle();
                _labelMinimumSize = size + getMargin().getSize(style);
            }
        }

        void Label::_preLayoutEvent(System::Event::PreLayout&)
        {
            DJV_PRIVATE_PTR();
            glm::vec2 minimumSize = _labelMinimumSize;
            if (!p.sizeGroup.expired())
            {
                if (auto sizeGroup = p.sizeGroup.lock())
                {
                    minimumSize = sizeGroup->getMinimumSize();
                }
            }
            _setMinimumSize(minimumSize);
        }

        void Label::_layoutEvent(System::Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            p.paintGeometry = getMargin().bbox(getGeometry(), style);
            bool center = false;
            switch (p.textHAlign)
            {
            case TextHAlign::Center: center = true; break;
            default: break;
            }
            switch (p.textVAlign)
            {
            case TextVAlign::Center:
            case TextVAlign::Baseline: center = true; break;
            default: break;
            }
            if (center)
            {
                p.paintCenter = p.paintGeometry.getCenter();
            }
        }

        void Label::_paintEvent(System::Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            if (p.glyphs.size())
            {
                const auto& style = _getStyle();

                const auto& render = _getRender();
                //render->setFillColor(Image::Color(1.F, 0.F, 0.F, .5F));
                //render->drawRect(p.paintGeometry);

                glm::vec2 pos = p.paintGeometry.min;
                switch (p.textHAlign)
                {
                case TextHAlign::Center:
                    pos.x = p.paintCenter.x - p.textSize.x / 2.F;
                    break;
                case TextHAlign::Right:
                    pos.x = p.paintGeometry.max.x - p.textSize.x;
                    break;
                default: break;
                }
                switch (p.textVAlign)
                {
                case TextVAlign::Center:
                    pos.y = p.paintCenter.y - p.textSize.y / 2.F;
                    break;
                case TextVAlign::Top:
                    pos.y = p.paintGeometry.min.y;
                    break;
                case TextVAlign::Bottom:
                    pos.y = p.paintGeometry.max.y - p.textSize.y;
                    break;
                case TextVAlign::Baseline:
                    pos.y = p.paintCenter.y - p.fontMetrics.ascender / 2.F;
                    break;
                default: break;
                }

                //render->setFillColor(Image::Color(1.F, 0.F, 0.F));
                //render->drawRect(BBox2f(pos.x, pos.y, p.textSize.x, p.textSize.y));

                render->setFillColor(style->getColor(p.textColorRole));
                //! \bug Why the extra subtract by one here?
                render->drawText(p.glyphs, glm::vec2(floorf(pos.x), floorf(pos.y + p.fontMetrics.ascender - 1.F)));
            }
        }

        void Label::_initEvent(System::Event::Init& event)
        {
            if (event.getData().resize || event.getData().font)
            {
                _fontUpdate();
            }
        }

        void Label::_updateEvent(System::Event::Update& event)
        {
            DJV_PRIVATE_PTR();
            if (p.fontMetricsFuture.valid() &&
                p.fontMetricsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.fontMetrics = p.fontMetricsFuture.get();
                    p.labelMinimumSizeInit = true;
                    _resize();
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), System::LogLevel::Error);
                }
            }
            if (p.textSizeFuture.valid() &&
                p.textSizeFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.textSize = p.textSizeFuture.get();
                    p.labelMinimumSizeInit = true;
                    _resize();
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), System::LogLevel::Error);
                }
            }
            if (p.sizeStringFuture.valid() &&
                p.sizeStringFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.sizeStringSize = p.sizeStringFuture.get();
                    p.labelMinimumSizeInit = true;
                    _resize();
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), System::LogLevel::Error);
                }
            }
            if (p.glyphsFuture.valid() &&
                p.glyphsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.glyphs = p.glyphsFuture.get();
                    _redraw();
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), System::LogLevel::Error);
                }
            }
        }

        void Label::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.textSizeFuture = p.fontSystem->measure(p.text, p.fontInfo, p.elide);
            if (!p.text.size())
            {
                p.glyphs.clear();
            }
            p.glyphsFuture = p.fontSystem->getGlyphs(p.text, p.fontInfo, p.elide);
        }

        void Label::_sizeStringUpdate()
        {
            DJV_PRIVATE_PTR();
            if (!p.sizeString.empty())
            {
                p.sizeStringFuture = p.fontSystem->measure(p.sizeString, p.fontInfo);
            }
        }

        void Label::_fontUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            p.fontInfo = p.fontFamily.empty() ?
                style->getFontInfo(p.fontFace, p.fontSizeRole) :
                style->getFontInfo(p.fontFamily, p.fontFace, p.fontSizeRole);
            p.fontMetricsFuture = p.fontSystem->getMetrics(p.fontInfo);
            _textUpdate();
            _sizeStringUpdate();
        }

        struct LabelSizeGroup::Private
        {
            std::vector<std::weak_ptr<Label> > labels;
            glm::vec2 minimumSize = glm::vec2(0.F, 0.F);
        };

        LabelSizeGroup::LabelSizeGroup() :
            _p(new Private)
        {}

        LabelSizeGroup::~LabelSizeGroup()
        {}

        std::shared_ptr<LabelSizeGroup> LabelSizeGroup::create()
        {
            return std::shared_ptr<LabelSizeGroup>(new LabelSizeGroup);
        }

        void LabelSizeGroup::addLabel(const std::weak_ptr<Label>& value)
        {
            DJV_PRIVATE_PTR();
            p.labels.push_back(value);
        }

        void LabelSizeGroup::removeLabel(const std::weak_ptr<Label>& value)
        {
            DJV_PRIVATE_PTR();
            if (auto label = value.lock())
            {
                for (auto i = p.labels.begin(); i != p.labels.end(); ++i)
                {
                    if (auto otherLabel = i->lock())
                    {
                        if (label == otherLabel)
                        {
                            p.labels.erase(i);
                            break;
                        }
                    }
                }
            }
        }

        void LabelSizeGroup::clearLabels()
        {
            DJV_PRIVATE_PTR();
            p.labels.clear();
        }

        const glm::vec2& LabelSizeGroup::getMinimumSize() const
        {
            return _p->minimumSize;
        }

        void LabelSizeGroup::calcMinimumSize()
        {
            DJV_PRIVATE_PTR();
            p.minimumSize = glm::vec2(0.F, 0.F);
            auto i = p.labels.begin();
            while (i != p.labels.end())
            {
                if (auto label = i->lock())
                {
                    p.minimumSize = glm::max(p.minimumSize, label->_labelMinimumSize);
                    ++i;
                }
                else
                {
                    i = p.labels.erase(i);
                }
            }
        }

    } // namespace UI
} // namespace djv
