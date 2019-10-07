//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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
        namespace Render
        {
            class Render2D;

        } // namespace Render

    } // namespace AV

    namespace UI
    {
        class Action;
        class EventSystem;
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
            virtual ~Widget();

            //! Create a new widget.
            static std::shared_ptr<Widget> create(const std::shared_ptr<Core::Context>&);

            //! Get the top-level window.
            std::shared_ptr<Window> getWindow() const;

            //! \name Visibility
            ///@{

            bool isVisible(bool parents = false) const;
            virtual void setVisible(bool);
            void show();
            void hide();

            bool isClipped() const;
            const Core::BBox2f & getClipRect() const;

            float getOpacity(bool parents = false) const;
            void setOpacity(float);

            ///@}

            //! \name Layout
            ///@{

            const glm::vec2& getMinimumSize() const;
            const glm::vec2& getDesiredSize() const;
            const Core::BBox2f & getGeometry() const;
            glm::vec2 getSize() const;
            float getWidth() const;
            float getHeight() const;
            void setGeometry(const Core::BBox2f &);
            void move(const glm::vec2 &);
            void resize(const glm::vec2 &);

            //! This function is called during the layout event to determine the height of a
            //! widget based upon the given width. This is used for example by FlowLayout
            //! and TextBlock.
            virtual float getHeightForWidth(float) const;

            const Layout::Margin & getMargin() const;
            void setMargin(const Layout::Margin &);

            HAlign getHAlign() const;
            VAlign getVAlign() const;
            void setHAlign(HAlign);
            void setVAlign(VAlign);

            //! Utility function for computing widget geometry.
            static Core::BBox2f getAlign(
                const Core::BBox2f &,
                const glm::vec2 & minimumSize,
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
            bool hasTextFocus() const;
            void takeTextFocus();
            void releaseTextFocus();
            virtual void nextTextFocus(const std::shared_ptr<Widget>&);
            virtual void prevTextFocus(const std::shared_ptr<Widget>&);

            ///@}

            //! \name Actions
            ///@{

            const std::vector<std::shared_ptr<Action> > & getActions() const;
            virtual void addAction(const std::shared_ptr<Action> &);
            virtual void removeAction(const std::shared_ptr<Action> &);
            virtual void clearActions();

            ///@}

            //! \name Tooltip
            ///@{

            const std::string & getTooltip() const;
            void setTooltip(const std::string &);

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
            bool event(Core::Event::Event &) override;

        protected:
            //! \name Events
            ///@{

            virtual void _styleEvent(Core::Event::Style &) {}
            virtual void _preLayoutEvent(Core::Event::PreLayout &) {}
            virtual void _layoutEvent(Core::Event::Layout &) {}
            virtual void _clipEvent(Core::Event::Clip &) {}
            virtual void _paintEvent(Core::Event::Paint &);
            virtual void _paintOverlayEvent(Core::Event::PaintOverlay&);
            virtual void _pointerEnterEvent(Core::Event::PointerEnter &);
            virtual void _pointerLeaveEvent(Core::Event::PointerLeave &);
            virtual void _pointerMoveEvent(Core::Event::PointerMove &);
            virtual void _buttonPressEvent(Core::Event::ButtonPress &) {}
            virtual void _buttonReleaseEvent(Core::Event::ButtonRelease &) {}
            virtual void _scrollEvent(Core::Event::Scroll &) {}
            virtual void _dropEvent(Core::Event::Drop &) {}
            virtual void _keyPressEvent(Core::Event::KeyPress &);
            virtual void _keyReleaseEvent(Core::Event::KeyRelease &) {}
            virtual void _textFocusEvent(Core::Event::TextFocus &) {}
            virtual void _textFocusLostEvent(Core::Event::TextFocusLost &) {}
            virtual void _textEvent(Core::Event::Text &) {}

            ///@}

            //! \name Convenience Functions
            ///@{

            const std::weak_ptr<EventSystem>& _getEventSystem() const;
            const std::shared_ptr<AV::Render::Render2D>& _getRender() const;
            const std::shared_ptr<Style::Style> & _getStyle() const;

            ///@}

            //! Call this function when the widget needs resizing.
            void _resize();

            //! Call this function to redraw the widget.
            void _redraw();

            //! Set the minimum size. This is computed and set in the pre-layout event.
            void _setMinimumSize(const glm::vec2&);

            //! Set the desired size. This is computed and set in the pre-layout event.
            void _setDesiredSize(const glm::vec2&);

            static float _getUpdateTime();
            const std::map<Core::Event::PointerID, glm::vec2> _getPointerHover() const;

            std::string _getTooltipText() const;
            std::shared_ptr<Widget> _createTooltipDefault(const std::string &);
            virtual std::shared_ptr<Widget> _createTooltip(const glm::vec2 & pos);

        private:
            std::vector<std::shared_ptr<Widget> >
                                _childWidgets;

            static float        _updateTime;

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
                float timer = 0.F;
                std::shared_ptr<Tooltip> tooltip;
            };
            std::map<Core::Event::PointerID, TooltipData>
                                _pointerToTooltips;

            static bool         _resizeRequest;
            static bool         _redrawRequest;

            std::weak_ptr<EventSystem>              _eventSystem;
            std::shared_ptr<AV::Render::Render2D>   _render;
            std::shared_ptr<Style::Style>           _style;

            friend class EventSystem;
        };

    } // namespace UI
} // namespace djv

#include <djvUI/WidgetInline.h>
