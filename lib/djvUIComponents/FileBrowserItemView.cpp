// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserItemView.h>

#include <djvUI/ITooltipWidget.h>
#include <djvUI/IconSystem.h>
#include <djvUI/SelectionModel.h>

#include <djvRender2D/FontSystem.h>
#include <djvRender2D/Render.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IOSystem.h>
#include <djvAV/ThumbnailSystem.h>
#include <djvAV/TimeFunc.h>

#include <djvOCIO/OCIOSystem.h>

#include <djvImage/ImageDataFunc.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfoFunc.h>

#include <djvCore/StringFormat.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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

                struct Item
                {
                    System::File::Info info;
                    std::string name;

                    Math::BBox2f geometry;

                    bool nameLinesInit = true;
                    std::vector<Render2D::Font::TextLine> nameLines;

                    bool ioInfoInit = true;
                    bool ioInfoValid = false;
                    AV::IO::Info ioInfo;

                    bool thumbnailInit = true;
                    Image::Size thumbnailSize = Image::Size(100, 50);
                    std::shared_ptr<Image::Image> thumbnail;

                    bool nameGlyphsInit = true;
                    std::vector<std::shared_ptr<Render2D::Font::Glyph> > nameGlyphs;
                    bool sizeGlyphsInit = true;
                    std::vector<std::shared_ptr<Render2D::Font::Glyph> > sizeGlyphs;
                    bool timeGlyphsInit = true;
                    std::vector<std::shared_ptr<Render2D::Font::Glyph> > timeGlyphs;
                };

            } // namespace

            struct ItemView::Private
            {
                std::shared_ptr<Render2D::Font::FontSystem> fontSystem;
                ViewType viewType = ViewType::First;
                std::shared_ptr<SelectionModel> selectionModel;
                std::set<size_t> selection;
                Render2D::Font::Metrics nameFontMetrics;
                std::future<Render2D::Font::Metrics> nameFontMetricsFuture;
                std::vector<Item> items;
                std::map<size_t, std::future<std::vector<Render2D::Font::TextLine> > > nameLinesFutures;
                std::map<size_t, AV::ThumbnailSystem::InfoFuture> ioInfoFutures;
                Image::Size thumbnailSize = Image::Size(100, 50);
                std::map<size_t, AV::ThumbnailSystem::ImageFuture> thumbnailFutures;
                std::map<size_t, std::chrono::steady_clock::time_point> thumbnailTimers;
                std::map<System::File::Type, std::shared_ptr<Image::Image> > icons;
                std::map<System::File::Type, std::future<std::shared_ptr<Image::Image> > > iconsFutures;
                std::map<size_t, std::future<std::vector<std::shared_ptr<Render2D::Font::Glyph> > > > nameGlyphsFutures;
                std::map<size_t, std::future<std::vector<std::shared_ptr<Render2D::Font::Glyph> > > > sizeGlyphsFutures;
                std::map<size_t, std::future<std::vector<std::shared_ptr<Render2D::Font::Glyph> > > > timeGlyphsFutures;
                std::vector<float> split = { .7F, .8F, 1.F };
                OCIO::Config ocioConfig;
                std::string outputColorSpace;

                std::shared_ptr<Observer::ValueObserver<OCIO::Config> > ocioConfigObserver;

                size_t hover = invalid;
                size_t grab = invalid;
                System::Event::PointerID pressedId = System::Event::invalidID;
                glm::vec2 pressedPos = glm::vec2(0.F, 0.F);
                std::function<void(const std::vector<System::File::Info>&)> selectedCallback;
                std::function<void(const std::set<size_t>&)> selectedCallback2;
                std::function<void(const std::vector<System::File::Info>&)> activatedCallback;
                std::function<void(const std::set<size_t>&)> activatedCallback2;
            };

            void ItemView::_init(SelectionType selectionType, const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();
                setClassName("djv::UI::FileBrowser::ItemView");

                p.fontSystem = context->getSystemT<Render2D::Font::FontSystem>();

                p.selectionModel = SelectionModel::create(selectionType);

                auto weak = std::weak_ptr<ItemView>(std::dynamic_pointer_cast<ItemView>(shared_from_this()));
                p.selectionModel->setCallback(
                    [weak](const std::set<size_t>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->selection = value;
                            if (widget->_p->selectedCallback)
                            {
                                widget->_p->selectedCallback(widget->_getSelectedItems(widget->_p->selection));
                            }
                            if (widget->_p->selectedCallback2)
                            {
                                widget->_p->selectedCallback2(widget->_p->selection);
                            }
                        }
                    });

                auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.ocioConfigObserver = Observer::ValueObserver<OCIO::Config>::create(
                    ocioSystem->observeCurrentConfig(),
                    [weak, contextWeak](const OCIO::Config& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->ocioConfig = value;
                                auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
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

            std::shared_ptr<ItemView> ItemView::create(SelectionType selectionType, const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<ItemView>(new ItemView);
                out->_init(selectionType, context);
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

            void ItemView::setThumbnailSize(const Image::Size& value)
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

            void ItemView::setItems(const std::vector<System::File::Info>& value)
            {
                _p->items.clear();
                const size_t size = value.size();
                _p->items.resize(size);
                for (size_t i = 0; i < size; ++i)
                {
                    _p->items[i].info = value[i];
                }
                _p->selectionModel->setCount(value.size());
                _itemsUpdate();
            }

            std::set<size_t> ItemView::getSelected() const
            {
                return _p->selectionModel->getSelected();
            }

            void ItemView::setSelected(const std::set<size_t>& value)
            {
                _p->selectionModel->setSelected(value);
                _redraw();
            }

            void ItemView::selectAll()
            {
                _p->selectionModel->selectAll();
                _redraw();
            }

            void ItemView::selectNone()
            {
                _p->selectionModel->selectNone();
                _redraw();
            }

            void ItemView::invertSelection()
            {
                _p->selectionModel->invertSelection();
                _redraw();
            }

            void ItemView::setSelectedCallback(const std::function<void(const std::vector<System::File::Info>&)>& value)
            {
                _p->selectedCallback = value;
            }

            void ItemView::setSelectedCallback(const std::function<void(const std::set<size_t>&)>& value)
            {
                _p->selectedCallback2 = value;
            }

            void ItemView::setActivatedCallback(const std::function<void(const std::vector<System::File::Info>&)>& value)
            {
                _p->activatedCallback = value;
            }

            void ItemView::setActivatedCallback(const std::function<void(const std::set<size_t>&)>& value)
            {
                _p->activatedCallback2 = value;
            }

            float ItemView::getHeightForWidth(float value) const
            {
                DJV_PRIVATE_PTR();
                float out = 0.F;
                const size_t itemsSize = p.items.size();
                if (itemsSize > 0)
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
                        const size_t rows = itemsSize / columns + (itemsSize % columns ? 1 : 0);
                        out = (p.thumbnailSize.h + p.nameFontMetrics.lineHeight * 2.F + m * 2.F + sh * 2.F) * rows;
                        out += s * (rows + 1);
                        break;
                    }
                    case ViewType::List:
                        out = std::max(static_cast<float>(p.thumbnailSize.h), p.nameFontMetrics.lineHeight + m * 2.F) * itemsSize;
                        break;
                    default: break;
                    }
                }
                return out;
            }

            void ItemView::_layoutEvent(System::Event::Layout& event)
            {
                DJV_PRIVATE_PTR();
                const Math::BBox2f& g = getGeometry();
                const auto& style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float s = style->getMetric(MetricsRole::Spacing);
                const float sh = style->getMetric(MetricsRole::Shadow);
                glm::vec2 pos = g.min;
                const size_t itemsSize = p.items.size();
                switch (p.viewType)
                {
                case ViewType::Tiles:
                    pos += s;
                    for (size_t i = 0; i < itemsSize; ++i)
                    {
                        const float itemHeight = p.thumbnailSize.h + p.nameFontMetrics.lineHeight * 2.F + m * 2.F + sh * 2.F;
                        const float itemWidth = p.thumbnailSize.w + sh * 2.F;
                        p.items[i].geometry = Math::BBox2f(pos.x, pos.y, itemWidth, itemHeight);
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
                    for (size_t i = 0; i < itemsSize; ++i)
                    {
                        const float itemHeight = std::max(static_cast<float>(p.thumbnailSize.h), p.nameFontMetrics.lineHeight + m * 2.F);
                        p.items[i].geometry = Math::BBox2f(pos.x, pos.y, g.w(), itemHeight);
                        pos.y += itemHeight;
                    }
                    break;
                default: break;
                }
            }

            void ItemView::_clipEvent(System::Event::Clip& event)
            {
                DJV_PRIVATE_PTR();
                if (isClipped())
                    return;
                if (auto context = getContext().lock())
                {
                    const auto& style = _getStyle();
                    const auto& clipRect = event.getClipRect();
                    const size_t itemsSize = p.items.size();
                    for (size_t i = 0; i < itemsSize; ++i)
                    {
                        auto& item = p.items[i];
                        if (item.geometry.intersects(clipRect))
                        {
                            if (item.nameLinesInit)
                            {
                                item.nameLinesInit = false;
                                const auto k = p.nameLinesFutures.find(i);
                                if (k == p.nameLinesFutures.end())
                                {
                                    const float m = style->getMetric(MetricsRole::MarginSmall);
                                    const auto fontInfo = style->getFontInfo(Render2D::Font::faceDefault, MetricsRole::FontMedium);
                                    item.name = item.info.getFileName(Math::Frame::invalid, false);
                                    p.nameLinesFutures[i] = p.fontSystem->textLines(
                                        item.name,
                                        p.thumbnailSize.w - static_cast<uint16_t>(m * 2.F),
                                        fontInfo);
                                }
                            }
                            if (item.ioInfoInit)
                            {
                                item.ioInfoInit = false;
                                if (p.ioInfoFutures.find(i) == p.ioInfoFutures.end())
                                {
                                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                                    auto ioSystem = context->getSystemT<AV::IO::IOSystem>();
                                    if (thumbnailSystem && ioSystem)
                                    {
                                        if (ioSystem->canRead(item.info))
                                        {
                                            p.ioInfoFutures[i] = thumbnailSystem->getInfo(item.info);
                                        }
                                    }
                                }
                            }
                            if (item.thumbnailInit)
                            {
                                item.thumbnailInit = false;
                                if (p.thumbnailFutures.find(i) == p.thumbnailFutures.end())
                                {
                                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                                    auto ioSystem = context->getSystemT<AV::IO::IOSystem>();
                                    if (thumbnailSystem && ioSystem && ioSystem->canRead(item.info))
                                    {
                                        p.thumbnailFutures[i] = thumbnailSystem->getImage(item.info, p.thumbnailSize);
                                    }
                                }
                            }
                            if (item.nameGlyphsInit)
                            {
                                item.nameGlyphsInit = false;
                                if (p.nameGlyphsFutures.find(i) == p.nameGlyphsFutures.end())
                                {
                                    const std::string& label = item.info.getFileName(Math::Frame::invalid, false);
                                    const auto fontInfo = style->getFontInfo(Render2D::Font::faceDefault, MetricsRole::FontMedium);
                                    p.nameGlyphsFutures[i] = p.fontSystem->getGlyphs(label, fontInfo);
                                }
                            }
                            if (item.sizeGlyphsInit)
                            {
                                item.sizeGlyphsInit = false;
                                if (p.sizeGlyphsFutures.find(i) == p.sizeGlyphsFutures.end())
                                {
                                    std::stringstream ss;
                                    const uint64_t size = item.info.getSize();
                                    ss << Memory::getSizeLabel(size);
                                    std::stringstream ss2;
                                    ss2 << Memory::getUnitLabel(size);
                                    ss << _getText(ss2.str());
                                    const auto fontInfo = style->getFontInfo(Render2D::Font::faceDefault, MetricsRole::FontMedium);
                                    p.sizeGlyphsFutures[i] = p.fontSystem->getGlyphs(ss.str(), fontInfo);
                                }
                            }
                            if (item.timeGlyphsInit)
                            {
                                item.timeGlyphsInit = false;
                                if (p.timeGlyphsFutures.find(i) == p.timeGlyphsFutures.end())
                                {
                                    const std::string& label = AV::Time::getLabel(item.info.getTime());
                                    const auto fontInfo = style->getFontInfo(Render2D::Font::faceDefault, MetricsRole::FontMedium);
                                    p.timeGlyphsFutures[i] = p.fontSystem->getGlyphs(label, fontInfo);
                                }
                            }
                        }
                        else if (auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>())
                        {
                            const auto j = p.ioInfoFutures.find(i);
                            if (j != p.ioInfoFutures.end())
                            {
                                item.ioInfoInit = true;
                                item.ioInfoValid = false;
                                thumbnailSystem->cancelInfo(j->second.uid);
                                p.ioInfoFutures.erase(j);
                            }
                            const auto k = p.thumbnailFutures.find(i);
                            if (k != p.thumbnailFutures.end())
                            {
                                item.thumbnailInit = true;
                                item.thumbnail.reset();
                                thumbnailSystem->cancelImage(k->second.uid);
                                p.thumbnailFutures.erase(k);
                            }
                        }
                    }
                }
            }

            void ItemView::_paintEvent(System::Event::Paint& event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float s = style->getMetric(MetricsRole::Spacing);
                const float sh = style->getMetric(MetricsRole::Shadow);

                const auto& render = _getRender();
                const auto& ut = _getUpdateTime();
                const size_t itemsSize = p.items.size();
                for (size_t i = 0; i < itemsSize; ++i)
                {
                    const auto& item = p.items[i];
                    Math::BBox2f itemGeometry = item.geometry;

                    const bool selected = p.selectionModel->isSelected(i);
                    switch (p.viewType)
                    {
                    case ViewType::Tiles:
                        render->setFillColor(style->getColor(ColorRole::Shadow));
                        render->drawShadow(itemGeometry.margin(0, -sh, 0, 0), sh);
                        itemGeometry = itemGeometry.margin(-sh);
                        render->setFillColor(style->getColor(selected ? ColorRole::Checked : ColorRole::BackgroundBellows));
                        render->drawRect(itemGeometry);
                        break;
                    case ViewType::List:
                        if (selected)
                        {
                            render->setFillColor(style->getColor(ColorRole::Checked));
                            render->drawRect(itemGeometry);
                        }
                        break;
                    default: break;
                    }

                    if (ViewType::List == p.viewType)
                    {
                        render->pushClipRect(Math::BBox2f(
                            itemGeometry.min.x,
                            itemGeometry.min.y,
                            itemGeometry.w() * p.split[0],
                            itemGeometry.h()));
                    }
                    float opacity = 0.F;
                    if (item.thumbnail)
                    {
                        opacity = 1.F;
                        const auto j = p.thumbnailTimers.find(i);
                        if (j != p.thumbnailTimers.end())
                        {
                            const auto t = std::chrono::duration_cast<std::chrono::milliseconds>(ut - j->second);
                            opacity = std::min(t.count() / static_cast<float>(thumbnailFadeTime), 1.F);
                        }
                        const uint16_t w = item.thumbnail->getWidth();
                        const uint16_t h = item.thumbnail->getHeight();
                        glm::vec2 pos(0.F, 0.F);
                        switch (p.viewType)
                        {
                        case ViewType::Tiles:
                            pos.x = floor(itemGeometry.min.x + p.thumbnailSize.w / 2.F - w / 2.F);
                            pos.y = floor(itemGeometry.min.y + p.thumbnailSize.h - h);
                            break;
                        case ViewType::List:
                            pos.x = floor(itemGeometry.min.x);
                            pos.y = floor(itemGeometry.min.y + itemGeometry.h() / 2.F - h / 2.F);
                            break;
                        default: break;
                        }
                        render->setFillColor(Image::Color(0.F, 0.F, 0.F, opacity));
                        render->drawRect(Math::BBox2f(pos.x, pos.y, w, h));
                        render->setFillColor(Image::Color(1.F, 1.F, 1.F, opacity));
                        Render2D::ImageOptions options;
                        auto k = p.ocioConfig.imageColorSpaces.find(item.thumbnail->getPluginName());
                        if (k != p.ocioConfig.imageColorSpaces.end())
                        {
                            options.colorSpace.input = k->second;
                        }
                        else
                        {
                            k = p.ocioConfig.imageColorSpaces.find(std::string());
                            if (k != p.ocioConfig.imageColorSpaces.end())
                            {
                                options.colorSpace.input = k->second;
                            }
                        }
                        options.colorSpace.output = p.outputColorSpace;
                        render->drawImage(item.thumbnail, pos, options);
                    }
                    if (opacity < 1.F)
                    {
                        const auto j = p.icons.find(item.info.getType());
                        if (j != p.icons.end())
                        {
                            const uint16_t w = j->second->getWidth();
                            const uint16_t h = j->second->getHeight();
                            glm::vec2 pos(0.F, 0.F);
                            switch (p.viewType)
                            {
                            case ViewType::Tiles:
                                pos.x = floor(itemGeometry.min.x + p.thumbnailSize.w / 2.F - w / 2.F);
                                pos.y = floor(itemGeometry.min.y + p.thumbnailSize.h - h);
                                break;
                            case ViewType::List:
                                pos.x = floor(itemGeometry.min.x);
                                pos.y = floor(itemGeometry.min.y + itemGeometry.h() / 2.F - h / 2.F);
                                break;
                            default: break;
                            }
                            auto c = style->getColor(ColorRole::Button).convert(Image::Type::RGBA_F32);
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
                            if (!item.nameLines.empty())
                            {
                                float x = itemGeometry.min.x + m;
                                float y = itemGeometry.max.y - p.nameFontMetrics.lineHeight * std::min(item.nameLines.size(), static_cast<size_t>(2)) - m;
                                size_t line = 0;
                                for (auto k = item.nameLines.begin(); k != item.nameLines.end() && line < 2; ++k, ++line)
                                {
                                    //! \bug Why the extra subtract by one here?
                                    render->drawText(
                                        k->glyphs,
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
                            float x = itemGeometry.min.x + p.thumbnailSize.w + s;
                            float y = itemGeometry.min.y + itemGeometry.h() / 2.F - p.nameFontMetrics.lineHeight / 2.F;
                            if (!item.nameGlyphs.empty())
                            {
                                //! \bug Why the extra subtract by one here?
                                render->drawText(
                                    item.nameGlyphs,
                                    glm::vec2(
                                        floorf(x),
                                        floorf(y + p.nameFontMetrics.ascender - 1.F)));
                            }

                            render->popClipRect();

                            x = itemGeometry.min.x + itemGeometry.w() * p.split[0] + m;
                            if (!item.sizeGlyphs.empty())
                            {
                                render->pushClipRect(Math::BBox2f(
                                    itemGeometry.min.x + itemGeometry.w() * p.split[0],
                                    itemGeometry.min.y,
                                    itemGeometry.w() * (p.split[1] - p.split[0]),
                                    itemGeometry.h()));

                                //! \bug Why the extra subtract by one here?
                                render->drawText(
                                    item.sizeGlyphs,
                                    glm::vec2(
                                        floorf(x),
                                        floorf(y + p.nameFontMetrics.ascender - 1.F)));

                                render->popClipRect();
                            }

                            x = itemGeometry.min.x + itemGeometry.w() * p.split[1] + m;
                            if (!item.timeGlyphs.empty())
                            {
                                render->pushClipRect(Math::BBox2f(
                                    itemGeometry.min.x + itemGeometry.w() * p.split[1],
                                    itemGeometry.min.y,
                                    itemGeometry.w() * (p.split[2] - p.split[1]),
                                    itemGeometry.h()));

                                //! \bug Why the extra subtract by one here?
                                render->drawText(
                                    item.timeGlyphs,
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

                    if (p.grab == i)
                    {
                        render->setFillColor(style->getColor(ColorRole::Pressed));
                        render->drawRect(itemGeometry);
                    }
                    else if (p.hover == i)
                    {
                        render->setFillColor(style->getColor(ColorRole::Hovered));
                        render->drawRect(itemGeometry);
                    }
                }
            }

            void ItemView::_pointerEnterEvent(System::Event::PointerEnter& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                const auto& pointerInfo = event.getPointerInfo();
                const size_t itemsSize = p.items.size();
                for (size_t i = 0; i < itemsSize; ++i)
                {
                    if (p.items[i].geometry.contains(pointerInfo.pos))
                    {
                        p.hover = i;
                        _redraw();
                        break;
                    }
                }
            }

            void ItemView::_pointerLeaveEvent(System::Event::PointerLeave& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                if (p.hover != invalid)
                {
                    p.hover = invalid;
                    _redraw();
                }
            }

            void ItemView::_pointerMoveEvent(System::Event::PointerMove& event)
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
                        p.pressedId = System::Event::invalidID;
                        _redraw();
                    }
                }
                else
                {
                    const size_t itemsSize = p.items.size();
                    for (size_t i = 0; i < itemsSize; ++i)
                    {
                        if (p.items[i].geometry.contains(pointerInfo.pos))
                        {
                            p.hover = i;
                            _redraw();
                            break;
                        }
                    }
                }
            }

            void ItemView::_buttonPressEvent(System::Event::ButtonPress& event)
            {
                DJV_PRIVATE_PTR();
                if (p.pressedId)
                    return;
                const auto& pointerInfo = event.getPointerInfo();
                const size_t itemsSize = p.items.size();
                for (size_t i = 0; i < itemsSize; ++i)
                {
                    if (p.items[i].geometry.contains(pointerInfo.pos))
                    {
                        event.accept();
                        p.grab = i;
                        p.pressedId = pointerInfo.id;
                        p.pressedPos = pointerInfo.pos;
                        _redraw();
                        break;
                    }
                }
            }

            void ItemView::_buttonReleaseEvent(System::Event::ButtonRelease& event)
            {
                DJV_PRIVATE_PTR();
                const auto& pointerInfo = event.getPointerInfo();
                if (p.grab != invalid && pointerInfo.id == p.pressedId)
                {
                    event.accept();
                    p.grab = invalid;
                    p.pressedId = System::Event::invalidID;
                    const auto& hover = _getPointerHover();
                    const auto i = hover.find(pointerInfo.id);
                    if (i != hover.end())
                    {
                        const size_t itemsSize = p.items.size();
                        for (size_t j = 0; j < itemsSize; ++j)
                        {
                            const auto& item = p.items[j];
                            if (item.geometry.contains(i->second))
                            {
                                const int modifiers = event.getKeyModifiers();
                                if (0 == modifiers)
                                {
                                    if (p.activatedCallback)
                                    {
                                        p.activatedCallback({ item.info });
                                    }
                                    if (p.activatedCallback2)
                                    {
                                        p.activatedCallback2({ j });
                                    }
                                }
                                else
                                {
                                    p.selectionModel->select(j, modifiers);
                                }
                            }
                        }
                    }
                    _redraw();
                }
            }

            void ItemView::_keyPressEvent(System::Event::KeyPress& event)
            {
                DJV_PRIVATE_PTR();
                if (!event.isAccepted())
                {
                    switch (event.getKey())
                    {
                    case GLFW_KEY_ENTER:
                    case GLFW_KEY_KP_ENTER:
                        if (!p.selection.empty())
                        {
                            event.accept();
                            if (p.activatedCallback)
                            {
                                p.activatedCallback(_getSelectedItems(p.selection));
                            }
                            if (p.activatedCallback2)
                            {
                                p.activatedCallback2(p.selection);
                            }
                        }
                        break;
                    default: break;
                    }
                }
            }

            void ItemView::_keyReleaseEvent(System::Event::KeyRelease& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
            }

            std::shared_ptr<ITooltipWidget> ItemView::_createTooltip(const glm::vec2& pos)
            {
                DJV_PRIVATE_PTR();
                std::shared_ptr<ITooltipWidget> out;
                std::string text;
                const size_t itemsSize = p.items.size();
                for (size_t i = 0; i < itemsSize; ++i)
                {
                    const auto& item = p.items[i];
                    if (item.geometry.contains(pos))
                    {
                        if (item.ioInfoValid)
                        {
                            text = _getTooltip(item.info, item.ioInfo);
                        }
                        else
                        {
                            text = _getTooltip(item.info);
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

            void ItemView::_initEvent(System::Event::Init& event)
            {
                _iconsUpdate();
                if (event.getData().text)
                {
                    _itemsUpdate();
                }
            }

            void ItemView::_updateEvent(System::Event::Update& event)
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
                        _log(e.what(), System::LogLevel::Error);
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
                                p.items[i->first].nameLines = i->second.get();
                                _resize();
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), System::LogLevel::Error);
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
                                auto& item = p.items[i->first];
                                item.ioInfo = i->second.future.get();
                                item.ioInfoValid = true;
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), System::LogLevel::Error);
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
                            auto& item = p.items[i->first];
                            item.thumbnail = nullptr;
                            try
                            {
                                if (const auto image = i->second.future.get())
                                {
                                    item.thumbnail = image;
                                    p.thumbnailTimers[i->first] = _getUpdateTime();
                                    _resize();
                                }
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), System::LogLevel::Error);
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
                                _log(e.what(), System::LogLevel::Error);
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
                                p.items[i->first].nameGlyphs = i->second.get();
                                _resize();
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), System::LogLevel::Error);
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
                                p.items[i->first].sizeGlyphs = i->second.get();
                                _resize();
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), System::LogLevel::Error);
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
                                p.items[i->first].timeGlyphs = i->second.get();
                                _resize();
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), System::LogLevel::Error);
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

            std::vector<System::File::Info> ItemView::_getSelectedItems(const std::set<size_t>& value) const
            {
                DJV_PRIVATE_PTR();
                std::vector<System::File::Info> out;
                const size_t itemsSize = p.items.size();
                for (const auto& i : value)
                {
                    if (i < itemsSize)
                    {
                        out.push_back(p.items[i].info);
                    }
                }
                return out;
            }

            std::string ItemView::_getTooltip(const System::File::Info& fileInfo) const
            {
                std::stringstream ss;
                ss << fileInfo << '\n';
                ss << '\n';
                std::stringstream ss2;
                const size_t size = fileInfo.getSize();
                ss2 << Memory::getUnitLabel(size);
                ss << _getText(DJV_TEXT("file_browser_file_tooltip_size")) << ": " <<
                    Memory::getSizeLabel(size) << _getText(ss2.str()) << '\n';
                ss << _getText(DJV_TEXT("file_browser_file_tooltip_last_modification_time")) << ": " << AV::Time::getLabel(fileInfo.getTime());
                return ss.str();
            }

            std::string ItemView::_getTooltip(const System::File::Info& fileInfo, const AV::IO::Info& avInfo) const
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
                        const AV::Time::Units timeUnits = avSystem->observeTimeUnits()->get();
                        ss << _getText(DJV_TEXT("file_browser_file_tooltip_video_duration")) << ": " <<
                            AV::Time::toString(avInfo.videoSequence.getFrameCount(), avInfo.videoSpeed, timeUnits);
                        switch (timeUnits)
                        {
                        case AV::Time::Units::Frames:
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
                    for (size_t i = 0; i < static_cast<size_t>(System::File::Type::Count); ++i)
                    {
                        const auto type = static_cast<System::File::Type>(i);
                        std::string name;
                        switch (type)
                        {
                        case System::File::Type::Directory: name = "djvIconDirectory";    break;
                        case System::File::Type::Sequence:  name = "djvIconFileSequence"; break;
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
                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                    const size_t itemsSize = p.items.size();
                    for (size_t i = 0; i < itemsSize; ++i)
                    {
                        auto& item = p.items[i];
                        item.name.clear();
                        item.nameLinesInit = true;
                        item.nameLines.clear();
                        item.thumbnailInit = true;
                        item.thumbnail.reset();
                        const auto j = p.thumbnailFutures.find(i);
                        if (j != p.thumbnailFutures.end())
                        {
                            thumbnailSystem->cancelImage(j->second.uid);
                        }
                    }

                    p.nameLinesFutures.clear();
                    p.thumbnailFutures.clear();

                    const auto& style = _getStyle();
                    const auto& clipRect = getClipRect();
                    for (size_t i = 0; i < itemsSize; ++i)
                    {
                        auto& item = p.items[i];
                        if (item.geometry.intersects(clipRect))
                        {
                            const auto fontInfo = style->getFontInfo(Render2D::Font::faceDefault, MetricsRole::FontMedium);
                            item.name = item.info.getFileName(Math::Frame::invalid, false);
                            const float m = style->getMetric(MetricsRole::MarginSmall);
                            p.nameLinesFutures[i] = p.fontSystem->textLines(
                                item.name,
                                p.thumbnailSize.w - static_cast<uint16_t>(m * 2.F),
                                fontInfo);

                            auto ioSystem = context->getSystemT<AV::IO::IOSystem>();
                            if (ioSystem && ioSystem->canRead(item.info))
                            {
                                p.thumbnailFutures[i] = thumbnailSystem->getImage(item.info, p.thumbnailSize);
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
                        style->getFontInfo(Render2D::Font::faceDefault, MetricsRole::FontMedium));

                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                    const size_t itemsSize = p.items.size();
                    for (size_t i = 0; i < itemsSize; ++i)
                    {
                        auto& item = p.items[i];
                        item.name.clear();
                        item.nameLinesInit = true;
                        item.nameLines.clear();
                        item.ioInfoInit = true;
                        item.ioInfoValid = false;
                        item.thumbnailInit = true;
                        item.thumbnail.reset();
                        item.nameGlyphsInit = true;
                        item.nameGlyphs.clear();
                        item.sizeGlyphsInit = true;
                        item.sizeGlyphs.clear();
                        item.timeGlyphsInit = true;
                        item.timeGlyphs.clear();
                        const auto j = p.thumbnailFutures.find(i);
                        if (j != p.thumbnailFutures.end())
                        {
                            thumbnailSystem->cancelImage(j->second.uid);
                        }
                    }

                    p.nameLinesFutures.clear();
                    p.ioInfoFutures.clear();
                    p.thumbnailFutures.clear();
                    p.thumbnailTimers.clear();
                    p.nameGlyphsFutures.clear();
                    p.sizeGlyphsFutures.clear();
                    p.timeGlyphsFutures.clear();
                }
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
