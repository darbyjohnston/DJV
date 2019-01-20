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

#include <djvAV/IO.h>
#include <djvAV/IconSystem.h>
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
                std::vector<FileSystem::FileInfo> items;
                AV::Font::Metrics fontMetrics;
                std::future<AV::Font::Metrics> fontMetricsFuture;
                std::map<size_t, BBox2f> itemGeometry;
                std::map<FileSystem::FileType, std::shared_ptr<AV::Image::Image> > icons;
                std::map<FileSystem::FileType, std::future<std::shared_ptr<AV::Image::Image> > > iconsFutures;
                std::map<size_t, std::vector<AV::Font::TextLine> > breakLines;
                std::map<size_t, std::future<std::vector<AV::Font::TextLine> > > breakLinesFutures;
                std::map<size_t, AV::IO::Info> ioInfo;
                std::map<size_t, std::future<AV::IO::Info> > ioInfoFutures;
                std::map<size_t, std::shared_ptr<AV::Image::Image> > thumbnails;
                std::map<size_t, std::future<std::shared_ptr<AV::Image::Image> > > thumbnailsFutures;
                std::map<size_t, float> thumbnailsTimers;
                size_t hover = invalid;
                size_t grab = invalid;
                Event::PointerID pressedId = Event::InvalidID;
                glm::vec2 pressedPos = glm::vec2(0.f, 0.f);
                std::function<void(const FileSystem::FileInfo &)> callback;

                std::string getTooltip(const FileSystem::FileInfo &, Context * context);
                std::string getTooltip(const FileSystem::FileInfo &, const AV::IO::Info &, Context *);
            };

            void ItemView::_init(Context * context)
            {
                UI::Widget::_init(context);

                setClassName("djv::UI::FileBrowser::ItemView");

                DJV_PRIVATE_PTR();

                auto weak = std::weak_ptr<ItemView>(std::dynamic_pointer_cast<ItemView>(shared_from_this()));
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
                float out = 0.f;
                DJV_PRIVATE_PTR();
                if (const size_t itemCount = p.items.size())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float m = style->getMetric(Style::MetricsRole::Margin);
                        const float ms = style->getMetric(Style::MetricsRole::MarginSmall);
                        const float s = style->getMetric(Style::MetricsRole::Spacing);
                        const float ts = style->getMetric(Style::MetricsRole::ThumbnailLarge);
                        size_t columns = 1;
                        float x = m + ts;
                        while (x < value - ts - m)
                        {
                            ++columns;
                            x += s + ts;
                        }
                        const size_t rows = itemCount / columns + (itemCount % columns ? 1 : 0);
                        out = (ts / 2.f + p.fontMetrics.lineHeight * 2.f + ms * 2.f) * rows + s * (rows - 1) + m * 2.f;
                    }
                }
                return out;
            }

            void ItemView::_styleEvent(Event::Style&)
            {
                _itemsUpdate();
            }

            void ItemView::_preLayoutEvent(Event::PreLayout& event)
            {
                DJV_PRIVATE_PTR();
                if (p.fontMetricsFuture.valid())
                {
                    try
                    {
                        p.fontMetrics = p.fontMetricsFuture.get();
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                }
            }

            void ItemView::_layoutEvent(Event::Layout& event)
            {
                if (auto style = _getStyle().lock())
                {
                    DJV_PRIVATE_PTR();
                    const BBox2f & g = getGeometry();
                    const float m = style->getMetric(Style::MetricsRole::Margin);
                    const float ms = style->getMetric(Style::MetricsRole::MarginSmall);
                    const float s = style->getMetric(Style::MetricsRole::Spacing);
                    const float ts = style->getMetric(Style::MetricsRole::ThumbnailLarge);
                    const float itemHeight = ts / 2.f + p.fontMetrics.lineHeight * 2.f + ms * 2.f;
                    p.itemGeometry.clear();
                    glm::vec2 pos = g.min + m;
                    auto item = p.items.begin();
                    size_t i = 0;
                    for (; item != p.items.end(); ++item, ++i)
                    {
                        p.itemGeometry[i] = BBox2f(pos.x, pos.y, ts, itemHeight);
                        pos.x += ts;
                        if (pos.x < g.max.x - ts - m)
                        {
                            pos.x += s;
                        }
                        else
                        {
                            pos.x = g.min.x + m;
                            pos.y += s + itemHeight;
                        }
                    }
                }
            }

            void ItemView::_clipEvent(Event::Clip& event)
            {
                DJV_PRIVATE_PTR();
                auto context = getContext();
                auto style = _getStyle().lock();
                auto fontSystem = _getFontSystem().lock();
                const auto & clipRect = event.getClipRect();
                for (const auto & i : p.itemGeometry)
                {
                    if (i.second.intersects(clipRect))
                    {
                        {
                            const auto j = p.breakLines.find(i.first);
                            if (j == p.breakLines.end())
                            {
                                const auto k = p.breakLinesFutures.find(i.first);
                                if (k == p.breakLinesFutures.end())
                                {
                                    if (i.first < p.items.size())
                                    {
                                        const auto & fileInfo = p.items[i.first];
                                        const float ts = style->getMetric(Style::MetricsRole::ThumbnailLarge);
                                        const float ms = style->getMetric(Style::MetricsRole::MarginSmall);
                                        const auto fontInfo = style->getFontInfo(AV::Font::Info::faceDefault, Style::MetricsRole::FontMedium);
                                        p.breakLinesFutures[i.first] = fontSystem->breakLines(
                                            fileInfo.getFileName(Frame::Invalid, false), ts - ms * 2.f, fontInfo);
                                    }
                                }
                            }
                        }
                        {
                            const auto j = p.ioInfo.find(i.first);
                            if (j == p.ioInfo.end())
                            {
                                const auto k = p.ioInfoFutures.find(i.first);
                                if (k == p.ioInfoFutures.end())
                                {
                                    if (i.first < p.items.size())
                                    {
                                        const auto & fileInfo = p.items[i.first];
                                        auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>().lock();
                                        auto iconSystem = context->getSystemT<AV::Image::IconSystem>().lock();
                                        auto ioSystem = context->getSystemT<AV::IO::System>().lock();
                                        if (thumbnailSystem && iconSystem && ioSystem)
                                        {
                                            if (ioSystem->canRead(fileInfo))
                                            {
                                                p.ioInfoFutures[i.first] = thumbnailSystem->getInfo(fileInfo.getPath());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            void ItemView::_paintEvent(Event::Paint& event)
            {
                DJV_PRIVATE_PTR();
                if (const size_t itemCount = p.items.size())
                {
                    if (auto render = _getRender().lock())
                    {
                        if (auto style = _getStyle().lock())
                        {
                            const BBox2f & g = getGeometry();
                            const float ts = style->getMetric(Style::MetricsRole::ThumbnailLarge);
                            const float ms = style->getMetric(Style::MetricsRole::MarginSmall);
                            {
                                const auto i = p.itemGeometry.find(p.grab);
                                if (i != p.itemGeometry.end())
                                {
                                    render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Checked)));
                                    render->drawRect(i->second);
                                }
                            }
                            {
                                const auto i = p.itemGeometry.find(p.hover);
                                if (i != p.itemGeometry.end())
                                {
                                    render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Hover)));
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
                                    bool thumbnail = false;
                                    {
                                        const auto j = p.thumbnails.find(index);
                                        if (j != p.thumbnails.end())
                                        {
                                            float opacity = 1.f;
                                            const auto k = p.thumbnailsTimers.find(index);
                                            if (k != p.thumbnailsTimers.end())
                                            {
                                                opacity = std::min((ut - k->second) / thumbnailsFadeTime, 1.f);
                                            }
                                            thumbnail = true;
                                            const auto & size = j->second->getSize();
                                            float x = floor(i->second.min.x + ts / 2.f - size.x / 2.f);
                                            float y = floor(i->second.min.y + ts / 2.f - size.y);
                                            render->setFillColor(_getColorWithOpacity(AV::Image::Color(1.f, 1.f, 1.f, opacity)));
                                            render->drawImage(
                                                j->second,
                                                BBox2f(x, y, static_cast<float>(size.x), static_cast<float>(size.y)),
                                                AV::Render::ImageCache::Static);
                                        }
                                    }
                                    if (!thumbnail)
                                    {
                                        const auto j = p.icons.find(item->getType());
                                        if (j != p.icons.end())
                                        {
                                            const auto & size = j->second->getSize();
                                            float x = floor(i->second.min.x + ts / 2.f - size.x / 2.f);
                                            float y = floor(i->second.min.y + ts / 2.f - size.y);
                                            render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Button)));
                                            render->drawFilledImage(
                                                j->second,
                                                BBox2f(x, y, static_cast<float>(size.x), static_cast<float>(size.y)),
                                                AV::Render::ImageCache::Static);
                                        }
                                    }
                                    {
                                        const auto j = p.breakLines.find(index);
                                        if (j != p.breakLines.end())
                                        {
                                            float x = i->second.min.x;
                                            float y = i->second.max.y - p.fontMetrics.lineHeight * 2.f - ms;
                                            size_t line = 0;
                                            auto k = j->second.begin();
                                            render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Foreground)));
                                            render->setCurrentFont(style->getFontInfo(AV::Font::Info::faceDefault, Style::MetricsRole::FontMedium));
                                            for (; k != j->second.end() && line < 2; ++k, ++line)
                                            {
                                                render->drawText(k->text, glm::vec2(floorf(x + ts / 2.f - k->size.x / 2.f), y + p.fontMetrics.ascender));
                                                y += p.fontMetrics.lineHeight;
                                            }
                                        }
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
                        const bool accepted = distance < style->getMetric(Style::MetricsRole::Drag);
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

            void ItemView::_updateEvent(Event::Update& event)
            {
                DJV_PRIVATE_PTR();
                {
                    auto i = p.iconsFutures.begin();
                    while (i != p.iconsFutures.end())
                    {
                        if (i->second.valid())
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
                {
                    auto i = p.breakLinesFutures.begin();
                    while (i != p.breakLinesFutures.end())
                    {
                        if (i->second.valid())
                        {
                            try
                            {
                                p.breakLines[i->first] = i->second.get();
                                _redraw();
                            }
                            catch (const std::exception& e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.breakLinesFutures.erase(i);
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
                        if (i->second.valid() &&
                            i->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                const auto ioInfo = i->second.get();
                                p.ioInfo[i->first] = ioInfo;
                                auto context = getContext();
                                auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>().lock();
                                auto style = _getStyle().lock();
                                if (thumbnailSystem && style && i->first < p.items.size())
                                {
                                    const float ts = style->getMetric(Style::MetricsRole::ThumbnailLarge);
                                    glm::vec2 size(ts, ts / 2.f);
                                    if (ioInfo.video.size())
                                    {
                                        const glm::ivec2 & videoSize = ioInfo.video[0].info.size;
                                        const float aspect = Vector::getAspect(videoSize);
                                        if (videoSize.x >= videoSize.y)
                                        {
                                            size.y = ts / aspect;
                                            if (size.y > ts / 2.f)
                                            {
                                                const float r = (ts / 2.f) / size.y;
                                                size *= r;
                                            }
                                        }
                                        else
                                        {
                                            size.x = ts * aspect;
                                            if (size.x > ts)
                                            {
                                                const float r = ts / size.x;
                                                size.y *= r;
                                            }
                                        }
                                    }
                                    p.thumbnailsFutures[i->first] = thumbnailSystem->getImage(
                                        p.items[i->first].getPath(),
                                        glm::ivec2(static_cast<int>(ceilf(size.x)), static_cast<int>(ceilf(size.y))));
                                }
                            }
                            catch (const std::exception& e)
                            {
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
                        if (i->second.valid() &&
                            i->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                p.thumbnails[i->first] = i->second.get();
                                p.thumbnailsTimers[i->first] = _getUpdateTime();
                                _redraw();
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
            }

            void ItemView::_itemsUpdate()
            {
                if (auto style = _getStyle().lock())
                {
                    if (auto fontSystem = _getFontSystem().lock())
                    {
                        DJV_PRIVATE_PTR();

                        const auto fontInfo = style->getFontInfo(AV::Font::Info::faceDefault, Style::MetricsRole::FontMedium);
                        p.fontMetricsFuture = fontSystem->getMetrics(fontInfo);

                        auto context = getContext();
                        if (auto iconSystem = context->getSystemT<AV::Image::IconSystem>().lock())
                        {
                            const int size = static_cast<int>(style->getMetric(Style::MetricsRole::Icon));
                            p.iconsFutures[FileSystem::FileType::File] = iconSystem->getImage("djvIconFileBrowserFileLarge", size);
                            p.iconsFutures[FileSystem::FileType::Sequence] = iconSystem->getImage("djvIconFileBrowserFileLarge", size);
                            p.iconsFutures[FileSystem::FileType::Directory] = iconSystem->getImage("djvIconFileBrowserDirectoryLarge", size);
                        }

                        const size_t itemCount = p.items.size();
                        p.breakLines.clear();
                        p.breakLinesFutures.clear();
                        p.ioInfo.clear();
                        p.ioInfoFutures.clear();
                        p.thumbnails.clear();
                        p.thumbnailsFutures.clear();
                    }
                }
                _resize();
            }

            std::string ItemView::Private::getTooltip(const FileSystem::FileInfo & fileInfo, Context * context)
            {
                std::stringstream ss;
                ss << fileInfo.getFileName(Frame::Invalid, false) << '\n';
                ss << '\n';
                ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Type")) << ": " << fileInfo.getType() << '\n';
                ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Size")) << ": " << Memory::getSizeLabel(fileInfo.getSize()) << '\n';
                ss << context->getText(DJV_TEXT("djv::UI::FileBrowser", "Modification time")) << ": " << Time::getLabel(fileInfo.getTime());
                return ss.str();
            }

            std::string ItemView::Private::getTooltip(const FileSystem::FileInfo & fileInfo, const AV::IO::Info & avInfo, Context * context)
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
