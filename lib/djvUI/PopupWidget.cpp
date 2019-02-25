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
#include <djvUI/IWindowSystem.h>
#include <djvUI/MenuButton.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

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
                void _init(Context *);
                OverlayWidget();

            public:
                static std::shared_ptr<OverlayWidget> create(Context *);

                void setWidget(const std::shared_ptr<Widget> &);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;
                void _buttonPressEvent(Event::ButtonPress &) override;
                void _buttonReleaseEvent(Event::ButtonRelease &) override;

            private:
                std::shared_ptr<Border> _border;
            };

            void OverlayWidget::_init(Context * context)
            {
                Widget::_init(context);
                setBackgroundRole(ColorRole::Background);
                setPointerEnabled(true);

                _border = Layout::Border::create(context);
                _border->setParent(shared_from_this());
            }

            OverlayWidget::OverlayWidget()
            {}

            std::shared_ptr<OverlayWidget> OverlayWidget::create(Context * context)
            {
                auto out = std::shared_ptr< OverlayWidget>(new OverlayWidget);
                out->_init(context);
                return out;
            }

            void OverlayWidget::setWidget(const std::shared_ptr<Widget> & widget)
            {
                _border->addWidget(widget);
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
                void _init(Context *);
                OverlayLayout();

            public:
                static std::shared_ptr<OverlayLayout> create(Context *);

                void addWidget(const std::shared_ptr<Widget>&, const std::weak_ptr<Widget> &);
                void removeWidget(const std::shared_ptr<Widget>&);
                void clearWidgets();

            protected:
                void _layoutEvent(Event::Layout&) override;
                void _paintEvent(Event::Paint&) override;

            private:
                std::map<std::shared_ptr<Widget>, std::weak_ptr<Widget> > _widgetToButton;
            };

            void OverlayLayout::_init(Context * context)
            {
                Widget::_init(context);
            }

            OverlayLayout::OverlayLayout()
            {}

            std::shared_ptr<OverlayLayout> OverlayLayout::create(Context * context)
            {
                auto out = std::shared_ptr<OverlayLayout>(new OverlayLayout);
                out->_init(context);
                return out;
            }

            void OverlayLayout::addWidget(const std::shared_ptr<Widget>& widget, const std::weak_ptr<Widget> & button)
            {
                widget->setParent(shared_from_this());
                _widgetToButton[widget] = button;
            }

            void OverlayLayout::removeWidget(const std::shared_ptr<Widget>& value)
            {
                value->setParent(nullptr);
                const auto j = _widgetToButton.find(value);
                if (j != _widgetToButton.end())
                {
                    _widgetToButton.erase(j);
                }
            }

            void OverlayLayout::clearWidgets()
            {
                auto children = getChildren();
                for (auto& child : children)
                {
                    child->setParent(nullptr);
                }
                _widgetToButton.clear();
            }

            void OverlayLayout::_layoutEvent(Event::Layout&)
            {
                const BBox2f & g = getGeometry();
                for (const auto & i : _widgetToButton)
                {
                    if (auto button = i.second.lock())
                    {
                        const auto & buttonBBox = button->getGeometry();
                        const auto & minimumSize = i.first->getMinimumSize();
                        std::vector<BBox2f> geomCandidates;
                        const BBox2f aboveLeft(
                            glm::vec2(std::min(buttonBBox.max.x - minimumSize.x, buttonBBox.min.x), buttonBBox.min.y + 1 - minimumSize.y),
                            glm::vec2(buttonBBox.max.x, buttonBBox.min.y + 1));
                        const BBox2f aboveRight(
                            glm::vec2(buttonBBox.min.x, buttonBBox.min.y + 1 - minimumSize.y),
                            glm::vec2(std::max(buttonBBox.min.x + minimumSize.x, buttonBBox.max.x), buttonBBox.min.y + 1));
                        const BBox2f belowLeft(
                            glm::vec2(std::min(buttonBBox.max.x - minimumSize.x, buttonBBox.min.x), buttonBBox.max.y - 1),
                            glm::vec2(buttonBBox.max.x, buttonBBox.max.y - 1 + minimumSize.y));
                        const BBox2f belowRight(
                            glm::vec2(buttonBBox.min.x, buttonBBox.max.y - 1),
                            glm::vec2(std::max(buttonBBox.min.x + minimumSize.x, buttonBBox.max.x), buttonBBox.max.y - 1 + minimumSize.y));
                        geomCandidates.push_back(belowRight.intersect(g));
                        geomCandidates.push_back(belowLeft.intersect(g));
                        geomCandidates.push_back(aboveRight.intersect(g));
                        geomCandidates.push_back(aboveLeft.intersect(g));
                        if (geomCandidates.size())
                        {
                            std::sort(geomCandidates.begin(), geomCandidates.end(),
                                [](const BBox2f & a, const BBox2f & b) -> bool
                            {
                                return a.getArea() > b.getArea();
                            });
                            const auto & geom = geomCandidates.front();
                            i.first->move(geom.min);
                            i.first->resize(geom.getSize());
                        }
                    }
                }
            }

            void OverlayLayout::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                /*if (auto render = _getRender().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float s = style->getMetric(MetricsRole::Shadow);
                        render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Shadow)));
                        for (const auto & i : getChildrenT<Widget>())
                        {
                            BBox2f g = i->getGeometry();
                            g.min.x += s;
                            g.min.y += s;
                            g.max.x += s;
                            g.max.y += s;
                            render->drawRect(g);
                        }
                    }
                }*/
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
            std::shared_ptr<ValueObserver<bool> > closeObserver;
        };

        void PopupWidget::_init(Context * context)
        {
            IContainer::_init(context);

            setClassName("djv::UI::PopupWidget");

            DJV_PRIVATE_PTR();
            p.closeAction = Action::create();
            p.closeAction->setShortcut(GLFW_KEY_ESCAPE);
            addAction(p.closeAction);

            p.button = Button::Menu::create(context);
            p.button->setParent(shared_from_this());

            p.overlayWidget = OverlayWidget::create(context);

            p.overlayLayout = OverlayLayout::create(context);
            p.overlayLayout->addWidget(p.overlayWidget, p.button);

            p.overlay = Layout::Overlay::create(context);
            p.overlay->setCaptureKeyboard(false);
            p.overlay->setAnchor(p.button);
            p.overlay->setFadeIn(false);
            p.overlay->setBackgroundRole(ColorRole::None);
            p.overlay->addWidget(p.overlayLayout);

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
                [weak, context](bool value)
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

        std::shared_ptr<PopupWidget> PopupWidget::create(Context * context)
        {
            auto out = std::shared_ptr<PopupWidget>(new PopupWidget);
            out->_init(context);
            return out;
        }

        void PopupWidget::setWidget(const std::shared_ptr<Widget>& widget)
        {
            DJV_PRIVATE_PTR();
            p.overlayWidget->setWidget(widget);
        }

        void PopupWidget::open()
        {
            DJV_PRIVATE_PTR();
            if (auto windowSystem = getContext()->getSystemT<IWindowSystem>().lock())
            {
                if (auto window = windowSystem->observeCurrentWindow()->get())
                {
                    window->addWidget(p.overlay);
                    p.closeAction->setEnabled(true);
                    p.overlay->moveToFront();
                    p.overlay->show();
                }
            }
        }

        void PopupWidget::close()
        {
            DJV_PRIVATE_PTR();
            p.closeAction->setEnabled(false);
            p.button->setChecked(false);
            p.overlay->hide();
            p.overlay->setParent(nullptr);
        }

        const std::string& PopupWidget::getIcon() const
        {
            return _p->button->getIcon();
        }

        void PopupWidget::setIcon(const std::string& value)
        {
            _p->button->setIcon(value);
        }

        const std::string& PopupWidget::getText() const
        {
            return _p->button->getText();
        }

        void PopupWidget::setText(const std::string& value)
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

        void PopupWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->button->getMinimumSize());
        }

        void PopupWidget::_layoutEvent(Event::Layout& event)
        {
            _p->button->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv