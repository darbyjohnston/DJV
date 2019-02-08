//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvUI/FileBrowserItemView.h>

#include <djvUI/IconSystem.h>

#include <djvAV/IO.h>
#include <djvAV/Render2D.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            namespace
            {
                //! \todo [1.0 S] Should this be configurable?
                const float thumbnailsFadeTime = .2f;

                const size_t invalid = static_cast<size_t>(-1);

            } // namespace

            struct ItemView::Private
            {
                ViewType viewType = ViewType::First;
                std::vector<FileSystem::FileInfo> items;
                AV::Font::Metrics typeFontMetrics;
                std::future<AV::Font::Metrics> typeFontMetricsFuture;
                AV::Font::Metrics nameFontMetrics;
                std::future<AV::Font::Metrics> nameFontMetricsFuture;
                std::map<size_t, BBox2f> itemGeometry;
                std::map<size_t, std::vector<AV::Font::TextLine> > nameLines;
                std::map<size_t, std::future<std::vector<AV::Font::TextLine> > > nameLinesFutures;
                std::map<size_t, AV::IO::Info> ioInfo;
                std::map<size_t, AV::ThumbnailSystem::InfoFuture> ioInfoFutures;
                std::map<size_t, std::shared_ptr<AV::Image::Image> > thumbnails;
                std::map<size_t, AV::ThumbnailSystem::ImageFuture> thumbnailsFutures;
                std::map<size_t, float> thumbnailsTimers;
                std::map<FileSystem::FileType, std::shared_ptr<AV::Image::Image> > icons;
                std::map<FileSystem::FileType, std::future<std::shared_ptr<AV::Image::Image> > > iconsFutures;
                size_t hover = invalid;
                size_t grab = invalid;
                Event::PointerID pressedId = Event::InvalidID;
                glm::vec2 pressedPos = glm::vec2(0.f, 0.f);
                std::function<void(const FileSystem::FileInfo &)> callback;

				MetricsRole getThumbnailWidth() const;
				MetricsRole getThumbnailHeight() const;
                std::string getTooltip(const FileSystem::FileInfo &, Context * context) const;
                std::string getTooltip(const FileSystem::FileInfo &, const AV::IO::Info &, Context *) const;
            };

            void ItemView::_init(Context * context)
            {
                UI::Widget::_init(context);
                setClassName("djv::UI::FileBrowser::ItemView");
            }

            ItemView::ItemView() :
                _p(new Private)
            {}

            ItemView::~ItemView()
            {}

            std::shared_ptr<ItemView> ItemView::create(Context * context)
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

            void ItemView::setItems(const std::vector<FileSystem::FileInfo> & value)
            {
                _p->items = value;
                _itemsUpdate();
            }

            void ItemView::setCallback(const std::function<void(const FileSystem::FileInfo &)> & value)
            {
                _p->callback = value;
            }

            float ItemView::getHeightForWidth(float value) const
            {
                DJV_PRIVATE_PTR();
                float out = 0.f;
                if (const size_t itemCount = p.items.size())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float m = style->getMetric(MetricsRole::MarginSmall);
                        const float s = style->getMetric(MetricsRole::SpacingSmall);
                        const float tw = style->getMetric(p.getThumbnailWidth());
                        const float th = style->getMetric(p.getThumbnailHeight());
                        switch (p.viewType)
                        {
                        case ViewType::ThumbnailsLarge:
                        case ViewType::ThumbnailsSmall:
                        {
                            size_t columns = 1;
                            float x = tw + m * 2.f;
                            while (x < value - (tw + m * 2.f))
                            {
                                ++columns;
                                x += tw + m * 2.f;
                            }
                            const size_t rows = itemCount / columns + (itemCount % columns ? 1 : 0);
                            out = (th + s + p.nameFontMetrics.lineHeight * 2.f + m * 2.f) * rows;
                            break;
                        }
                        case ViewType::ListView:
                            out = (std::max(th, p.nameFontMetrics.lineHeight) + m * 2.f) * itemCount;
                            break;
                        default: break;
                        }
                    }
                }
                return out;
            }

            void ItemView::_styleEvent(Event::Style&)
            {
                _iconsUpdate();
                _itemsUpdate();
            }

            void ItemView::_preLayoutEvent(Event::PreLayout& event)
            {
                DJV_PRIVATE_PTR();
                if (p.nameFontMetricsFuture.valid() &&
					p.nameFontMetricsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        p.nameFontMetrics = p.nameFontMetricsFuture.get();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                }
                if (p.typeFontMetricsFuture.valid() &&
					p.typeFontMetricsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        p.typeFontMetrics = p.typeFontMetricsFuture.get();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                }
            }

            void ItemView::_layoutEvent(Event::Layout& event)
            {
				DJV_PRIVATE_PTR();
                if (auto style = _getStyle().lock())
                {
                    const BBox2f & g = getGeometry();
                    const float m = style->getMetric(MetricsRole::MarginSmall);
                    const float s = style->getMetric(MetricsRole::SpacingSmall);
                    const float tw = style->getMetric(p.getThumbnailWidth());
                    const float th = style->getMetric(p.getThumbnailHeight());
                    p.itemGeometry.clear();
                    glm::vec2 pos = g.min;
                    auto item = p.items.begin();
                    size_t i = 0;
                    for (; item != p.items.end(); ++item, ++i)
                    {
                        switch (p.viewType)
                        {
                        case ViewType::ThumbnailsLarge:
                        case ViewType::ThumbnailsSmall:
                        {
                            const float itemHeight = th + s + p.nameFontMetrics.lineHeight * 2.f + m * 2.f;
                            p.itemGeometry[i] = BBox2f(pos.x, pos.y, tw + m * 2.f, itemHeight);
                            pos.x += tw + m * 2.f;
                            if (pos.x > g.max.x - (tw + m * 2.f))
                            {
                                pos.x = g.min.x;
                                pos.y += itemHeight;
                            }
                            break;
                        }
                        case ViewType::ListView:
                        {
                            const float itemHeight = std::max(th, p.nameFontMetrics.lineHeight) + m * 2.f;
                            p.itemGeometry[i] = BBox2f(pos.x, pos.y, g.w(), itemHeight);
                            pos.y += itemHeight;
                            break;
                        }
                        default: break;
                        }
                    }
                }
            }

            void ItemView::_clipEvent(Event::Clip& event)
            {
				DJV_PRIVATE_PTR();
				if (isClipped())
					return;
                auto context = getContext();
                auto style = _getStyle().lock();
                auto fontSystem = _getFontSystem().lock();
                const auto & clipRect = event.getClipRect();
                for (const auto & i : p.itemGeometry)
                {
                    if (i.first < p.items.size() && i.second.intersects(clipRect))
                    {
                        {
                            const auto j = p.nameLines.find(i.first);
                            if (j == p.nameLines.end())
                            {
                                const auto k = p.nameLinesFutures.find(i.first);
                                if (k == p.nameLinesFutures.end())
                                {
                                    const auto & fileInfo = p.items[i.first];
                                    const float tw = style->getMetric(p.getThumbnailWidth());
                                    const float m = style->getMetric(MetricsRole::MarginSmall);
                                    const auto fontInfo = style->getFontInfo(AV::Font::Info::faceDefault, MetricsRole::FontMedium);
                                    p.nameLinesFutures[i.first] = fontSystem->textLines(
                                        fileInfo.getFileName(Frame::Invalid, false), tw - m * 2.f, fontInfo);
                                }
                            }
                        }
                        {
                            if (p.ioInfo.find(i.first) == p.ioInfo.end())
                            {
                                if (p.ioInfoFutures.find(i.first) == p.ioInfoFutures.end())
                                {
                                    const auto & fileInfo = p.items[i.first];
                                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>().lock();
                                    auto ioSystem = context->getSystemT<AV::IO::System>().lock();
                                    if (thumbnailSystem && ioSystem)
                                    {
                                        const auto & path = fileInfo.getPath();
                                        if (ioSystem->canRead(path))
                                        {
                                            p.ioInfoFutures[i.first] = thumbnailSystem->getInfo(path);
                                        }
                                    }
                                }
                            }
                        }
                        {
                            if (p.thumbnails.find(i.first) == p.thumbnails.end())
                            {
                                if (p.thumbnailsFutures.find(i.first) == p.thumbnailsFutures.end())
                                {
                                    const auto & fileInfo = p.items[i.first];
                                    const auto & path = fileInfo.getPath();
                                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>().lock();
                                    auto ioSystem = context->getSystemT<AV::IO::System>().lock();
                                    auto style = _getStyle().lock();
                                    if (thumbnailSystem && ioSystem && ioSystem->canRead(path) && style)
                                    {
                                        const float tw = style->getMetric(p.getThumbnailWidth());
                                        const float th = style->getMetric(p.getThumbnailHeight());
                                        p.thumbnailsFutures[i.first] = thumbnailSystem->getImage(
                                            path,
                                            glm::ivec2(static_cast<int>(tw), static_cast<int>(th)));
                                    }
                                }
                            }
                        }
                    }
                    else if (auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>().lock())
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
                            const auto j = p.thumbnailsFutures.find(i.first);
                            if (j != p.thumbnailsFutures.end())
                            {
                                thumbnailSystem->cancelImage(j->second.uid);
                                p.thumbnailsFutures.erase(j);
                            }
                        }
                    }
                }
            }

            void ItemView::_paintEvent(Event::Paint& event)
            {
                DJV_PRIVATE_PTR();
                if (auto render = _getRender().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float m = style->getMetric(MetricsRole::MarginSmall);
                        const float s = style->getMetric(MetricsRole::SpacingSmall);
                        const float tw = style->getMetric(p.getThumbnailWidth());
                        const float th = style->getMetric(p.getThumbnailHeight());
                        {
                            const auto i = p.itemGeometry.find(p.grab);
                            if (i != p.itemGeometry.end())
                            {
                                render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Pressed)));
                                render->drawRect(i->second);
                            }
                        }
                        {
                            const auto i = p.itemGeometry.find(p.hover);
                            const auto j = p.itemGeometry.find(p.grab);
                            if (i != p.itemGeometry.end() && i != j)
                            {
                                render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Hovered)));
                                render->drawRect(i->second);
                            }
                        }
                        const float ut = _getUpdateTime();
                        auto item = p.items.begin();
                        size_t index = 0;
                        for (; item != p.items.end(); ++item, ++index)
                        {
                            const auto i = p.itemGeometry.find(index);
                            if (i != p.itemGeometry.end())
                            {
                                float opacity = 0.f;
                                {
                                    const auto j = p.thumbnails.find(index);
                                    if (j != p.thumbnails.end())
                                    {
										if (j->second)
										{
											opacity = 1.f;
											const auto k = p.thumbnailsTimers.find(index);
											if (k != p.thumbnailsTimers.end())
											{
												opacity = std::min((ut - k->second) / thumbnailsFadeTime, 1.f);
											}
											const auto & size = j->second->getSize();
											float x = 0.f;
											float y = 0.f;
											switch (p.viewType)
											{
											case ViewType::ThumbnailsLarge:
											case ViewType::ThumbnailsSmall:
												x = floor(i->second.min.x + m + tw / 2.f - size.x / 2.f);
												y = floor(i->second.min.y + m + th - size.y);
												break;
											case ViewType::ListView:
												x = floor(i->second.min.x + m);
												y = floor(i->second.min.y + i->second.h() / 2.f - size.y / 2.f);
												break;
											default: break;
											}
											render->setFillColor(_getColorWithOpacity(AV::Image::Color(1.f, 1.f, 1.f, opacity)));
											render->drawImage(
												j->second,
												BBox2f(x, y, static_cast<float>(size.x), static_cast<float>(size.y)),
												AV::Render::ImageCache::Static);
										}
                                    }
                                }
                                if (opacity < 1.f)
                                {
                                    const auto j = p.icons.find(item->getType());
                                    if (j != p.icons.end())
                                    {
                                        const auto & size = j->second->getSize();
                                        float x = 0.f;
                                        float y = 0.f;
                                        switch (p.viewType)
                                        {
                                        case ViewType::ThumbnailsLarge:
                                        case ViewType::ThumbnailsSmall:
                                            x = floor(i->second.min.x + m + tw / 2.f - size.x / 2.f);
                                            y = floor(i->second.min.y + m + th - size.y);
                                            break;
                                        case ViewType::ListView:
                                            x = floor(i->second.min.x + m);
                                            y = floor(i->second.min.y + i->second.h() / 2.f - size.y / 2.f);
                                            break;
                                        default: break;
                                        }
                                        auto c = style->getColor(ColorRole::Button).convert(AV::Image::Type::RGBA_F32);
                                        c.setF32(1.f - opacity, 3);
                                        render->setFillColor(_getColorWithOpacity(c));
                                        render->drawFilledImage(
                                            j->second,
                                            BBox2f(x, y, static_cast<float>(size.x), static_cast<float>(size.y)),
                                            AV::Render::ImageCache::Static);
                                    }
                                }
                                {
                                    render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Foreground)));
                                    render->setCurrentFont(style->getFontInfo(AV::Font::Info::faceDefault, MetricsRole::FontMedium));
                                    switch (p.viewType)
                                    {
                                    case ViewType::ThumbnailsLarge:
                                    case ViewType::ThumbnailsSmall:
                                    {
                                        const auto j = p.nameLines.find(index);
                                        if (j != p.nameLines.end())
                                        {
                                            float x = i->second.min.x + m;
                                            float y = i->second.max.y - p.nameFontMetrics.lineHeight * 2.f - m;
                                            size_t line = 0;
                                            auto k = j->second.begin();
                                            for (; k != j->second.end() && line < 2; ++k, ++line)
                                            {
                                                render->drawText(
                                                    k->text,
                                                    glm::vec2(
                                                        floorf(x + tw / 2.f - k->size.x / 2.f),
                                                        floorf(y + p.nameFontMetrics.ascender)));
                                                y += p.nameFontMetrics.lineHeight;
                                            }
                                        }
                                        break;
                                    }
                                    case ViewType::ListView:
                                    {
                                        float x = i->second.min.x + m + tw + s;
                                        float y = i->second.min.y + i->second.h() / 2.f - p.nameFontMetrics.lineHeight / 2.f;
                                        render->drawText(
                                            item->getFileName(Frame::Invalid, false),
                                            glm::vec2(
                                                floorf(x),
                                                floorf(y + p.nameFontMetrics.ascender)));
                                        break;
                                    }
                                    default: break;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            void ItemView::_pointerEnterEvent(Event::PointerEnter& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                const auto & pointerInfo = event.getPointerInfo();
                for (const auto & i : p.itemGeometry)
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
                const auto & pointerInfo = event.getPointerInfo();
                if (p.grab != invalid && pointerInfo.id == p.pressedId)
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float distance = glm::length(pointerInfo.projectedPos - p.pressedPos);
                        const bool accepted = distance < style->getMetric(MetricsRole::Drag);
                        event.setAccepted(accepted);
                        if (!accepted)
                        {
                            p.grab = invalid;
                            p.pressedId = Event::InvalidID;
                            _redraw();
                        }
                    }
                }
                else
                {
                    for (const auto & i : p.itemGeometry)
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
                const auto & pointerInfo = event.getPointerInfo();
                for (const auto & i : p.itemGeometry)
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
                const auto & pointerInfo = event.getPointerInfo();
                if (p.grab != invalid && pointerInfo.id == p.pressedId)
                {
                    event.accept();
                    p.grab = invalid;
                    p.pressedId = Event::InvalidID;
                    const auto & hover = _getPointerHover();
                    const auto i = hover.find(pointerInfo.id);
                    if (p.callback && i != hover.end())
                    {
                        for (const auto & j : p.itemGeometry)
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

            std::shared_ptr<Widget> ItemView::_createTooltip(const glm::vec2 & pos)
            {
                DJV_PRIVATE_PTR();
                std::string text;
                for (const auto & i : p.itemGeometry)
                {
                    if (i.second.contains(pos))
                    {
                        if (i.first < p.items.size())
                        {
                            auto context = getContext();
                            const auto & fileInfo = p.items[i.first];
                            const auto j = p.ioInfo.find(i.first);
                            if (j != p.ioInfo.end())
                            {
                                text = p.getTooltip(fileInfo, j->second, context);
                            }
                            else
                            {
                                text = p.getTooltip(fileInfo, context);
                            }
                        }
                        break;
                    }
                }
                return !text.empty() ? _createTooltipDefault(text) : nullptr;
            }

            void ItemView::_updateEvent(Event::Update& event)
            {
                DJV_PRIVATE_PTR();
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
                                _redraw();
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
                    auto i = p.thumbnailsFutures.begin();
                    while (i != p.thumbnailsFutures.end())
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
                                    p.thumbnailsTimers[i->first] = _getUpdateTime();
                                    _redraw();
                                }
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.thumbnailsFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
                if (p.thumbnailsTimers.size())
                {
                    const float ut = _getUpdateTime();
                    auto i = p.thumbnailsTimers.begin();
                    while (i != p.thumbnailsTimers.end())
                    {
                        if (ut - i->second > thumbnailsFadeTime)
                        {
                            i = p.thumbnailsTimers.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                    _redraw();
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
                                _redraw();
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
            }

            void ItemView::_iconsUpdate()
            {
                DJV_PRIVATE_PTR();
                p.icons.clear();
                if (auto style = _getStyle().lock())
                {
                    if (auto iconSystem = getContext()->getSystemT<IconSystem>().lock())
                    {
                        for (size_t i = 0; i < static_cast<size_t>(FileSystem::FileType::Count); ++i)
                        {
                            const auto type = static_cast<FileSystem::FileType>(i);
                            std::string name;
                            switch (type)
                            {
                            case FileSystem::FileType::Directory:
                                switch (p.viewType)
                                {
                                case ViewType::ThumbnailsLarge: name = "djvIconFileBrowserDirectoryLarge"; break;
                                case ViewType::ThumbnailsSmall: name = "djvIconFileBrowserDirectorySmall"; break;
                                case ViewType::ListView:        name = "djvIconFileBrowserDirectoryList";  break;
                                default: break;
                                }
                                break;
                            default:
                                switch (p.viewType)
                                {
                                case ViewType::ThumbnailsLarge: name = "djvIconFileBrowserFileLarge"; break;
                                case ViewType::ThumbnailsSmall: name = "djvIconFileBrowserFileSmall"; break;
                                case ViewType::ListView:        name = "djvIconFileBrowserFileList";  break;
                                default: break;
                                }
                                break;
                            }
                            _p->iconsFutures[type] = iconSystem->getIcon(name, static_cast<int>(style->getMetric(MetricsRole::Icon)));
                        }
                    }
                }
            }

            void ItemView::_itemsUpdate()
            {
				DJV_PRIVATE_PTR();
				if (auto style = _getStyle().lock())
                {
                    if (auto fontSystem = _getFontSystem().lock())
                    {
                        p.nameFontMetricsFuture = fontSystem->getMetrics(
                            style->getFontInfo(AV::Font::Info::faceDefault, MetricsRole::FontMedium));
                        p.typeFontMetricsFuture = fontSystem->getMetrics(
                            style->getFontInfo(AV::Font::Info::faceDefault, MetricsRole::FontMedium));
                        p.nameLines.clear();
                        p.nameLinesFutures.clear();
                        p.ioInfo.clear();
                        p.ioInfoFutures.clear();
                        p.thumbnails.clear();
                        p.thumbnailsFutures.clear();
                    }
                }
                _resize();
            }

			MetricsRole ItemView::Private::getThumbnailWidth() const
            {
                switch (viewType)
                {
                case ViewType::ThumbnailsLarge: return MetricsRole::ThumbnailWidthLarge;
                case ViewType::ThumbnailsSmall: return MetricsRole::ThumbnailWidthSmall;
                case ViewType::ListView: return MetricsRole::Icon;
                default: break;
                }
                return MetricsRole::None;
            }

			MetricsRole ItemView::Private::getThumbnailHeight() const
            {
                switch (viewType)
                {
                case ViewType::ThumbnailsLarge: return MetricsRole::ThumbnailHeightLarge;
                case ViewType::ThumbnailsSmall: return MetricsRole::ThumbnailHeightSmall;
                case ViewType::ListView: return MetricsRole::Icon;
                default: break;
                }
                return MetricsRole::None;
            }

            std::string ItemView::Private::getTooltip(const FileSystem::FileInfo & fileInfo, Context * context) const
            {
                std::stringstream ss;
                ss << fileInfo.getFileName(Frame::Invalid, false) << '\n';
                ss << '\n';
                ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Size")) << ": " << Memory::getSizeLabel(fileInfo.getSize()) << '\n';
                ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Last modification time")) << ": " << Time::getLabel(fileInfo.getTime());
                return ss.str();
            }

            std::string ItemView::Private::getTooltip(const FileSystem::FileInfo & fileInfo, const AV::IO::Info & avInfo, Context * context) const
            {
                std::stringstream ss;
                ss << getTooltip(fileInfo, context);
                size_t track = 0;
                for (const auto & videoInfo : avInfo.video)
                {
                    ss << '\n' << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Video track")) << " #" << track << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Size")) << ": " << videoInfo.info.size << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Type")) << ": " << videoInfo.info.type << '\n';
                    ss.precision(2);
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Speed")) << ": " <<
                        Time::Speed::speedToFloat(videoInfo.speed) <<
                        context->getText(DJV_TEXT("djv::UI::FileBrowser", "FPS")) << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Duration")) << ": " <<
                        Time::getLabel(Time::durationToSeconds(videoInfo.duration));
                    ++track;
                }
                track = 0;
                for (const auto & audioInfo : avInfo.audio)
                {
                    ss << '\n' << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Audio track")) << " #" << track << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Channels")) << ": " <<
                        audioInfo.info.channelCount << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Type")) << ": " <<
                        audioInfo.info.type << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Sample rate")) << ": " <<
                        audioInfo.info.sampleRate / 1000.f << DJV_TEXT("djv::UI::FileBrowser", "kHz") << '\n';
                    ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Duration")) << ": " <<
                        Time::getLabel(Time::durationToSeconds(audioInfo.duration));
                    ++track;
                }
                return ss.str();
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
