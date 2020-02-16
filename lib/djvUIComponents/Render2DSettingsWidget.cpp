//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvUIComponents/Render2DSettingsWidget.h>

#include <djvUI/CheckBox.h>
#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>

#include <djvAV/AVSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Render2DImageSettingsWidget::Private
        {
            AV::Render::ImageFilterOptions filterOptions;
            std::shared_ptr<UI::ComboBox> filterComboBox[2];
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<ValueObserver<AV::Render::ImageFilterOptions> > filterOptionsObserver;
        };

        void Render2DImageSettingsWidget::_init(const std::shared_ptr<Context>& context)
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
            auto contextWeak = std::weak_ptr<Context>(context);
            p.filterComboBox[0]->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->filterOptions.min = static_cast<AV::Render::ImageFilter>(value);
                            auto avSystem = context->getSystemT<AV::AVSystem>();
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
                            widget->_p->filterOptions.mag = static_cast<AV::Render::ImageFilter>(value);
                            auto avSystem = context->getSystemT<AV::AVSystem>();
                            avSystem->setImageFilterOptions(widget->_p->filterOptions);
                        }
                    }
                });

            auto avSystem = context->getSystemT<AV::AVSystem>();
            p.filterOptionsObserver = ValueObserver<AV::Render::ImageFilterOptions>::create(
                avSystem->observeImageFilterOptions(),
                [weak](const AV::Render::ImageFilterOptions& value)
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

        std::shared_ptr<Render2DImageSettingsWidget> Render2DImageSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<Render2DImageSettingsWidget>(new Render2DImageSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string Render2DImageSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_render2d_section_image");
        }

        std::string Render2DImageSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_render2d");
        }

        std::string Render2DImageSettingsWidget::getSettingsSortKey() const
        {
            return "ZZ";
        }

        void Render2DImageSettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.filterComboBox[0], _getText(DJV_TEXT("settings_render2d_minify_filter")) + ":");
            p.layout->setText(p.filterComboBox[1], _getText(DJV_TEXT("settings_render2d_magnify_filter")) + ":");
            _widgetUpdate();
        }

        void Render2DImageSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.filterComboBox[0]->clearItems();
            p.filterComboBox[1]->clearItems();
            for (auto i : AV::Render::getImageFilterEnums())
            {
                std::stringstream ss;
                ss << i;
                p.filterComboBox[0]->addItem(_getText(ss.str()));
                p.filterComboBox[1]->addItem(_getText(ss.str()));
            }
            p.filterComboBox[0]->setCurrentItem(static_cast<int>(p.filterOptions.min));
            p.filterComboBox[1]->setCurrentItem(static_cast<int>(p.filterOptions.mag));
        }

        struct Render2DTextSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckBox> lcdCheckBox;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<bool> > lcdTextObserver;
        };

        void Render2DTextSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::Render2DTextSettingsWidget");

            p.lcdCheckBox = UI::CheckBox::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->addChild(p.lcdCheckBox);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.lcdCheckBox->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto avSystem = context->getSystemT<AV::AVSystem>();
                        avSystem->setLCDText(value);
                    }
                });

            auto avSystem = context->getSystemT<AV::AVSystem>();
            auto weak = std::weak_ptr<Render2DTextSettingsWidget>(std::dynamic_pointer_cast<Render2DTextSettingsWidget>(shared_from_this()));
            p.lcdTextObserver = ValueObserver<bool>::create(
                avSystem->observeLCDText(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->lcdCheckBox->setChecked(value);
                    }
                });
        }

        Render2DTextSettingsWidget::Render2DTextSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<Render2DTextSettingsWidget> Render2DTextSettingsWidget::create(const std::shared_ptr<Context>& context)
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
            return "ZZ";
        }

        void Render2DTextSettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            p.lcdCheckBox->setText(_getText(DJV_TEXT("settings_render_2d_text_lcd_rendering")));
        }

    } // namespace UI
} // namespace djv

