// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/PPMSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/GroupBox.h>

#include <djvAV/IOSystem.h>
#include <djvAV/PPMFunc.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct PPMSettingsWidget::Private
        {
            std::shared_ptr<ComboBox> comboBox;
            std::shared_ptr<FormLayout> layout;
        };

        void PPMSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::PPMSettingsWidget");

            p.comboBox = ComboBox::create(context);

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.comboBox);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<PPMSettingsWidget>(std::dynamic_pointer_cast<PPMSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.comboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto io = context->getSystemT<AV::IO::IOSystem>();
                        AV::IO::PPM::Options options;
                        rapidjson::Document document;
                        auto& allocator = document.GetAllocator();
                        fromJSON(io->getOptions(AV::IO::PPM::pluginName, allocator), options);
                        options.data = static_cast<AV::IO::PPM::Data>(value);
                        io->setOptions(AV::IO::PPM::pluginName, toJSON(options, allocator));
                    }
                });
        }

        PPMSettingsWidget::PPMSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<PPMSettingsWidget> PPMSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<PPMSettingsWidget>(new PPMSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string PPMSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_io_section_ppm");
        }

        std::string PPMSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_io");
        }

        std::string PPMSettingsWidget::getSettingsSortKey() const
        {
            return "d";
        }

        void PPMSettingsWidget::setLabelSizeGroup(const std::weak_ptr<Text::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void PPMSettingsWidget::_initEvent(System::Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.comboBox, _getText(DJV_TEXT("settings_io_data_type")) + ":");
                _widgetUpdate();
            }
        }

        void PPMSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto io = context->getSystemT<AV::IO::IOSystem>();
                AV::IO::PPM::Options options;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                fromJSON(io->getOptions(AV::IO::PPM::pluginName, allocator), options);
                std::vector<std::string> items;
                for (auto i : AV::IO::PPM::getDataEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    items.push_back(_getText(ss.str()));
                }
                p.comboBox->setItems(items);
                p.comboBox->setCurrentItem(static_cast<int>(options.data));
            }
        }

    } // namespace UI
} // namespace djv

