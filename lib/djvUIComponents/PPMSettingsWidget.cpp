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
    namespace UIComponents
    {
        namespace Settings
        {
            struct PPMWidget::Private
            {
                std::shared_ptr<UI::ComboBox> comboBox;
                std::shared_ptr<UI::FormLayout> layout;
            };

            void PPMWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                IWidget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UIComponents::Settings::PPMWidget");

                p.comboBox = UI::ComboBox::create(context);

                p.layout = UI::FormLayout::create(context);
                p.layout->addChild(p.comboBox);
                addChild(p.layout);

                _widgetUpdate();

                auto weak = std::weak_ptr<PPMWidget>(std::dynamic_pointer_cast<PPMWidget>(shared_from_this()));
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

            PPMWidget::PPMWidget() :
                _p(new Private)
            {}

            std::shared_ptr<PPMWidget> PPMWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<PPMWidget>(new PPMWidget);
                out->_init(context);
                return out;
            }

            std::string PPMWidget::getSettingsName() const
            {
                return DJV_TEXT("settings_io_section_ppm");
            }

            std::string PPMWidget::getSettingsGroup() const
            {
                return DJV_TEXT("settings_title_io");
            }

            std::string PPMWidget::getSettingsSortKey() const
            {
                return "d";
            }

            void PPMWidget::_initEvent(System::Event::Init& event)
            {
                IWidget::_initEvent(event);
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.layout->setText(p.comboBox, _getText(DJV_TEXT("settings_io_data_type")) + ":");
                    _widgetUpdate();
                }
            }

            void PPMWidget::_widgetUpdate()
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

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

