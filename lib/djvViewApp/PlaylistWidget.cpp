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

#include <djvViewApp/PlaylistWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/IButton.h>
#include <djvUI/FormLayout.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Splitter.h>

#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/Path.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            class Button : public UI::Button::IButton
            {
                DJV_NON_COPYABLE(Button);

            protected:
                void _init(const std::shared_ptr<Media>&, Context*);
                Button()
                {}

            public:
                static std::shared_ptr<Button> create(const std::shared_ptr<Media>&, Context*);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;
                void _paintEvent(Event::Paint&) override;

            private:
                std::shared_ptr<UI::Label> _label;
                std::shared_ptr<UI::FormLayout> _infoLayout;
                std::shared_ptr<UI::VerticalLayout> _layout;
                std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > _imageObserver;
            };

            void Button::_init(const std::shared_ptr<Media>& media, Context* context)
            {
                IButton::_init(context);

                _label = UI::Label::create(context);
                _label->setText(Core::FileSystem::Path(media->getFileName()).getFileName());
                _label->setTextHAlign(UI::TextHAlign::Left);
                _label->setFontSizeRole(UI::MetricsRole::FontLarge);

                auto imageWidget = UI::ImageWidget::create(context);
                imageWidget->setSizeRole(UI::MetricsRole::TextColumn);

                _layout = UI::VerticalLayout::create(context);
                _layout->setMargin(UI::MetricsRole::Margin);
                _layout->addChild(_label);
                auto hLayout = UI::HorizontalLayout::create(context);
                hLayout->addChild(imageWidget);
                _layout->addChild(hLayout);
                _layout->setStretch(hLayout, UI::RowStretch::Expand);
                addChild(_layout);

                _imageObserver = ValueObserver<std::shared_ptr<AV::Image::Image> >::create(
                    media->observeCurrentImage(),
                    [imageWidget](const std::shared_ptr<AV::Image::Image> & value)
                {
                    imageWidget->setImage(value);
                });
            }

            std::shared_ptr<Button> Button::create(const std::shared_ptr<Media> & media, Context* context)
            {
                auto out = std::shared_ptr<Button>(new Button);
                out->_init(media, context);
                return out;
            }

            void Button::_preLayoutEvent(Event::PreLayout& event)
            {
                auto style = _getStyle();
                const float m = style->getMetric(UI::MetricsRole::MarginSmall);
                glm::vec2 size = _layout->getMinimumSize();
                size.x += m;
                _setMinimumSize(size);
            }

            void Button::_layoutEvent(Event::Layout&)
            {
                auto style = _getStyle();
                BBox2f g = getGeometry();
                const float m = style->getMetric(UI::MetricsRole::MarginSmall);
                g.min.x += m;
                _layout->setGeometry(g);
            }

            void Button::_paintEvent(Event::Paint& event)
            {
                IButton::_paintEvent(event);
                auto render = _getRender();
                auto style = _getStyle();
                const BBox2f& g = getGeometry();
                const float m = style->getMetric(UI::MetricsRole::MarginSmall);
                const UI::ColorRole checkedColorRole = getCheckedColorRole();
                if (_isToggled() && checkedColorRole != UI::ColorRole::None)
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(checkedColorRole)));
                    render->drawRect(BBox2f(g.min.x, g.min.y, m, g.h()));
                }
                if (_isPressed())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(getPressedColorRole())));
                    render->drawRect(g);
                }
                else if (_isHovered())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(getHoveredColorRole())));
                    render->drawRect(g);
                }
            }

            class ListWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(ListWidget);

            protected:
                void _init(Context*);
                ListWidget()
                {}

            public:
                static std::shared_ptr<ListWidget> create(Context*);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

            private:
                std::vector<std::shared_ptr<Media> > _media;
                std::shared_ptr<UI::ButtonGroup> _buttonGroup;
                std::shared_ptr<UI::VerticalLayout> _layout;
                std::shared_ptr<ListObserver<std::shared_ptr<Media> > > _mediaObserver;
                std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > _currentMediaObserver;
            };

            void ListWidget::_init(Context* context)
            {
                Widget::_init(context);

                _buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);
                
                _layout = UI::VerticalLayout::create(context);
                _layout->setSpacing(UI::MetricsRole::None);
                addChild(_layout);

                auto weak = std::weak_ptr<ListWidget>(std::dynamic_pointer_cast<ListWidget>(shared_from_this()));
                _buttonGroup->setRadioCallback(
                    [weak, context](int index)
                {
                    if (auto widget = weak.lock())
                    {
                        auto fileSystem = context->getSystemT<FileSystem>();
                        fileSystem->setCurrentMedia(widget->_media[index]);
                    }
                });

                auto fileSystem = context->getSystemT<FileSystem>();
                _mediaObserver = ListObserver<std::shared_ptr<Media>>::create(
                    fileSystem->observeMedia(),
                    [weak, context](const std::vector<std::shared_ptr<Media> > & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_media = value;
                        widget->_buttonGroup->clearButtons();
                        widget->_layout->clearChildren();
                        size_t j = 0;
                        for (const auto& i : value)
                        {
                            auto button = Button::create(i, context);
                            widget->_buttonGroup->addButton(button);
                            widget->_layout->addChild(button);
                            if (j < value.size() - 1)
                            {
                                widget->_layout->addSeparator();
                            }
                            ++j;
                        }
                    }
                });

                _currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        const auto i = std::find(widget->_media.begin(), widget->_media.end(), value);
                        if (i != widget->_media.end())
                        {
                            widget->_buttonGroup->setChecked(i - widget->_media.begin());
                        }
                    }
                });
            }

            std::shared_ptr<ListWidget> ListWidget::create(Context* context)
            {
                auto out = std::shared_ptr<ListWidget>(new ListWidget);
                out->_init(context);
                return out;
            }

            void ListWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void ListWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            class MenuBarSpacer : public UI::Widget
            {
                DJV_NON_COPYABLE(MenuBarSpacer);

            protected:
                MenuBarSpacer()
                {}

            public:
                static std::shared_ptr<MenuBarSpacer> create(Context*);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
            };

            std::shared_ptr<MenuBarSpacer> MenuBarSpacer::create(Context* context)
            {
                auto out = std::shared_ptr<MenuBarSpacer>(new MenuBarSpacer);
                out->_init(context);
                return out;
            }

            void MenuBarSpacer::_preLayoutEvent(Event::PreLayout&)
            {
                auto style = _getStyle();
                const float is = style->getMetric(UI::MetricsRole::Icon);
                _setMinimumSize(glm::vec2(0, is));
            }

        } // namespace

        struct PlaylistWidget::Private
        {
            std::shared_ptr<MediaWidget> mediaWidget;
            std::shared_ptr<ListWidget> listWidget;
            std::shared_ptr<UI::Layout::Splitter> splitter;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
        };

        void PlaylistWidget::_init(Context* context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            p.mediaWidget = MediaWidget::create(context);

            p.listWidget = ListWidget::create(context);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.listWidget);

            p.splitter = UI::Layout::Splitter::create(UI::Orientation::Horizontal, context);
            p.splitter->addChild(p.mediaWidget);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(MenuBarSpacer::create(context));
            vLayout->addChild(scrollWidget);
            vLayout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.splitter->addChild(vLayout);
            p.splitter->setSplit({ .75f, 1.f });
            addChild(p.splitter);

            auto fileSystem = context->getSystemT<FileSystem>();
            auto weak = std::weak_ptr<PlaylistWidget>(std::dynamic_pointer_cast<PlaylistWidget>(shared_from_this()));
            p.currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                fileSystem->observeCurrentMedia(),
                [weak](const std::shared_ptr<Media> & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->mediaWidget->setMedia(value);
                }
            });
        }

        PlaylistWidget::PlaylistWidget() :
            _p(new Private)
        {}

        PlaylistWidget::~PlaylistWidget()
        {}

        std::shared_ptr<PlaylistWidget> PlaylistWidget::create(Context* context)
        {
            auto out = std::shared_ptr<PlaylistWidget>(new PlaylistWidget);
            out->_init(context);
            return out;
        }

        void PlaylistWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->splitter->getMinimumSize());
        }

        void PlaylistWidget::_layoutEvent(Event::Layout&)
        {
            _p->splitter->setGeometry(getGeometry());
        }

        void PlaylistWidget::_localeEvent(Event::Locale& event)
        {
            DJV_PRIVATE_PTR();
        }

    } // namespace ViewApp
} // namespace djv

