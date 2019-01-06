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

#include <djvUI/Icon.h>

#include <djvAV/IO.h>
#include <djvAV/IconSystem.h>
#include <djvAV/Image.h>
#include <djvAV/Render2DSystem.h>

#include <djvCore/Path.h>
#include <djvCore/UID.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Icon::Private
        {
            FileSystem::Path path;
            Style::ColorRole iconColorRole = Style::ColorRole::Foreground;
            std::future<AV::IO::Info> infoFuture;
            bool infoFutureRequest = false;
            std::future<std::shared_ptr<AV::Image::Image> > imageFuture;
            bool imageFutureRequest = false;
            AV::Image::Info info;
            std::shared_ptr<AV::Image::Image> image;
            UID uid = 0;
        };

        void Icon::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::UI::Icon");
        }

        Icon::Icon() :
            _p(new Private)
        {}

        Icon::~Icon()
        {}

        std::shared_ptr<Icon> Icon::create(Context * context)
        {
            auto out = std::shared_ptr<Icon>(new Icon);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Icon> Icon::create(const FileSystem::Path& path, Context * context)
        {
            auto out = Icon::create(context);
            out->setIcon(path);
            return out;
        }

        const FileSystem::Path& Icon::getIcon() const
        {
            return _p->path;
        }

        void Icon::setIcon(const FileSystem::Path& value)
        {
            if (value == _p->path)
                return;
            _p->path = value;
            if (auto system = _getIconSystem().lock())
            {
                _p->infoFuture = system->getInfo(_p->path);
                _p->infoFutureRequest = true;
                _p->imageFuture = system->getImage(_p->path);
                _p->imageFutureRequest = true;
            }
            _p->uid = _p->path.isEmpty() ? 0 : createUID();
            _resize();
        }

        Style::ColorRole Icon::getIconColorRole() const
        {
            return _p->iconColorRole;
        }

        void Icon::setIconColorRole(Style::ColorRole value)
        {
            _p->iconColorRole = value;
        }

        void Icon::_preLayoutEvent(Event::PreLayout& event)
        {
            if (auto style = _getStyle().lock())
            {
                if (_p->infoFutureRequest)
                {
                    _p->infoFutureRequest = false;
                    try
                    {
                        const auto info = _p->infoFuture.get();
                        if (info.video.size())
                        {
                            _p->info = info.video.front().info;
                            _resize();
                        }
                    }
                    catch (const std::exception & e)
                    {
                        _p->info = AV::Image::Info();
                        _log(e.what(), LogLevel::Error);
                    }
                }
                _setMinimumSize(glm::vec2(_p->info.size) + getMargin().getSize(style));
            }
        }

        void Icon::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            if (auto render = _getRenderSystem().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);
                    const glm::vec2 c = g.getCenter();

                    // Draw the icon.
                    if (_p->imageFutureRequest)
                    {
                        _p->imageFutureRequest = false;
                        try
                        {
                            _p->image = _p->imageFuture.get();
                            _redraw();
                        }
                        catch (const std::exception & e)
                        {
                            _p->image = nullptr;
                            _log(e.what(), LogLevel::Error);
                        }
                    }
                    if (_p->image && _p->image->isValid())
                    {
                        const glm::vec2& size = _p->info.size;
                        glm::vec2 pos = glm::vec2(0.f, 0.f);
                        switch (getHAlign())
                        {
                        case HAlign::Center:
                        case HAlign::Fill:   pos.x = c.x - size.x / 2.f; break;
                        case HAlign::Left:   pos.x = g.min.x; break;
                        case HAlign::Right:  pos.x = g.max.x - size.x; break;
                        default: break;
                        }
                        switch (getVAlign())
                        {
                        case VAlign::Center:
                        case VAlign::Fill:   pos.y = c.y - size.y / 2.f; break;
                        case VAlign::Top:    pos.y = g.min.y; break;
                        case VAlign::Bottom: pos.y = g.max.y - size.y; break;
                        default: break;
                        }
                        if (_p->iconColorRole != Style::ColorRole::None)
                        {
                            render->setFillColor(_getColorWithOpacity(style->getColor(_p->iconColorRole)));
                            render->drawFilledImage(_p->image, pos, false, _p->uid);
                        }
                        else
                        {
                            render->setFillColor(_getColorWithOpacity(AV::Image::Color(1.f, 1.f, 1.f)));
                            render->drawImage(_p->image, pos, false, _p->uid);
                        }
                    }
                }
            }
        }

    } // namespace UI
} // namespace djv
