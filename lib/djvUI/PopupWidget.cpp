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

#include <djvUI/PopupWidget.h>

#include <djvUI/Action.h>
#include <djvUI/Border.h>
#include <djvUI/EventSystem.h>
#include <djvUI/LayoutUtil.h>
#include <djvUI/MenuButton.h>
#include <djvUI/Overlay.h>
#include <djvUI/StackLayout.h>
#include <djvUI/Style.h>
#include <djvUI/Window.h>

#include <djvAV/Render2D.h>

#include <djvCore/Context.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class OverlayWidget : public Widget
            {
                DJV_NON_COPYABLE(OverlayWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                OverlayWidget();

            public:
                static std::shared_ptr<OverlayWidget> create(const std::shared_ptr<Context>&);

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;
                void _buttonPressEvent(Event::ButtonPress &) override;
                void _buttonReleaseEvent(Event::ButtonRelease &) override;

            private:
                std::shared_ptr<StackLayout> _layout;
                std::shared_ptr<Border> _border;
            };

            void OverlayWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::PopupWidget::OverlayWidget");
                setBackgroundRole(ColorRole::Background);
                setPointerEnabled(true);

                _layout = StackLayout::create(context);

                _border = Border::create(context);
                _border->addChild(_layout);
                Widget::addChild(_border);
            }

            OverlayWidget::OverlayWidget()
            {}

            std::shared_ptr<OverlayWidget> OverlayWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr< OverlayWidget>(new OverlayWidget);
                out->_init(context);
                return out;
            }

            void OverlayWidget::addChild(const std::shared_ptr<IObject>& value)
            {
                _layout->addChild(value);
            }

            void OverlayWidget::removeChild(const std::shared_ptr<IObject>& value)
            {
                _layout->removeChild(value);
            }

            void OverlayWidget::_buttonPressEvent(Event::ButtonPress & event)
            {
                event.accept();
            }

            void OverlayWidget::_buttonReleaseEvent(Event::ButtonRelease & event)
            {
                event.accept();
            }

            void OverlayWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_border->getMinimumSize());
            }

            void OverlayWidget::_layoutEvent(Event::Layout &)
            {
                _border->setGeometry(getGeometry());
            }

            class OverlayLayout : public Widget
            {
                DJV_NON_COPYABLE(OverlayLayout);

            protected:
                void _init(const std::shared_ptr<Context>&);
                OverlayLayout();

            public:
                static std::shared_ptr<OverlayLayout> create(const std::shared_ptr<Context>&);

                void setButton(const std::shared_ptr<Widget> &, const std::weak_ptr<Widget> &);

            protected:
                void _layoutEvent(Event::Layout &) override;
                void _paintEvent(Event::Paint &) override;

                void _childRemovedEvent(Event::ChildRemoved&) override;

            private:
                std::map<std::shared_ptr<Widget>, std::weak_ptr<Widget> > _widgetToButton;
            };

            void OverlayLayout::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::PopupWidget::OverlayLayout");
            }

            OverlayLayout::OverlayLayout()
            {}

            std::shared_ptr<OverlayLayout> OverlayLayout::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<OverlayLayout>(new OverlayLayout);
                out->_init(context);
                return out;
            }

            void OverlayLayout::setButton(const std::shared_ptr<Widget> & widget, const std::weak_ptr<Widget> & button)
            {
                _widgetToButton[widget] = button;
            }

            void OverlayLayout::_layoutEvent(Event::Layout &)
            {
                const BBox2f & g = getGeometry();
                for (const auto & i : _widgetToButton)
                {
                    if (auto button = i.second.lock())
                    {
                        const auto & buttonBBox = button->getGeometry();
                        const auto & minimumSize = i.first->getMinimumSize();
                        i.first->setGeometry(Layout::getPopupGeometry(g, buttonBBox, minimumSize));
                    }
                }
            }

            void OverlayLayout::_paintEvent(Event::Paint & event)
            {
                Widget::_paintEvent(event);
                const auto& style = _getStyle();
                const float sh = style->getMetric(MetricsRole::Shadow);
                auto render = _getRender();
                render->setFillColor(style->getColor(ColorRole::Shadow));
                for (const auto & i : getChildWidgets())
                {
                    BBox2f g = i->getGeometry();
                    g.min.x -= sh;
                    g.max.x += sh;
                    g.max.y += sh;
                    if (g.isValid())
                    {
                        render->drawShadow(g, sh);
                    }
                }
            }

            void OverlayLayout::_childRemovedEvent(Event::ChildRemoved& event)
            {
                if (auto widget = std::dynamic_pointer_cast<Widget>(event.getChild()))
                {
                    const auto j = _widgetToButton.find(widget);
                    if (j != _widgetToButton.end())
                    {
                        _widgetToButton.erase(j);
                    }
                }
            }

        } // namespace

        struct PopupWidget::Private
        {
            std::shared_ptr<Action> closeAction;
            std::shared_ptr<Button::Menu> button;
            std::shared_ptr<OverlayWidget> overlayWidget;
            std::shared_ptr<OverlayLayout> overlayLayout;
            std::shared_ptr<Layout::Overlay> overlay;
            std::function<void(int)> callback;
            std::function<void(void)> openCallback;
            std::function<void(void)> closeCallback;
            std::shared_ptr<ValueObserver<bool> > closeObserver;
        };

        void PopupWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::PopupWidget");

            p.closeAction = Action::create();
            p.closeAction->setShortcut(GLFW_KEY_ESCAPE);
            addAction(p.closeAction);

            p.button = Button::Menu::create(Button::MenuStyle::Tool, context);
            Widget::addChild(p.button);

            p.overlayWidget = OverlayWidget::create(context);

            p.overlayLayout = OverlayLayout::create(context);
            p.overlayLayout->addChild(p.overlayWidget);
            p.overlayLayout->setButton(p.overlayWidget, p.button);

            p.overlay = Layout::Overlay::create(context);
            p.overlay->setAnchor(p.button);
            p.overlay->setFadeIn(false);
            p.overlay->setBackgroundRole(ColorRole::None);
            p.overlay->addChild(p.overlayLayout);
            Widget::addChild(p.overlay);

            auto weak = std::weak_ptr<PopupWidget>(std::dynamic_pointer_cast<PopupWidget>(shared_from_this()));
            p.overlay->setCloseCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    widget->close();
                }
            });

            p.button->setCheckedCallback(
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    if (value)
                    {
                        widget->open();
                    }
                    else
                    {
                        widget->close();
                    }
                }
            });

            p.closeObserver = ValueObserver<bool>::create(
                p.closeAction->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->close();
                    }
                }
            });
        }

        PopupWidget::PopupWidget() :
            _p(new Private)
        {}

        PopupWidget::~PopupWidget()
        {}

        std::shared_ptr<PopupWidget> PopupWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<PopupWidget>(new PopupWidget);
            out->_init(context);
            return out;
        }

        void PopupWidget::open()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto windowSystem = context->getSystemT<EventSystem>();
                if (auto window = windowSystem->getCurrentWindow().lock())
                {
                    window->addChild(p.overlay);
                    p.closeAction->setEnabled(true);
                    p.overlay->moveToFront();
                    p.overlay->show();
                    if (p.openCallback)
                    {
                        p.openCallback();
                    }
                }
            }
        }

        void PopupWidget::close()
        {
            DJV_PRIVATE_PTR();
            p.closeAction->setEnabled(false);
            p.button->setChecked(false);
            p.overlay->hide();
            if (p.closeCallback)
            {
                p.closeCallback();
            }
        }

        const std::string & PopupWidget::getIcon() const
        {
            return _p->button->getIcon();
        }

        void PopupWidget::setIcon(const std::string & value)
        {
            _p->button->setIcon(value);
        }

        const std::string & PopupWidget::getText() const
        {
            return _p->button->getText();
        }

        void PopupWidget::setText(const std::string & value)
        {
            _p->button->setText(value);
        }

        bool PopupWidget::hasCapturePointer() const
        {
            return _p->overlay->hasCapturePointer();
        }

        bool PopupWidget::hasCaptureKeyboard() const
        {
            return _p->overlay->hasCaptureKeyboard();
        }

        void PopupWidget::setCapturePointer(bool value)
        {
            _p->overlay->setCapturePointer(value);
        }

        void PopupWidget::setCaptureKeyboard(bool value)
        {
            _p->overlay->setCaptureKeyboard(value);
        }

        void PopupWidget::setOpenCallback(const std::function<void(void)>& value)
        {
            _p->openCallback = value;
        }

        void PopupWidget::setCloseCallback(const std::function<void(void)>& value)
        {
            _p->closeCallback = value;
        }

        void PopupWidget::addChild(const std::shared_ptr<IObject> & value)
        {
            _p->overlayWidget->addChild(value);
        }

        void PopupWidget::removeChild(const std::shared_ptr<IObject> & value)
        {
            _p->overlayWidget->removeChild(value);
        }

        void PopupWidget::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->button->getMinimumSize());
        }

        void PopupWidget::_layoutEvent(Event::Layout & event)
        {
            _p->button->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv
