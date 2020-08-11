// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>
#include <djvUI/Margin.h>

#include <djvAV/Enum.h>

#include <djvCore/BBox.h>
#include <djvCore/Event.h>
#include <djvCore/IObject.h>

#include <functional>
#include <memory>

namespace djv
{
    namespace AV
    {
        namespace Render2D
        {
            class Render;

        } // namespace Render

    } // namespace AV

    namespace UI
    {
        class Action;
        class EventSystem;
        class ITooltipWidget;
        class Tooltip;
        class Window;

        //! This class provides the base widget functionality.
        class Widget : public Core::IObject
        {
            DJV_NON_COPYABLE(Widget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            Widget();

        public:
            ~Widget() override;

            //! Create a new widget.
            static std::shared_ptr<Widget> create(const std::shared_ptr<Core::Context>&);

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
            const Core::BBox2f& getClipRect() const;

            float getOpacity(bool parents = false) const;
            void setOpacity(float);

            ///@}

            //! \name Layout
            ///@{

            const glm::vec2& getMinimumSize() const;
            const glm::vec2& getDesiredSize() const;
            const Core::BBox2f& getGeometry() const;
            glm::vec2 getSize() const;
            float getWidth() const;
            float getHeight() const;
            void setGeometry(const Core::BBox2f&);
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

            //! Utility function for computing widget geometry.
            static Core::BBox2f getAlign(
                const Core::BBox2f&,
                const glm::vec2& minimumSize,
                HAlign,
                VAlign);

            ///@}

            //! \name Style
            ///@{

            ColorRole getBackgroundRole() const;
            virtual void setBackgroundRole(ColorRole);

            const std::vector<Side>& getShadowOverlay() const;
            void setShadowOverlay(const std::vector<Side>&);

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

            static bool areTooltipsEnabled();
            static void setTooltipsEnabled(bool);

            ///@}

            //! Get the child widgets.
            const std::vector<std::shared_ptr<Widget> >& getChildWidgets() const;

            //! Get the number of widgets that currently exist.
            static size_t getGlobalWidgetCount();

            void moveToFront() override;
            void moveToBack() override;
            void setEnabled(bool) override;
            bool event(Core::Event::Event&) override;

        protected:
            //! \name Events
            ///@{

            virtual void _initLayoutEvent(Core::Event::InitLayout&) {}
            virtual void _preLayoutEvent(Core::Event::PreLayout&) {}
            virtual void _layoutEvent(Core::Event::Layout&) {}
            virtual void _clipEvent(Core::Event::Clip&) {}
            virtual void _paintEvent(Core::Event::Paint&);
            virtual void _paintOverlayEvent(Core::Event::PaintOverlay&);
            virtual void _pointerEnterEvent(Core::Event::PointerEnter&);
            virtual void _pointerLeaveEvent(Core::Event::PointerLeave&);
            virtual void _pointerMoveEvent(Core::Event::PointerMove&);
            virtual void _buttonPressEvent(Core::Event::ButtonPress&) {}
            virtual void _buttonReleaseEvent(Core::Event::ButtonRelease&) {}
            virtual void _scrollEvent(Core::Event::Scroll&) {}
            virtual void _dropEvent(Core::Event::Drop&) {}
            virtual void _keyPressEvent(Core::Event::KeyPress&);
            virtual void _keyReleaseEvent(Core::Event::KeyRelease&) {}
            virtual void _textFocusEvent(Core::Event::TextFocus&) {}
            virtual void _textFocusLostEvent(Core::Event::TextFocusLost&) {}
            virtual void _textInputEvent(Core::Event::TextInput&) {}
            
            ///@}

            //! \name Convenience Functions
            ///@{

            const std::weak_ptr<EventSystem>& _getEventSystem() const;
            const std::shared_ptr<AV::Render2D::Render>& _getRender() const;
            const std::shared_ptr<Style::Style>& _getStyle() const;

            ///@}

            //! Call this function when the widget needs resizing.
            void _resize();

            //! Call this function to redraw the widget.
            void _redraw();

            //! Set the minimum size. This is computed and set in the pre-layout event.
            void _setMinimumSize(const glm::vec2&);

            //! Set the desired size. This is computed and set in the pre-layout event.
            void _setDesiredSize(const glm::vec2&);

            static const std::chrono::steady_clock::time_point& _getUpdateTime();
            const std::map<Core::Event::PointerID, glm::vec2> _getPointerHover() const;

            std::string _getTooltipText() const;
            std::shared_ptr<ITooltipWidget> _createTooltipDefault();
            virtual std::shared_ptr<ITooltipWidget> _createTooltip(const glm::vec2& pos);

        private:
            std::vector<std::shared_ptr<Widget> > _childWidgets;

            static std::chrono::steady_clock::time_point _updateTime;

            bool                _visible         = true;
            bool                _visibleInit     = true;
            bool                _parentsVisible  = true;
            bool                _clipped         = false;
            Core::BBox2f        _clipRect        = Core::BBox2f(0.F, 0.F, 0.F, 0.F);
            float               _opacity         = 1.F;
            float               _parentsOpacity  = 1.F;

            Core::BBox2f        _geometry        = Core::BBox2f(0.F, 0.F, 0.F, 0.F);
            glm::vec2           _minimumSize     = glm::vec2(0.F, 0.F);
            glm::vec2           _desiredSize     = glm::vec2(0.F, 0.F);
            Layout::Margin      _margin;
            HAlign              _hAlign          = HAlign::Fill;
            VAlign              _vAlign          = VAlign::Fill;

            ColorRole           _backgroundRole  = ColorRole::None;
            std::vector<Side>   _shadowOverlay;

            bool                _pointerEnabled  = false;
            std::map<Core::Event::PointerID, glm::vec2>
                                _pointerHover;

            std::vector<std::shared_ptr<Action> >
                                _actions;

            std::string         _tooltipText;
            static bool         _tooltipsEnabled;
            struct TooltipData
            {
                std::chrono::steady_clock::time_point timer;
                std::shared_ptr<Tooltip> tooltip;
            };
            std::map<Core::Event::PointerID, TooltipData>
                                _pointerToTooltips;

            static bool         _resizeRequest;
            static bool         _redrawRequest;

            std::weak_ptr<EventSystem>              _eventSystem;
            std::shared_ptr<AV::Render2D::Render>   _render;
            std::shared_ptr<Style::Style>           _style;

            friend class EventSystem;
        };

    } // namespace UI
} // namespace djv

#include <djvUI/WidgetInline.h>
