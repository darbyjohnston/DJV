// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceWidgetPrivate.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/FormLayout.h>
#include <djvUI/Label.h>
#include <djvUI/ListWidget.h>
#include <djvUI/Menu.h>
#include <djvUI/PopupButton.h>
#include <djvUI/PopupMenu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Spacer.h>
#include <djvUI/ToolButton.h>

#include <djvAV/IOSystem.h>

#include <djvOCIO/OCIOSystem.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            class ColorSpacesWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(ColorSpacesWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                ColorSpacesWidget();

            public:
                ~ColorSpacesWidget() override;

                static std::shared_ptr<ColorSpacesWidget> create(const std::shared_ptr<System::Context>&);

                void setImage(const std::string& value);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

                void _initEvent(System::Event::Init&) override;

            private:
                void _widgetUpdate();

                std::string _image;
                std::vector<std::string> _colorSpaces;
                OCIO::ImageColorSpaces _imageColorSpaces;

                std::shared_ptr<UI::ListWidget> _listWidget;
                std::shared_ptr<UIComponents::SearchBox> _searchBox;

                std::shared_ptr<UI::VerticalLayout> _layout;

                std::shared_ptr<Observer::List<std::string> > _colorSpacesObserver;
                std::shared_ptr<Observer::Map<std::string, std::string> > _imageColorSpacesObserver;
            };

            void ColorSpacesWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::ViewApp::ColorSpacesWidget");

                _listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
                _listWidget->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);

                _searchBox = UIComponents::SearchBox::create(context);

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
                auto hLayout = UI::HorizontalLayout::create(context);
                hLayout->setSpacing(UI::MetricsRole::None);
                hLayout->addChild(_searchBox);
                hLayout->setStretch(_searchBox, UI::RowStretch::Expand);
                _layout->addChild(hLayout);
                addChild(_layout);

                auto weak = std::weak_ptr<ColorSpacesWidget>(std::dynamic_pointer_cast<ColorSpacesWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<System::Context>(context);
                _listWidget->setRadioCallback(
                    [weak, contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                auto imageColorSpaces = widget->_imageColorSpaces;
                                imageColorSpaces[widget->_image] =
                                    value >= 0 && value < static_cast<int>(widget->_colorSpaces.size()) ?
                                    widget->_colorSpaces[value] :
                                    std::string();
                                auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
                                ocioSystem->setImageColorSpaces(imageColorSpaces);
                            }
                        }
                    });

                _searchBox->setFilterCallback(
                    [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_listWidget->setFilter(value);
                        }
                    });

                auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
                _colorSpacesObserver = Observer::List<std::string>::create(
                    ocioSystem->observeColorSpaces(),
                    [weak](const std::vector<std::string>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_colorSpaces = value;
                            widget->_widgetUpdate();
                        }
                    });

                _imageColorSpacesObserver = Observer::Map<std::string, std::string>::create(
                    ocioSystem->observeImageColorSpaces(),
                    [weak](const OCIO::ImageColorSpaces& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_imageColorSpaces = value;
                            widget->_widgetUpdate();
                        }
                    });
            }

            ColorSpacesWidget::ColorSpacesWidget()
            {}

            ColorSpacesWidget::~ColorSpacesWidget()
            {}

            std::shared_ptr<ColorSpacesWidget> ColorSpacesWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<ColorSpacesWidget>(new ColorSpacesWidget);
                out->_init(context);
                return out;
            }

            void ColorSpacesWidget::setImage(const std::string& value)
            {
                if (value == _image)
                    return;
                _image = value;
                _widgetUpdate();
            }

            void ColorSpacesWidget::_preLayoutEvent(System::Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void ColorSpacesWidget::_layoutEvent(System::Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void ColorSpacesWidget::_initEvent(System::Event::Init& event)
            {
                if (event.getData().text)
                {
                    _widgetUpdate();
                }
            }

            void ColorSpacesWidget::_widgetUpdate()
            {
                if (auto context = getContext().lock())
                {
                    std::vector<UI::ListItem> items;
                    for (size_t i = 0; i < _colorSpaces.size(); ++i)
                    {
                        UI::ListItem item;
                        item.text = !_colorSpaces[i].empty() ?
                            _colorSpaces[i] :
                            _getText(DJV_TEXT("av_ocio_image_none"));
                        items.emplace_back(item);
                    }
                    int index = -1;
                    const auto i = _imageColorSpaces.find(_image);
                    if (i != _imageColorSpaces.end())
                    {
                        const auto j = std::find(_colorSpaces.begin(), _colorSpaces.end(), i->second);
                        if (j != _colorSpaces.end())
                        {
                            index = static_cast<int>(j - _colorSpaces.begin());
                        }
                    }
                    _listWidget->setItems(items);
                    _listWidget->setChecked(index);
                }
            }

        } // namespace

        struct ColorSpaceImageWidget::Private
        {
            OCIO::ImageColorSpaces imageColorSpaces;
            std::vector<std::string> images;
            bool deleteEnabled = false;

            std::vector<std::shared_ptr<UI::PopupButton> > buttons;
            std::shared_ptr<UI::ButtonGroup> deleteButtonGroup;
            std::shared_ptr<UI::ActionGroup> addActionGroup;
            std::shared_ptr<UI::Menu> addMenu;
            std::shared_ptr<UI::PopupMenu> addButton;
            std::shared_ptr<UI::ToolButton> deleteButton;

            std::shared_ptr<UI::FormLayout> formLayout;
            std::shared_ptr<UI::VerticalLayout> addButtonLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<Observer::Map<std::string, std::string> > imageColorSpacesObserver;
        };

        void ColorSpaceImageWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            p.deleteButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);

            p.addActionGroup = UI::ActionGroup::create(UI::ButtonType::Push);
            p.addMenu = UI::Menu::create(context);
            p.addMenu->setIcon("djvIconAddSmall");
            p.addMenu->setMinimumSizeRole(UI::MetricsRole::None);
            p.addButton = UI::PopupMenu::create(context);
            p.addButton->setMenu(p.addMenu);
            p.addButton->setInsideMargin(UI::MetricsRole::None);

            p.deleteButton = UI::ToolButton::create(context);
            p.deleteButton->setButtonType(UI::ButtonType::Toggle);
            p.deleteButton->setIcon("djvIconClearSmall");
            p.deleteButton->setInsideMargin(UI::MetricsRole::None);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addExpander();
            hLayout->addChild(p.addButton);
            hLayout->addChild(p.deleteButton);
            p.layout->addChild(hLayout);
            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->setMargin(UI::MetricsRole::MarginSmall);
            p.layout->addChild(p.formLayout);
            addChild(p.layout);

            _valueUpdate();
            _widgetUpdate();

            auto weak = std::weak_ptr<ColorSpaceImageWidget>(std::dynamic_pointer_cast<ColorSpaceImageWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.deleteButtonGroup->setPushCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto imageColorSpaces = widget->_p->imageColorSpaces;
                            int index = 0;
                            for (auto i = imageColorSpaces.begin(); i != imageColorSpaces.end(); ++i, ++index)
                            {
                                if (index == value)
                                {
                                    imageColorSpaces.erase(i);
                                    break;
                                }
                            }
                            auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
                            ocioSystem->setImageColorSpaces(imageColorSpaces);
                        }
                    }
                });

            p.deleteButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->deleteEnabled = value;
                        widget->_widgetUpdate();
                    }
                });

            auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
            p.imageColorSpacesObserver = Observer::Map<std::string, std::string>::create(
                ocioSystem->observeImageColorSpaces(),
                [weak](const OCIO::ImageColorSpaces& value)
                {
                    if (auto widget = weak.lock())
                    {
                        std::vector<std::string> images;
                        for (const auto& i : value)
                        {
                            images.push_back(i.first);
                        }
                        widget->_p->imageColorSpaces = value;
                        if (images != widget->_p->images)
                        {
                            widget->_p->images = images;
                            widget->_widgetUpdate();
                        }
                        else
                        {
                            widget->_valueUpdate();
                        }
                    }
                });
        }

        ColorSpaceImageWidget::ColorSpaceImageWidget() :
            _p(new Private)
        {}

        ColorSpaceImageWidget::~ColorSpaceImageWidget()
        {}

        std::shared_ptr<ColorSpaceImageWidget> ColorSpaceImageWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceImageWidget>(new ColorSpaceImageWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceImageWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceImageWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceImageWidget::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.addButton->setTooltip(_getText(DJV_TEXT("widget_color_space_add_image_tooltip")));
                p.deleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_images_tooltip")));
            }
        }

        void ColorSpaceImageWidget::_valueUpdate()
        {
            DJV_PRIVATE_PTR();
            DJV_ASSERT(p.imageColorSpaces.size() == p.images.size());
            DJV_ASSERT(p.imageColorSpaces.size() == p.buttons.size());
            for (size_t i = 0; i < p.images.size(); ++i)
            {
                const auto j = p.imageColorSpaces.find(p.images[i]);
                if (j != p.imageColorSpaces.end())
                {
                    const std::string& text = !j->second.empty() ? j->second : _getText(DJV_TEXT("av_ocio_image_none"));
                    p.buttons[i]->setText(text);
                    p.buttons[i]->setTooltip(text);
                }
            }
        }

        void ColorSpaceImageWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.buttons.clear();
                p.addMenu->clearActions();
                p.formLayout->clearChildren();

                std::vector<std::shared_ptr<UI::Button::IButton> > deleteButtons;
                size_t j = 0;
                for (const auto& i : p.imageColorSpaces)
                {
                    auto button = UI::PopupButton::create(UI::MenuButtonStyle::ComboBox, context);
                    const std::string& text = !i.second.empty() ? i.second : _getText(DJV_TEXT("av_ocio_image_none"));
                    button->setText(text);
                    button->setPopupIcon("djvIconPopupMenu");
                    button->setTextElide(textElide);
                    button->setTooltip(text);
                    p.buttons.push_back(button);
                    std::string image = i.first;
                    auto weakContext = std::weak_ptr<System::Context>(context);
                    button->setOpenCallback(
                        [weakContext, image]() -> std::shared_ptr<UI::Widget>
                        {
                            std::shared_ptr<UI::Widget> out;
                            if (auto context = weakContext.lock())
                            {
                                auto widget = ColorSpacesWidget::create(context);
                                widget->setImage(image);
                                out = widget;
                            }
                            return out;
                        });

                    auto deleteButton = UI::ToolButton::create(context);
                    deleteButton->setIcon("djvIconClearSmall");
                    deleteButton->setInsideMargin(UI::MetricsRole::None);
                    deleteButton->setVisible(p.deleteEnabled);
                    deleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_image_tooltip")));
                    deleteButtons.push_back(deleteButton);

                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::MetricsRole::None);
                    hLayout->addChild(button);
                    hLayout->setStretch(button, UI::RowStretch::Expand);
                    hLayout->addChild(deleteButton);
                    p.formLayout->addChild(hLayout);
                    p.formLayout->setText(
                        hLayout,
                        (!i.first.empty() ? i.first : _getText(DJV_TEXT("av_ocio_images_default"))) + ":");

                    ++j;
                }
                p.deleteButtonGroup->setButtons(deleteButtons);

                auto io = context->getSystemT<AV::IO::IOSystem>();
                auto pluginNames = io->getPluginNames();
                pluginNames.insert(pluginNames.begin(), std::string());
                std::vector<std::string> pluginNamesList;
                std::vector<std::shared_ptr<UI::Action> > actions;
                for (const auto& j : pluginNames)
                {
                    const auto k = p.imageColorSpaces.find(j);
                    if (k == p.imageColorSpaces.end())
                    {
                        pluginNamesList.push_back(j);
                        auto action = UI::Action::create();
                        action->setText(!j.empty() ? j : _getText(DJV_TEXT("av_ocio_images_default")));
                        actions.push_back(action);
                        p.addMenu->addAction(action);
                    }
                }
                p.addActionGroup->setActions(actions);
                auto weak = std::weak_ptr<ColorSpaceImageWidget>(std::dynamic_pointer_cast<ColorSpaceImageWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.addActionGroup->setPushCallback(
                    [pluginNamesList, weak, contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                if (value >= 0 && value < static_cast<int>(pluginNamesList.size()))
                                {
                                    auto imageColorSpaces = widget->_p->imageColorSpaces;
                                    imageColorSpaces[pluginNamesList[value]] = std::string();
                                    auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
                                    ocioSystem->setImageColorSpaces(imageColorSpaces);
                                }
                            }
                        }
                    });
                p.addButton->setEnabled(!pluginNamesList.empty());
                p.deleteButton->setEnabled(!p.imageColorSpaces.empty());
                if (p.imageColorSpaces.empty())
                {
                    p.deleteEnabled = false;
                    p.deleteButton->setChecked(false);
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

