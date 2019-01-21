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

namespace djv
{
    namespace UI
    {
        inline Widget::Widget()
        {}

        inline bool Widget::isVisible(bool parents) const
        {
            return parents ? (_parentsVisible && _visible) : _visible;
        }

        inline bool Widget::isClipped() const
        {
            return _clipped;
        }

        inline float Widget::getOpacity(bool parents) const
        {
            return parents ? (_opacity * _parentsOpacity) : _opacity;
        }

        inline const glm::vec2& Widget::getMinimumSize() const
        {
            return _minimumSize;
        }

        inline const Core::BBox2f& Widget::getGeometry() const
        {
            return _geometry;
        }

        inline glm::vec2 Widget::getSize() const
        {
            return _geometry.getSize();
        }

        inline float Widget::getWidth() const
        {
            return _geometry.w();
        }

        inline float Widget::getHeight() const
        {
            return _geometry.h();
        }

        inline float Widget::getHeightForWidth(float) const
        {
            return getMinimumSize().y;
        }

        inline const Layout::Margin& Widget::getMargin() const
        {
            return _margin;
        }

        inline HAlign Widget::getHAlign() const
        {
            return _hAlign;
        }

        inline VAlign Widget::getVAlign() const
        {
            return _vAlign;
        }

        inline Style::ColorRole Widget::getBackgroundRole() const
        {
            return _backgroundRole;
        }

        inline Style::MetricsRole Widget::getRoundedCorners() const
        {
            return _roundedCorners;
        }

        inline AV::Side Widget::getRoundedCornersSide() const
        {
            return _roundedCornersSide;
        }

        inline bool Widget::isPointerEnabled() const
        {
            return _pointerEnabled;
        }

        inline const std::vector<std::shared_ptr<Action> > & Widget::getActions() const
        {
            return _actions;
        }

        inline const std::weak_ptr<AV::Font::System>& Widget::_getFontSystem() const
        {
            return _fontSystem;
        }

        inline const std::weak_ptr<AV::Render::Render2D>& Widget::_getRender() const
        {
            return _render;
        }

        inline const std::weak_ptr<IconSystem>& Widget::_getIconSystem() const
        {
            return _iconSystem;
        }

        inline const std::weak_ptr<Style::Style>& Widget::_getStyle() const
        {
            return _style;
        }

        inline const std::string & Widget::getTooltip() const
        {
            return _tooltipText;
        }

        inline float Widget::_getUpdateTime() const
        {
            return _updateTime;
        }

        inline float Widget::_getElapsedTime() const
        {
            return _elapsedTime;
        }

        inline const std::map<Core::Event::PointerID, glm::vec2> Widget::_getPointerHover() const
        {
            return _pointerHover;
        }

    } // namespace UI
} // namespace djv

