// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>
#include <djvUI/Margin.h>

#include <djvSystem/Event.h>
#include <djvSystem/IObject.h>

#include <djvMath/BBox.h>

#include <functional>
#include <memory>
#include <set>

namespace djv
{
    namespace Render2D
    {
        class Render;

    } // namespace Render

    namespace UI
    {
        class Action;
        class EventSystem;
        class ITooltipWidget;
        class Tooltip;
        class Window;

        //! Base class for widgets.
        class Widget : public System::IObject
        {
            DJV_NON_COPYABLE(Widget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            Widget();

        public:
            ~Widget() override;

            //! Create a new widget.
            static std::shared_ptr<Widget> create(const std::shared_ptr<System::Context>&);

            //! Get the top-level window.
            std::shared_ptr<Window> getWindow() const;

            //! \name Visibility
            ///@{

            bool isVisible(bool parents = false) const;

            //! \todo Should we have show/hide events instead of making this function
            //! virtual?
            virtual void setVisible(bool);
            void show();
            void hide();

            bool isClipped() const;
            const Math::BBox2f& getClipRect() const;

            float getOpacity(bool parents = false) const;

            void setOpacity(float);

            ///@}

            //! \name Layout
            ///@{

            const glm::vec2& getMinimumSize() const;
            const Math::BBox2f& getGeometry() const;
            glm::vec2 getSize() const;
            float getWidth() const;
            float getHeight() const;

            void setGeometry(const Math::BBox2f&);
            void move(const glm::vec2&);
            void resize(const glm::vec2&);

            //! This function is called during the layout event to determine the height of a
            //! widget based upon the given width. This is used for example by FlowLayout
            //! and TextBlock.
            virtual float getHeightForWidth(float) const;

            const Layout::Margin& getMargin() const;

            void setMargin(const Layout::Margin&);

            HAlign getHAlign() const;
            VAlign getVAlign() const;

            void setHAlign(HAlign);
            void setVAlign(VAlign);

            ///@}

            //! \name Style
            ///@{

            ColorRole getBackgroundColorRole() const;

            virtual void setBackgroundColorRole(ColorRole);

            const std::set<Side>& getShadowOverlay() const;

            void setShadowOverlay(const std::set<Side>&);

            ///@}

            //! \name Input
            ///@{

            bool isPointerEnabled() const;

            void setPointerEnabled(bool);

            virtual std::shared_ptr<Widget> getFocusWidget();

            virtual bool acceptFocus(TextFocusDirection);
            bool hasTextFocus();
            void takeTextFocus();
            void releaseTextFocus();
            virtual void nextTextFocus(const std::shared_ptr<Widget>&);
            virtual void prevTextFocus(const std::shared_ptr<Widget>&);

            ///@}

            //! \name Actions
            ///@{

            const std::vector<std::shared_ptr<Action> >& getActions() const;

            virtual void addAction(const std::shared_ptr<Action>&);
            virtual void removeAction(const std::shared_ptr<Action>&);
            virtual void clearActions();

            ///@}

            //! \name Tooltip
            ///@{

            const std::string& getTooltip() const;

            void setTooltip(const std::string&);

            ///@}

            //! Get the child widgets.
            const std::vector<std::shared_ptr<Widget> >& getChildWidgets() const;

            //! Get the number of widgets that currently exist.
            static size_t getGlobalWidgetCount();

            void moveToFront() override;
            void moveToBack() override;
            void setEnabled(bool) override;
            bool event(System::Event::Event&) override;

        protected:
            //! \name Events
            ///@{

            virtual void _initLayoutEvent(System::Event::InitLayout&) {}
            virtual void _preLayoutEvent(System::Event::PreLayout&) {}
            virtual void _layoutEvent(System::Event::Layout&) {}
            virtual void _clipEvent(System::Event::Clip&) {}
            virtual void _paintEvent(System::Event::Paint&);
            virtual void _paintOverlayEvent(System::Event::PaintOverlay&);
            virtual void _pointerEnterEvent(System::Event::PointerEnter&);
            virtual void _pointerLeaveEvent(System::Event::PointerLeave&);
            virtual void _pointerMoveEvent(System::Event::PointerMove&);
            virtual void _buttonPressEvent(System::Event::ButtonPress&) {}
            virtual void _buttonReleaseEvent(System::Event::ButtonRelease&) {}
            virtual void _scrollEvent(System::Event::Scroll&) {}
            virtual void _dropEvent(System::Event::Drop&) {}
            virtual void _keyPressEvent(System::Event::KeyPress&);
            virtual void _keyReleaseEvent(System::Event::KeyRelease&) {}
            virtual void _textFocusEvent(System::Event::TextFocus&) {}
            virtual void _textFocusLostEvent(System::Event::TextFocusLost&) {}
            virtual void _textInputEvent(System::Event::TextInput&) {}
            
            ///@}

            //! \name Convenience Functions
            ///@{

            const std::weak_ptr<EventSystem>& _getEventSystem() const;
            const std::shared_ptr<Render2D::Render>& _getRender() const;
            const std::shared_ptr<Style::Style>& _getStyle() const;

            ///@}

            //! Call this function when the widget needs resizing.
            void _resize();

            //! Call this function to redraw the widget.
            void _redraw();

            //! Set the minimum size. This is computed and set in the pre-layout event.
            void _setMinimumSize(const glm::vec2&);

            const std::chrono::steady_clock::time_point& _getUpdateTime();
            const std::map<System::Event::PointerID, glm::vec2> _getPointerHover() const;

            std::string _getTooltipText() const;
            std::shared_ptr<ITooltipWidget> _createTooltipDefault();
            virtual std::shared_ptr<ITooltipWidget> _createTooltip(const glm::vec2& pos);

        private:
            std::vector<std::shared_ptr<Widget> > _childWidgets;

            std::chrono::steady_clock::time_point _updateTime;

            bool                _visible             = true;
            bool                _visibleInit         = true;
            bool                _parentsVisible      = true;
            bool                _clipped             = false;
            Math::BBox2f        _clipRect            = Math::BBox2f(0.F, 0.F, 0.F, 0.F);
            float               _opacity             = 1.F;
            float               _parentsOpacity      = 1.F;

            Math::BBox2f        _geometry            = Math::BBox2f(0.F, 0.F, 0.F, 0.F);
            glm::vec2           _minimumSize         = glm::vec2(0.F, 0.F);
            Layout::Margin      _margin;
            HAlign              _hAlign              = HAlign::Fill;
            VAlign              _vAlign              = VAlign::Fill;

            ColorRole           _backgroundColorRole = ColorRole::None;
            std::set<Side>      _shadowOverlay;

            bool _pointerEnabled = false;
            std::map<System::Event::PointerID, glm::vec2> _pointerHover;

            std::vector<std::shared_ptr<Action> > _actions;

            std::string _tooltipText;
            struct TooltipData
            {
                std::chrono::steady_clock::time_point timer;
                std::shared_ptr<Tooltip> tooltip;
            };
            std::map<System::Event::PointerID, TooltipData> _pointerToTooltips;
            std::set<std::shared_ptr<Tooltip> > _tooltipsToDelete;

            std::weak_ptr<EventSystem> _eventSystem;
            std::shared_ptr<Render2D::Render> _render;
            std::shared_ptr<Style::Style> _style;

            friend class EventSystem;
        };

    } // namespace UI
} // namespace djv

#include <djvUI/WidgetInline.h>
