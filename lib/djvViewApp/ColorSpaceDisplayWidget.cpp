// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceWidgetPrivate.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/FormLayout.h>
#include <djvUI/ListWidget.h>
#include <djvUI/PopupButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Spacer.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            class DisplaysWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(DisplaysWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                DisplaysWidget();

            public:
                ~DisplaysWidget() override;

                static std::shared_ptr<DisplaysWidget> create(const std::shared_ptr<Context>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();

                AV::OCIO::Displays _displays;

                std::shared_ptr<UI::ListWidget> _listWidget;
                std::shared_ptr<UI::SearchBox> _searchBox;

                std::shared_ptr<UI::VerticalLayout> _layout;

                std::shared_ptr<ValueObserver<AV::OCIO::Displays> > _displaysObserver;
            };

            void DisplaysWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::ViewApp::DisplaysWidget");

                _listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
                _listWidget->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);

                _searchBox = UI::SearchBox::create(context);

                _layout = UI::VerticalLayout::create(context);

                _layout = UI::VerticalLayout::create(context);
                _layout->setSpacing(UI::MetricsRole::None);
                _layout->setBackgroundRole(UI::ColorRole::Background);
                auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
                scrollWidget->setBorder(false);
                scrollWidget->addChild(_listWidget);
                _layout->addChild(scrollWidget);
                _layout->setStretch(scrollWidget, UI::RowStretch::Expand);
                _layout->addSeparator();
                _layout->addChild(_searchBox);
                addChild(_layout);

                auto contextWeak = std::weak_ptr<Context>(context);
                _listWidget->setRadioCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                            ocioSystem->setCurrentDisplay(value);
                        }
                    });

                auto weak = std::weak_ptr<DisplaysWidget>(std::dynamic_pointer_cast<DisplaysWidget>(shared_from_this()));
                _searchBox->setFilterCallback(
                    [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_listWidget->setFilter(value);
                        }
                    });

                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                _displaysObserver = ValueObserver<AV::OCIO::Displays>::create(
                    ocioSystem->observeDisplays(),
                    [weak](const AV::OCIO::Displays& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_displays = value;
                            widget->_widgetUpdate();
                        }
                    });
            }

            DisplaysWidget::DisplaysWidget()
            {}

            DisplaysWidget::~DisplaysWidget()
            {}

            std::shared_ptr<DisplaysWidget> DisplaysWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<DisplaysWidget>(new DisplaysWidget);
                out->_init(context);
                return out;
            }

            void DisplaysWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void DisplaysWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void DisplaysWidget::_initEvent(Event::Init& event)
            {
                if (event.getData().text)
                {
                    _widgetUpdate();
                }
            }

            void DisplaysWidget::_widgetUpdate()
            {
                if (auto context = getContext().lock())
                {
                    std::vector<UI::ListItem> items;
                    for (size_t i = 0; i < _displays.first.size(); ++i)
                    {
                        const auto& display = _displays.first[i];
                        UI::ListItem item;
                        item.text = !display.empty() ? display : _getText(DJV_TEXT("av_ocio_display_none"));
                        items.emplace_back(item);
                    }
                    _listWidget->setItems(items, _displays.second);
                }
            }

            class ViewsWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(ViewsWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                ViewsWidget();

            public:
                ~ViewsWidget() override;

                static std::shared_ptr<ViewsWidget> create(const std::shared_ptr<Context>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();

                AV::OCIO::Views _views;

                std::shared_ptr<UI::ListWidget> _listWidget;
                std::shared_ptr<UI::SearchBox> _searchBox;

                std::shared_ptr<UI::VerticalLayout> _layout;

                std::shared_ptr<ValueObserver<AV::OCIO::Views> > _viewsObserver;
            };

            void ViewsWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::ViewApp::ViewsWidget");

                _listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
                _listWidget->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);

                _searchBox = UI::SearchBox::create(context);

                _layout = UI::VerticalLayout::create(context);

                _layout = UI::VerticalLayout::create(context);
                _layout->setSpacing(UI::MetricsRole::None);
                _layout->setBackgroundRole(UI::ColorRole::Background);
                auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
                scrollWidget->setBorder(false);
                scrollWidget->addChild(_listWidget);
                _layout->addChild(scrollWidget);
                _layout->setStretch(scrollWidget, UI::RowStretch::Expand);
                _layout->addSeparator();
                _layout->addChild(_searchBox);
                addChild(_layout);

                auto contextWeak = std::weak_ptr<Context>(context);
                _listWidget->setRadioCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                            ocioSystem->setCurrentView(value);
                        }
                    });

                auto weak = std::weak_ptr<ViewsWidget>(std::dynamic_pointer_cast<ViewsWidget>(shared_from_this()));
                _searchBox->setFilterCallback(
                    [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_listWidget->setFilter(value);
                        }
                    });

                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                _viewsObserver = ValueObserver<AV::OCIO::Views>::create(
                    ocioSystem->observeViews(),
                    [weak](const AV::OCIO::Views& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_views = value;
                            widget->_widgetUpdate();
                        }
                    });
            }

            ViewsWidget::ViewsWidget()
            {}

            ViewsWidget::~ViewsWidget()
            {}

            std::shared_ptr<ViewsWidget> ViewsWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ViewsWidget>(new ViewsWidget);
                out->_init(context);
                return out;
            }

            void ViewsWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void ViewsWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void ViewsWidget::_initEvent(Event::Init& event)
            {
                if (event.getData().text)
                {
                    _widgetUpdate();
                }
            }

            void ViewsWidget::_widgetUpdate()
            {
                if (auto context = getContext().lock())
                {
                    std::vector<UI::ListItem> items;
                    for (size_t i = 0; i < _views.first.size(); ++i)
                    {
                        const auto& view = _views.first[i];
                        UI::ListItem item;
                        item.text = !view.empty() ? view : _getText(DJV_TEXT("av_ocio_view_none"));
                        items.emplace_back(item);
                    }
                    _listWidget->setItems(items, _views.second);
                }
            }

        } // namespace

        struct ColorSpaceDisplayWidget::Private
        {
            AV::OCIO::Config currentConfig;

            std::shared_ptr<UI::PopupButton> displaysPopupButton;
            std::shared_ptr<UI::PopupButton> viewsPopupButton;
            std::shared_ptr<UI::FormLayout> layout;

            std::shared_ptr<ValueObserver<AV::OCIO::Config> > currentConfigObserver;
        };

        void ColorSpaceDisplayWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            p.displaysPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::ComboBox, context);
            p.displaysPopupButton->setPopupIcon("djvIconPopupMenu");

            p.viewsPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::ComboBox, context);
            p.viewsPopupButton->setPopupIcon("djvIconPopupMenu");

            p.layout = UI::FormLayout::create(context);
            p.layout->setMargin(UI::MetricsRole::MarginSmall);
            p.layout->setSpacing(UI::MetricsRole::SpacingSmall);
            p.layout->addChild(p.displaysPopupButton);
            p.layout->addChild(p.viewsPopupButton);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.displaysPopupButton->setOpenCallback(
                [contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        out = DisplaysWidget::create(context);
                    }
                    return out;
                });

            p.viewsPopupButton->setOpenCallback(
                [contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        out = ViewsWidget::create(context);
                    }
                    return out;
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            auto weak = std::weak_ptr<ColorSpaceDisplayWidget>(std::dynamic_pointer_cast<ColorSpaceDisplayWidget>(shared_from_this()));
            p.currentConfigObserver = ValueObserver<AV::OCIO::Config>::create(
                ocioSystem->observeCurrentConfig(),
                [weak](const AV::OCIO::Config& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->currentConfig = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ColorSpaceDisplayWidget::ColorSpaceDisplayWidget() :
            _p(new Private)
        {}

        ColorSpaceDisplayWidget::~ColorSpaceDisplayWidget()
        {}

        std::shared_ptr<ColorSpaceDisplayWidget> ColorSpaceDisplayWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceDisplayWidget>(new ColorSpaceDisplayWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceDisplayWidget::setSizeGroup(const std::shared_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void ColorSpaceDisplayWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceDisplayWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceDisplayWidget::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.displaysPopupButton, _getText(DJV_TEXT("widget_color_space_display")) + ":");
                p.layout->setText(p.viewsPopupButton, _getText(DJV_TEXT("widget_color_space_view")) + ":");
                _widgetUpdate();
            }
        }

        void ColorSpaceDisplayWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.displaysPopupButton->setText(
                !p.currentConfig.display.empty() ?
                p.currentConfig.display :
                _getText(DJV_TEXT("av_ocio_display_none")));
            p.viewsPopupButton->setText(
                !p.currentConfig.view.empty() ?
                p.currentConfig.view :
                _getText(DJV_TEXT("av_ocio_view_none")));
        }

    } // namespace ViewApp
} // namespace djv

