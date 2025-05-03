// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/FilesToolPrivate.h>

#include <tlTimelineUI/ThumbnailSystem.h>

#include <dtk/ui/DrawUtil.h>
#include <dtk/core/Context.h>

#include <optional>

namespace djv
{
    namespace app
    {
        struct FileButton::Private
        {
            std::shared_ptr<FilesModelItem> item;

            struct SizeData
            {
                std::optional<float> displayScale;
                int margin = 0;
                int spacing = 0;
                int border = 0;
                dtk::FontInfo fontInfo;
                dtk::FontMetrics fontMetrics;
                dtk::Size2I textSize;
            };
            SizeData size;

            struct ThumbnailData
            {
                bool init = true;
                float scale = 1.F;
                int height = 40;
                tl::timelineui::ThumbnailRequest request;
                std::shared_ptr<dtk::Image> image;
            };
            ThumbnailData thumbnail;

            struct DrawData
            {
                std::vector<std::shared_ptr<dtk::Glyph> > glyphs;
            };
            std::optional<DrawData> draw;
        };

        void FileButton::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<FilesModelItem>& item,
            const std::shared_ptr<IWidget>& parent)
        {
            IButton::_init(context, "tl::play_app::FileButton", parent);
            DTK_P();
            const std::string s = dtk::elide(item->path.get(-1, tl::file::PathType::FileName));
            setText(s);
            setCheckable(true);
            setHStretch(dtk::Stretch::Expanding);
            setAcceptsKeyFocus(true);
            _buttonRole = dtk::ColorRole::None;
            p.item = item;
        }

        FileButton::FileButton() :
            _p(new Private)
        {}

        FileButton::~FileButton()
        {}

        std::shared_ptr<FileButton> FileButton::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<FilesModelItem>& item,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<FileButton>(new FileButton);
            out->_init(context, item, parent);
            return out;
        }

        void FileButton::tickEvent(
            bool parentsVisible,
            bool parentsEnabled,
            const dtk::TickEvent& event)
        {
            IWidget::tickEvent(parentsVisible, parentsEnabled, event);
            DTK_P();
            if (p.thumbnail.request.future.valid() &&
                p.thumbnail.request.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                p.thumbnail.image = p.thumbnail.request.future.get();
                _setSizeUpdate();
                _setDrawUpdate();
            }
        }

        void FileButton::sizeHintEvent(const dtk::SizeHintEvent& event)
        {
            IButton::sizeHintEvent(event);
            DTK_P();

            if (!p.size.displayScale.has_value() ||
                (p.size.displayScale.has_value() && p.size.displayScale.value() != event.displayScale))
            {
                p.size.displayScale = event.displayScale;
                p.size.margin = event.style->getSizeRole(dtk::SizeRole::MarginInside, event.displayScale);
                p.size.spacing = event.style->getSizeRole(dtk::SizeRole::SpacingSmall, event.displayScale);
                p.size.border = event.style->getSizeRole(dtk::SizeRole::Border, event.displayScale);
                p.size.fontInfo = event.style->getFontRole(_fontRole, event.displayScale);
                p.size.fontMetrics = event.fontSystem->getMetrics(p.size.fontInfo);
                p.size.textSize = event.fontSystem->getSize(_text, p.size.fontInfo);
                p.draw.reset();
            }

            if (event.displayScale != p.thumbnail.scale)
            {
                p.thumbnail.init = true;
                p.thumbnail.scale = event.displayScale;
                p.thumbnail.height = 40 * event.displayScale;
            }
            if (p.thumbnail.init)
            {
                p.thumbnail.init = false;
                if (auto context = getContext())
                {
                    auto thumbnailSystem = context->getSystem<tl::timelineui::ThumbnailSystem>();
                    p.thumbnail.request = thumbnailSystem->getThumbnail(
                        p.item->path,
                        p.thumbnail.height);
                }
            }

            dtk::Size2I thumbnailSize;
            if (p.thumbnail.image)
            {
                const dtk::Size2I& size = p.thumbnail.image->getSize();
                thumbnailSize = dtk::Size2I(size.w * p.thumbnail.image->getInfo().pixelAspectRatio, size.h);
            }
            dtk::Size2I sizeHint;
            sizeHint.w =
                thumbnailSize.w +
                p.size.spacing +
                p.size.textSize.w + p.size.margin * 2 +
                p.size.margin * 2 +
                p.size.border * 4;
            sizeHint.h =
                std::max(p.size.fontMetrics.lineHeight, thumbnailSize.h) +
                p.size.margin * 2 +
                p.size.border * 4;
            _setSizeHint(sizeHint);
        }

        void FileButton::clipEvent(const dtk::Box2I& clipRect, bool clipped)
        {
            IButton::clipEvent(clipRect, clipped);
            DTK_P();
            if (clipped)
            {
                p.draw.reset();
            }
        }

        void FileButton::drawEvent(
            const dtk::Box2I& drawRect,
            const dtk::DrawEvent& event)
        {
            IButton::drawEvent(drawRect, event);
            DTK_P();

            if (!p.draw.has_value())
            {
                p.draw = Private::DrawData();
            }

            const dtk::Box2I& g = getGeometry();
            const bool enabled = isEnabled();

            if (hasKeyFocus())
            {
                event.render->drawMesh(
                    dtk::border(g, p.size.border * 2),
                    event.style->getColorRole(dtk::ColorRole::KeyFocus));
            }

            const dtk::Box2I g2 = dtk::margin(g, -p.size.border * 2);
            if (isChecked())
            {
                event.render->drawRect(
                    g2,
                    event.style->getColorRole(dtk::ColorRole::Checked));
            }
            if (_isMousePressed() && dtk::contains(g, _getMousePos()))
            {
                event.render->drawRect(
                    g2,
                    event.style->getColorRole(dtk::ColorRole::Pressed));
            }
            else if (_isMouseInside())
            {
                event.render->drawRect(
                    g2,
                    event.style->getColorRole(dtk::ColorRole::Hover));
            }

            const dtk::Box2I g3 = dtk::margin(g2, -p.size.margin);
            int x = g3.min.x;
            if (p.thumbnail.image)
            {
                const dtk::Size2I& size = p.thumbnail.image->getSize();
                const dtk::Size2I thumbnailSize(size.w * p.thumbnail.image->getInfo().pixelAspectRatio, size.h);
                event.render->drawImage(
                    p.thumbnail.image,
                    dtk::Box2I(x, g3.y(), thumbnailSize.w, thumbnailSize.h));
                x += thumbnailSize.w + p.size.spacing;
            }

            if (!_text.empty())
            {
                if (p.draw->glyphs.empty())
                {
                    p.draw->glyphs = event.fontSystem->getGlyphs(_text, p.size.fontInfo);
                }
                const dtk::V2I pos(
                    x + p.size.margin,
                    g3.y() + g3.h() / 2 - p.size.textSize.h / 2);
                event.render->drawText(
                    p.draw->glyphs,
                    p.size.fontMetrics,
                    pos,
                    event.style->getColorRole(dtk::ColorRole::Text));
            }
        }

        void FileButton::keyPressEvent(dtk::KeyEvent& event)
        {
            DTK_P();
            if (0 == event.modifiers)
            {
                switch (event.key)
                {
                case dtk::Key::Enter:
                    event.accept = true;
                    click();
                    break;
                case dtk::Key::Escape:
                    if (hasKeyFocus())
                    {
                        event.accept = true;
                        releaseKeyFocus();
                    }
                    break;
                default: break;
                }
            }
        }

        void FileButton::keyReleaseEvent(dtk::KeyEvent& event)
        {
            event.accept = true;
        }
    }
}
