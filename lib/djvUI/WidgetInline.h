//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

        inline void Widget::show()
        {
            setVisible(true);
        }

        inline void Widget::hide()
        {
            setVisible(false);
        }

        inline bool Widget::isClipped() const
        {
            return _clipped;
        }

        inline const Core::BBox2f & Widget::getClipRect() const
        {
            return _clipRect;
        }

        inline float Widget::getOpacity(bool parents) const
        {
            return parents ? (_opacity * _parentsOpacity) : _opacity;
        }

        inline const glm::vec2& Widget::getMinimumSize() const
        {
            return _minimumSize;
        }

        inline const glm::vec2& Widget::getDesiredSize() const
        {
            return _desiredSize;
        }

        inline const Core::BBox2f & Widget::getGeometry() const
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

        inline void Widget::move(const glm::vec2& value)
        {
            const glm::vec2 size = _geometry.getSize();
            Core::BBox2f geometry;
            geometry.min = value;
            geometry.max = value + size;
            setGeometry(geometry);
        }

        inline void Widget::resize(const glm::vec2& value)
        {
            setGeometry(Core::BBox2f(_geometry.min, _geometry.min + value));
        }

        inline float Widget::getHeightForWidth(float) const
        {
            return getMinimumSize().y;
        }

        inline const Layout::Margin & Widget::getMargin() const
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

        inline ColorRole Widget::getBackgroundRole() const
        {
            return _backgroundRole;
        }

        inline const std::vector<Side>& Widget::getShadowOverlay() const
        {
            return _shadowOverlay;
        }

        inline bool Widget::isPointerEnabled() const
        {
            return _pointerEnabled;
        }

        inline const std::vector<std::shared_ptr<Action> > & Widget::getActions() const
        {
            return _actions;
        }

        inline const std::string& Widget::getTooltip() const
        {
            return _tooltipText;
        }

        inline bool Widget::areTooltipsEnabled()
        {
            return _tooltipsEnabled;
        }

        inline const std::vector<std::shared_ptr<Widget> >& Widget::getChildWidgets() const
        {
            return _childWidgets;
        }

        inline const std::weak_ptr<EventSystem>& Widget::_getEventSystem() const
        {
            return _eventSystem;
        }

        inline const std::shared_ptr<AV::Render2D::Render> & Widget::_getRender() const
        {
            return _render;
        }

        inline const std::shared_ptr<Style::Style> & Widget::_getStyle() const
        {
            return _style;
        }

        inline void Widget::_redraw()
        {
            _redrawRequest = true;
        }

        inline void Widget::_resize()
        {
            _resizeRequest = true;
        }

        inline const std::chrono::steady_clock::time_point& Widget::_getUpdateTime()
        {
            return _updateTime;
        }

        inline const std::map<Core::Event::PointerID, glm::vec2> Widget::_getPointerHover() const
        {
            return _pointerHover;
        }

    } // namespace UI
} // namespace djv

