// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserItemView.h>

#include <djvUI/ITooltipWidget.h>
#include <djvUI/IconSystem.h>

#include <djvAV/AVSystem.h>
#include <djvAV/OCIOSystem.h>
#include <djvAV/FontSystem.h>
#include <djvAV/IOSystem.h>
#include <djvAV/Render2D.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfoFunc.h>
#include <djvCore/StringFormat.h>
#include <djvCore/TimeFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            namespace
            {
                //! \todo Should this be configurable?
                const size_t thumbnailFadeTime = 200;

                const size_t invalid = static_cast<size_t>(-1);

            } // namespace

            struct ItemView::Private
            {
                std::shared_ptr<AV::Font::System> fontSystem;
                ViewType viewType = ViewType::First;
                std::vector<FileSystem::FileInfo> items;
                AV::Font::Metrics nameFontMetrics;
                std::future<AV::Font::Metrics> nameFontMetricsFuture;
                std::map<size_t, BBox2f> itemGeometry;
                std::map<size_t, std::string> names;
                std::map<size_t, std::vector<AV::Font::TextLine> > nameLines;
                std::map<size_t, std::future<std::vector<AV::Font::TextLine> > > nameLinesFutures;
                std::map<size_t, AV::IO::Info> ioInfo;
                std::map<size_t, AV::ThumbnailSystem::InfoFuture> ioInfoFutures;
                AV::Image::Size thumbnailSize = AV::Image::Size(100, 50);
                std::map<size_t, std::shared_ptr<AV::Image::Image> > thumbnails;
                std::map<size_t, AV::ThumbnailSystem::ImageFuture> thumbnailFutures;
                std::map<size_t, std::chrono::steady_clock::time_point> thumbnailTimers;
                std::map<FileSystem::FileType, std::shared_ptr<AV::Image::Image> > icons;
                std::map<FileSystem::FileType, std::future<std::shared_ptr<AV::Image::Image> > > iconsFutures;
                std::map<size_t, std::vector<std::shared_ptr<AV::Font::Glyph> > > nameGlyphs;
                std::map<size_t, std::future<std::vector<std::shared_ptr<AV::Font::Glyph> > > > nameGlyphsFutures;
                std::map<size_t, std::vector<std::shared_ptr<AV::Font::Glyph> > > sizeGlyphs;
                std::map<size_t, std::future<std::vector<std::shared_ptr<AV::Font::Glyph> > > > sizeGlyphsFutures;
                std::map<size_t, std::vector<std::shared_ptr<AV::Font::Glyph> > > timeGlyphs;
                std::map<size_t, std::future<std::vector<std::shared_ptr<AV::Font::Glyph> > > > timeGlyphsFutures;
                std::vector<float> split = { .7F, .8F, 1.F };
                AV::OCIO::Config ocioConfig;
                std::string outputColorSpace;

                std::shared_ptr<ValueObserver<AV::OCIO::Config> > ocioConfigObserver;

                size_t hover = invalid;
                size_t grab = invalid;
                Event::PointerID pressedId = Event::invalidID;
                glm::vec2 pressedPos = glm::vec2(0.F, 0.F);
                std::function<void(const FileSystem::FileInfo&)> callback;
            };

            void ItemView::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();
                setClassName("djv::UI::FileBrowser::ItemView");

                p.fontSystem = context->getSystemT<AV::Font::System>();

                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                auto weak = std::weak_ptr<ItemView>(std::dynamic_pointer_cast<ItemView>(shared_from_this()));
                auto contextWeak = std::weak_ptr<Context>(context);
                p.ocioConfigObserver = ValueObserver<AV::OCIO::Config>::create(
                    ocioSystem->observeCurrentConfig(),
                    [weak, contextWeak](const AV::OCIO::Config& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->ocioConfig = value;
                                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                widget->_p->outputColorSpace = ocioSystem->getColorSpace(value.display, value.view);
                                widget->_itemsUpdate();
                            }
                        }
                    });
            }

            ItemView::ItemView() :
                _p(new Private)
            {}

            ItemView::~ItemView()
            {}

            std::shared_ptr<ItemView> ItemView::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ItemView>(new ItemView);
                out->_init(context);
                return out;
            }

            void ItemView::setViewType(ViewType value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.viewType)
                    return;
                p.viewType = value;
                _iconsUpdate();
                _itemsUpdate();
            }

            void ItemView::setThumbnailSize(const AV::Image::Size& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.thumbnailSize)
                    return;
                p.thumbnailSize = value;
                _iconsUpdate();
                _thumbnailsSizeUpdate();
                _resize();
            }

            void ItemView::setSplit(const std::vector<float>& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.split)
                    return;
                p.split = value;
                _resize();
            }

            void ItemView::setItems(const std::vector<FileSystem::FileInfo>& value)
            {
                _p->items = value;
                _itemsUpdate();
            }

            void ItemView::setCallback(const std::function<void(const FileSystem::FileInfo&)>& value)
            {
                _p->callback = value;
            }

            float ItemView::getHeightForWidth(float value) const
            {
                DJV_PRIVATE_PTR();
                float out = 0.F;
                if (const size_t itemCount = p.items.size())
                {
                    const auto& style = _getStyle();
                    const float m = style->getMetric(MetricsRole::MarginSmall);
                    const float s = style->getMetric(MetricsRole::Spacing);
                    const float sh = style->getMetric(MetricsRole::Shadow);
                    switch (p.viewType)
                    {
                    case ViewType::Tiles:
                    {
                        size_t columns = 1;
                        const float itemWidth = p.thumbnailSize.w + sh * 2.F;
                        float x = s + itemWidth;
                        while (x < value - itemWidth)
                        {
                            ++columns;
                            x += itemWidth + s;
                        }
                        const size_t rows = itemCount / columns + (itemCount % columns ? 1 : 0);
                        out = (p.thumbnailSize.h + p.nameFontMetrics.lineHeight * 2.F + m * 2.F + sh * 2.F) * rows;
                        out += s * (rows + 1);
                        break;
                    }
                    case ViewType::List:
                        out = std::max(static_cast<float>(p.thumbnailSize.h), p.nameFontMetrics.lineHeight + m * 2.F) * itemCount;
                        break;
                    default: break;
                    }
                }
                return out;
            }

            void ItemView::_layoutEvent(Event::Layout& event)
            {
                DJV_PRIVATE_PTR();
                const BBox2f& g = getGeometry();
                const auto& style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float s = style->getMetric(MetricsRole::Spacing);
                const float sh = style->getMetric(MetricsRole::Shadow);
                p.itemGeometry.clear();
                glm::vec2 pos = g.min;
                auto item = p.items.begin();
                size_t i = 0;
                switch (p.viewType)
                {
                case ViewType::Tiles:
                    pos += s;
                    for (; item != p.items.end(); ++item, ++i)
                    {
                        const float itemHeight = p.thumbnailSize.h + p.nameFontMetrics.lineHeight * 2.F + m * 2.F + sh * 2.F;
                        const float itemWidth = p.thumbnailSize.w + sh * 2.F;
                        p.itemGeometry[i] = BBox2f(pos.x, pos.y, itemWidth, itemHeight);
                        pos.x += itemWidth;
                        if (pos.x > g.max.x - itemWidth)
                        {
                            pos.x = g.min.x + s;
                            pos.y += itemHeight + s;
                        }
                        else
                        {
                            pos.x += s;
                        }
                    }
                    break;
                case ViewType::List:
                    for (; item != p.items.end(); ++item, ++i)
                    {
                        const float itemHeight = std::max(static_cast<float>(p.thumbnailSize.h), p.nameFontMetrics.lineHeight + m * 2.F);
                        p.itemGeometry[i] = BBox2f(pos.x, pos.y, g.w(), itemHeight);
                        pos.y += itemHeight;
                    }
                    break;
                default: break;
                }
            }

            void ItemView::_clipEvent(Event::Clip& event)
            {
                DJV_PRIVATE_PTR();
                if (isClipped())
                    return;
                if (auto context = getContext().lock())
                {
                    const auto& style = _getStyle();
                    const auto& clipRect = event.getClipRect();
                    for (const auto& i : p.itemGeometry)
                    {
                        if (i.first < p.items.size() && i.second.intersects(clipRect))
                        {
                            const auto& fileInfo = p.items[i.first];
                            {
                                const auto j = p.nameLines.find(i.first);
                                if (j == p.nameLines.end())
                                {
                                    const auto k = p.nameLinesFutures.find(i.first);
                                    if (k == p.nameLinesFutures.end())
                                    {
                                        const float m = style->getMetric(MetricsRole::MarginSmall);
                                        const auto fontInfo = style->getFontInfo(AV::Font::faceDefault, MetricsRole::FontMedium);
                                        p.names[i.first] = fileInfo.getFileName(Frame::invalid, false);
                                        p.nameLinesFutures[i.first] = p.fontSystem->textLines(
                                            p.names[i.first],
                                            p.thumbnailSize.w - static_cast<uint16_t>(m * 2.F),
                                            fontInfo);
                                    }
                                }
                            }
                            if (p.ioInfo.find(i.first) == p.ioInfo.end())
                            {
                                if (p.ioInfoFutures.find(i.first) == p.ioInfoFutures.end())
                                {
                                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                                    auto ioSystem = context->getSystemT<AV::IO::System>();
                                    if (thumbnailSystem && ioSystem)
                                    {
                                        if (ioSystem->canRead(fileInfo))
                                        {
                                            p.ioInfoFutures[i.first] = thumbnailSystem->getInfo(fileInfo);
                                        }
                                    }
                                }
                            }
                            if (p.thumbnails.find(i.first) == p.thumbnails.end())
                            {
                                if (p.thumbnailFutures.find(i.first) == p.thumbnailFutures.end())
                                {
                                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                                    auto ioSystem = context->getSystemT<AV::IO::System>();
                                    if (thumbnailSystem && ioSystem && ioSystem->canRead(fileInfo))
                                    {
                                        p.thumbnailFutures[i.first] = thumbnailSystem->getImage(fileInfo, p.thumbnailSize);
                                    }
                                }
                            }
                            if (p.nameGlyphs.find(i.first) == p.nameGlyphs.end())
                            {
                                if (p.nameGlyphsFutures.find(i.first) == p.nameGlyphsFutures.end())
                                {
                                    const std::string& label = fileInfo.getFileName(Frame::invalid, false);
                                    const auto fontInfo = style->getFontInfo(AV::Font::faceDefault, MetricsRole::FontMedium);
                                    p.nameGlyphsFutures[i.first] = p.fontSystem->getGlyphs(label, fontInfo);
                                }
                            }
                            if (p.sizeGlyphs.find(i.first) == p.sizeGlyphs.end())
                            {
                                if (p.sizeGlyphsFutures.find(i.first) == p.sizeGlyphsFutures.end())
                                {
                                    std::stringstream ss;
                                    const uint64_t size = fileInfo.getSize();
                                    ss << Memory::getSizeLabel(size);
                                    std::stringstream ss2;
                                    ss2 << Memory::getUnitLabel(size);
                                    ss << _getText(ss2.str());
                                    const auto fontInfo = style->getFontInfo(AV::Font::faceDefault, MetricsRole::FontMedium);
                                    p.sizeGlyphsFutures[i.first] = p.fontSystem->getGlyphs(ss.str(), fontInfo);
                                }
                            }
                            if (p.timeGlyphs.find(i.first) == p.timeGlyphs.end())
                            {
                                if (p.timeGlyphsFutures.find(i.first) == p.timeGlyphsFutures.end())
                                {
                                    const std::string& label = Time::getLabel(fileInfo.getTime());
                                    const auto fontInfo = style->getFontInfo(AV::Font::faceDefault, MetricsRole::FontMedium);
                                    p.timeGlyphsFutures[i.first] = p.fontSystem->getGlyphs(label, fontInfo);
                                }
                            }
                        }
                        else if (auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>())
                        {
                            {
                                const auto j = p.ioInfoFutures.find(i.first);
                                if (j != p.ioInfoFutures.end())
                                {
                                    thumbnailSystem->cancelInfo(j->second.uid);
                                    p.ioInfoFutures.erase(j);
                                }
                            }
                            {
                                const auto j = p.thumbnailFutures.find(i.first);
                                if (j != p.thumbnailFutures.end())
                                {
                                    thumbnailSystem->cancelImage(j->second.uid);
                                    p.thumbnailFutures.erase(j);
                                }
                            }
                        }
                    }
                }
            }

            void ItemView::_paintEvent(Event::Paint& event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float s = style->getMetric(MetricsRole::Spacing);
                const float sh = style->getMetric(MetricsRole::Shadow);

                const auto& render = _getRender();
                const auto& ut = _getUpdateTime();
                auto item = p.items.begin();
                size_t index = 0;
                for (; item != p.items.end(); ++item, ++index)
                {
                    const auto i = p.itemGeometry.find(index);
                    if (i != p.itemGeometry.end())
                    {
                        BBox2f itemGeometry = i->second;

                        if (ViewType::Tiles == p.viewType)
                        {
                            render->setFillColor(style->getColor(ColorRole::Shadow));
                            render->drawShadow(itemGeometry.margin(0, -sh, 0, 0), sh);
                            itemGeometry = itemGeometry.margin(-sh);
                            render->setFillColor(style->getColor(ColorRole::BackgroundBellows));
                            render->drawRect(itemGeometry);
                        }

                        if (ViewType::List == p.viewType)
                        {
                            render->pushClipRect(BBox2f(
                                itemGeometry.min.x,
                                itemGeometry.min.y,
                                itemGeometry.w() * p.split[0],
                                itemGeometry.h()));
                        }
                        float opacity = 0.F;
                        {
                            const auto j = p.thumbnails.find(index);
                            if (j != p.thumbnails.end())
                            {
                                if (j->second)
                                {
                                    opacity = 1.F;
                                    const auto k = p.thumbnailTimers.find(index);
                                    if (k != p.thumbnailTimers.end())
                                    {
                                        const auto t = std::chrono::duration_cast<std::chrono::milliseconds>(ut - k->second);
                                        opacity = std::min(t.count() / static_cast<float>(thumbnailFadeTime), 1.F);
                                    }
                                    const uint16_t w = j->second->getWidth();
                                    const uint16_t h = j->second->getHeight();
                                    glm::vec2 pos(0.F, 0.F);
                                    switch (p.viewType)
                                    {
                                    case ViewType::Tiles:
                                        pos.x = floor(i->second.min.x + sh + p.thumbnailSize.w / 2.F - w / 2.F);
                                        pos.y = floor(i->second.min.y + sh + p.thumbnailSize.h - h);
                                        break;
                                    case ViewType::List:
                                        pos.x = floor(i->second.min.x);
                                        pos.y = floor(i->second.min.y + i->second.h() / 2.F - h / 2.F);
                                        break;
                                    default: break;
                                    }
                                    render->setFillColor(AV::Image::Color(0.F, 0.F, 0.F, opacity));
                                    render->drawRect(BBox2f(pos.x, pos.y, w, h));
                                    render->setFillColor(AV::Image::Color(1.F, 1.F, 1.F, opacity));
                                    AV::Render2D::ImageOptions options;
                                    auto l = p.ocioConfig.imageColorSpaces.find(j->second->getPluginName());
                                    if (l != p.ocioConfig.imageColorSpaces.end())
                                    {
                                        options.colorSpace.input = l->second;
                                    }
                                    else
                                    {
                                        l = p.ocioConfig.imageColorSpaces.find(std::string());
                                        if (l != p.ocioConfig.imageColorSpaces.end())
                                        {
                                            options.colorSpace.input = l->second;
                                        }
                                    }
                                    options.colorSpace.output = p.outputColorSpace;
                                    render->drawImage(j->second, pos, options);
                                }
                            }
                        }
                        if (opacity < 1.F)
                        {
                            const auto j = p.icons.find(item->getType());
                            if (j != p.icons.end())
                            {
                                const uint16_t w = j->second->getWidth();
                                const uint16_t h = j->second->getHeight();
                                glm::vec2 pos(0.F, 0.F);
                                switch (p.viewType)
                                {
                                case ViewType::Tiles:
                                    pos.x = floor(i->second.min.x + sh + p.thumbnailSize.w / 2.F - w / 2.F);
                                    pos.y = floor(i->second.min.y + sh + p.thumbnailSize.h - h);
                                    break;
                                case ViewType::List:
                                    pos.x = floor(i->second.min.x);
                                    pos.y = floor(i->second.min.y + i->second.h() / 2.F - h / 2.F);
                                    break;
                                default: break;
                                }
                                auto c = style->getColor(ColorRole::Button).convert(AV::Image::Type::RGBA_F32);
                                c.setF32(1.F - opacity, 3);
                                render->setFillColor(c);
                                render->drawFilledImage(j->second, pos);
                            }
                        }
                        {
                            render->setFillColor(style->getColor(ColorRole::Foreground));
                            switch (p.viewType)
                            {
                            case ViewType::Tiles:
                            {
                                const auto j = p.names.find(index);
                                const auto k = p.nameLines.find(index);
                                if (j != p.names.end() && k != p.nameLines.end())
                                {
                                    float x = i->second.min.x + m + sh;
                                    float y = i->second.max.y - p.nameFontMetrics.lineHeight * std::min(k->second.size(), static_cast<size_t>(2)) - m - sh;
                                    size_t line = 0;
                                    for (auto l = k->second.begin(); l != k->second.end() && line < 2; ++l, ++line)
                                    {
                                        //! \bug Why the extra subtract by one here?
                                        render->drawText(
                                            l->glyphs,
                                            glm::vec2(
                                                //floorf(x + p.thumbnailSize.x / 2.F - l->size.x / 2.F),
                                                floor(x),
                                                floorf(y + p.nameFontMetrics.ascender - 1.F)));
                                        y += p.nameFontMetrics.lineHeight;
                                    }
                                }
                                break;
                            }
                            case ViewType::List:
                            {
                                float x = i->second.min.x + p.thumbnailSize.w + s;
                                float y = i->second.min.y + i->second.h() / 2.F - p.nameFontMetrics.lineHeight / 2.F;
                                auto j = p.nameGlyphs.find(index);
                                if (j != p.nameGlyphs.end())
                                {
                                    //! \bug Why the extra subtract by one here?
                                    render->drawText(
                                        j->second,
                                        glm::vec2(
                                            floorf(x),
                                            floorf(y + p.nameFontMetrics.ascender - 1.F)));
                                }

                                render->popClipRect();

                                x = i->second.min.x + i->second.w() * p.split[0] + m;
                                j = p.sizeGlyphs.find(index);
                                if (j != p.sizeGlyphs.end())
                                {
                                    render->pushClipRect(BBox2f(
                                        itemGeometry.min.x + itemGeometry.w() * p.split[0],
                                        itemGeometry.min.y,
                                        itemGeometry.w() * (p.split[1] - p.split[0]),
                                        itemGeometry.h()));

                                    //! \bug Why the extra subtract by one here?
                                    render->drawText(
                                        j->second,
                                        glm::vec2(
                                            floorf(x),
                                            floorf(y + p.nameFontMetrics.ascender - 1.F)));

                                    render->popClipRect();
                                }

                                x = i->second.min.x + i->second.w() * p.split[1] + m;
                                j = p.timeGlyphs.find(index);
                                if (j != p.timeGlyphs.end())
                                {
                                    render->pushClipRect(BBox2f(
                                        itemGeometry.min.x + itemGeometry.w() * p.split[1],
                                        itemGeometry.min.y,
                                        itemGeometry.w() * (p.split[2] - p.split[1]),
                                        itemGeometry.h()));

                                    //! \bug Why the extra subtract by one here?
                                    render->drawText(
                                        j->second,
                                        glm::vec2(
                                            floorf(x),
                                            floorf(y + p.nameFontMetrics.ascender - 1.F)));

                                    render->popClipRect();
                                }
                                break;
                            }
                            default: break;
                            }
                        }

                        if (p.grab == index)
                        {
                            render->setFillColor(style->getColor(ColorRole::Pressed));
                            render->drawRect(itemGeometry);
                        }
                        else if (p.hover == index)
                        {
                            render->setFillColor(style->getColor(ColorRole::Hovered));
                            render->drawRect(itemGeometry);
                        }
                    }
                }
            }

            void ItemView::_pointerEnterEvent(Event::PointerEnter& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                const auto& pointerInfo = event.getPointerInfo();
                for (const auto& i : p.itemGeometry)
                {
                    if (i.second.contains(pointerInfo.pos))
                    {
                        p.hover = i.first;
                        _redraw();
                        break;
                    }
                }
            }

            void ItemView::_pointerLeaveEvent(Event::PointerLeave& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                if (p.hover != invalid)
                {
                    p.hover = invalid;
                    _redraw();
                }
            }

            void ItemView::_pointerMoveEvent(Event::PointerMove& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                const auto& pointerInfo = event.getPointerInfo();
                if (p.grab != invalid && pointerInfo.id == p.pressedId)
                {
                    const float distance = glm::length(pointerInfo.projectedPos - p.pressedPos);
                    const auto& style = _getStyle();
                    const bool accepted = distance < style->getMetric(MetricsRole::Drag);
                    event.setAccepted(accepted);
                    if (!accepted)
                    {
                        p.grab = invalid;
                        p.pressedId = Event::invalidID;
                        _redraw();
                    }
                }
                else
                {
                    for (const auto& i : p.itemGeometry)
                    {
                        if (i.second.contains(pointerInfo.pos))
                        {
                            p.hover = i.first;
                            _redraw();
                            break;
                        }
                    }
                }
            }

            void ItemView::_buttonPressEvent(Event::ButtonPress& event)
            {
                DJV_PRIVATE_PTR();
                if (p.pressedId)
                    return;
                const auto& pointerInfo = event.getPointerInfo();
                for (const auto& i : p.itemGeometry)
                {
                    if (i.second.contains(pointerInfo.pos))
                    {
                        event.accept();
                        p.grab = i.first;
                        p.pressedId = pointerInfo.id;
                        p.pressedPos = pointerInfo.pos;
                        _redraw();
                        break;
                    }
                }
            }

            void ItemView::_buttonReleaseEvent(Event::ButtonRelease& event)
            {
                DJV_PRIVATE_PTR();
                const auto& pointerInfo = event.getPointerInfo();
                if (p.grab != invalid && pointerInfo.id == p.pressedId)
                {
                    event.accept();
                    p.grab = invalid;
                    p.pressedId = Event::invalidID;
                    const auto& hover = _getPointerHover();
                    const auto i = hover.find(pointerInfo.id);
                    if (p.callback && i != hover.end())
                    {
                        for (const auto& j : p.itemGeometry)
                        {
                            if (j.second.contains(i->second))
                            {
                                if (j.first < p.items.size())
                                {
                                    p.callback(p.items[j.first]);
                                }
                            }
                        }
                    }
                    _redraw();
                }
            }

            std::shared_ptr<ITooltipWidget> ItemView::_createTooltip(const glm::vec2& pos)
            {
                DJV_PRIVATE_PTR();
                std::shared_ptr<ITooltipWidget> out;
                std::string text;
                for (const auto& i : p.itemGeometry)
                {
                    if (i.second.contains(pos))
                    {
                        if (i.first < p.items.size())
                        {
                            const auto& fileInfo = p.items[i.first];
                            const auto j = p.ioInfo.find(i.first);
                            if (j != p.ioInfo.end())
                            {
                                text = _getTooltip(fileInfo, j->second);
                            }
                            else
                            {
                                text = _getTooltip(fileInfo);
                            }
                        }
                        break;
                    }
                }
                if (!text.empty())
                {
                    out = _createTooltipDefault();
                    out->setTooltip(text);
                }
                return out;
            }

            void ItemView::_initEvent(Event::Init& event)
            {
                _iconsUpdate();
                if (event.getData().text)
                {
                    _itemsUpdate();
                }
            }

            void ItemView::_updateEvent(Event::Update& event)
            {
                DJV_PRIVATE_PTR();
                if (p.nameFontMetricsFuture.valid() &&
                    p.nameFontMetricsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        p.nameFontMetrics = p.nameFontMetricsFuture.get();
                        _resize();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                }
                {
                    auto i = p.nameLinesFutures.begin();
                    while (i != p.nameLinesFutures.end())
                    {
                        if (i->second.valid() &&
                            i->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                p.nameLines[i->first] = i->second.get();
                                _resize();
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.nameLinesFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
                {
                    auto i = p.ioInfoFutures.begin();
                    while (i != p.ioInfoFutures.end())
                    {
                        if (i->second.future.valid() &&
                            i->second.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                p.ioInfo[i->first] = i->second.future.get();
                            }
                            catch (const std::exception& e)
                            {
                                p.ioInfo[i->first] = AV::IO::Info();
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.ioInfoFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
                {
                    auto i = p.thumbnailFutures.begin();
                    while (i != p.thumbnailFutures.end())
                    {
                        if (i->second.future.valid() &&
                            i->second.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            p.thumbnails[i->first] = nullptr;
                            try
                            {
                                if (const auto image = i->second.future.get())
                                {
                                    p.thumbnails[i->first] = image;
                                    p.thumbnailTimers[i->first] = _getUpdateTime();
                                    _resize();
                                }
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.thumbnailFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
                if (p.thumbnailTimers.size())
                {
                    const auto& ut = _getUpdateTime();
                    auto i = p.thumbnailTimers.begin();
                    while (i != p.thumbnailTimers.end())
                    {
                        const auto t = std::chrono::duration_cast<std::chrono::milliseconds>(ut - i->second);
                        if (t.count() > thumbnailFadeTime)
                        {
                            i = p.thumbnailTimers.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                    _resize();
                }
                {
                    auto i = p.iconsFutures.begin();
                    while (i != p.iconsFutures.end())
                    {
                        if (i->second.valid() &&
                            i->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                p.icons[i->first] = i->second.get();
                                _resize();
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.iconsFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
                {
                    auto i = p.nameGlyphsFutures.begin();
                    while (i != p.nameGlyphsFutures.end())
                    {
                        if (i->second.valid() &&
                            i->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                p.nameGlyphs[i->first] = i->second.get();
                                _resize();
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.nameGlyphsFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
                {
                    auto i = p.sizeGlyphsFutures.begin();
                    while (i != p.sizeGlyphsFutures.end())
                    {
                        if (i->second.valid() &&
                            i->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                p.sizeGlyphs[i->first] = i->second.get();
                                _resize();
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.sizeGlyphsFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
                {
                    auto i = p.timeGlyphsFutures.begin();
                    while (i != p.timeGlyphsFutures.end())
                    {
                        if (i->second.valid() &&
                            i->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                p.timeGlyphs[i->first] = i->second.get();
                                _resize();
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.timeGlyphsFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
            }

            std::string ItemView::_getTooltip(const FileSystem::FileInfo& fileInfo) const
            {
                std::stringstream ss;
                ss << fileInfo << '\n';
                ss << '\n';
                std::stringstream ss2;
                const size_t size = fileInfo.getSize();
                ss2 << Memory::getUnitLabel(size);
                ss << _getText(DJV_TEXT("file_browser_file_tooltip_size")) << ": " <<
                    Memory::getSizeLabel(size) << _getText(ss2.str()) << '\n';
                ss << _getText(DJV_TEXT("file_browser_file_tooltip_last_modification_time")) << ": " << Time::getLabel(fileInfo.getTime());
                return ss.str();
            }

            std::string ItemView::_getTooltip(const FileSystem::FileInfo& fileInfo, const AV::IO::Info& avInfo) const
            {
                std::string out;
                if (auto context = getContext().lock())
                {
                    std::stringstream ss;
                    ss.precision(2);
                    ss << _getTooltip(fileInfo);

                    if (avInfo.videoSequence.getFrameCount() > 1)
                    {
                        ss << '\n' << '\n';
                        ss << _getText(DJV_TEXT("file_browser_file_tooltip_video_speed")) << ": " <<
                            std::fixed << avInfo.videoSpeed.toFloat() <<
                            _getText(DJV_TEXT("file_browser_file_tooltip_video_fps")) << '\n';
                        auto avSystem = context->getSystemT<AV::AVSystem>();
                        const Time::Units timeUnits = avSystem->observeTimeUnits()->get();
                        ss << _getText(DJV_TEXT("file_browser_file_tooltip_video_duration")) << ": " <<
                            Time::toString(avInfo.videoSequence.getFrameCount(), avInfo.videoSpeed, timeUnits);
                        switch (timeUnits)
                        {
                        case Time::Units::Frames:
                            ss << " " << _getText(DJV_TEXT("file_browser_file_tooltip_video_unit_frames"));
                            break;
                        default: break;
                        }
                    }

                    for (const auto& videoInfo : avInfo.video)
                    {
                        ss << '\n' << '\n';
                        ss << videoInfo.name << '\n';
                        ss << _getText(DJV_TEXT("file_browser_file_tooltip_video_dimensions")) << ": " << videoInfo.size << " " << std::fixed << videoInfo.size.getAspectRatio() << '\n';
                        std::stringstream ss2;
                        ss2 << videoInfo.type;
                        ss << _getText(DJV_TEXT("file_browser_file_tooltip_video_type")) << ": " << _getText(ss2.str());
                    }

                    if (avInfo.audio.isValid())
                    {
                        ss << '\n' << '\n';
                        ss << avInfo.audio.name << '\n';
                        ss << _getText(DJV_TEXT("file_browser_file_tooltip_audio_channels")) << ": " << static_cast<int>(avInfo.audio.channelCount) << '\n';
                        std::stringstream ss2;
                        ss2 << avInfo.audio.type;
                        ss << _getText(DJV_TEXT("file_browser_file_tooltip_audio_type")) << ": " << _getText(ss2.str()) << '\n';
                        ss << _getText(DJV_TEXT("file_browser_file_tooltip_audio_sample_rate")) << ": " <<
                            avInfo.audio.sampleRate / 1000.F << _getText(DJV_TEXT("file_browser_file_tooltip_audio_unit_khz")) << '\n';
                        ss << _getText(DJV_TEXT("file_browser_file_tooltip_audio_duration")) << ": " <<
                            (avInfo.audio.sampleRate > 0 ? (avInfo.audio.sampleCount / static_cast<float>(avInfo.audio.sampleRate)) : 0.F) <<
                          " " << _getText(DJV_TEXT("file_browser_file_tooltip_audio_unit_seconds"));
                    }
                    out = ss.str();
                }
                return out;
            }

            void ItemView::_iconsUpdate()
            {
                DJV_PRIVATE_PTR();
                if (auto context = getContext().lock())
                {
                    p.icons.clear();
                    auto iconSystem = context->getSystemT<IconSystem>();
                    for (size_t i = 0; i < static_cast<size_t>(FileSystem::FileType::Count); ++i)
                    {
                        const auto type = static_cast<FileSystem::FileType>(i);
                        std::string name;
                        switch (type)
                        {
                        case FileSystem::FileType::Directory: name = "djvIconDirectory";    break;
                        case FileSystem::FileType::Sequence:  name = "djvIconFileSequence"; break;
                        default: name = "djvIconFile"; break;
                        }
                        p.iconsFutures[type] = iconSystem->getIcon(name, p.thumbnailSize.h);
                    }
                }
            }

            void ItemView::_thumbnailsSizeUpdate()
            {
                DJV_PRIVATE_PTR();
                if (auto context = getContext().lock())
                {
                    p.thumbnails.clear();
                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                    const auto& style = _getStyle();
                    p.names.clear();
                    p.nameLines.clear();
                    p.nameLinesFutures.clear();
                    for (const auto& i : p.itemGeometry)
                    {
                        const auto j = p.thumbnailFutures.find(i.first);
                        if (j != p.thumbnailFutures.end())
                        {
                            thumbnailSystem->cancelImage(j->second.uid);
                            p.thumbnailFutures.erase(j);
                        }
                    }
                    p.thumbnailFutures.clear();

                    const auto& clipRect = getClipRect();
                    for (const auto& i : p.itemGeometry)
                    {
                        if (i.first < p.items.size() && i.second.intersects(clipRect))
                        {
                            if (p.thumbnails.find(i.first) == p.thumbnails.end())
                            {
                                const auto& fileInfo = p.items[i.first];
                                {
                                    const auto j = p.nameLines.find(i.first);
                                    if (j == p.nameLines.end())
                                    {
                                        const auto k = p.nameLinesFutures.find(i.first);
                                        if (k == p.nameLinesFutures.end())
                                        {
                                            const float m = style->getMetric(MetricsRole::MarginSmall);
                                            const auto fontInfo = style->getFontInfo(AV::Font::faceDefault, MetricsRole::FontMedium);
                                            p.names[i.first] = fileInfo.getFileName(Frame::invalid, false);
                                            p.nameLinesFutures[i.first] = p.fontSystem->textLines(
                                                p.names[i.first],
                                                p.thumbnailSize.w - static_cast<uint16_t>(m * 2.F),
                                                fontInfo);
                                        }
                                    }
                                }
                                if (p.thumbnailFutures.find(i.first) == p.thumbnailFutures.end())
                                {
                                    auto ioSystem = context->getSystemT<AV::IO::System>();
                                    if (ioSystem && ioSystem->canRead(fileInfo))
                                    {
                                        p.thumbnailFutures[i.first] = thumbnailSystem->getImage(fileInfo, p.thumbnailSize);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            void ItemView::_itemsUpdate()
            {
                DJV_PRIVATE_PTR();
                if (auto context = getContext().lock())
                {
                    const auto& style = _getStyle();
                    p.nameFontMetricsFuture = p.fontSystem->getMetrics(
                        style->getFontInfo(AV::Font::faceDefault, MetricsRole::FontMedium));
                    p.names.clear();
                    p.nameLines.clear();
                    p.nameLinesFutures.clear();
                    p.ioInfo.clear();
                    p.ioInfoFutures.clear();
                    p.thumbnails.clear();
                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                    for (const auto& i : p.itemGeometry)
                    {
                        const auto j = p.thumbnailFutures.find(i.first);
                        if (j != p.thumbnailFutures.end())
                        {
                            thumbnailSystem->cancelImage(j->second.uid);
                            p.thumbnailFutures.erase(j);
                        }
                    }
                    p.thumbnailFutures.clear();
                    p.thumbnailTimers.clear();
                    p.nameGlyphs.clear();
                    p.nameGlyphsFutures.clear();
                    p.sizeGlyphs.clear();
                    p.sizeGlyphsFutures.clear();
                    p.timeGlyphs.clear();
                    p.timeGlyphsFutures.clear();
                }
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
