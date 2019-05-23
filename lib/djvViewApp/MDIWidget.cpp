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

#include <djvViewApp/MDIWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Border.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/MDIWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolButton.h>

#include <djvCore/Context.h>
#include <djvCore/Path.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            class SubWidget : public UI::MDI::IWidget
            {
                DJV_NON_COPYABLE(SubWidget);

            protected:
                void _init(const std::shared_ptr<Media>& , Context*);
                SubWidget()
                {}

            public:
                static std::shared_ptr<SubWidget> create(const std::shared_ptr<Media> &, Context*);

                const std::shared_ptr<Media>& getMedia() const;

                void setMaximized(float) override;

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

            private:
                void _opacityUpdate();

                float _fade = 1.f;
                std::shared_ptr<UI::Label> _titleLabel;
                std::shared_ptr<UI::ToolButton> _maximizeButton;
                std::shared_ptr<UI::ToolButton> _closeButton;
                std::shared_ptr<UI::HorizontalLayout> _titleBar;
                std::shared_ptr<MediaWidget> _mediaWidget;
                std::shared_ptr<UI::StackLayout> _layout;
                std::shared_ptr<ValueObserver<float> > _fadeObserver;
            };

            void SubWidget::_init(const std::shared_ptr<Media>& media, Context* context)
            {
                IWidget::_init(context);
                setClassName("djv::ViewApp::MDIWidget::SubWidget");

                _titleLabel = UI::Label::create(context);
                _titleLabel->setText(Core::FileSystem::Path(media->getFileName()).getFileName());
                _titleLabel->setTextHAlign(UI::TextHAlign::Left);
                _titleLabel->setMargin(UI::MetricsRole::Margin);

                _maximizeButton = UI::ToolButton::create(context);
                _maximizeButton->setIcon("djvIconViewLibSDI");
                _maximizeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

                _closeButton = UI::ToolButton::create(context);
                _closeButton->setIcon("djvIconClose");
                _closeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

                _titleBar = UI::HorizontalLayout::create(context);
                _titleBar->setBackgroundRole(UI::ColorRole::Overlay);
                _titleBar->setSpacing(UI::MetricsRole::None);
                _titleBar->addChild(_titleLabel);
                _titleBar->setStretch(_titleLabel, UI::RowStretch::Expand);
                _titleBar->addChild(_maximizeButton);
                _titleBar->addChild(_closeButton);

                _mediaWidget = MediaWidget::create(context);
                _mediaWidget->setMedia(media);

                _layout = UI::StackLayout::create(context);
                _layout->addChild(_mediaWidget);
                auto vLayout = UI::VerticalLayout::create(context);
                vLayout->addChild(_titleBar);
                vLayout->addExpander();
                _layout->addChild(vLayout);
                addChild(_layout);

                _opacityUpdate();

                auto weak = std::weak_ptr<SubWidget>(std::dynamic_pointer_cast<SubWidget>(shared_from_this()));
                _maximizeButton->setClickedCallback(
                    [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto canvas = std::dynamic_pointer_cast<UI::MDI::Canvas>(widget->getParent().lock()))
                        {
                            widget->moveToFront();
                            canvas->setMaximized(!canvas->isMaximized());
                        }
                    }
                });

                _closeButton->setClickedCallback(
                    [media, context]
                {
                    auto fileSystem = context->getSystemT<FileSystem>();
                    fileSystem->close(media);
                });

                auto windowSystem = context->getSystemT<WindowSystem>();
                if (windowSystem)
                {
                    _fadeObserver = ValueObserver<float>::create(
                        windowSystem->observeFade(),
                        [weak](float value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_fade = value;
                            widget->_opacityUpdate();
                        }
                    });
                }
            }

            std::shared_ptr<SubWidget> SubWidget::create(const std::shared_ptr<Media>& media, Context* context)
            {
                auto out = std::shared_ptr<SubWidget>(new SubWidget);
                out->_init(media, context);
                return out;
            }

            const std::shared_ptr<Media>& SubWidget::getMedia() const
            {
                return _mediaWidget->getMedia();
            }

            void SubWidget::setMaximized(float value)
            {
                IWidget::setMaximized(value);
                _opacityUpdate();
                _resize();
            }

            void SubWidget::_preLayoutEvent(Event::PreLayout&)
            {
                auto style = _getStyle();
                const float m = style->getMetric(UI::MetricsRole::Handle);
                _setMinimumSize(_layout->getMinimumSize() + m * (1.f - _getMaximized()));
            }

            void SubWidget::_layoutEvent(Event::Layout&)
            {
                auto style = _getStyle();
                const float m = style->getMetric(UI::MetricsRole::Handle);
                _layout->setGeometry(getGeometry().margin(-m * (1.f - _getMaximized())));
            }

            void SubWidget::_opacityUpdate()
            {
                _titleBar->setOpacity(_fade * (1.f - _getMaximized()));
            }

        } // namespace

        struct MDIWidget::Private
        {
            std::shared_ptr<UI::MDI::Canvas> canvas;
            std::map<std::shared_ptr<Media>, std::shared_ptr<SubWidget> > subWidgets;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > openedObserver;
            std::shared_ptr<ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> > > opened2Observer;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > closedObserver;
        };

        void MDIWidget::_init(Context* context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MDIWidget");

            p.canvas = UI::MDI::Canvas::create(context);
            addChild(p.canvas);

            auto weak = std::weak_ptr<MDIWidget>(std::dynamic_pointer_cast<MDIWidget>(shared_from_this()));
            p.canvas->setActiveCallback(
                [weak, context](const std::shared_ptr<UI::MDI::IWidget> & value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto subWidget = std::dynamic_pointer_cast<SubWidget>(value))
                    {
                        auto fileSystem = context->getSystemT<FileSystem>();
                        fileSystem->setCurrentMedia(subWidget->getMedia());
                    }
                }
            });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.canvas->setMaximizedCallback(
                    [windowSystem](bool value)
                {
                    windowSystem->setMaximized(value);
                });
            }

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        const auto i = widget->_p->subWidgets.find(value);
                        if (i != widget->_p->subWidgets.end())
                        {
                            i->second->moveToFront();
                        }
                    }
                });

                p.openedObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeOpened(),
                    [weak, context](const std::shared_ptr<Media>& value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            auto subWidget = SubWidget::create(value, context);
                            widget->_p->canvas->addChild(subWidget);
                            widget->_p->subWidgets[value] = subWidget;
                        }
                    }
                });

                p.opened2Observer = ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> >::create(
                    fileSystem->observeOpened2(),
                    [weak, context](const std::pair<std::shared_ptr<Media>, glm::vec2>& value)
                {
                    if (value.first)
                    {
                        if (auto widget = weak.lock())
                        {
                            auto subWidget = SubWidget::create(value.first, context);
                            widget->_p->canvas->addChild(subWidget);
                            widget->_p->canvas->setWidgetGeometry(subWidget, BBox2f(value.second, value.second));
                            widget->_p->subWidgets[value.first] = subWidget;
                        }
                    }
                });

                p.closedObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeClosed(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            const auto i = widget->_p->subWidgets.find(value);
                            if (i != widget->_p->subWidgets.end())
                            {
                                widget->_p->canvas->removeChild(i->second);
                                widget->_p->subWidgets.erase(i);
                            }
                        }
                    }
                });
            }
        }

        MDIWidget::MDIWidget() :
            _p(new Private)
        {}

        MDIWidget::~MDIWidget()
        {}

        std::shared_ptr<MDIWidget> MDIWidget::create(Context* context)
        {
            auto out = std::shared_ptr<MDIWidget>(new MDIWidget);
            out->_init(context);
            return out;
        }

        void MDIWidget::setMaximized(bool value)
        {
            _p->canvas->setMaximized(value);
        }

        void MDIWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->canvas->getMinimumSize());
        }

        void MDIWidget::_layoutEvent(Event::Layout&)
        {
            _p->canvas->setGeometry(getGeometry());
        }

        void MDIWidget::_localeEvent(Event::Locale& event)
        {}

    } // namespace ViewApp
} // namespace djv

