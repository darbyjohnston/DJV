// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace UI
    {
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

        inline const Math::BBox2f& Widget::getClipRect() const
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

        inline const Math::BBox2f& Widget::getGeometry() const
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

        inline ColorRole Widget::getBackgroundColorRole() const
        {
            return _backgroundColorRole;
        }

        inline const std::set<Side>& Widget::getShadowOverlay() const
        {
            return _shadowOverlay;
        }

        inline bool Widget::isPointerEnabled() const
        {
            return _pointerEnabled;
        }

        inline const std::vector<std::shared_ptr<Action> >& Widget::getActions() const
        {
            return _actions;
        }

        inline const std::string& Widget::getTooltip() const
        {
            return _tooltipText;
        }

        inline const std::vector<std::shared_ptr<Widget> >& Widget::getChildWidgets() const
        {
            return _childWidgets;
        }

        inline const std::weak_ptr<EventSystem>& Widget::_getEventSystem() const
        {
            return _eventSystem;
        }

        inline const std::shared_ptr<Render2D::Render>& Widget::_getRender() const
        {
            return _render;
        }

        inline const std::shared_ptr<Style::Style>& Widget::_getStyle() const
        {
            return _style;
        }

        inline const std::chrono::steady_clock::time_point& Widget::_getUpdateTime()
        {
            return _updateTime;
        }

        inline const std::map<System::Event::PointerID, glm::vec2> Widget::_getPointerHover() const
        {
            return _pointerHover;
        }

    } // namespace UI
} // namespace djv
