// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/Render2DSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToggleButton.h>

#include <djvRender2D/DataFunc.h>
#include <djvRender2D/RenderSystem.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Render2DImageSettingsWidget::Private
        {
            Render2D::ImageFilterOptions filterOptions;
            std::shared_ptr<UI::ComboBox> filterComboBox[2];
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<Observer::Value<Render2D::ImageFilterOptions> > filterOptionsObserver;
        };

        void Render2DImageSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettingsWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::Render2DImageSettingsWidget");

            p.filterComboBox[0] = UI::ComboBox::create(context);
            p.filterComboBox[1] = UI::ComboBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->addChild(p.filterComboBox[0]);
            p.layout->addChild(p.filterComboBox[1]);
            addChild(p.layout);

            auto weak = std::weak_ptr<Render2DImageSettingsWidget>(std::dynamic_pointer_cast<Render2DImageSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.filterComboBox[0]->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->filterOptions.min = static_cast<Render2D::ImageFilter>(value);
                            auto avSystem = context->getSystemT<Render2D::RenderSystem>();
                            avSystem->setImageFilterOptions(widget->_p->filterOptions);
                        }
                    }
                });
            p.filterComboBox[1]->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->filterOptions.mag = static_cast<Render2D::ImageFilter>(value);
                            auto avSystem = context->getSystemT<Render2D::RenderSystem>();
                            avSystem->setImageFilterOptions(widget->_p->filterOptions);
                        }
                    }
                });

            auto avSystem = context->getSystemT<Render2D::RenderSystem>();
            p.filterOptionsObserver = Observer::Value<Render2D::ImageFilterOptions>::create(
                avSystem->observeImageFilterOptions(),
                [weak](const Render2D::ImageFilterOptions& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->filterOptions = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        Render2DImageSettingsWidget::Render2DImageSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<Render2DImageSettingsWidget> Render2DImageSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<Render2DImageSettingsWidget>(new Render2DImageSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string Render2DImageSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_render2d_section_images");
        }

        std::string Render2DImageSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_render2d");
        }

        std::string Render2DImageSettingsWidget::getSettingsSortKey() const
        {
            return "e";
        }

        void Render2DImageSettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void Render2DImageSettingsWidget::_initEvent(System::Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.filterComboBox[0], _getText(DJV_TEXT("settings_render2d_minify_filter")) + ":");
                p.layout->setText(p.filterComboBox[1], _getText(DJV_TEXT("settings_render2d_magnify_filter")) + ":");
                _widgetUpdate();
            }
        }

        void Render2DImageSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            std::vector<std::string> items;
            for (auto i : Render2D::getImageFilterEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.filterComboBox[0]->setItems(items);
            p.filterComboBox[0]->setCurrentItem(static_cast<int>(p.filterOptions.min));
            p.filterComboBox[1]->setItems(items);
            p.filterComboBox[1]->setCurrentItem(static_cast<int>(p.filterOptions.mag));
        }

        struct Render2DTextSettingsWidget::Private
        {
            std::shared_ptr<UI::ToggleButton> lcdRenderingButton;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<Observer::Value<bool> > lcdRenderingObserver;
        };

        void Render2DTextSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettingsWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::Render2DTextSettingsWidget");

            p.lcdRenderingButton = UI::ToggleButton::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.lcdRenderingButton);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.lcdRenderingButton->setCheckedCallback(
                [contextWeak](bool value)
            {
                if (auto context = contextWeak.lock())
                {
                    auto avSystem = context->getSystemT<Render2D::RenderSystem>();
                    avSystem->setTextLCDRendering(value);
                }
            });

            auto avSystem = context->getSystemT<Render2D::RenderSystem>();
            auto weak = std::weak_ptr<Render2DTextSettingsWidget>(std::dynamic_pointer_cast<Render2DTextSettingsWidget>(shared_from_this()));
            p.lcdRenderingObserver = Observer::Value<bool>::create(
                avSystem->observeTextLCDRendering(),
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->lcdRenderingButton->setChecked(value);
                }
            });
        }

        Render2DTextSettingsWidget::Render2DTextSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<Render2DTextSettingsWidget> Render2DTextSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<Render2DTextSettingsWidget>(new Render2DTextSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string Render2DTextSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_render_2d_section_text");
        }

        std::string Render2DTextSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_render2d");
        }

        std::string Render2DTextSettingsWidget::getSettingsSortKey() const
        {
            return "e";
        }

        void Render2DTextSettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void Render2DTextSettingsWidget::_initEvent(System::Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.lcdRenderingButton, _getText(DJV_TEXT("settings_render_2d_text_lcd_rendering")) + ":");
            }
        }

    } // namespace UI
} // namespace djv

